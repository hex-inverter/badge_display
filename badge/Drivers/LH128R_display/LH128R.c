/*
 * L128R.c
 *
 *  Created on: Oct 6, 2024
 *      Author: Boris
 */
#include "LH128R.h"


/* commands */
#define SET_COL_ADDR	0x2A
#define SET_ROW_ADDR	0x2B
#define WRITE_DATA 		0x2C
#define WRITE_DATA_CONT 0x3C
#define SET_BRIGHTNESS	0x51
#define SLEEP_OUT		0x11
#define DISPLAY_ON 		0x29

SPI_HandleTypeDef* spi_handler;
GPIO_TypeDef* dc_port;
uint16_t dc_pin = 0;
GPIO_TypeDef* cs_port;
uint16_t cs_pin = 0;
GPIO_TypeDef* rst_port;
uint16_t rst_pin = 0;

uint8_t first_write_after_frame_set_flag = 0;

void send_command(uint8_t input_command);
void send_data(uint8_t* data, uint16_t size);


void generate_spi_packet(pixel_data_t* pixel)
{
	uint8_t spi_data[3] = {
			/* screen controller ignores bottom two bits */
			pixel->r,
			pixel->g,
			pixel->b,
	};
	send_data(spi_data, sizeof(spi_data));
}

void send_spi_data(pixel_data_t* pixel, uint16_t size_in_bytes)
{
	send_data((uint8_t*) pixel, size_in_bytes);
}

/* TODO: change return value */
void LH128R_init(SPI_HandleTypeDef* spiX, GPIO_TypeDef* dcPortX, uint16_t dcPinX, GPIO_TypeDef* csPortX, uint16_t csPinX, GPIO_TypeDef* rstPortX, uint16_t rstPinX)
{
	GPIO_InitTypeDef GPIO_Init =
	{
		.Pin   = dcPinX,
		.Mode  = GPIO_MODE_OUTPUT_PP,
		.Pull  = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_VERY_HIGH,
	};
	HAL_GPIO_Init(dcPortX, &GPIO_Init);

	GPIO_Init.Pin = csPinX;
	HAL_GPIO_Init(csPortX, &GPIO_Init);


	GPIO_Init.Pin = rstPinX;
	HAL_GPIO_Init(rstPortX, &GPIO_Init);

	spi_handler = spiX;

	dc_port = dcPortX;
	dc_pin = dcPinX;

	cs_port = csPortX;
	cs_pin = csPinX;

	rst_port = rstPortX;
	rst_pin = rstPinX;
	//HAL_SPI_Init(spi_handler);
	HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(dc_port, dc_pin, GPIO_PIN_SET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(rst_port, rst_pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(rst_port, rst_pin, GPIO_PIN_SET);
	HAL_Delay(120);

	/* Initial Sequence */
	uint8_t* p_init_data;
	uint8_t initial_data[] =
	{
			/*command, size of data, data */
			1,  0x14,
			1,  0x14,
			1,  0x40, 
			1,  0xFF,
			1,  0xFF,
			1,  0xFF,
			1,  0x0A,
			1,  0x21,
			1,  0x00,
			1,  0x80,
			1,  0x01,
			1,  0x01,
			1,  0xFF,
			1,  0xFF,
			2,  0x00, 0x00,
			/* orientation: one of the four: */
			1,  0x18, 0x28, 0x48, 0x88,
			/* color modes, select one: 12bit, 16bit, 18bit */
			1,  0x03, 0x05, 0x06,
			4,  0x08, 0x08, 0x08, 0x08,
			1,  0x06,
			1,  0x00,
			3,  0x60, 0x01, 0x04,
			1,  0x13,
			1,  0x13,
			1,  0x22,
			1,  0x11,
			2,  0x10, 0x0E,
			3,  0x21, 0x0C, 0x02,
			6,  0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
			6,  0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
			6,  0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
			6,  0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
			2,  0x1B, 0x0B,
			1,  0x77,
			1,  0x63,
			9,  0x07 ,0x07 ,0x04 ,0x0E ,0x0F ,0x09 ,0x07 ,0x08 ,0x03,
			1,  0x34,
			12, 0x18 ,0x0D ,0x71 ,0xED ,0x70 ,0x70 ,0x18 ,0x0F ,0x71 ,0xEF ,0x70 ,0x70,
			12, 0x18 ,0x11 ,0x71 ,0xF1 ,0x70 ,0x70 ,0x18 ,0x13 ,0x71 ,0xF3 ,0x70 ,0x70,
			7,  0x28 ,0x29 ,0xF1 ,0x01 ,0xF1 ,0x00 ,0x07,
			10, 0x3C ,0x00 ,0xCD ,0x67 ,0x45 ,0x45 ,0x10 ,0x00 ,0x00 ,0x00,
			10, 0x00, 0x3C ,0x00 ,0x00 ,0x00 ,0x01 ,0x54 ,0x10 ,0x32 ,0x98,
			7,  0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00,
			2,  0x3E, 0x07,
	};
	p_init_data = initial_data;




	send_command(0xEF);
    
    send_command(0xEB);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xFE);
    send_command(0xEF);
    
    send_command(0xEB);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x84);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;

	send_command(0x85);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;

	send_command(0x86);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x87);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x88);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x89);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x8A);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x8B);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x8C);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x8D);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x8E);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x8F);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    
    send_command(0xB6);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x36);
