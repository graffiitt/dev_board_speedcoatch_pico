#ifndef GPS_H
#define GPS_H

#include <stdio.h>

#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 4
#define UART_RX_PIN 5

void initGps();
void gps_on();
void gps_off();

void gps_init_perih();
void gps_parce_buffer();

void gps_parce_msg(const uint8_t*);

#endif