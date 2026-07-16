/*
 * flash_disk.h
 *
 *  Created on: Jul 15, 2026
 *      Author: richd
 */

#ifndef INC_FLASH_DISK_H_
#define INC_FLASH_DISK_H_

#include <stdint.h>
#include <stdbool.h>

#define FLASH_DISK_ERASE_SIZE     4096
/*
 * USB logical sector size
 *
 * USB Mass Storage normally uses
 * 512 byte sectors
 */
#define FLASH_DISK_BLOCK_SIZE      512


/*
 * W25Q128:
 *
 * 16MB = 16 * 1024 * 1024 bytes
 *
 * Number of 512 byte sectors:
 *
 * 16777216 / 512 = 32768
 */
#define FLASH_DISK_BLOCK_COUNT     32768



/*
 * Initialize the flash disk
 *
 * Returns:
 * true  -> flash detected
 * false -> flash not found
 */
bool FlashDisk_Init(void);



/*
 * Read one logical sector
 *
 * block:
 *     logical sector number
 *
 * buffer:
 *     512 byte destination buffer
 */
bool FlashDisk_Read(uint32_t block,
                    uint8_t *buffer);



/*
 * Write one logical sector
 *
 * block:
 *     logical sector number
 *
 * buffer:
 *     512 byte source buffer
 */
bool FlashDisk_Write(uint32_t block,
                     uint8_t *buffer);



/*
 * Get total number of sectors
 */
uint32_t FlashDisk_GetBlockCount(void);



/*
 * Get sector size
 */
uint32_t FlashDisk_GetBlockSize(void);

#endif /* INC_FLASH_DISK_H_ */
