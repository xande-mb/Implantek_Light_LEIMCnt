#include "fj_interface.h"
#include "CTTKernel/kernel.h"
#include "CTTKernel/kernel/tasker.h"
#include <PortManager/portman.h>
#include <UartInterface/UartCommand.h>
#include <Application/Peripherals/spi.h>
#define FCY 16000000
#include <libpic30.h>
#include <p24Fxxxx.h>
#include <stdint.h>
#include <string.h>

static const PORTS_t ss_fj_port = PORT_F;
static const uint16_t ss_fj_bit = 7;

static uint8_t enabled_fans[5] = {1,1,1,1,1};

#define SSON portman_clearOutput(ss_fj_port,ss_fj_bit); __delay_us(100);

#define SSOFF __delay_us(20); portman_setOutput(ss_fj_port,ss_fj_bit);

typedef enum led_service_state_e {
	STATE_GET_LED,
	STATE_GET_PEDAL,
	STATE_CHANGE_VALUES,
	STATE_GET_FANS,
	STATE_VERIFY_ERRORS,

} led_service_state_t;

static struct {
	flags_pedal_t pedal_flags;
	led_vars_t led_vars;
	uint16_t led_pwm_to_put;
	uint8_t led_should_be_on;
	uint8_t fans_fb[6];
	led_service_state_t state;
	uint16_t number_of_runs;
	uint16_t number_of_crc_errors;
	uint16_t cam_should_be_on;
	uint8_t is_in_error;
	uint8_t is_inintialized;
	uint8_t disable_led;
	fj_error_flags_t error_flags;
} _this = {
	.led_should_be_on = 0,
	.led_pwm_to_put = 10
};

uint8_t fj_cam_is_on() {
	return _this.cam_should_be_on;
}

uint8_t fj_set_cam_status(uint8_t hab) {
	_this.cam_should_be_on = hab;
	return 0;
}

flags_pedal_t * fj_flags_pedal() {
	return &_this.pedal_flags;
}

uint8_t fj_is_led_on() {

	return _this.led_should_be_on;//_this.led_vars.ivinmon > 8;

}

uint8_t fj_is_initialized() {

	return _this.is_inintialized;

}

uint8_t fj_is_fiber_conn() {

	return _this.led_vars.fiber_switch;

}

void fj_set_led_on_off(uint8_t on_off) {
	_this.led_should_be_on = on_off;
}

void fj_set_led_pwm(uint8_t value) {
	_this.led_pwm_to_put = value;
}

uint8_t fj_is_in_error() {
	return _this.is_in_error;
}

fj_error_flags_t fj_get_error_flags() {
	return _this.error_flags;
}


void fj_set_fan_fb(uint8_t fan, uint8_t enable){
    enabled_fans[fan] = enable;
}

static void fj_service_task(void * param);

static void init_commands();

void fj_init(uint16_t disable_fan_error) {
    
	_this.is_inintialized = 0;
	_this.disable_led = disable_fan_error;
    
	SSOFF;
    
	portman_setAsDigital(ss_fj_port, ss_fj_bit);
	portman_setAsOutput(ss_fj_port, ss_fj_bit);
    
    init_commands();
    
	kernel_create_task(fj_service_task, NULL, "fj_task", 0);
}

uint8_t fj_get_version() {

	unsigned int length = 9;
	unsigned char motor_data_send[20] = {0x22, length, 0x01, 0x01, 0, 0, 0, 0, 0};
	unsigned char motor_data_receive[20];
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send

	unsigned char crc_ok = 0;

	SSON;

	crc_ok = spi_send_receive_crc(motor_data_send, motor_data_receive, length);

	SSOFF;

	//	int i;
	//	kernel_message(MODE_DEBUG, "CRC: %u \n",crc_ok);
	//	for (i = 0; i < length+1; i++){
	//		kernel_message(MODE_DEBUG, "Sent: %u, received: %u\n", motor_data_send[i], motor_data_receive[i]);
	//	}
	//	
	return crc_ok;


}

uint8_t fj_get_pedal_state(flags_pedal_t * flags_pedal) {

	unsigned int length = 7;
	unsigned char data_send[7] = {0x22, length, 0x02, 0x02, 0, 0, 0};
	unsigned char data_receive[7];
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send
	SSON;
	uint8_t crc_ok = spi_send_receive_crc(data_send, data_receive, length);
	SSOFF;
	//	memcpy(debug_vector, motor_data_receive, 7);
	if (crc_ok)
		memcpy(flags_pedal, &data_receive[4], 1);
	return crc_ok;
}

uint8_t fj_set_led(unsigned char duty_per100) {

	unsigned int length = 7;
	unsigned char data_send[7] = {0x22, length, 0x03, 0x03, duty_per100 / 2, 0, 0};
	unsigned char data_receive[7];
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send
	SSON;
	uint8_t crc_ok = spi_send_receive_crc(data_send, data_receive, length);
	SSOFF;
	//	memcpy(debug_vector, data_receive, 7);
	return crc_ok;
}

