#include "model_manager.h"
#include <Memory/eeprom_i2c.h>
#include <UartInterface/UartCommand.h>
#include <string.h>
#include <stdlib.h>

static model_data_t readed_data;
static model_data_t user_data;
static eeprom_data_unit_t * memory_unit;

//static model_eeprom_data_t

void init_commands();

void modelManager_init(){
	
	model_data_t default_data = {.cam_type = CAMERA_HD, .equip_sn = 0, .camera_sn = 0, .equip_model = MODEL_TRIPLET};
	
	memory_unit = eeprom_init_data(sizeof (model_data_t), (uint8_t *) & default_data, 0);
	
	modelManager_read();
	
	init_commands();
	
}

void modelManager_read(){
	
	eeprom_read_data(memory_unit, (uint8_t *) & readed_data);
	
}

void modelManagerWrite(){
	
	eeprom_write_data(memory_unit, (uint8_t *) & user_data);
	
	modelManager_read();

}

model_data_t * modelManager_getDataReaded(){
	
	memcpy(&user_data,&readed_data,sizeof(model_data_t));
	
	return &user_data;
	
}


static void cmd_set_model_vars(char **args, unsigned char n_args) {

	model_data_t * data = modelManager_getDataReaded();
	
	if (n_args == 4){
		
		if (args[0][0] != 'K'){
			data->equip_model = atoi(args[0]);
		}
		
		if (args[1][0] != 'K'){
			data->cam_type = atoi(args[1]);
		}
		
		if (args[2][0] != 'K'){
			data->equip_sn = atoi(args[2]);
		}
		
		if (args[3][0] != 'K'){
			data->camera_sn = atoi(args[3]);
		}
		
		modelManagerWrite();
		
		serial_printf("<OK>\n");
		
	} else if (n_args == 0){
		
		serial_printf("<OK %u %u %u %u>\n", data->equip_model, data->cam_type, data->equip_sn, data->camera_sn);
		
	} else {
		
		serial_printf("<ERR>\n");
		
	}
	
	
}

static command_t comand_model_data = {// Flash
	.cmd_identifier = "model_data",
	.handler = cmd_set_model_vars,
};

void init_commands(){
	uart_add_comand(&comand_model_data);
}

