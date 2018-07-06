#ifndef ACTIVITYMAIN_H
#define ACTIVITYMAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <ActivityManager/ActivityMan.h>
#include <Application/interfaces/ep_interface.h>
#include <Application/interfaces/fj_interface.h>
#include <Application/interfaces/dmd_interface.h>

    typedef enum STATE_DEVICE_e {
	DEV_STATE_OFF,
	DEV_STATE_ON,
	DEV_STATE_DISABLED,
	DEV_STATE_PARCIAL,
    } state_of_device_t;

    typedef enum MOTOR_DIRECTION_e {
	DIRECTION_NONE,
	DIRECTION_FORW,
	DIRECTION_BACK,
	DIRECTION_OSC,
	DIRECTION_WINDOW,
    } direction_of_motor_t;

    typedef struct {
	touchable_t* bt_select_motor;
	touchable_t* bt_select_light;
	touchable_t* bt_select_cam;
	touchable_t* bt_select_rec;
	touchable_t* bt_menu;
	touchable_t* bt_voltar_sub;
	//	scheduler_context_t my_scheduler;
	unsigned char active_motor;
	unsigned int velocity;
	unsigned char pwm_perc;
    unsigned char safe_disable_til_next;
	unsigned char is_error_temp;
	unsigned char is_error_vent;
	char* str_error;
	unsigned char send_rec;
	unsigned char send_stop;
	state_of_device_t state_led;
	state_of_device_t state_camera;
	state_of_device_t state_recorder;
    } DraftMain_localc_variables_t;

    extern ActivityContext_t ActivityMain;

    extern DraftMain_localc_variables_t DraftMain_localc_variables;

#define LIGHTBLUEBG			RGB565CONVERT(155, 194, 207)
#define LIGHTBLUEDELIMS			RGB565CONVERT(180, 212, 222)
#define BLUEBTBAR			RGB565CONVERT(4, 98, 122)
#define CYANBTBAR			RGB565CONVERT(24, 241, 255)


    void select_light_source();

    void select_recorder();

    void select_cam();

    void select_motor();

    void select_none();

#ifdef	__cplusplus
}
#endif

#endif // ACTIVITYMAIN_H
