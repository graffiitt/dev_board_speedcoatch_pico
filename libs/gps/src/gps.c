#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "gps.h"

#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 4
#define UART_RX_PIN 5

static uint8_t arr[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xE8, 0x03, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x96};

void initGps()
{
    gpio_deinit(2);
    gpio_init(2);
    gpio_set_dir(2, GPIO_OUT);
    gpio_put(2, 1);
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));
    uart_set_hw_flow(UART_ID, false, false);

    for (int8_t i = 0; i < sizeof arr; i++)
        uart_putc_raw(UART_ID, *(arr + i));
}

void gps_on()
{
    gpio_put(2, 1);
}

void gps_off()
{
    gpio_put(2, 0);
}
