#ifndef BLUETOOTH_CORE_H
#define BLUETOOTH_CORE_H

#include "../cvector.h"
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#define BLE_SIZE_NAME 20

typedef struct {
    uint16_t heart_rate;
    uint8_t sensor_contact; // 2 -- no contact, 3 -- good contacts
} hrt_info_t;

typedef struct {
    char name[BLE_SIZE_NAME];
    bd_addr_t address;
    bd_addr_type_t addr_type;
} ble_device_t;

extern cvector(ble_device_t) ble_devices;
extern hrt_info_t hrt;

void ble_init();
void ble_on();
void ble_off();

void ble_clearDevices();
void ble_startScan();
void ble_stopScan();

void ble_connect( bd_addr_t address, bd_addr_type_t addr_type);
void ble_disconnect();

#endif