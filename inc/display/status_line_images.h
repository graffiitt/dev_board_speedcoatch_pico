#ifndef IMAGES_H
#define IMAGES_H
// Generated data for images.yaml

#define BLUETOOTH_IMG                  0
#define SATELLITE_VARIANT_IMG          1
#define SATELLITE_UPLINK_IMG           2
#define HEARTH_IMG                     3
#define BATTERY_10_IMG                 4
#define BATTERY_30_IMG                 5
#define BATTERY_50_IMG                 6
#define BATTERY_70_IMG                 7
#define BATTERY_100_IMG                8
#define BATTERY_CHARGING_IMG           9

#include <inttypes.h>
#include <pico.h>

extern const uint8_t status_line_images[] __in_flash();

#endif  // IMAGES_H
