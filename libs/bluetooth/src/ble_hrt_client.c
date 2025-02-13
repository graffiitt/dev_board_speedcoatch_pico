#include "bluetooth/bluetooth_core.h"
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

hrt_info_t hrt = {0, 2};

extern int listener_registered;
extern hci_con_handle_t connection_handle;
extern gatt_client_notification_t notification_listener;
static gatt_client_service_t heart_rate_service;
static gatt_client_characteristic_t heart_rate_measurement_characteristic;

static void handle_gatt_hrt_notify_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    switch (hci_event_packet_get_type(packet))
    {
    case GATT_EVENT_NOTIFICATION:
        if (gatt_event_notification_get_value(packet)[0] & 1)
        {
            hrt.heart_rate = little_endian_read_16(gatt_event_notification_get_value(packet), 1);
        }
        else
        {
            hrt.heart_rate = gatt_event_notification_get_value(packet)[1];
        }
        hrt.sensor_contact = (gatt_event_notification_get_value(packet)[0] >> 1) & 3;
        break;
    }
}

static void handle_gatt_hrt_en_notify_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    switch (hci_event_packet_get_type(packet))
    {
    case GATT_EVENT_QUERY_COMPLETE:
        printf("Notifications enabled, ATT status 0x%02x\n", gatt_event_query_complete_get_att_status(packet));
        break;
    }
}

static void handle_gatt_hrt_meas_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    UNUSED(packet_type);
    UNUSED(channel);
    UNUSED(size);
    uint8_t att_status;
    switch (hci_event_packet_get_type(packet))
    {
    case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
        gatt_event_characteristic_query_result_get_characteristic(packet, &heart_rate_measurement_characteristic);
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        att_status = gatt_event_query_complete_get_att_status(packet);
        if (att_status != ATT_ERROR_SUCCESS)
        {
            printf("CHARACTERISTIC_QUERY_RESULT - ATT Error 0x%02x.\n", att_status);
            // gap_disconnect(connection_handle);
            break;
        }
        // register handler for notifications
        listener_registered = 1;
        gatt_client_listen_for_characteristic_value_updates(&notification_listener, handle_gatt_hrt_notify_event, connection_handle, &heart_rate_measurement_characteristic);
        // enable notifications
        printf("Enable Notify on Heart Rate Measurements characteristic.\n");
        gatt_client_write_client_characteristic_configuration(handle_gatt_hrt_en_notify_event, connection_handle,
                                                              &heart_rate_measurement_characteristic, GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION);
        break;
    default:
        break;
    }
}

void handle_gatt_client_hrt_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    UNUSED(packet_type);
    UNUSED(channel);
    UNUSED(size);

    uint8_t att_status;

    switch (hci_event_packet_get_type(packet))
    {
    case GATT_EVENT_SERVICE_QUERY_RESULT:
        // store service (we expect only one)
        gatt_event_service_query_result_get_service(packet, &heart_rate_service);
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        att_status = gatt_event_query_complete_get_att_status(packet);
        if (att_status != ATT_ERROR_SUCCESS)
        {
            // gap_disconnect(connection_handle);
            break;
        }
        printf("Search for Heart Rate Measurement characteristic.\n");
        gatt_client_discover_characteristics_for_service_by_uuid16(handle_gatt_hrt_meas_event, connection_handle, &heart_rate_service, ORG_BLUETOOTH_CHARACTERISTIC_HEART_RATE_MEASUREMENT);
        break;
    default:
        break;
    }
}