uint8_t fj_set_cam(unsigned char hab, 
                    unsigned char awb,
                    unsigned char wind,
                    unsigned char flex,
                    unsigned char ctemp
        ) {

	unsigned int length = 11;
	unsigned char data_send[11] = {0x22, length, 0x06, 0x06, hab,  awb, wind, flex, ctemp, 0, 0};
	unsigned char data_receive[11];
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send
	SSON;
	uint8_t crc_ok = spi_send_receive_crc(data_send, data_receive, length);
	SSOFF;
	//	memcpy(debug_vector, data_receive, 7);
	return crc_ok;
}

uint8_t fj_unset_led() {

	unsigned int length = 7;
	unsigned char data_send[7] = {0x22, length, 0x04, 0x04, 0, 0, 0};
	unsigned char data_receive[7];
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send
	SSON;
	uint8_t crc_ok = spi_send_receive_crc(data_send, data_receive, length);
	SSOFF;
	//	memcpy(debug_vector, data_receive, 7);
	return crc_ok;
}

uint8_t fj_get_led_vars(led_vars_t * led_feedback) {

	unsigned int length = 17;
	unsigned char data_send[17] = {0x22, length, 0x05, 0x05, 0, 0, 0};
	unsigned char data_receive[17];
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send

	SSON;

	uint8_t crc_ok = spi_send_receive_crc(data_send, data_receive, length);

	SSOFF;

	if (crc_ok)
		memcpy(led_feedback, &data_receive[4], 11);

	return crc_ok;
}

uint8_t fj_get_fans_fb(unsigned char * error_fan) {

	unsigned int length = 11;
	unsigned char data_send[11] = {0x22, length, 0x07, 0x07, 0, 0, 0, 0, 0, 0, 0};
	unsigned char data_receive[11];
	//Append_CRC((unsigned char*) &Spi1.sdo, (Spi1.length - 2)); //calcula crc 16bits e armazena nas 2 posições finais de data_send

	SSON;
	uint8_t crc_ok = spi_send_receive_crc(data_send, data_receive, length);
	SSOFF;

	if (crc_ok)
		memcpy(error_fan, &data_receive[4], 5);

	return crc_ok;
}


#undef DEBUG_CONTEXT
#define DEBUG_CONTEXT "FJ"

