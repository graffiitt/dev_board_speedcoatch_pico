#ifndef GPS_H
#define GPS_H

#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define MINIMUM_SPEED 0.01

typedef struct
{
    double longtitude;
    double latitude;
} Position_t;

typedef struct
{
    Position_t currentPose;
    double distance; // in meters
    uint32_t time;     // in sec
    float speed;
    uint8_t sattelits;
    bool active;
} GPS_info;

extern GPS_info current_state_gps;

void initGps();
void gps_on();
void gps_off();

void gpsTask();

void gps_init_perih();

void gps_parce_msg(const uint8_t *);
void gps_calc_distance();

#endif