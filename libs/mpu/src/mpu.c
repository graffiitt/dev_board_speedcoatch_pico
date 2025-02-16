#include "mpu.h"

#include "hardware/dma.h"
#include "hardware/i2c.h"
#include "FreeRTOS.h"
#include "task.h"

#define I2C_CHANNEL  i2c_default

static int mpu_addr = 0x68;

static void mpu_reset()
{
    uint8_t buf[] = {0x6B, 0x80};
    i2c_write_blocking(I2C_CHANNEL, mpu_addr, buf, sizeof buf, false);
    vTaskDelay(pdMS_TO_TICKS(100));
    buf[1] = 0x00;
    i2c_write_blocking(I2C_CHANNEL, mpu_addr, buf, sizeof buf, false);
    vTaskDelay(pdMS_TO_TICKS(10));
}

void mpu_init()
{
    i2c_init(I2C_CHANNEL, 400 * 1000);
    gpio_deinit(20);
    gpio_deinit(21);
    gpio_set_function(20, GPIO_FUNC_I2C);
    gpio_set_function(21, GPIO_FUNC_I2C);
    gpio_pull_up(21);
    gpio_pull_up(20);
    mpu_reset();
}

void mpu_sleep(bool state)
{

}

// double/float = (temp / 340.0) + 36.53
int16_t mpu_getTemp()
{
    uint8_t buffer[2];
    uint8_t val = 0x41;
    i2c_write_blocking(i2c_default, mpu_addr, &val, 1, true);
    i2c_read_blocking(i2c_default, mpu_addr, buffer, 2, false);  // False - finished with bus

    return buffer[0] << 8 | buffer[1];
}
