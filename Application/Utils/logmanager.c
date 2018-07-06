#include "logmanager.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Memory/SerialFlash.h"
#include "UartInterface/UartCommand.h"
#include "CTTKernel/kernel.h"
#include "Utils/base64.h"

#define MAX_SIZE_BLOCKS 16

#define BLOCK_ALIGN 8

#define NUMBER_OF_BLOCKS(size) (size)/BLOCK_ALIGN + 1

#define LOG_FLASH_START_PAGE 0x0E

#define HEADER_SAYS_ALOC(BYTE) ((((BYTE)>>4)&0b1111) == 0b0101)

#define HEADER_GET_BLOCKS(BYTE) (0b00001111&(BYTE))

#define HEADER_GET_SIZE(BYTE) ((HEADER_GET_BLOCKS(BYTE))*BLOCK_ALIGN)

#define CREATE_HEADER_BYTE(size) (0b01010000 + NUMBER_OF_BLOCKS(size))

typedef struct {
	uint8_t add2;
	uint8_t add1;
	uint8_t add0;

} flash_add_t;

static struct {
	uint8_t initial_page;
	uint8_t initial_add1;
	uint8_t last_page;
	uint16_t log_count;
	flash_add_t next_log_add;

} _this;

static void add_sum(flash_add_t * add, uint8_t bytes) {

	uint16_t new_add0 = (add->add0 + bytes) % 256;
	uint16_t new_add1 = (add->add1 + (add->add0 + bytes) / 256) % 256;
	uint16_t new_add2 = (add->add2 + (add->add1 + (add->add0 + bytes) / 256) / 256) % 256;

	add->add0 = new_add0;
	add->add1 = new_add1;
	add->add2 = new_add2;


}

void init_comands();

void init_log_manager(uint8_t initial_page, uint8_t initial_add1, uint8_t last_page) {

	_this.initial_page = initial_page;
	_this.initial_add1 = initial_add1;
	_this.last_page = last_page;

	init_comands();

}

void erase_logs() {

	int i = _this.initial_page;

	for (i = _this.initial_page; i <= _this.last_page; i++) {
		serialflash_serase(i, 0, 0);
		serialflash_serase(i, 128, 0);
	}



}

uint16_t discover_logs() {

	uint8_t found = 0;
	uint16_t log_count = 0;
	flash_add_t current_add;
	current_add.add2 = _this.initial_page;
	current_add.add1 = _this.initial_add1;
	current_add.add0 = 0;

	while (!found) {

		uint8_t bytes[10];
		serialflash_read(current_add.add2, current_add.add1, current_add.add0, bytes, 1);

		uint8_t header = bytes[0];

		//		kernel_message(MODE_DEBUG, "Foi encontrado %u no header\n",header);

		if (HEADER_SAYS_ALOC(header)) {
			//			kernel_message(MODE_DEBUG,"size %u %u\n",HEADER_GET_SIZE(header), header);
			add_sum(&current_add, HEADER_GET_SIZE(header));
			log_count++;
		} else {
			found = 1;
			_this.log_count = log_count;
			_this.next_log_add = current_add;
			//			kernel_message(MODE_DEBUG, "Foi encontrado %u no count\n",log_count);
		}

	}

	return log_count;
}

static void write_bytes(flash_add_t add, uint8_t * bytes, uint16_t size) {

	if (add.add0 + size < 256) {

		serialflash_write_size(add.add2, add.add1, add.add0, bytes, size);

	} else {
		flash_add_t _add;
		_add.add0 = add.add0;
		_add.add1 = add.add1;
		_add.add2 = add.add2;
		uint16_t maxsize = 256 - add.add0;
		add_sum(&_add, maxsize);
		serialflash_write_size(add.add2, add.add1, add.add0, bytes, maxsize);
		serialflash_write_size(_add.add2, _add.add1, _add.add0, &bytes[maxsize], size - maxsize);

	}

}

uint8_t add_log_id(char * str_ident) {
	// find where_there is no id

	flash_add_t add;

	add.add2 = _this.initial_page;
	add.add1 = 0;
	add.add0 = 0;

	uint8_t found = 0;
	uint8_t index = 0;

	while (!found) {

		uint8_t bytes[10];
		serialflash_read(add.add2, add.add1, add.add0, bytes, 1);

		if (bytes[0] != 0xFF) {

			add_sum(&add, 8);
			index++;

		} else {
			found = 1;
		}

	}

	char buffer[10] = {};

	memcpy(buffer, str_ident, 7);

	write_bytes(add, (uint8_t *) buffer, 8);

	return index;

}

