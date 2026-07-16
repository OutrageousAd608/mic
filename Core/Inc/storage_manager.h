/*
 * storage_manager.h
 *
 *  Created on: Jul 16, 2026
 *      Author: richd
 */

#ifndef INC_STORAGE_MANAGER_H_
#define INC_STORAGE_MANAGER_H_

#include "main.h"
#include <stdbool.h>


typedef enum
{
    STORAGE_IDLE,
    STORAGE_USB_CONNECTED,
    STORAGE_RECORDING

} StorageState;


void StorageManager_Init(void);

void StorageManager_Task(void);


bool StorageManager_IsRecording(void);

bool StorageManager_USBAllowed(void);


void StorageManager_StartRecording(void);

void StorageManager_StopRecording(void);

#endif /* INC_STORAGE_MANAGER_H_ */
