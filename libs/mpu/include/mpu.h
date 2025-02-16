#ifndef MPU_H
#define MPU_H

#include "pico/stdlib.h"

void mpu_init();
void mpu_sleep(bool state);
int16_t mpu_getTemp();

#endif