#include "dmd_interface.h"
#include "CTTKernel/kernel.h"
#include <PortManager/portman.h>
#include <Application/Peripherals/spi.h>
#include <UartInterface/UartCommand.h>
#define FCY 16000000
#include <libpic30.h>
#include <p24Fxxxx.h>
#include <string.h> //null

static PORTS_t ss_dmd_port = PORT_G;
static uint16_t ss_dmd_bit = 2;

#define SSON portman_clearOutput(ss_dmd_port,ss_dmd_bit); __delay_us(100);

#define SSOFF __delay_us(20); portman_setOutput(ss_dmd_port,ss_dmd_bit);

void dmd_task(void * params);

static void init_commands();

enum DMD_STATE_e {
	DMD_STATE_GET_FLAGS,
	DMD_STATE_GET_INFO,
	DMD_STATE_VERIFY_ERROR,

};

#define ENTRANCES_TO_CALCULATE_CRC_ERROR		20
#define MAX_CRC_FAILS							10

typedef struct {
	enum DMD_STATE_e state;
	uint16_t crc_errors;
    uint16_t previous_crc_ok;
	uint16_t task_entrances;
    uint16_t current_vel;
    uint16_t current_current;
	motor_flags_t motor_flags;
	board_flags_t board_flags;
	motor_flags_t motor_flags_aux;
	board_flags_t board_flags_aux;
	mem_data_exchange_t motor_info;
} dmd_task_vars_t;



static dmd_task_vars_t _this = {
	
};

void dmd_init() {

	SSOFF;

	portman_setAsDigital(ss_dmd_port, ss_dmd_bit);
	portman_setAsOutput(ss_dmd_port, ss_dmd_bit);
	
	kernel_create_task(dmd_task, NULL, "dmd_task", 0);
	
	 init_commands();

}


uint8_t dmd_hab_motor(uint16_t velocity, uint8_t flag, uint8_t direction){
	
	unsigned int length = 10;
	unsigned char motor_data_send[10] = {0x21, length, 0x10, 0x10, flag, velocity%256, velocity>>8, direction, 0, 0};
	unsigned char motor_data_receive[10];	
	unsigned char crc_ok = 0;

	SSON;

	crc_ok = spi_send_receive_crc(motor_data_send, motor_data_receive, length);

	SSOFF;
	
	return crc_ok;
}

uint8_t dmd_write_motor(uint16_t motor_id){
	
	unsigned int length = 8;
	unsigned char motor_data_send[8] = {0x21, length, 0x11, 0x11, motor_id, 0,  0, 0};
	unsigned char motor_data_receive[8];	
	unsigned char crc_ok = 0;

	SSON;
	
	crc_ok = spi_send_receive_crc(motor_data_send, motor_data_receive, length);
	
	SSOFF;
	
	return crc_ok;
}


