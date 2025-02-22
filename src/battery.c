#include "battery.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "display/status_line_images.h"
#include "bluetooth/bluetooth_core.h"

#define MIN_VOLTAGE 3.2f
#define MAX_VOLTAGE 4.0f
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

    battery_service_server_set_battery_value(battery);
    return BATTERY_10_IMG;
}
