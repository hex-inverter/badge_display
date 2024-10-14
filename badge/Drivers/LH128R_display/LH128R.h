/*
 * LH128R.h
 *
 *  Created on: Oct 6, 2024
 *      Author: Boris
 */

#ifndef LH128R_DISPLAY_LH128R_H_
#define LH128R_DISPLAY_LH128R_H_

#include "stdint.h"

#include "stm32h5xx_hal.h"
#include "stm32h5xx_hal_spi.h"
#include "stm32h5xx_hal_gpio.h"

#define ORIENTATION 0

typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} pixel_data_t;

void LH128R_init(SPI_HandleTypeDef* spiX,
		GPIO_TypeDef* dcPortX,	uint16_t dcPinX,
		GPIO_TypeDef* csPortX, 	uint16_t csPinX,
		GPIO_TypeDef* rstPortX, uint16_t rstPinX);

void LH128R_set_and_write_to_area(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, pixel_data_t* data_to_write);

void LH128R_set_area(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

void LH128R_write_to_area(pixel_data_t* data_to_write, uint16_t data_size);

void send_spi_data(pixel_data_t* pixel, uint16_t size_in_bytes);


#endif /* LH128R_DISPLAY_LH128R_H_ */