static uint8_t dmd_get_flags(motor_flags_t * motor_flags, board_flags_t * board_flags, uint16_t * velocity) {

	unsigned int length = 14;
	unsigned char motor_data_send[14] = {0x21, length, 0x0E, 0x0E, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char motor_data_receive[14];
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send

	unsigned char crc_ok = 0;

	SSON;

	crc_ok = spi_send_receive_crc(motor_data_send, motor_data_receive, length);

	SSOFF;
	
//	memcpy()

	int i;
//	kernel_message(MODE_PRODUCTION, "CRC: %u \n", crc_ok);
//	for (i = 0; i < length + 1; i++) {
//		kernel_message(MODE_PRODUCTION, "Sent: %u, received: %u\n", motor_data_send[i], motor_data_receive[i]);
//	}
    
	
	if (crc_ok){
		memcpy(motor_flags,&motor_data_receive[6],2);
		memcpy(board_flags,&motor_data_receive[4],2);
		memcpy(&_this.current_vel,&motor_data_receive[8],2);
		memcpy(&_this.current_current,&motor_data_receive[10],2);
	}
    
    

    static unsigned int ct_teste = 0;
    
    if ( ((ct_teste++) % 10) == 0){
        
//        kernel_message(MODE_PRODUCTION, "vel: %u, cur: %u\n", vel, cur);
        
//        kernel_message(MODE_PRODUCTION, "Got Flags: M: %u %u %u H:%u R:%u RUN:%u DIR %u REV %u, %u\n", 
//
//                board_flags->selected_motor, 
//                board_flags->motor1_con,
//                board_flags->motor2_con,
//                motor_flags->motor_habilita, 
//                motor_flags->motor_recognized, 
//                motor_flags->motor_runing,
//                motor_flags->dir_active,
//                motor_flags->rev_active,
//                vel
//                );
	}
	return crc_ok;


}


static uint8_t dmd_get_mem_info(mem_data_exchange_t * motor_mem) {

	unsigned int length = 6 + sizeof (mem_data_exchange_t);
	unsigned char motor_data_send[length];
	unsigned char motor_data_receive[length];
	
	memcpy(motor_data_send, ((uint8_t [4]) {0x21, length, 0x0F, 0x0F}), 4);
	
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send

	unsigned char crc_ok = 0;

	SSON;

	crc_ok = spi_send_receive_crc(motor_data_send, motor_data_receive, length);

	SSOFF;

//	int i;
//	kernel_message(MODE_DEBUG, "CRC: %u \n", crc_ok);
//	for (i = 0; i < length + 1; i++) {
//		kernel_message(MODE_DEBUG, "Sent: %u, received: %u\n", motor_data_send[i], motor_data_receive[i]);
//	}
	
//	kernel_message(MODE_DEBUG, "Got Info: M: %u %u %u\n", 
//			motor_mem->manufacturer, 
//			motor_mem->type,
//			motor_mem->id
//	);
	
	
	if (crc_ok){
		memcpy(motor_mem,&motor_data_receive[4],sizeof(mem_data_exchange_t));
	}

	return crc_ok;


}


static uint8_t dmd_change_motor(uint8_t motor_to_change){
	
	
	unsigned int length = 6 + 1;
	unsigned char motor_data_send[length];
	unsigned char motor_data_receive[length];	
		
	memcpy(motor_data_send, ((uint8_t [4]) {0x21, length, CMD_ALTERA_MOTOR, CMD_ALTERA_MOTOR}), 4);
	
	motor_data_send[4] = motor_to_change;
	
	unsigned char crc_ok = 0;
	
	SSON;

	crc_ok = spi_send_receive_crc(motor_data_send, motor_data_receive, length);

	SSOFF;
	
	return crc_ok;
	
}


motor_flags_t * dmd_motor_flags(){
	return &_this.motor_flags;
}

board_flags_t * dmd_board_flags(){
	return &_this.board_flags;
}

mem_data_exchange_t * dmd_motor_info(){
	
	return &_this.motor_info;
	
}


uint8_t dmd_alternate_motor(){
    
//    dmd_hab_motor(0, 0, 0);
	
	board_flags_t * flags = dmd_board_flags();
	
	uint8_t cur_motor = flags->selected_motor;
	
	if (cur_motor == 1){
		if (!flags->motor2_con)
			return 0;
	}
	
	if (cur_motor == 2){
		if (!flags->motor1_con)
			return 0;
	}
	
	if (cur_motor == 0 || cur_motor > 2){
		return 0;
	}
	
	uint8_t alt = cur_motor == 1 ? 2: 1;
	
	uint8_t crc = dmd_change_motor(alt);
	
	return crc;
	
	
}


void dmd_task(void * params) {

	if (kTask_isInitialization(NULL)) {
		_this.crc_errors = 0;
		_this.state = DMD_STATE_GET_FLAGS;
		_this.task_entrances = 0;
	}
	
	_this.task_entrances +=1;

	switch (_this.state) {
		case DMD_STATE_GET_FLAGS:
		{
//			motor_flags_t old_mflags = _this.motor_flags;
			uint8_t crc_ok = dmd_get_flags(&_this.motor_flags_aux,&_this.board_flags_aux, _this.current_vel);
			_this.previous_crc_ok = crc_ok;

			_this.state = DMD_STATE_GET_INFO;
			_this.crc_errors += !crc_ok;

			break;
		}
		case DMD_STATE_GET_INFO:
		{
            if (_this.previous_crc_ok){
                memcpy(&_this.motor_flags,&_this.motor_flags_aux, sizeof(motor_flags_t));
                memcpy(&_this.board_flags,&_this.board_flags_aux, sizeof(board_flags_t));
            }
			if (_this.previous_crc_ok && _this.motor_flags.motor_recognized){
				_this.state = DMD_STATE_VERIFY_ERROR;
				_this.crc_errors += !dmd_get_mem_info(&_this.motor_info);
			} else {
				_this.state = DMD_STATE_VERIFY_ERROR;
			}
			
			break;
		}
		case DMD_STATE_VERIFY_ERROR:
		{
			
			if (_this.task_entrances%ENTRANCES_TO_CALCULATE_CRC_ERROR == 0){
				if (_this.crc_errors > MAX_CRC_FAILS){
					// Estado de ERRO!
					kernel_message(MODE_PRODUCTION,"TOO MUCH CRC ERRORS!\n")
				} else {
					
				}
				_this.crc_errors = 0;
			}
			_this.state = DMD_STATE_GET_FLAGS;
			break;
		}

	}

	sTask_DelayUntil(NULL,100*tMILLIS);
	
}


static void cmd_write_motor(char **args, unsigned char n_args) {

	if (n_args == 1){
		unsigned char id = atoi(args[0]);
		dmd_write_motor(id);
		serial_printf("<OK %u>\n", id);
		
	} else {
		
		serial_printf("<ERR>\n");
		
	}
	
	
}

static void cmd_get_motor_data(char **args, unsigned char n_args) {


    serial_printf("<OK %u %u %u %u %u %u %u %u>\n",
            _this.current_vel,
            _this.current_current,
            _this.board_flags.selected_motor,
            _this.motor_flags.motor_runing,
            _this.motor_flags.rev_active,
            _this.motor_flags.dir_active,
            _this.motor_flags.motor_recognized,
            _this.motor_info.id
        );
    
}

static command_t comand_get_motor_data = {
    .cmd_identifier = "get_motor_data",
    .handler = cmd_get_motor_data,
};

static command_t comand_write_motor = {// Flash
	.cmd_identifier = "dmd_write_motor",
	.handler = cmd_write_motor,
};

static void init_commands(){
	uart_add_comand(&comand_write_motor);
	uart_add_comand(&comand_get_motor_data);
}

