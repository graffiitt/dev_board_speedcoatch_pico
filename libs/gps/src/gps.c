#include "pico/stdlib.h"
#include "gps.h"
#include "uart_parser.h"
#include "string.h"
#include <math.h>

#define EARTH_RADIUS_KM 6371.0

TaskHandle_t gpsTaskHandle;
GPS_info current_state_gps = {{0, 0}, 0.0f, 0, 0, 0, 0};

void initGps()
{
    gps_init_perih();

    gpio_deinit(2);
    gpio_init(2);
    gpio_set_dir(2, GPIO_OUT);
    gps_on();
}

void gps_on()
{
    gpio_put(2, 1);
    xTaskCreate(gpsTask, "gpsTask", 526, NULL, tskIDLE_PRIORITY + 14, &gpsTaskHandle);
}

void gps_off()
{
    gpio_put(2, 0);
    vTaskDelete(gpsTaskHandle);
}

static bool gps_check_gps(const uint8_t *buf)
{
    uint8_t lenght = 0;
    while ((buf[lenght] != '\n') && (lenght <= 99))
        lenght++;
    if (lenght < 6)
        return false;
    uint8_t crc;

    for (size_t i = 0; i < (lenght - 4); i++)
    {
        crc ^= buf[i];
    }

    char crc_buf[] = {buf[lenght - 3], buf[lenght - 2]};
    int crc_msg = (int)strtol(crc_buf, NULL, 16);
    if (crc_msg == crc)
        return true;
    return false;
}

double convert_to_degrees(const char *coord)
{
    double degrees, minutes;
    sscanf(coord, "%2lf%lf", &degrees, &minutes);
    return degrees + (minutes / 60.0);
}

double haversine_distance(Position_t pose1, Position_t pose2)
{
    double dlat = (pose2.latitude - pose1.latitude) * M_PI / 180.0;
    double dlon = (pose2.longtitude - pose1.longtitude) * M_PI / 180.0;
    pose1.latitude = pose1.latitude * M_PI / 180.0;
    pose2.latitude = pose2.latitude * M_PI / 180.0;

    double a = sin(dlat / 2) * sin(dlat / 2) +
               sin(dlon / 2) * sin(dlon / 2) * cos(pose1.latitude) * cos(pose2.latitude);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS_KM * c;
}

void gps_calc_distance()
{
    static Position_t lastPose = {0, 0};

    if ((lastPose.latitude == current_state_gps.currentPose.latitude) &&
        (lastPose.longtitude == current_state_gps.currentPose.longtitude))
        return;

    if ((lastPose.latitude == 0) && (lastPose.longtitude == 0))
    {
        lastPose.latitude = current_state_gps.currentPose.latitude;
        lastPose.longtitude = current_state_gps.currentPose.longtitude;
        return;
    }

    current_state_gps.distance += haversine_distance(lastPose, current_state_gps.currentPose);

    lastPose.latitude = current_state_gps.currentPose.latitude;
    lastPose.longtitude = current_state_gps.currentPose.longtitude;
}

void gps_parce_msg(const uint8_t *msg)
{
    if (!gps_check_gps(msg))
        return;

    if (strstr((char *)msg, "GNGGA") != NULL)
    {
        char type[6] = {0};
        char time[11] = {0};
        char lat[11] = {0};
        char lat_dir = 0;
        char lon[12] = {0};
        char lon_dir = 0;
        int fix_quality = 0;
        int satellites = 0;

        sscanf(msg, "%5[^,],%10[^,],%10[^,],%c,%11[^,],%c,%d,%d,",
               type, time, lat, &lat_dir, lon, &lon_dir, &fix_quality,
               &satellites);

        if (fix_quality != 0)
            current_state_gps.active = 1;
        else
            current_state_gps.active = 0;

        if (current_state_gps.active)
        {
            current_state_gps.currentPose.latitude = convert_to_degrees(lat);
            current_state_gps.currentPose.longtitude = convert_to_degrees(lon);
            if (lat_dir == 'S')
                current_state_gps.currentPose.latitude = -current_state_gps.currentPose.latitude;
            if (lon_dir == 'W')
                current_state_gps.currentPose.longtitude = -current_state_gps.currentPose.longtitude;
        }
        current_state_gps.sattelits = satellites;
    }

    if (strstr((char *)msg, "GNVTG") != NULL)
    {
        char track_true[30] = {0};
        float speed_kmh = 0;
        sscanf((const char *)msg, "$GNVTG,%[^N]N,%f,K",
               track_true, &speed_kmh);
        current_state_gps.speed = speed_kmh;
    }
}