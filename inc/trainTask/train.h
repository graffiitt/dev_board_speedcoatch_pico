#ifndef TRAIN_H
#define TRAIN_H

#include "display/disp_task.h"
#include "gps.h"

typedef struct __attribute__((packed)){
    int16_t year;    ///< 0..4095
    int8_t month;    ///< 1..12, 1 is January
    int8_t day;      ///< 1..28,29,30,31 depending on month
    int8_t hour;     ///< 0..23
    int8_t min;      ///< 0..59
} My_datetime_t; // size 6

typedef struct  __attribute__((packed))
{   
    int offset_train[33]; // 3 page size 23byte
    int offset_Train_details[33]; // size 22

    uint8_t current_num_train;
    uint32_t current_num_trains_det;

    double all_distance;

} HeaderTrain_t;

typedef struct __attribute__((packed)){
    Position_t pose;
    uint32_t time;
    uint8_t pulse;
    uint8_t stroke;
} Point_t; // size

typedef struct __attribute__((packed)){
    My_datetime_t time;
    double distance;
    uint32_t time_train;
    float ave_speed;
    uint8_t ave_stroke;
} Train_t;


void trainTask(void *params);
void endTrain();

#endif