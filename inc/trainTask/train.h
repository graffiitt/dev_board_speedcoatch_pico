#ifndef TRAIN_H
#define TRAIN_H

#include "display/disp_task.h"

typedef struct __attribute__((__packed__))
{
    double logtitude;
    double latitude;
    uint32_t time_sec;
    uint8_t pulse;
    uint8_t stroke_rate;
} TrainPointData_t;

typedef struct 
{
    uint8_t id;
    uint32_t time;
    uint32_t timeTrain;
    double distance;
} TrainMetadata_t;

typedef struct 
{
    uint8_t currentTrainOffset; 

    uint8_t trainOffsets[10];
    TrainMetadata_t trains[20];
} TrainData_t;


void trainTask(void *params);
void endTrain();

#endif