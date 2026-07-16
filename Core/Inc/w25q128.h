/*
 * w25q128.h
 *
 *  Created on: Jul 14, 2026
 *      Author: richd
 */

#ifndef INC_W25Q128_H_
#define INC_W25Q128_H_

#include "main.h"
#include <stdbool.h>

bool W25Q128_Init(void);
bool W25Q128_ReadJEDEC(uint8_t id[3]);
uint8_t W25Q128_ReadStatus1(void);
void W25Q128_WriteEnable(void);
void W25Q128_WaitBusy(void);
void W25Q128_EraseSector4K(uint32_t address);
void W25Q128_PageProgram(uint32_t address, uint8_t *data, uint16_t length);
void W25Q128_ReadData(uint32_t address, uint8_t *data, uint32_t length);
void W25Q128_EraseBlock32K(uint32_t address);
void W25Q128_EraseBlock64K(uint32_t address);
void W25Q128_ChipErase(void);
void W25Q128_Write(uint32_t address, uint8_t *data, uint32_t length);

#endif /* INC_W25Q128_H_ */
