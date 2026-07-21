/*
 * flash_disk.c
 *
 *  Created on: Jul 15, 2026
 *      Author: richd
 */


#include "flash_disk.h"
#include "w25q128.h"
#include <stddef.h>
#include <string.h>

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

bool FlashDisk_Stream(uint32_t block, uint8_t *buffer)
{
    if(buffer == NULL || block >= FLASH_DISK_BLOCK_COUNT)
    {
        return false;
    }

    uint32_t address = block * FLASH_DISK_BLOCK_SIZE;
    
    // Calculate boundaries for the first physical sector
    uint32_t sector1_addr = address & ~(FLASH_DISK_ERASE_SIZE - 1);
    uint32_t offset1 = address - sector1_addr;
    uint32_t len1 = FLASH_DISK_ERASE_SIZE - offset1;

    // 1. Safely update the first physical sector
    uint32_t time1 = HAL_GetTick();
    W25Q128_ReadData(sector1_addr, erase_buffer, FLASH_DISK_ERASE_SIZE);
    memcpy(erase_buffer + offset1, buffer, len1);
    W25Q128_EraseSector4K(sector1_addr);
    W25Q128_Write(sector1_addr, erase_buffer, FLASH_DISK_ERASE_SIZE);
    uint32_t time2 = HAL_GetTick() - time1;

    // 2. If the 4096-byte write spills over into the next physical sector, update it
    if(len1 < FLASH_DISK_ERASE_SIZE)
    {
        uint32_t sector2_addr = sector1_addr + FLASH_DISK_ERASE_SIZE;
        uint32_t len2 = FLASH_DISK_ERASE_SIZE - len1;

        W25Q128_ReadData(sector2_addr, erase_buffer, FLASH_DISK_ERASE_SIZE);
        memcpy(erase_buffer, buffer + len1, len2);
        W25Q128_EraseSector4K(sector2_addr);
        W25Q128_Write(sector2_addr, erase_buffer, FLASH_DISK_ERASE_SIZE);
    }
    uint32_t time3 = HAL_GetTick() - time1;

    return true;
}
