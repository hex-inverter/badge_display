/*
 * gif_decoder.c
 *
 *  Created on: Oct 15, 2024
 *      Author: Boris
 */
#include <stdint.h>


typedef struct {
	/* DATA FROM HEADER */
	//uint8_t[3] format; /* Is always equat to "GIF" */
	//uint8_t[3] version;
	uint16_t width;
	uint16_t height;
	uint8_t global_table_exists : 1;
	//uint8_t color_res_bits : 3; /* Equal to color resolution bits -1 */
	//uint8_t sorted_flag : 1;
	uint8_t global_table_bits : 3; /* value = z -> 2^(z+1) bits in table */
	uint8_t global_table : 1;
	uint8_t global_table : 1;
	uint8_t bckgnd_clr;
	uint8_t aspect_ratio;
	/* DATA FROM IMAGE DESCRIPTOR */

} gif_data_t;

typedef struct
{
	uint16_t left;
	uint16_t top;
	uint16_t width;
	uint16_t height;
	uint8_t local_color_flag : 1;
	uint8_t interlace_flag : 1;
	uint8_t sort_flag : 1;
	uint8_t local_color_table_size : 3;
	
} block_coordinates_t;


uint8_t initial_table[255*3] = {0}; /* 3 colors for each index */
uint16_t initial_table_size = 0; 



uint8_t parse_header(gif_data_t header, uint8_t* header_data)
{
	if("G" != *header_data || "I" != *(header_data+1) || "F" != *(header_data+2) )
	{
		/* The header data should start with "GIF" */
		return 0;
	}

	header.width = ( *(header_data+7) << 8 ) |  *(header_data+6);
	header.height = ( *(header_data+9) << 8 ) |  *(header_data+8);
	header.global_table_exists = *(header_data+10) >> 7;
	header.global_table_bits = *(header_data+10) & 0x03;
	header.bckgnd_clr = *(header_data+11);
	header.aspect_ratio = *(header_data+12);
}

uint8_t parse_color_table(uint8_t* table_data, uint8_t* table, gif_data_t header)
{
	initial_table_size = 2 << header.global_table_bits;
	memcpy(initial_table, table_data, initial_table_size*3);
}

uint8_t pase_image_descriptor(block_coordinates_t block_coordinates, uint8_t* block_data)
{
	if("," != (*block_data))
	{
		return 0;
	}

	block_coordinates.left = ( *(block_data+2) << 8 ) |  *(block_data+1);
	block_coordinates.left = ( *(block_data+4) << 8 ) |  *(block_data+3);
	block_coordinates.width = ( *(block_data+6) << 8 ) |  *(block_data+5);
	block_coordinates.height = ( *(block_data+8) << 8 ) |  *(block_data+7);
	block_coordinates.local_color_flag = *(block_data+8) >> 8;
	block_coordinates.local_color_table_size = *(block_data+8) & 0x03;
}



typedef struct {
	color_table_extension_t* next_element;
	uint8_t* indexes;
	uint16_t size;
} color_table_extension_t;

uint16_t extension_table_size;
color_table_extension_t* last_table_entry;

uint8_t create_new_table_extension_element(color_table_extension_t* previous_code, uint8_t index_to_add)
{
	/* TODO: check malloc return */
	last_table_entry->next_element = (color_table_extension_t*) malloc(sizeof(color_table_extension_t));
	last_table_entry = last_table_entry->next_element;

	last_table_entry->size = previous_code->size + 1;
	last_table_entry->indexes = (uint16_t) malloc(last_table_entry->size);
	memcpy(last_table_entry->indexes, previous_code->indexes, previous_code->size);
	/* copy index_to_add to last element */
	*((last_table_entry->indexes)+(last_table_entry->size)-1) = index_to_add;

	extension_table_size++;
}

/* Recursive delete function */
uint8_t delete_table_extension()
{
	color_table_extension_t* next_table = last_table_entry->indexes;
}


uint8_t get_table_extension_indexes_pointer(uint16_t code, uint8_t* indexes_pointer, uint16_t number_of_indexes)
{
	/* check if code matches init table element */
	if((code < initial_table_size+2) || (indexes_pointer == NULL) || (number_of_indexes == 0))
	{
		return 0;
	}

	color_table_extension_t* table_element;
	/* TODO: check off by one error */
	code = code - initial_table_size+2;
	for(uint16_t element_num = 0; element_num < code; element_num++)
	{
		table_element = table_element->next_element;
	}
	indexes_pointer = table_element->indexes;
	number_of_indexes = table_element->size;

	return 1;
}


uint8_t indexes_to_rgb(uint8_t* indexes, uint16_t num_of_indexes, uint8_t* output_data)
{
	if((num_of_indexes == 0) || (indexes == NULL) || (output_data == NULL))
	{
		return 0;
	}

	for(uint16_t index = 0; index < num_of_indexes; index++)
	{
		*output_data = initial_table[index*3];
		output_data++;
		*output_data = initial_table[index*3+1];
		output_data++;
		*output_data = initial_table[index*3+2];
		output_data++;
	}
	return 1;
}


uint8_t parse_block_data(uint8_t* decoded_data, uint8_t* data, uint8_t* color_table, uint8_t color_table_size)
{
	uint8_t lwz_minimum = data++;
	uint8_t subblock_size = data++;
	while(1)
	{
		uint16_t last_table_addr = color_table_size+2;
		/* At *(color_table+(2 << color_table_size)), there is "clear code",
		   at *(color_table+(2 << color_table_size)+1), "there is end of information code"
		*/
		uint16_t data_buffer = ( (*(data) << 8) | *(data+1) );
		data += 2;
		uint8_t remainig_bits = 16;
		uint16_t prev_code = 0;

		/* Get next code */
		uint16_t new_code = data_buffer & (1 << lwz_minimum)


		/* TODO: free elements */

		for(uint8_t byte = 0; byte < subblock_size; byte++)
		{


		}
		if(/* next block == 00 */)
		{
			continue;
		}
		else
		{
			subblock_size == 0; /* next block */
		}
	}
}

/* TODO: ignore plain text extension */
/* TODO: ignore application extension */
/* TODO: ignore comment extension */

uint8_t check_EOF(uint8_t* data)
{
	if(";" == *data)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}








