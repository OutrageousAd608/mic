/*
 * wav_writer.h
 *
 *  Created on: Jul 16, 2026
 *      Author: richd
 */

#ifndef INC_WAV_WRITER_H_
#define INC_WAV_WRITER_H_

#include "main.h"
#include <stdbool.h>


bool WAV_Create(char *filename);

bool WAV_WriteSamples(uint16_t *samples,
                      uint32_t count);

bool WAV_Close(void);

bool WAV_CreateNew(void);

#endif /* INC_WAV_WRITER_H_ */