uint8_t find_id(char * id_str, uint8_t should_insert) {
	flash_add_t add;

	add.add2 = _this.initial_page;
	add.add1 = 0;
	add.add0 = 0;

	uint8_t found = 0;
	uint8_t index = 0;

	while (!found) {

		uint8_t bytes[10];
		serialflash_read(add.add2, add.add1, add.add0, bytes, 8);

		if (strncmp(id_str, (char *) bytes, 8) != 0) {

			add_sum(&add, 8);
			index++;

			if (add.add1 >= _this.initial_add1)
				return 0xFF;

			if (bytes[0] == 0xFF) {
				if (should_insert){
					char buffer[10] = {};
					memcpy(buffer, id_str, 7);
					write_bytes(add, (uint8_t *)buffer, 8);
					return index;
				} else {
					return 0xFF;
				}
			}

		} else {
			found = 1;
		}

	}

	return index;
}

void add_log(uint8_t * bytes, uint16_t size) {

	uint8_t buffer[128];

	memset(buffer, 0xFF, 128);

	buffer[0] = CREATE_HEADER_BYTE(size);

	//	kernel_message(MODE_DEBUG, "header will be: %u\n", buffer[0]);

	memcpy(&buffer[1], bytes, size);

	write_bytes(_this.next_log_add, buffer, size);

	add_sum(&_this.next_log_add, HEADER_GET_SIZE(buffer[0]));

	//	kernel_message(MODE_DEBUG,"%u %u %u \n", _this.next_log_add.add2,_this.next_log_add.add1,_this.next_log_add.add0);


}

void add_klog(uint8_t index, uint8_t sub_index, uint16_t time, uint8_t * bytes, uint16_t size){
	
	uint8_t buffer[128] = {};
	
	buffer[0] = ((index&0xf)<<4) + (sub_index&0xf);
	
//	uint16_t time = kTime_micros()/1000000L;
	
	memcpy(&buffer[1],&time,2);
	
	memcpy(&buffer[3], bytes, size);

	add_log(buffer, size+3);
	
}

static void cmd_insert_log(char **args, unsigned char n_args) {

	char * argument = args[0];
	uint8_t index = atoi(args[1]);
	uint8_t sub_index = atoi(args[2]);
	uint16_t i, count = 0;
	char buffer[100];

	//	kernel_message(MODE_DEBUG,"received - %u\n",strlen(argument));

	for (i = 0; i < strlen(argument); i += 4) {
		uint8_t temp[4] = {};
		uint8_t n = base64Decode((uint8_t*) & argument[i], temp);
		//		kernel_message(MODE_DEBUG,"n - %u\n",n);
		int j;
		for (j = 0; j < n; j++) {
			buffer[count++] = temp[j];
			//				kernel_message(MODE_DEBUG,"%u %lu\n",temp[j],csum);
		}

	}
	
	kernel_message(MODE_DEBUG, "Got %s %s %u %u\n", args[1], args[2], index, sub_index);

	//	kernel_message(MODE_DEBUG,"insert_log of %u\n",count);

	add_klog(index, sub_index, kTime_micros()/1000000L, (uint8_t *) buffer, count);
	
//	add_log(buffer, count);

	serial_printf("<OK %u, %u %u %u>\n", count, _this.next_log_add.add2, _this.next_log_add.add1, _this.next_log_add.add0);

}


static void cmd_insert_log_id(char **args, unsigned char n_args) {

	char * argument = args[0];

	uint8_t log_id = add_log_id(argument);

	serial_printf("<OK %u>\n", log_id);

}

static void cmd_find_log_id(char **args, unsigned char n_args) {

	char * argument = args[0];

	uint8_t log_id = find_id(argument, 0);

	serial_printf("<OK %u>\n", log_id);

}

static void cmd_read_log(char **args, unsigned char n_args) {

	//TODO

}

static void cmd_init_log(char **args, unsigned char n_args) {

	discover_logs();

	serial_printf("<OK %u %u %u %u>\n", _this.log_count, _this.next_log_add.add2, _this.next_log_add.add1, _this.next_log_add.add0);

}

static void cmd_erase_log(char **args, unsigned char n_args) {

	erase_logs();

	serial_printf("<OK>\n");

}

command_t comand_init_log = {// Flash
	.cmd_identifier = "init_log",
	.handler = cmd_init_log,
};

command_t comand_erase_log = {
	.cmd_identifier = "erase_logs",
	.handler = cmd_erase_log,
};

command_t comand_insert_log = {
	.cmd_identifier = "insert_log",
	.handler = cmd_insert_log,
};

command_t comand_insert_log_id = {
	.cmd_identifier = "insert_log_id",
	.handler = cmd_insert_log_id,
};
command_t comand_find_log_id = {
	.cmd_identifier = "find_log_id",
	.handler = cmd_find_log_id,
};

void init_comands() {
	uart_add_comand(&comand_init_log);
	uart_add_comand(&comand_erase_log);
	uart_add_comand(&comand_insert_log);
	uart_add_comand(&comand_insert_log_id);
	uart_add_comand(&comand_find_log_id);
}
