#include "bluetooth/bluetooth_core.h"
#include "mygatt.h"

typedef enum
{
    TC_OFF,
    TC_IDLE,
    TC_W4_SCAN_RESULT,
    TC_W4_CONNECT,
    TC_W4_SERVICE_RESULT,
    TC_W4_HEART_RATE_MEASUREMENT_CHARACTERISTIC,
    TC_W4_ENABLE_NOTIFICATIONS_COMPLETE,
    TC_W4_SENSOR_LOCATION_CHARACTERISTIC,
    TC_W4_SENSOR_LOCATION,
    TC_CONNECTED
} gc_state_t;

cvector(ble_device_t) ble_devices = NULL;
// addr and type of device with correct name
static bd_addr_t le_streamer_addr;
static bd_addr_type_t le_streamer_addr_type;

static hci_con_handle_t connection_handle;
static gatt_client_characteristic_t heart_rate_measurement_characteristic;

static gatt_client_notification_t notification_listener;
int listener_registered;

static gc_state_t state = TC_OFF;
static btstack_packet_callback_registration_t hci_event_callback_registration;

// prototypes
extern void drawDisplay();
extern void handle_gatt_client_hrt_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
static void append_device(uint8_t *address, char *name, int size_name, bd_addr_type_t addr_type);

// returns 1 if name is found in advertisement
static int advertisement_report_contains_uuid16(uint16_t uuid16, uint8_t *advertisement_report)
{
    // get advertisement from report event
    const uint8_t *adv_data = gap_event_advertising_report_get_data(advertisement_report);
    uint8_t adv_len = gap_event_advertising_report_get_data_length(advertisement_report);

    // iterate over advertisement data
    ad_context_t context;
    int found = 0;
    for (ad_iterator_init(&context, adv_len, adv_data); ad_iterator_has_more(&context); ad_iterator_next(&context))
    {
        uint8_t data_type = ad_iterator_get_data_type(&context);
        uint8_t data_size = ad_iterator_get_data_len(&context);
        const uint8_t *data = ad_iterator_get_data(&context);
        int i;
        switch (data_type)
        {
        case BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS:
        case BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS:
            // compare common prefix
            for (i = 0; i < data_size; i += 2)
            {
                if (little_endian_read_16(data, i) == uuid16)
                {
                    found = 1;
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
    return found;
}

static void dump_advertisement_data(const uint8_t *adv_data, uint8_t adv_size, uint8_t *address, bd_addr_type_t addr_type)
{
    ad_context_t context;
    for (ad_iterator_init(&context, adv_size, (uint8_t *)adv_data); ad_iterator_has_more(&context); ad_iterator_next(&context))
    {
        uint8_t data_type = ad_iterator_get_data_type(&context);
        uint8_t size = ad_iterator_get_data_len(&context);
        const uint8_t *data = ad_iterator_get_data(&context);
        if ((data_type == BLUETOOTH_DATA_TYPE_SHORTENED_LOCAL_NAME) || (data_type == BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME))
        {
            append_device(address, (char *)data, size, addr_type);
            return;
        }
    }
}

// // handler for seacrh hearth reate service
// static void pactet_handler_scanner(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
// {
//     UNUSED(channel);
//     UNUSED(size);

//     if (packet_type != HCI_EVENT_PACKET)
//         return;

//     if (hci_event_packet_get_type(packet) == GAP_EVENT_ADVERTISING_REPORT)
//     {
//         bd_addr_t address;
//         uint8_t length;

//         if (!advertisement_report_contains_uuid16(ORG_BLUETOOTH_SERVICE_HEART_RATE, packet))
//             return;
//         gap_event_advertising_report_get_address(packet, address);
//         bd_addr_type_t le_streamer_addr_type = gap_event_advertising_report_get_address_type(packet);

//         length = gap_event_advertising_report_get_data_length(packet);
//         const uint8_t *data = gap_event_advertising_report_get_data(packet);
//         printf("Advertisement (legacy) event: addr %s \n", bd_addr_to_str(address));
//         dump_advertisement_data(data, length, address, le_streamer_addr_type);
//     }
// }

static void hci_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    UNUSED(channel);
    UNUSED(size);

    if (packet_type != HCI_EVENT_PACKET)
        return;

    switch (hci_event_packet_get_type(packet))
    {
    case BTSTACK_EVENT_STATE:
        // BTstack activated, get started
        if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING)
        {
        }
        else
        {
            state = TC_OFF;
        }
        break;

    case GAP_EVENT_ADVERTISING_REPORT:
        bd_addr_t address;
        uint8_t length;

        if (!advertisement_report_contains_uuid16(ORG_BLUETOOTH_SERVICE_HEART_RATE, packet))
            return;
        gap_event_advertising_report_get_address(packet, address);
        bd_addr_type_t le_streamer_addr_type = gap_event_advertising_report_get_address_type(packet);

        length = gap_event_advertising_report_get_data_length(packet);
        const uint8_t *data = gap_event_advertising_report_get_data(packet);
        printf("Advertisement (legacy) event: addr %s \n", bd_addr_to_str(address));
        dump_advertisement_data(data, length, address, le_streamer_addr_type);
        break;

    case HCI_EVENT_META_GAP:
        // wait for connection complete
        if (hci_event_gap_meta_get_subevent_code(packet) != GAP_SUBEVENT_LE_CONNECTION_COMPLETE)
            break;
        connection_handle = gap_subevent_le_connection_complete_get_connection_handle(packet);
        printf("Search for Heart Rate service.\n");
        state = TC_W4_SERVICE_RESULT;
        gatt_client_discover_primary_services_by_uuid16(handle_gatt_client_hrt_event,
                                                        connection_handle,
                                                        ORG_BLUETOOTH_SERVICE_HEART_RATE);
        break;
    case HCI_EVENT_DISCONNECTION_COMPLETE:
        // unregister listener
        connection_handle = HCI_CON_HANDLE_INVALID;
        if (listener_registered)
        {
            listener_registered = 0;
            gatt_client_stop_listening_for_characteristic_value_updates(&notification_listener);
        }

        printf("Disconnected %s\n", bd_addr_to_str(le_streamer_addr));
        if (state == TC_OFF)
            break;
        // gatt_heart_rate_client_start();
        break;
    default:
        break;
    }
}

void append_device(uint8_t *address, char *name, int size_name, bd_addr_type_t addr_type)
{
    printf("%s \n", name);

    for (int i = 0; i < cvector_size(ble_devices); i++)
    {
        if (bd_addr_cmp(cvector_at(ble_devices, i)->address, address) == 0)
            return;
    }
    ble_device_t it;
    if (size_name > BLE_SIZE_NAME)
    {
        size_name = BLE_SIZE_NAME - 1;
    }
    strcpy(it.name, name);
    // for (uint8_t i = 0; i < size_name; i++)
    // {
    //     it.name[i] = name[i];
    // }
    // it.name[size_name] = '\0';
    it.addr_type = addr_type;
    bd_addr_copy(it.address, address);

    cvector_push_back(ble_devices, it);
    drawDisplay();
}

void ble_init()
{
    if(cyw43_arch_init())
    {
        printf("error init ble \n");
        return;
    } 
    hci_power_control(HCI_POWER_ON);

    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(sizeof profile_data, (uint8_t *)profile_data);
    gap_advertisements_enable(1);

    hci_event_callback_registration.callback = &hci_event_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    l2cap_init();

    gatt_client_init();

    sm_init();
    sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    hci_power_control(HCI_POWER_OFF);

}

void ble_on()
{
    hci_power_control(HCI_POWER_ON);
}

void ble_off()
{
    if (listener_registered)
        gap_disconnect(connection_handle);

    hci_power_control(HCI_POWER_OFF);
}

void ble_clearDevices()
{
    cvector_clear(ble_devices);
}

void ble_startScan()
{
    gap_set_scan_parameters(1, 48, 48);
    gap_start_scan();
}

void ble_stopScan()
{
    gap_stop_scan();
}

void ble_connect(bd_addr_t address, bd_addr_type_t addr_type)
{
    gap_connect(address, addr_type);
}

void ble_disconnect()
{
    gap_disconnect(connection_handle);
}
