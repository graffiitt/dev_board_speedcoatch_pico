#ifndef W25QXX_H
#define W25QXX_H

#include "pico/stdio.h"

#define _W25QXX_USE_FREERTOS  0

// Настройки SPI
#define SPI_PORT spi1
#define PIN_SCK  10   // SPI1 SCK
#define PIN_MOSI 11   // SPI1 MOSI
#define PIN_MISO 12   // SPI1 MISO
#define PIN_CS   13   // SPI1 Chip Select (CS)

// Команды W25Q128
#define CMD_WRITE_ENABLE    0x06
#define CMD_READ_STATUS     0x05
#define CMD_PAGE_PROGRAM    0x02
#define CMD_READ_DATA       0x03
#define CMD_SECTOR_ERASE    0x20
#define CMD_PAGE_ERASE      0xD8
#define CMD_CHIP_ERASE      0xC7
#define CMD_READ_ID         0x90
#define CMD_POWER_DOWN 0xB9        
#define CMD_RELEASE_POWER_DOWN 0xAB  

#define SECTOR_SIZE 4096
#define PAGE_SIZE   256

void initSPI();
void flash_enter_power_down();
void flash_exit_power_down();
void read_flash_id();

/// @brief  clear 4096 bytes
/// @param addr number sector
void flash_erase_sector(uint32_t addr);  

/// @brief clear 256 bytes
/// @param addr number page
void flash_erase_page(uint32_t addr);    

void flash_write_in_Page(uint16_t page, uint32_t addrInPage, const uint8_t *data, size_t length);
void flash_write_data(uint32_t addr, const uint8_t *data, size_t length);
void flash_read_data(uint32_t addr, uint8_t *buffer, size_t length);

#endif