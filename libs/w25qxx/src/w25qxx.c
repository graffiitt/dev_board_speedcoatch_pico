#include "w25qxx/w25qxx.h"
#include "hardware/spi.h"
#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"

#if (_W25QXX_USE_FREERTOS == 1)
#define W25qxx_Delay(delay) vTaskDelay(delay)
#include "FreeRTOS.h"
#include "task.h"
#else
#define W25qxx_Delay(delay) sleep_ms(delay)
#endif

// Функция для выбора чипа (CS)
static inline void cs_select()
{
    gpio_put(PIN_CS, 0);
}

// Функция для снятия выбора чипа (CS)
static inline void cs_deselect()
{
    gpio_put(PIN_CS, 1);
}

// Отправка команды без данных
static void send_command(uint8_t cmd)
{
    cs_select();
    spi_write_blocking(SPI_PORT, &cmd, 1);
    cs_deselect();
}

// Чтение регистра статуса
static uint8_t read_status()
{
    uint8_t cmd = CMD_READ_STATUS;
    uint8_t status;
    cs_select();
    spi_write_blocking(SPI_PORT, &cmd, 1);
    spi_read_blocking(SPI_PORT, 0, &status, 1);
    cs_deselect();
    return status;
}

// Ожидание готовности флеш-памяти
static void wait_for_ready()
{
    while (read_status() & 0x01)
    {
        W25qxx_Delay(1); // Ожидание, пока флаг "занято" (BUSY) не сбросится
    }
}

void initSPI()
{
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    cs_deselect();

    // Инициализация SPI
    spi_init(SPI_PORT, 40000 * 1000);
}

void flash_enter_power_down()
{
    send_command(CMD_POWER_DOWN);
}

void flash_exit_power_down()
{
    send_command(CMD_RELEASE_POWER_DOWN);
}

// Чтение ID чипа
void flash_read_flash_id()
{
    uint8_t cmd[] = {CMD_READ_ID, 0x00, 0x00, 0x00};
    uint8_t id[2];
    cs_select();
    spi_write_blocking(SPI_PORT, cmd, 4);
    spi_read_blocking(SPI_PORT, 0, id, 2);
    cs_deselect();
    printf("Manufacturer ID: 0x%02X, Device ID: 0x%02X\n", id[0], id[1]);
}

void flash_erase_sector(uint32_t addr)
{
    uint8_t cmd[] = {CMD_SECTOR_ERASE, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};
    send_command(CMD_WRITE_ENABLE);
    cs_select();
    spi_write_blocking(SPI_PORT, cmd, 4);
    cs_deselect();
    wait_for_ready();
}

void flash_erase_page(uint32_t addr)
{
    uint8_t cmd[] = {CMD_PAGE_ERASE, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};
    send_command(CMD_WRITE_ENABLE);
    cs_select();
    spi_write_blocking(SPI_PORT, cmd, 4);
    cs_deselect();
    wait_for_ready();
}

// Запись данных на флеш-память
void flash_write_data(uint32_t addr, const uint8_t *data, size_t length)
{
    while (length > 0)
    {
        size_t chunk_size = length > PAGE_SIZE ? PAGE_SIZE : length;
        uint8_t cmd[4] = {CMD_PAGE_PROGRAM, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};

        send_command(CMD_WRITE_ENABLE);
        cs_select();
        spi_write_blocking(SPI_PORT, cmd, 4);
        spi_write_blocking(SPI_PORT, data, chunk_size);
        cs_deselect();
        wait_for_ready();

        addr += chunk_size;
        data += chunk_size;
        length -= chunk_size;
    }
}

// Чтение данных из флеш-памяти
void flash_read_data(uint32_t addr, uint8_t *buffer, size_t length)
{
    uint8_t cmd[4] = {CMD_READ_DATA, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};
    cs_select();
    spi_write_blocking(SPI_PORT, cmd, 4);
    spi_read_blocking(SPI_PORT, 0, buffer, length);
    cs_deselect();
}
