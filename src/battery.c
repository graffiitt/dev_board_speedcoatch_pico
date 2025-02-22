#include "battery.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "display/status_line_images.h"
#include "bluetooth/bluetooth_core.h"

#define MIN_VOLTAGE 3.0f
#define MAX_VOLTAGE 4.2f
#define DELTA_VOLTAGE (MAX_VOLTAGE - MIN_VOLTAGE)

int8_t battery = 0;

void setup_adc()
{
    adc_init();
    gpio_deinit(28);
    gpio_set_function(28, GPIO_FUNC_SIO);
    gpio_disable_pulls(28);
    gpio_set_input_enabled(28, false);
    adc_select_input(2);
}

int adc_getImageCharge()
{
    uint16_t res = adc_read();

    const float voltage = 3.3f / (1 << 12) * res * 2.61f;
    printf("%d", voltage);

    int8_t per = (uint8_t)((voltage - MIN_VOLTAGE) / DELTA_VOLTAGE * 100);
    if (per > 100)
        per = 100;
    else if (per < 1)
    {
        per = 1;
    }
    battery = per;
    battery_service_server_set_battery_value(battery);

    int id = BATTERY_10_IMG;
    if (battery <= 10)
    {
        id = BATTERY_10_IMG;
    }
    else if (battery <= 30)
    {
        id = BATTERY_30_IMG;
    }
    else if (battery <= 50)
    {
        id = BATTERY_50_IMG;
    }
    else if (battery <= 70)
    {
        id = BATTERY_70_IMG;
    }
    else if (battery <= 100)
    {
        id = BATTERY_100_IMG;
    }

    return id;
}
