#include "ep_interface.h"
#include "CTTKernel/kernel.h"
#include "UserInterface/Buzzer.h"
#include "rtcc_interface.h"
#include <PortManager/portman.h>
#include <Application/Peripherals/spi.h>
#define FCY 16000000
#include <libpic30.h>
#include <p24Fxxxx.h>
#include <string.h>


#define MAX_LENGTH_FILE_NAME 30

#define MAX_RECORDING_TIME (20*60)


static PORTS_t ss_z3_port = PORT_G;
static uint16_t ss_z3_bit = 3;

#define SSON portman_clearOutput(ss_z3_port,ss_z3_bit); __delay_us(100);

#define SSOFF __delay_us(20); portman_setOutput(ss_z3_port,ss_z3_bit);

void ep_service_task(void * params);

typedef enum ep_task_state_e {
	STATE_GET_FLAGS,
	STATE_VERIFY_STORAGE_MOUNT,
	STATE_VERIFY_STORAGE_SIZE,
	STATE_START_RECORDING,
	STATE_KEEP_VERIFYING_RECORDING,
	STATE_SAFE_RESTART,
	STATE_STOP_RECORDING,
	STATE_ERROR

} ep_task_state_t;

typedef struct {
	ep_task_state_t state;
	board_z3_status_t board_status;
	kTask_t * ep_service_task;
	uint16_t number_of_crc_errors;
	uint16_t recording_was_safe_stoped;
	uint16_t continuation_number;
	char last_bufer_name[MAX_LENGTH_FILE_NAME];
} ep_service_vars_t;

static ep_service_vars_t _this = {
	.ep_service_task = NULL,
	.recording_was_safe_stoped = 0,
	.continuation_number = 0,

};

void ep_init() {

	SSOFF;

	portman_setAsDigital(ss_z3_port, ss_z3_bit);
	portman_setAsOutput(ss_z3_port, ss_z3_bit);

	ep_start_task();

}

void ep_start_task() {
	if (_this.ep_service_task == NULL)
		_this.ep_service_task = kernel_create_task(ep_service_task, NULL, "ep_task", 0);
}

void ep_stop_task() {
	if (_this.ep_service_task != NULL) {
		sTask_delete(_this.ep_service_task);
		_this.ep_service_task = NULL;
	}

}

uint8_t ep_get_version() {

	unsigned int length = 9;
	unsigned char motor_data_send[20] = {0x22, length, 0x01, 0x01, 0, 0, 0, 0, 0};
	unsigned char motor_data_receive[20];
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send

	unsigned char crc_ok = 0;

	SSON;

	crc_ok = spi_send_receive_crc(motor_data_send, motor_data_receive, length);

	SSOFF;

	int i;
	kernel_message(MODE_DEBUG, "CRC: %u \n", crc_ok);
	for (i = 0; i < length + 1; i++) {
		kernel_message(MODE_DEBUG, "Sent: %u, received: %u\n", motor_data_send[i], motor_data_receive[i]);
	}

	return crc_ok;


}

board_z3_status_t * ep_z3_flags() {
	return &(_this.board_status);
}

uint8_t ep_get_flags(board_z3_status_t * board_z3_status) {

	unsigned int length = sizeof (board_z3_status_t) + 6;
	unsigned char motor_data_send[30] = {0x22, length, 0x02, 0x02, 0, 0, 0, 0};
	unsigned char motor_data_receive[30];
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send
	SSON;

	uint8_t crc_ok = spi_send_receive_crc(motor_data_send, motor_data_receive, length);

	SSOFF;

	//	int i;
	//	kernel_message(MODE_DEBUG, "CRC: %u, len: %u \n", crc_ok, sizeof(board_z3_status_t));
	//	for (i = 0; i < length + 1; i++) {
	//		kernel_message(MODE_DEBUG, "Sent: %u, received: %u\n", motor_data_send[i], motor_data_receive[i]);
	//	}

	if (crc_ok)
		memcpy(board_z3_status, &motor_data_receive[4], sizeof (board_z3_status_t));
	return crc_ok;
}


uint8_t ep_was_safe_stoped(){
	return _this.recording_was_safe_stoped;
}

