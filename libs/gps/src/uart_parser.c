#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "gps.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pico/util/queue.h"

#define SIZE_FIFO 5

static TaskHandle_t gpsHandle;
static uint8_t msg_buf[SIZE_FIFO][100] = {0};

static queue_t fifo;
uint8_t counter = 0;
uint8_t num_buffer = 0;

void on_uart_rx()
{
    // uint32_t ulPreviousMask;
    // ulPreviousMask = taskENTER_CRITICAL_FROM_ISR();

    while (uart_is_readable(UART_ID))
    {
        uint8_t ch = uart_getc(UART_ID);
        static uint8_t nmea = 0;
        if (ch == '$')
        {
            nmea = 0b1;
            counter = 0;
        }

        if (ch == 'G')
        {
            nmea |= 0b10;
        }

        if (nmea == 0b11)
        {
            msg_buf[num_buffer][counter] = ch;
            counter++;
            // if(counter >= 80) counter = 79;
        }
        if (ch == '\n')
        {
            nmea = 0;
           // queue_try_add(&fifo, msg_buf[num_buffer]);
            num_buffer++;
            if (num_buffer == SIZE_FIFO)
                num_buffer = 0;
            counter = 0;
        }
    }
    // taskEXIT_CRITICAL_FROM_ISR( ulPreviousMask );
}


void gps_init_perih()
{
    queue_init(&fifo, sizeof(uint8_t *), SIZE_FIFO);
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));
    uart_set_hw_flow(UART_ID, false, false);

    uart_set_fifo_enabled(UART_ID, false);

    irq_set_exclusive_handler(UART1_IRQ, on_uart_rx);
    irq_set_enabled(UART1_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);
}

// train task
void gps_parce_buffer()
{
    if (queue_is_empty(&fifo))
        return;
    uint8_t *msg;
    if (!queue_try_remove(&fifo, msg))
        return;
    gps_parce_msg(msg);
}