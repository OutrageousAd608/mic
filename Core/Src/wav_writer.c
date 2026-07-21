/*
 * wav_writer.c
 *
 *  Created on: Jul 16, 2026
 *      Author: richd
 */


#include "wav_writer.h"
#include "fatfs.h"
#include <string.h>
#include <stdio.h>


static FIL wav_file;
static uint32_t wav_data_size = 0;


typedef struct
{
    char riff[4];
    uint32_t file_size;
    char wave[4];

    char fmt[4];
    uint32_t fmt_size;

    uint16_t audio_format;
    uint16_t num_channels;

    uint32_t sample_rate;
    uint32_t byte_rate;

    uint16_t block_align;
    uint16_t bits_per_sample;

    // --- PADDING TO REACH 4096 BYTES ---
    char pad_chunk_id[4];
    uint32_t pad_chunk_size;
    uint8_t padding[4044]; 
    // ----------------------------------

    char data[4];
    uint32_t data_size;

} WAV_Header;


bool WAV_Create(char *filename)
{
    FRESULT res;
    res = f_open(&wav_file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if(res != FR_OK) return false;

    // MUST BE STATIC to prevent a stack overflow crash!
    static WAV_Header header; 
    memset(&header, 0, sizeof(WAV_Header)); 

    memcpy(header.riff,"RIFF",4);
    header.file_size = 0;
    memcpy(header.wave,"WAVE",4);

    memcpy(header.fmt,"fmt ",4);
    header.fmt_size = 16;
    header.audio_format = 1;
    header.num_channels = 1;
    header.sample_rate = 10000;
    header.bits_per_sample = 16;

    header.block_align = header.num_channels * header.bits_per_sample / 8;
    header.byte_rate = header.sample_rate * header.block_align;

    // Update the chunk size to match the new 4044-byte array
    memcpy(header.pad_chunk_id, "PAD ", 4);
    header.pad_chunk_size = 4044;

    memcpy(header.data,"data",4);
    header.data_size = 0;

    UINT written;
    f_write(&wav_file, &header, sizeof(header), &written);
    wav_data_size = 0;

    return true;
}



bool WAV_WriteSamples(uint16_t *samples, uint32_t count)
{
    // Force 32-bit alignment so FatFs doesn't break the chunk down for safety reasons
    __attribute__((aligned(4))) static int16_t converted[2048];

    for(uint32_t i=0; i<count; i++)
    {
        converted[i] = ((int32_t)samples[i]-2048)<<4;
    }

    UINT written;
    if(f_write(&wav_file, converted, count*2, &written) != FR_OK)
    {
        return false;
    }

    wav_data_size += written;
    return true;
}



bool WAV_Close(void)
{
    // MUST BE STATIC to prevent a stack overflow crash!
    static WAV_Header header; 
    memset(&header, 0, sizeof(WAV_Header)); 
    UINT written;

    memcpy(header.riff,"RIFF",4);
    
    // Adjust size for the 4096-byte header (4096 - 8 bytes for RIFF/size = 4088)
    header.file_size = 4088 + wav_data_size; 
    
    memcpy(header.wave,"WAVE",4);

    memcpy(header.fmt,"fmt ",4);
    header.fmt_size = 16;
    header.audio_format = 1;
    header.num_channels = 1;
    header.sample_rate = 10000;
    header.bits_per_sample = 16;

    header.block_align = header.num_channels * header.bits_per_sample / 8;
    header.byte_rate = header.sample_rate * header.block_align;

    // Update the chunk size to match the new 4044-byte array
    memcpy(header.pad_chunk_id, "PAD ", 4);
    header.pad_chunk_size = 4044;

    memcpy(header.data,"data",4);
    header.data_size = wav_data_size;

    f_lseek(&wav_file, 0);
    f_write(&wav_file, &header, sizeof(header), &written);
    f_close(&wav_file);

    return true;
}

bool WAV_CreateNew(void)
{
    char filename[20];

    for(int i = 0; i < 1000; i++)
    {
        sprintf(filename, "REC%03d.WAV", i);

        FILINFO info;

        if(f_stat(filename, &info) != FR_OK)
        {
            return WAV_Create(filename);
        }
    }

    return false;
}