uint8_t ep_start(char * fname, uint16_t is_continuation) {

	//	timestr = 

	unsigned int length = 50;
	unsigned char dataSend[50] = {0x22, length, 0x03, 0x03, 0};
	unsigned char dataReceive[50];


	if (is_continuation) {
		_this.continuation_number++;
		char buffer[10] = {};
		sprintf(buffer, "_%u", _this.continuation_number);
		strncpy((char*) &dataSend[6], _this.last_bufer_name, MAX_LENGTH_FILE_NAME);
		strcat((char*) &dataSend[6], buffer);
		kernel_message(MODE_DEBUG, "Continuation %s\n", (char*) &dataSend[6]);
	} else {

		memset(_this.last_bufer_name, 0, MAX_LENGTH_FILE_NAME);
		strncpy(_this.last_bufer_name, fname, MAX_LENGTH_FILE_NAME);
		strncpy((char*) &dataSend[6], fname, MAX_LENGTH_FILE_NAME);
		_this.continuation_number = 0;
	}
	strcat((char*) &dataSend[6], ".mp4");

	kernel_message(MODE_DEBUG, "fname: %s\n", (char*) &dataSend[6]);

	//	unsigned int length = 6;
	//	unsigned char motor_data_send[9] = {0x22, length, 0x03, 0x03, 0, 0};
	//	unsigned char motor_data_receive[9];
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send

	uint8_t crc_ok = 0;
	uint8_t tries = 5;
	while (!crc_ok && tries--) {
		SSON;
		crc_ok = spi_send_receive_crc(dataSend, dataReceive, length);
		SSOFF;
	}

	//	memcpy(debug_vector, motor_data_receive, 9);
	return crc_ok;
}

uint8_t ep_stop() {
	unsigned int length = 6;
	unsigned char motor_data_send[9] = {0x22, length, 0x04, 0x04, 0, 0};
	unsigned char motor_data_receive[9];
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send

	uint8_t crc_ok = 0;
	uint8_t tries = 5;
	while (!crc_ok && tries--) {
		SSON
		crc_ok = spi_send_receive_crc(motor_data_send, motor_data_receive, length);
		SSOFF;
	}
	//	memcpy(debug_vector, motor_data_receive, 9);
	return crc_ok;
}

uint8_t ep_setSystemTime(char *sysTime) {

	unsigned int length = 32;
	unsigned char dataSend[32] = {0x22, length, 0x11, 0x11, 0};
	unsigned char dataReceive[32];

	strcpy((char*) &dataSend[6], sysTime);

	SSON;

	uint8_t crcOk = spi_send_receive_crc(dataSend,
			dataReceive, length);

	SSOFF;

	return crcOk;
}


//! Monta o dispositivo de armazenamento na placa de gravação de vídeo Z3.

/*!
	\return '1' caso a comunicação tenha sido bem-sucedida; '0' caso contrário
 */
uint8_t ep_mountStorageDevice() {

	unsigned int length = 6;
	unsigned char dataSend[9] = {0x22, length, 0x05, 0x05, 0, 0,};
	unsigned char dataReceive[9];

	SSON;

	uint8_t crcOk = spi_send_receive_crc(dataSend, dataReceive, length);

	SSOFF;

	return crcOk;
}


//! Desmonta o dispositivo de armazenamento na placa de gravação de vídeo Z3.

/*!
	\return '1' caso a comunicação tenha sido bem-sucedida; '0' caso contrário
 */
uint8_t ep_unmountStorageDevice() {

	unsigned int length = 6;
	unsigned char dataSend[9] = {0x22, length, 0x09, 0x09, 0, 0};
	unsigned char dataReceive[9];


	SSON;

	uint8_t crcOk = spi_send_receive_crc(dataSend, dataReceive, length);

	SSOFF;

	return crcOk;
}

uint8_t ep_getStorageSpace(z3Storage_t *spaceInfo) {


	unsigned int length = 16;
	unsigned char dataSend[17] = {0x22, length, 0x06, 0x06, 0, 0, 0, 0, 0, 0,
		0, 0};
	unsigned char dataReceive[17];

	SSON;

	uint8_t crcOk = spi_send_receive_crc(dataSend, dataReceive, length);

	SSOFF;

	memcpy(spaceInfo, &dataReceive[4], sizeof (z3Storage_t));


	return crcOk;
}

uint8_t ep_recording_status() {
	return _this.board_status.record_comand_sent ?
			(_this.board_status.f_gravando || _this.board_status.f_pausado ? RECORDING_STARTED : RECORDING_INITIALIZED) :
			RECORDING_STOPED;
}