static void fj_service_task(void * param) {

	if (kTask_isInitialization(NULL)) {
		_this.state = STATE_GET_LED;
		_this.number_of_runs = 0;
	}


	switch (_this.state) {

		case STATE_GET_LED:
		{
			_this.number_of_runs++;

			if (_this.disable_led) {
				_this.state++;
				sTask_DelayUntil(NULL, 0);
				return;
			}

			uint8_t crc_ok = fj_get_led_vars(&_this.led_vars);
//									kernel_message(MODE_PRODUCTION,"Grabed led - Is: %u %u, NTCs: %u %u, f: %u, sw: %u, pwm: %u\n", 
//											_this.led_vars.ismon,
//											_this.led_vars.ivinmon,
//											_this.led_vars.ntc_led,
//											_this.led_vars.ntc_1,
//											_this.led_vars.fault,
//											_this.led_vars.fiber_switch,
//                                            _this.led_pwm_to_put)
			_this.number_of_crc_errors += !crc_ok;
			_this.state++;
			sTask_DelayUntil(NULL, 50 * tMILLIS);
			return;
		}
			break;
		case STATE_GET_PEDAL:
		{
			uint8_t crc_ok = fj_get_pedal_state(&_this.pedal_flags);
			_this.state++;
			_this.number_of_crc_errors += !crc_ok;
			sTask_DelayUntil(NULL, 30 * tMILLIS);
			return;
		}
			break;
		case STATE_CHANGE_VALUES:
		{
			uint8_t crc_ok = 0;
			if (!_this.disable_led) {
				if (_this.led_should_be_on && !_this.led_vars.fiber_switch) _this.led_should_be_on = 0;
				if (_this.led_should_be_on && !_this.is_in_error && _this.led_vars.fiber_switch) {
					crc_ok = fj_set_led(_this.led_pwm_to_put);
				} else {
					crc_ok = fj_unset_led();
				}
			}
			fj_set_cam(_this.cam_should_be_on, 0xff, 0xff, 0xff, 0xff);
			_this.number_of_crc_errors += !crc_ok;
			_this.state++;
			sTask_DelayUntil(NULL, 30 * tMILLIS);
			return;
		}
			break;
		case STATE_GET_FANS:
		{
			if (_this.disable_led) {
				_this.state++;
				sTask_DelayUntil(NULL, 0);
				return;
			}
			uint8_t crc_ok = fj_get_fans_fb(_this.fans_fb);
			_this.number_of_crc_errors += !crc_ok;
			_this.state++;
			sTask_DelayUntil(NULL, 30 * tMILLIS);
			return;
		}
			break;
		case STATE_VERIFY_ERRORS:
		{
			uint8_t error = 0;

			if (_this.number_of_crc_errors == 0)_this.is_inintialized = 1;

			if (!_this.disable_led) {

				if (_this.number_of_crc_errors == 0) {

					// Erro de fans
					int i = 0;
					for (i = 0; i < 5; i++) {
						error += _this.fans_fb[i]&enabled_fans[i];
						if (_this.fans_fb[i] && enabled_fans[i]) {
							kernel_message(MODE_PRE_PRODUCTION, "Fan %u entered in error condition!\n", i);

						}
					}

					if (error) {
						_this.error_flags.error_fan = 1;
					} else {
						_this.error_flags.error_fan = 0;
					}

					// Erro de corrente
					if (_this.led_vars.ivinmon > 1400) {
						error++;
						kernel_message(MODE_PRE_PRODUCTION, "Led current (%u) too high!\n", _this.led_vars.ivinmon);
						_this.error_flags.error_current_led = 1;
					} else {
						_this.error_flags.error_current_led = 0;
					}

					if (_this.led_vars.ivinmon > 500 && !_this.led_should_be_on) {
						error++;
						kernel_message(MODE_PRE_PRODUCTION, "Led current (%u) too high!\n", _this.led_vars.ivinmon);
						_this.error_flags.error_current_off_led = 1;
					} else {
						_this.error_flags.error_current_off_led = 0;
					}
                    
                    static unsigned int ctplt = 0;
                    if ((ctplt++)%4 == 0)
                        kernel_send_to_buff(0, "%u %u %llu\n", _this.led_vars.ntc_led, _this.led_vars.ntc_1, kTime_micros());
					// Erro de temperatura
					if (_this.led_vars.ntc_led > 3250 || _this.led_vars.ntc_1 > 3600 || _this.led_vars.ntc_led < 1000 || _this.led_vars.ntc_1 < 1000) {
						error++;
						kernel_message(MODE_PRE_PRODUCTION, "Temperature too high (%u %u)!\n", _this.led_vars.ntc_led, _this.led_vars.ntc_1);
						_this.error_flags.error_temperature_led = 1;
					} else {
						_this.error_flags.error_temperature_led = 0;
					}

				}
			}

			//Erro com.
			static uint8_t crc_error = 0;
			_this.error_flags.error_com = 0;

			if ((_this.number_of_runs % 10) == 0) {
				if (_this.number_of_crc_errors < 10) {
					crc_error = 0;
					_this.error_flags.error_com = 0;
				}
				if (_this.number_of_crc_errors > 20) {
					crc_error = 1;
					kernel_message(MODE_PRE_PRODUCTION, "Too much crc errors!\n");
					_this.error_flags.error_com = 1;
				}
				_this.number_of_crc_errors = 0;
			}


			error += crc_error;

			_this.is_in_error = error > 0;

			if (_this.is_in_error) {
				_this.led_should_be_on = 0;
				fj_unset_led();
			}

			_this.state = STATE_GET_LED;
			sTask_DelayUntil(NULL, 0 * tMILLIS);
			return;
		}
			break;
		default:
			break;
	}



}


static void cmd_set_led_data(char **args, unsigned char n_args) {

    if (n_args == 2) {

        uint16_t should_be_on = atoi(args[0]);
        uint16_t pwm = atoi(args[1]);
        fj_set_led_pwm(pwm);
        fj_set_led_on_off(should_be_on);
        serial_printf("<OK>\n");
        return;
    }

    serial_printf("<ERR>\n");

}

static void cmd_get_led_data(char **args, unsigned char n_args) {

    led_vars_t led_feedback;
    fj_get_led_vars(&led_feedback);

    serial_printf("<OK %u %u %u %u %u %u>\n",
            led_feedback.fiber_switch,
            led_feedback.ismon,
            led_feedback.ivinmon,
            led_feedback.ntc_1,
            led_feedback.ntc_led,
            _this.led_should_be_on ? _this.led_pwm_to_put : 0
        );
    
}

static void cmd_get_pedal_data(char **args, unsigned char n_args) {


    serial_printf("<OK %u %u %u %u>\n",
            _this.pedal_flags.pedal_direito,
            _this.pedal_flags.pedal_central,
            _this.pedal_flags.pedal_esquerdo,
            _this.pedal_flags.pedal_botao
        );
    
}

static command_t comand_set_led_data = {
    .cmd_identifier = "set_led_data",
    .handler = cmd_set_led_data,
};

static command_t comand_get_led_data = {
    .cmd_identifier = "get_led_data",
    .handler = cmd_get_led_data,
};

static command_t comand_get_pedal_data = {
    .cmd_identifier = "get_pedal_data",
    .handler = cmd_get_pedal_data,
};

static void init_commands() {
    uart_add_comand(&comand_set_led_data);
    uart_add_comand(&comand_get_led_data);
    uart_add_comand(&comand_get_pedal_data);
}