#if ORIENTATION == 0
    send_data(p_init_data+1, *p_init_data);
	p_init_data += 5;
#elif ORIENTATION == 1
    send_data(p_init_data+2, *p_init_data);
	p_init_data += 5;
#elif ORIENTATION == 2
    send_data(p_init_data+3, *p_init_data);
	p_init_data += 5;
#else
    send_data(p_init_data+4, *p_init_data);
	p_init_data += 5;
#endif
    
	/* Color mode */
    send_command(0x3A);
    send_data(p_init_data+3, *p_init_data);
	p_init_data += 4;
    
    send_command(0x90);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xBD);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xBC);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xFF);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xC3);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;

    send_command(0xC4);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xC9);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xBE);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xE1);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;

    send_command(0xDF);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xF0);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xF1);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xF2);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xF3);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xED);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xAE);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xCD);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x70);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0xE8);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x62);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x63);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x64);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x66);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x67);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x74);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x98);
    send_data(p_init_data+1, *p_init_data);
	p_init_data += (*p_init_data)+1;
    
    send_command(0x35);
    send_command(0x21);

	send_command(SLEEP_OUT);
	HAL_Delay(120);
	send_command(DISPLAY_ON);
	HAL_Delay(20);

	uint16_t diff = (uint16_t) (p_init_data - initial_data);
	uint16_t size = sizeof(initial_data);
	if(diff != size)
	{
		while(1);
	}
}

void send_command(uint8_t input_command)
{
	uint8_t command = input_command;
	HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(dc_port, dc_pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(spi_handler, &command, 1, 1000);
	HAL_GPIO_WritePin(dc_port, dc_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);

}

void send_data(uint8_t* data, uint16_t size)
{
	HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(dc_port, dc_pin, GPIO_PIN_SET);
	HAL_SPI_Transmit(spi_handler, data, size, 1000);
	HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
}

void LH128R_set_and_write_to_area(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, pixel_data_t* data_to_write)
{
	uint8_t column_coordinates[4] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
	uint8_t row_coordinates[4] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
	uint16_t data_size = (x_end - x_start) * (y_end - y_start);

	/* set column */
	send_command(SET_COL_ADDR);
	send_data(column_coordinates, 4);

	/* set row */
	send_command(SET_ROW_ADDR);
	send_data(row_coordinates, 4);

	/* send data */
	send_command(WRITE_DATA);
	for(uint16_t i = 0; i < data_size; i++)
	{
		generate_spi_packet(data_to_write);
		data_to_write++;
	}

	first_write_after_frame_set_flag = 0;
}

void LH128R_set_area(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
	uint8_t column_coordinates[4] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
	uint8_t row_coordinates[4] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};

	/* set column */
	send_command(SET_COL_ADDR);
	send_data(column_coordinates, 4);

	/* set row */
	send_command(SET_ROW_ADDR);
	send_data(row_coordinates, 4);

	first_write_after_frame_set_flag = 1;
}


void LH128R_write_to_area(pixel_data_t* data_to_write, uint16_t data_size)
{
	if(first_write_after_frame_set_flag)
	{
		send_command(WRITE_DATA);
		first_write_after_frame_set_flag = 0;

	}
	else
	{
		send_command(WRITE_DATA_CONT);
	}

	send_spi_data(data_to_write, data_size*3);

//	for(uint16_t i = 0; i < data_size; i++)
//	{
//		generate_spi_packet((uint8_t*) data_to_write);
//		data_to_write++;
//	}
}