uint8_t ep_is_media_mounted() {
	return _this.board_status.f_flash_ok_mounted;
}

uint8_t ep_is_initialized() {
	return _this.board_status.f_zdvr_ok;
}

uint16_t ep_recording_time() {
	return (uint16_t) _this.board_status.recordingTime;
}

//! Inicia a gravação de vídeo no ZDVR.

/*!
	\return '1' caso a comunicação tenha sido bem-sucedida; '0' caso contrário
 */
uint8_t ep_pause() {


	unsigned int length = 6;
	unsigned char dataSend[9] = {0x22, length, 0x07, 0x07, 0, 0};
	unsigned char dataReceive[9];

	SSON;

	uint8_t crcOk = spi_send_receive_crc(dataSend, dataReceive, length);

	SSOFF;

	return crcOk;
}


//! Inicia a gravação de vídeo no ZDVR.

/*!
	\return '1' caso a comunicação tenha sido bem-sucedida; '0' caso contrário
 */
uint8_t ep_resume() {


	unsigned int length = 6;
	unsigned char dataSend[9] = {0x22, length, 0x08, 0x08, 0, 0};
	unsigned char dataReceive[9];

	SSON;

	uint8_t crcOk = spi_send_receive_crc(dataSend, dataReceive, length);

	SSOFF;

	return crcOk;
}

void ep_service_task(void * params) {

	if (kTask_isInitialization(NULL)) {
		_this.recording_was_safe_stoped = 0;
	}

	switch (_this.state) {
		case STATE_GET_FLAGS:
		{
            uint8_t old_ok = _this.board_status.f_zdvr_ok;
			uint8_t crc_ok = ep_get_flags(&_this.board_status);
			static uint16_t counter = 0;
			counter++;
			if (counter % 10 == 0) {
				//				kernel_message(MODE_DEBUG, "Z %u F %u G %u P %u RS %u FR %u FE %u UE %u %lu\n",
				//						_this.board_status.f_zdvr_ok,
				//						_this.board_status.f_flash_ok_mounted,
				//						_this.board_status.f_gravando,
				//						_this.board_status.f_pausado,
				//						_this.board_status.f_record_comand_sent,
				//						_this.board_status.f_flash_con_remove,
				//						_this.board_status.flashError,
				//						_this.board_status.recordingTerminated,
				//						_this.board_status.recordingTime
				//						);
			}
            if (old_ok == 0 && _this.board_status.f_zdvr_ok){
                RTCC_t date = rtccRead();
                    char buffer[128];
    
                sprintf(buffer, "%04u-%02u-%02u %02u:%02u:%02u",
                        date.YEAR+2000,
                        date.MONTH,
                        date.DAY,
                        date.HOUR,
                        date.MINUTE,
                        date.SECOND
                        );
                
                ep_setSystemTime(buffer);
                
            }
            
			_this.number_of_crc_errors += crc_ok;
			_this.state = STATE_KEEP_VERIFYING_RECORDING;
			sTask_DelayUntil(NULL, 100 * tMILLIS);
		}
			return;
		case STATE_KEEP_VERIFYING_RECORDING:
		{
			if (_this.board_status.f_gravando) {
				if (ep_recording_time() > MAX_RECORDING_TIME && !_this.recording_was_safe_stoped) {
					kernel_message(MODE_DEBUG, "SAFE STOP\n")
					_this.recording_was_safe_stoped = 1;
					sys_buzz(3,7);
					sys_buzz(3,7);
					sys_buzz(3,7);
					ep_stop();
				}
			}
			
			if (_this.recording_was_safe_stoped && ep_recording_status() == RECORDING_STOPED && _this.board_status.recordingTerminated && !_this.board_status.f_record_comand_sent) {

				static counter_should_restart = 0;
				
				if (counter_should_restart++ > 5) {
					kernel_message(MODE_DEBUG, "SAFE restarting\n")
					_this.recording_was_safe_stoped = 0;
                    
                    z3Storage_t space;
                    
                    ep_getStorageSpace(&space);
                    
                    if ((space.size - space.used) > 400000){

                        ep_start("this_is_my_test", 1);
                    
                    }

					counter_should_restart = 0;
				}

			}
			_this.state = STATE_GET_FLAGS;
			sTask_DelayUntil(NULL, 0);
		}
			return;
		case STATE_SAFE_RESTART:
		{

		}

			return;
		default:
			return;

	}

}