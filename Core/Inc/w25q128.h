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

#endif /* INC_W25Q128_H_ */
