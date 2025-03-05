#include "pico/stdlib.h"
#include "gps.h"
#include "uart_parser.h"
#include "string.h"

void initGps()
{
    gps_init_perih();

    gpio_deinit(2);
    gpio_init(2);
    gpio_set_dir(2, GPIO_OUT);
    gpio_put(2, 1);
}

void gps_on()
{
    gps_init_perih();
    gpio_put(2, 1);
}

void gps_off()
{
    gpio_put(2, 0);
    uart_deinit(UART_ID);
}

void gps_parce_msg(const uint8_t *msg)
{
    if (strcmp(msg, "$GNRMC"))
    {
        static bool led_on;
        led_on = !led_on;
        // cyw43_arch_gpio_put(0, led_on);
    }
}