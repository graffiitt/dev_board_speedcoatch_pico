#include "bluetooth/ble_hrt_client.h"
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

int16_t hearth_rate = 0;
static gatt_client_service_t heart_rate_service;

void handle_gatt_client_hrt_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    UNUSED(packet_type);
    UNUSED(channel);
    UNUSED(size);

    uint16_t heart_rate;
    uint8_t att_status;

    // switch (hci_event_packet_get_type(packet))
    //         {
    //         case GATT_EVENT_SERVICE_QUERY_RESULT:
    //             // store service (we expect only one)
    //             gatt_event_service_query_result_get_service(packet, &heart_rate_service);
    //             break;
    //         case GATT_EVENT_QUERY_COMPLETE:
    //             att_status = gatt_event_query_complete_get_att_status(packet);
    //             if (att_status != ATT_ERROR_SUCCESS)
    //             {
    //                 printf("SERVICE_QUERY_RESULT - ATT Error 0x%02x.\n", att_status);
    //                 // gap_disconnect(connection_handle);
    //                 break;
    //             }
    //             printf("Search for Heart Rate Measurement characteristic.\n");
    //             gatt_client_discover_characteristics_for_service_by_uuid16(handle_gatt_client_event,
    //                                                                        connection_handle,
    //                                                                        &heart_rate_service,
    //                                                                        ORG_BLUETOOTH_CHARACTERISTIC_HEART_RATE_MEASUREMENT);
    //             break;
    //         default:
    //             break;
    //         }
    //         break;
}
