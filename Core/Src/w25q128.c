/*
 * w25q128.c
 *
 *  Created on: Jul 14, 2026
 *      Author: richd
 */


#include "w25q128.h"

extern SPI_HandleTypeDef hspi1;

#define W25Q_CMD_JEDEC_ID     0x9F
#define W25Q_CMD_READ_STATUS1 0x05
#define W25Q_CMD_WRITE_ENABLE 0x06

static void CS_Low(void)
{
    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port,
                      FLASH_CS_Pin,
                      GPIO_PIN_RESET);
}

static void CS_High(void)
{
    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port,
                      FLASH_CS_Pin,
                      GPIO_PIN_SET);
}

bool W25Q128_Init(void)
{
    uint8_t id[3];

    if (!W25Q128_ReadJEDEC(id))
    {
        return false;
    }

    if (id[0] != 0xEF)
        return false;

    if (id[1] != 0x40)
        return false;

    if (id[2] != 0x18)
        return false;

    return true;
}

bool W25Q128_ReadJEDEC(uint8_t id[3])
{
    uint8_t tx[4] = {W25Q_CMD_JEDEC_ID, 0x00, 0x00, 0x00};
    uint8_t rx[4];

    CS_Low();

    HAL_StatusTypeDef status =
        HAL_SPI_TransmitReceive(&hspi1,
                                tx,
                                rx,
                                4,
                                HAL_MAX_DELAY);

    CS_High();

    if (status != HAL_OK)
        return false;

    id[0] = rx[1];
    id[1] = rx[2];
    id[2] = rx[3];

    return true;
}

uint8_t W25Q128_ReadStatus1(void)
{
    uint8_t tx[2];
    uint8_t rx[2];

    tx[0] = W25Q_CMD_READ_STATUS1;
    tx[1] = 0x00;

    CS_Low();

    HAL_SPI_TransmitReceive(&hspi1,
                            tx,
                            rx,
                            2,
                            HAL_MAX_DELAY);

    CS_High();

    return rx[1];
}

void W25Q128_WriteEnable(void)
{
    uint8_t cmd = W25Q_CMD_WRITE_ENABLE;

    CS_Low();

    HAL_SPI_Transmit(&hspi1,
                     &cmd,
                     1,
                     HAL_MAX_DELAY);

    CS_High();
}

void W25Q128_WaitBusy(void)
{
    while (W25Q128_ReadStatus1() & 0x01)
    {
        // wait
    }
}
