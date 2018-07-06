#ifndef FJ_INTERFACE_H
#define FJ_INTERFACE_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>

    	typedef struct {
		unsigned pedal_direito : 1;
		unsigned pedal_esquerdo : 1;
		unsigned pedal_central : 1;
		unsigned pedal_botao : 1;
		unsigned pedal_detecta : 1;
		unsigned unused : 3;
	} flags_pedal_t;

	typedef struct {
		unsigned int ntc_led;
		unsigned int ntc_1;
		unsigned int fiber_switch;
		unsigned int ismon;
		unsigned int ivinmon;
		unsigned char fault;
	} led_vars_t;
	
	typedef struct {
	    unsigned error_fan : 1; 
	    unsigned error_current_led : 1; 
	    unsigned error_current_off_led : 1; 
	    unsigned error_temperature_led : 1; 
	    unsigned error_temperature_driver : 1; 
	    unsigned error_com : 1; 
	} fj_error_flags_t; 
	
	void fj_init(); 

	uint8_t fj_is_led_on(); 

	void fj_set_led_on_off(uint8_t on_off); 

	void fj_set_led_pwm(uint8_t value); 
	
	uint8_t fj_is_fiber_conn(); 
	
uint8_t fj_set_cam(unsigned char hab, 
                    unsigned char awb,
                    unsigned char wind,
                    unsigned char flex,
                    unsigned char ctemp
        );

	uint8_t fj_get_version(); 
	
	uint8_t fj_cam_is_on(); 
	
	uint8_t fj_set_cam_status(uint8_t hab); 
	
	flags_pedal_t * fj_flags_pedal(); 
	
	uint8_t fj_is_in_error(); 
	
	fj_error_flags_t fj_get_error_flags();
	
	uint8_t fj_is_initialized();
    
    void fj_set_fan_fb(uint8_t fan, uint8_t enable);
	
#ifdef	__cplusplus
}
#endif

#endif // FJ_INTERFACE_H
