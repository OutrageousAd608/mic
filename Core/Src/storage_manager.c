/*
 * storage_manager.c
 *
 *  Created on: Jul 16, 2026
 *      Author: richd
 */


#include "storage_manager.h"
#include "fatfs.h"
#include "usb_device.h"
#include "wav_writer.h"


static StorageState current_state;
static uint32_t samples_written = 0;

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim6;
extern uint16_t audio_buffer[1024];

extern volatile uint8_t adc_half_ready;
extern volatile uint8_t adc_full_ready;

static bool ButtonPressed(void)
{
    return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET;
}


void StorageManager_Init(void)
{
    current_state = STORAGE_IDLE;
}



void StorageManager_Task(void)
{

    switch(current_state)
    {

    case STORAGE_IDLE:

        if(ButtonPressed())
        {
            StorageManager_StartRecording();
        }

        break;



    case STORAGE_USB_CONNECTED:


        /*
         * USB owns flash
         * recording blocked
         */


        break;



    case STORAGE_RECORDING:

    if(adc_half_ready)
    {
        adc_half_ready = 0;

        WAV_WriteSamples(&audio_buffer[0],512);

        samples_written += 512;
    }


    if(adc_full_ready)
    {
        adc_full_ready = 0;

        WAV_WriteSamples(&audio_buffer[512],512);

        samples_written += 512;
    }


    if(!ButtonPressed() && samples_written > 16000)
    {
        StorageManager_StopRecording();
    }


    break;


    }

}



bool StorageManager_IsRecording(void)
{
    return current_state == STORAGE_RECORDING;
}



bool StorageManager_USBAllowed(void)
{
    return current_state != STORAGE_RECORDING;
}



void StorageManager_StartRecording(void)
{

    if(current_state == STORAGE_USB_CONNECTED)
    {
        return;
    }


    if(WAV_CreateNew())
    {
        samples_written = 0;

        adc_half_ready = 0;
        adc_full_ready = 0;

        HAL_TIM_Base_Start(&htim6);

        HAL_ADC_Start_DMA(&hadc1,
                          (uint32_t*)audio_buffer,
                          1024);


        current_state = STORAGE_RECORDING;
    }

}



void StorageManager_StopRecording(void)
{

    HAL_ADC_Stop_DMA(&hadc1);

    HAL_TIM_Base_Stop(&htim6);


    WAV_Close();


    current_state = STORAGE_IDLE;

}


