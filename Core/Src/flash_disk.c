/*
 * flash_disk.c
 *
 *  Created on: Jul 15, 2026
 *      Author: richd
 */


#include "flash_disk.h"
#include "w25q128.h"
#include <stddef.h>

static bool FlashDisk_UpdateEraseSector(uint32_t sector_address,
                                        uint32_t offset,
                                        uint8_t *data);

static uint8_t erase_buffer[FLASH_DISK_ERASE_SIZE];

bool FlashDisk_Init(void)
{
    return W25Q128_Init();
}



bool FlashDisk_Read(uint32_t block,
                    uint8_t *buffer)
{
    uint32_t address;

    if(buffer == NULL)
        {
            return false;
        }

    if(block >= FLASH_DISK_BLOCK_COUNT)
    {
        return false;
    }


    address = block * FLASH_DISK_BLOCK_SIZE;


    W25Q128_ReadData(address,
                     buffer,
                     FLASH_DISK_BLOCK_SIZE);


    return true;
}



bool FlashDisk_Write(uint32_t block,
                     uint8_t *buffer)
{
    uint32_t address;
    uint32_t sector_address;
    uint32_t offset;


    if(buffer == NULL)
    {
        return false;
    }


    if(block >= FLASH_DISK_BLOCK_COUNT)
    {
        return false;
    }

    address = block * FLASH_DISK_BLOCK_SIZE;

    sector_address = address & ~(FLASH_DISK_ERASE_SIZE - 1);

    offset = address - sector_address;



    return FlashDisk_UpdateEraseSector(sector_address,
                                       offset,
                                       buffer);
}



uint32_t FlashDisk_GetBlockCount(void)
{
    return FLASH_DISK_BLOCK_COUNT;
}



uint32_t FlashDisk_GetBlockSize(void)
{
    return FLASH_DISK_BLOCK_SIZE;
}

static bool FlashDisk_UpdateEraseSector(uint32_t sector_address,
                                        uint32_t offset,
                                        uint8_t *data)
{

    if(data == NULL)
    {
        return false;
    }

    if(offset + FLASH_DISK_BLOCK_SIZE >
       FLASH_DISK_ERASE_SIZE)
    {
        return false;
    }

    W25Q128_ReadData(sector_address,
                     erase_buffer,
                     FLASH_DISK_ERASE_SIZE);

    for(uint32_t i = 0;
        i < FLASH_DISK_BLOCK_SIZE;
        i++)
    {
        erase_buffer[offset + i] = data[i];
    }

    W25Q128_EraseSector4K(sector_address);

    W25Q128_Write(sector_address,
                  erase_buffer,
                  FLASH_DISK_ERASE_SIZE);



    return true;
}
