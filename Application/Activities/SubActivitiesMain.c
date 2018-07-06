#include "SubActivitiesMain.h"
#include "ActivityMain.h"
#include "CTTKernel/kernel.h"
#include "UserInterface/Buzzer.h"
#include "Application/interfaces/rtcc_interface.h"
#include "Application/interfaces/ep_interface.h"
#include "Application/images_list.h"
#include "Application/Utils/model_manager.h"
#include <p24Fxxxx.h>
//#include "../defines.h"
#include <libpic30.h>
#include <Application/str_resources.h>

enum SUBACT_GRAV_PHASE_e {
	SUBACT_GRAV_PHASE_INIT,
	SUBACT_GRAV_PHASE_REC,
	SUBACT_GRAV_PHASE_PAUSED,
	SUBACT_GRAV_PHASE_MOUNTED,
	SUBACT_GRAV_PHASE_WAIT_MOUNT,
	SUBACT_GRAV_PHASE_WAIT_UNMOUNT,
	SUBACT_GRAV_PHASE_WAIT_RECORDING,
	SUBACT_GRAV_PHASE_WAIT_FINISH_RECORDING,
	SUBACT_GRAV_PHASE_NONE,

};


#define _this sub_activities_data

#define _parent DraftMain_localc_variables

void subActivityLight_onCreate(void * param);
void subActivityLight_onDestroy(void * param);

void subActivityDefault_onCreate(void * param);
void subActivityDefault_onDestroy(void * param);
void subActivityDefault_onTick(void * param);

void subActivityCam_onCreate(void * param);
void subActivityCam_onDestroy(void * param);

void subActivityGravador_onCreate(void * param);
void subActivityGravador_onDestroy(void * param);
void subActivityGravador_onTick(void * param);

void subActivityVelocidade_onCreate(void * param);
void subActivityVelocidade_onDestroy(void * param);


void draw_button_toggle_led();

void draw_button_toggle_cam();

void show_message_confirm_wb();

void draw_button_toggle_led();

void clear_sub_space();

void subActivityGravador_set_phase(enum SUBACT_GRAV_PHASE_e phase);

SubActivityContext_t subActivityLight = {
	.onCreate = subActivityLight_onCreate,
	.onDestroy = subActivityLight_onDestroy,
};
SubActivityContext_t subActivityDefault = {
	.onCreate = subActivityDefault_onCreate,
	.onDestroy = subActivityDefault_onDestroy,
	.onTick = subActivityDefault_onTick,
};
SubActivityContext_t subActivityCam = {
	.onCreate = subActivityCam_onCreate,
	.onDestroy = subActivityCam_onDestroy,
};
SubActivityContext_t subActivityGravador = {
	.onCreate = subActivityGravador_onCreate,
	.onDestroy = subActivityGravador_onDestroy,
	.onTick = subActivityGravador_onTick,
};
SubActivityContext_t subActivityVelocidade = {
	.onCreate = subActivityVelocidade_onCreate,
	.onDestroy = subActivityVelocidade_onDestroy,
};

struct {
	touchable_t* dbPwmSetPoint;
	touchable_t* btTrocarMotor;
	touchable_t* btLedToggle;
	touchable_t* btCamToggle;
	touchable_t* btCamAWB;
	touchable_t* btCamWIND;
	touchable_t* btCamFLEX;
	touchable_t* btCamCTEMP;
	touchable_t* btCamConfirmWBTest;
	touchable_t* btRecStart;
	touchable_t* btRecStop;
	touchable_t* btRecPauseResume;
	touchable_t* btEject;
	touchable_t* btMount;
	touchable_t* dbVelocity;
	touchable_t* btVelocityUp;
	touchable_t* btVelocityDown;
	uint16_t grav_phase;
} sub_activities_data = {



};

//const char * str_led_liga = getRes(str_res_ligar);
//const char * str_led_desliga = getRes(str_res_desligar);

kTask_t * loading_task = NULL;

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t x_img;
	uint16_t y_img;
	uint16_t counter;
} loading_task_t;

void task_show_loading(void * params) {

	loading_task_t * p = (loading_task_t *) params;

	if (kTask_isInitialization(NULL)) {
		p->counter = 0;
	}

	
	if (p->counter%3 == 0){
		place_disp(p->x_img, p->y_img, IMG_HOURGLASS_DOWN);
	} else if (p->counter%3 == 1){
		place_disp(p->x_img, p->y_img, IMG_HOURGLASS);
	} else if (p->counter%3 == 2){
		place_disp(p->x_img, p->y_img, IMG_HOURGLASS_SIDE);
	}

	p->counter++;
//	display_set_font(VERDANA12NMONO);
//	SetColor(BLUEBTBAR);
//	Bar(p->x_img, p->y + 10 + 10, p->x_img + 30, p->y + 10 + 15);
//	SetColor(WHITE);
//	SetBgColor(BLUEBTBAR);
//	display_print_text(buffer_text, p->x + 1, p->y + 10, CENTER, 0);



	sTask_Delay(NULL, 700 * tMILLIS);
}

void init_loading_task(uint16_t x, uint16_t y) {

	if (loading_task != NULL) return;

	loading_task_t * params = (loading_task_t *) dd_alloc(sizeof (loading_task_t));

	params->x = x;
	params->y = y;


	unsigned int len = 30, hei = 30;

	picture_get_dim(IMG_HOURGLASS, &len, &hei);

	params->x_img = x - len / 2;
	params->y_img = y;

//	place_disp(params->x_img, params->y_img, IMG_HOURGLASS);

	loading_task = kTask_create(task_show_loading, params);

	kTask_bind_param(loading_task, params);

}

void delete_loading_task() {

	if (loading_task == NULL) return;

	loading_task_t * p = (loading_task_t *) kTask_get_param(loading_task);
	SetColor(WHITE);
	SetBgColor(BLUEBTBAR);
	display_print_text("", p->x, p->y, CENTER, 20);

	unsigned int len = 30, hei = 30;

	picture_get_dim(IMG_HOURGLASS, &len, &hei);

	SetColor(BLUEBTBAR);
	Bar(p->x - len / 2, p->y - 10, p->x + len / 2, p->y + hei);

	sTask_delete(loading_task);
	loading_task = NULL;



	//    dd_free();
}

void center_img_on_button(touchable_t * touchable) {
	unsigned int length, height;

	picture_get_dim(touchable->img_idx_pressed, &length, &height);

	touchable->x_img = touchable->length / 2 - length / 2;
	touchable->y_img = touchable->height / 2 - height / 2;



}

void clickdbPwmSetPoint(uint8_t event, uint8_t flag, void* obFlag) {
	flag = flag * 0.9 + 10;
	_parent.pwm_perc = flag - flag % 10;
	fj_set_led_pwm(_parent.pwm_perc);
}

void clickdbVelocityPerc(uint8_t event, uint8_t flag, void* obFlag) {
	//    _parent.velocity_perc = flag;
	_parent.velocity = flag * 1L * (dmd_motor_info()->max_vel - dmd_motor_info()->min_vel)*1L / 100L + dmd_motor_info()->min_vel;
	_parent.velocity -= _parent.velocity % (dmd_motor_info()->min_vel / 2);
}

void clickbtLedOn(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		if (_parent.state_led == DEV_STATE_ON) {
			_parent.state_led = DEV_STATE_OFF;
			fj_set_led_on_off(0);
		} else if (_parent.state_led == DEV_STATE_OFF) {
			_parent.state_led = DEV_STATE_ON;
			fj_set_led_on_off(1);
		}
		draw_button_toggle_led();
	}
}

void clickbtCamToggle(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		if (_parent.state_camera == DEV_STATE_ON) {
			_parent.state_camera = DEV_STATE_OFF;
			draw_button_toggle_cam();
			fj_set_cam_status(0);
		} else if (_parent.state_camera == DEV_STATE_OFF) {
			_parent.state_camera = DEV_STATE_ON;
			fj_set_cam_status(1);
			show_message_confirm_wb();
		}

	}
}

void clickbtCamOPTION(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		fj_set_cam(1,0,0,0,0);
	} else if (event == EVENT_PRESS || event == EVENT_EXTERN) {
        fj_set_cam(1,flag==1,flag==2,flag==3,flag==4);
    }
}

void clickbtTrocarMotor(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE && !dmd_motor_flags()->motor_runing) {
		uint8_t current_motor = dmd_board_flags()->selected_motor;
        _parent.safe_disable_til_next = 1;
		uint8_t crc = dmd_alternate_motor();
		if (crc) {
			if (current_motor == 1) {
				_this.btTrocarMotor->img_idx_unpress = IMG_M1_M2_UNPRESSED;
				_this.btTrocarMotor->img_idx_pressed = IMG_M2_M1_PRESSED;
			} else {
				_this.btTrocarMotor->img_idx_unpress = IMG_M2_M1_UNPRESSED;
				_this.btTrocarMotor->img_idx_pressed = IMG_M1_M2_PRESSED;
			}
		}
		//        _parent.active_motor = _parent.active_motor == 1 ? 2 : 1;
	}
}

void clickbtRecStart(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		RTCC_t time = rtccRead();
		char buffer[50];
		sprintf(buffer, "%02u-%02u-%02u_%02u-%02u-%02u",
				time.YEAR,
				time.MONTH,
				time.DAY,
				time.HOUR,
				time.MINUTE,
				time.SECOND
				);
		ep_start(buffer, 0);
	}
}

void clickbtRecStop(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		
		if (ep_stop()) {
			
			subActivityGravador_set_phase(SUBACT_GRAV_PHASE_WAIT_FINISH_RECORDING);
//			subActivityGravador_set_phase(SUBACT_GRAV_PHASE_WAIT_UNMOUNT);

		}
	}
}

void inc_dec_velocity(uint8_t f_up) {
	unsigned int pace = dmd_motor_info()->min_vel / 2;
	unsigned int my_velocity = _parent.velocity;
	if (f_up == 1) {
		if (my_velocity + pace <= dmd_motor_info()->max_vel) {
			my_velocity += pace;
			sys_buzz(10, 10);
		}
	} else if (f_up == 0) {
		if (my_velocity - pace >= dmd_motor_info()->min_vel) {
			my_velocity -= pace;
			sys_buzz(10, 10);
		}
	}
	_parent.velocity = my_velocity;

}

void clickbtVelocityUpDown(uint8_t event, uint8_t flag, void* obFlag) {
	static unsigned int counter = 0;
	if (event == EVENT_PRESS) {
		counter = 5;
	}
	if (event == EVENT_STILLPRESS || event == EVENT_MOVE) {
		if (counter++ >= 5) {
			counter = 0;

			inc_dec_velocity(flag);
			_this.dbVelocity->flag = (_parent.velocity - dmd_motor_info()->min_vel)*100L / (dmd_motor_info()->max_vel - dmd_motor_info()->min_vel);
			if (ActivityMain.curr_subactivity == &subActivityVelocidade) {
				bt_lib_2_drawTouchableDragBar(*_this.dbVelocity, _this.dbVelocity->flag);
			}
		}
	}
}

void draw_button_toggle_led() {
	if (_parent.state_led == DEV_STATE_ON) {
		_this.btLedToggle->border_color = YELLOW;
		_this.btLedToggle->text = (char *) getRes(str_res_desligar);
	} else {
		_this.btLedToggle->border_color = LIGHTBLUEDELIMS;
		_this.btLedToggle->text = (char *) getRes(str_res_ligar);
	}
	bt_lib_2_drawTouchableButton(*_this.btLedToggle);
}

void draw_button_toggle_cam() {
            ActivityMan_excludeTouchableSub(_this.btCamAWB);
        ActivityMan_excludeTouchableSub(_this.btCamFLEX);
        ActivityMan_excludeTouchableSub(_this.btCamWIND);
        ActivityMan_excludeTouchableSub(_this.btCamCTEMP);
        
	if (_parent.state_camera == DEV_STATE_ON) {
		_this.btCamToggle->border_color = YELLOW;
		_this.btCamToggle->text = (char *) getRes(str_res_desligar);
                

        bt_lib_2_drawTouchableButton(*_this.btCamAWB);
        ActivityMan_insertTouchableSub(_this.btCamAWB);
        bt_lib_2_drawTouchableButton(*_this.btCamFLEX);
        ActivityMan_insertTouchableSub(_this.btCamFLEX);
        bt_lib_2_drawTouchableButton(*_this.btCamWIND);
        ActivityMan_insertTouchableSub(_this.btCamWIND);
        bt_lib_2_drawTouchableButton(*_this.btCamCTEMP);
        ActivityMan_insertTouchableSub(_this.btCamCTEMP);
        
	} else {
		_this.btCamToggle->border_color = LIGHTBLUEDELIMS;
		_this.btCamToggle->text = (char *) getRes(str_res_ligar);
                
        bt_lib_2_eraseTouchable(*_this.btCamAWB, BLUEBTBAR);
        bt_lib_2_eraseTouchable(*_this.btCamFLEX, BLUEBTBAR);
        bt_lib_2_eraseTouchable(*_this.btCamWIND, BLUEBTBAR);
        bt_lib_2_eraseTouchable(*_this.btCamCTEMP, BLUEBTBAR);
        
	}
	bt_lib_2_drawTouchableButton(*_this.btCamToggle);
}

void subActivityLight_onCreate(void * param) {

	clear_sub_space();

	_this.btLedToggle = allocAsTouchable;
	setAsButton(_this.btLedToggle,
			.text = "",
			.coord_x = 52,
			.coord_y = 172,
			.height = 48,
			.length = 50,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_thickness = 1,
			.onClick = clickbtLedOn
			);
	draw_button_toggle_led();
	ActivityMan_insertTouchableSub(_this.btLedToggle);


	_this.dbPwmSetPoint = allocAsTouchable;
	setAsButton(_this.dbPwmSetPoint,
			.border_thickness = 3,
			.blink = 2,
			.color_txt_unpressed = BLUEBTBAR,
			.color = WHITE,
			.color_blink = LIGHTBLUEBG,
			.border_color = LIGHTBLUEBG,
			.border_color_blink = LIGHTRED,
			.coord_x = 120,
			.coord_y = 179,
			.length = 180,
			.height = 31,
			.flag = _parent.pwm_perc,
			.onClick = clickdbPwmSetPoint
			);
	//	kernel_message(MODE_DEBUG, "Inicializing sub activity light with %u pwm!\n",_this.dbPwmSetPoint->flag)
	bt_lib_2_drawTouchableDragBar(*_this.dbPwmSetPoint, _this.dbPwmSetPoint->flag);
	ActivityMan_insertTouchableSub(_this.dbPwmSetPoint);

	select_light_source();

}

void subActivityLight_onDestroy(void * param) {
	desalocTouchable(_this.dbPwmSetPoint);
	desalocTouchable(_this.btLedToggle);
}

void subActivityDefault_onCreate(void * param) {
	
	models_t model = modelManager_getDataReaded()->equip_model;

	clear_sub_space();

	select_none();

	_this.btTrocarMotor = allocAsTouchable;
	setAsButton(_this.btTrocarMotor,
			.text = "",
			.coord_x = DISP_HOR_RESOLUTION - 50,
			.coord_y = 172,
			.height = 48,
			.length = 50,
			.img_idx_pressed = dmd_board_flags()->selected_motor == 2 ? IMG_M1_M2_PRESSED : IMG_M2_M1_PRESSED ,
			.img_idx_unpress = dmd_board_flags()->selected_motor == 2 ? IMG_M2_M1_UNPRESSED : IMG_M1_M2_UNPRESSED ,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.onClick = clickbtTrocarMotor
			);
	
	
	if (model == MODEL_TRIPLET || model == MODEL_SHAVER){
		bt_lib_2_drawTouchableButton(*_this.btTrocarMotor);
		ActivityMan_insertTouchableSub(_this.btTrocarMotor);
	}
}

void subActivityDefault_onTick(void * param){
    
    unsigned int current_motor = dmd_board_flags()->selected_motor;
    
    unsigned int pressed_should_be = current_motor == 2 ? IMG_M1_M2_PRESSED: IMG_M2_M1_PRESSED;
    unsigned int unpressed_should_be = current_motor == 2 ? IMG_M2_M1_UNPRESSED: IMG_M1_M2_UNPRESSED;
    
    if (
            _this.btTrocarMotor->img_idx_pressed != pressed_should_be ||
            _this.btTrocarMotor->img_idx_unpress != unpressed_should_be
            ){
        _this.btTrocarMotor->img_idx_pressed = pressed_should_be;
        _this.btTrocarMotor->img_idx_unpress = unpressed_should_be;
        bt_lib_2_drawTouchableButton(*_this.btTrocarMotor);
    }
    
    
    
}

void subActivityDefault_onDestroy(void * param) {
	desalocTouchable(_this.btTrocarMotor);
}

void show_message_confirm_wb() {

	bt_lib_2_drawTouchableButton(*_this.btCamConfirmWBTest);
	ActivityMan_insertTouchableSub(_this.btCamConfirmWBTest);
	ActivityMan_excludeTouchableSub(_this.btCamToggle);
	ActivityMan_excludeTouchable(_parent.bt_voltar_sub);
	touchable_t bt_message = {};
	setAsButton(&bt_message,
			.text = getRes(str_res_o_balano_de_branco_foi_realizado),
			.coord_x = 52,
			.coord_y = 172,
			.height = 48,
			.length = 210,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = BLUEBTBAR,
			.border_thickness = 1,
			.onClick = NULL,
			.blink = 0,
			);

	bt_lib_2_drawTouchableButton(bt_message);

}

void click_bt_confirm_wb(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		clear_sub_space();
		ActivityMan_insertTouchableSub(_this.btCamToggle);
		draw_button_toggle_cam();
		ActivityMan_insertTouchable(_parent.bt_voltar_sub);
		ActivityMan_excludeTouchableSub(_this.btCamConfirmWBTest);
	}

}

void subActivityCam_onCreate(void * param) {
	select_cam();
	clear_sub_space();

	_this.btCamToggle = allocAsTouchable;
	setAsButton(_this.btCamToggle,
			.text = "",
			.coord_x = 52,
			.coord_y = 172,
			.height = 48,
			.length = 50,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_thickness = 1,
			.onClick = clickbtCamToggle
			);
	draw_button_toggle_cam();
	ActivityMan_insertTouchableSub(_this.btCamToggle);
    
	_this.btCamAWB = allocAsTouchable;
	setAsButton(_this.btCamAWB,
			.text = "AWB",
			.coord_x = 103,
			.coord_y = 172,
			.height = 48,
			.length = 50,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_thickness = 1,
			.onClick = clickbtCamOPTION,
            .flag = 1
			);

    
	_this.btCamFLEX = allocAsTouchable;
	setAsButton(_this.btCamFLEX,
			.text = "FLEX",
			.coord_x = 153,
			.coord_y = 172,
			.height = 48,
			.length = 50,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_thickness = 1,
			.onClick = clickbtCamOPTION,
            .flag = 2
			);
    

	_this.btCamWIND = allocAsTouchable;
	setAsButton(_this.btCamWIND,
			.text = "WIND",
			.coord_x = 203,
			.coord_y = 172,
			.height = 48,
			.length = 50,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_thickness = 1,
			.onClick = clickbtCamOPTION,
            .flag = 3
			);

    
	_this.btCamCTEMP = allocAsTouchable;
	setAsButton(_this.btCamCTEMP,
			.text = "CTEMP",
			.coord_x = 253,
			.coord_y = 172,
			.height = 48,
			.length = 50,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_thickness = 1,
			.onClick = clickbtCamOPTION,
            .flag = 4
			);
    

	_this.btCamConfirmWBTest = allocAsTouchable;
	setAsButton(_this.btCamConfirmWBTest,
			.text = "Ok",
			.reference_txt = CENTER,
			.coord_x = 270,
			.coord_y = 172,
			.height = 48,
			.length = 50,
			.reference_txt = CENTER,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = LIGHTBLUEDELIMS,
			.border_thickness = 1,
			.onClick = click_bt_confirm_wb
			);


}

void subActivityCam_onDestroy(void * param) {
	desalocTouchable(_this.btCamToggle);
	desalocTouchable(_this.btCamConfirmWBTest);
}

void clickbtMount(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {

		if (ep_mountStorageDevice()) {

			subActivityGravador_set_phase(SUBACT_GRAV_PHASE_WAIT_MOUNT);
		}
	}

}

void clickbtEject(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {

		if (ep_unmountStorageDevice()) {

			subActivityGravador_set_phase(SUBACT_GRAV_PHASE_WAIT_UNMOUNT);

		}


	}

}

void clickbtPauseResume(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {

		board_z3_status_t * flags = ep_z3_flags();
		
		if (flags->f_pausado) {
			ep_resume();
		} else {
			ep_pause();
			
		} 

	}

}

void subActivityGravador_set_phase_init() {
	clear_sub_space();
	delete_loading_task();

	ActivityMan_excludeTouchableSub(_this.btRecStart);
	ActivityMan_excludeTouchableSub(_this.btRecStop);
	ActivityMan_excludeTouchableSub(_this.btRecPauseResume);
	ActivityMan_excludeTouchableSub(_this.btMount);
	ActivityMan_excludeTouchableSub(_this.btEject);

//	bt_lib_2_drawTouchableButton(*_this.btMount);
//	ActivityMan_insertTouchableSub(_this.btMount);
    
    touchable_t bt_message = {};
	setAsButton(&bt_message,
			.text = getRes(str_res_inserir_media),
			.coord_x = 52,
			.coord_y = 172,
			.height = 48,
			.length = 260,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = BLUEBTBAR,
			.border_thickness = 1,
			.onClick = NULL,
			.blink = 0,
			);

	bt_lib_2_drawTouchableButton(bt_message);


}

void subActivityGravador_set_phase_recording(uint16_t is_paused) {
	clear_sub_space();
	delete_loading_task();

	ActivityMan_excludeTouchableSub(_this.btRecStart);
	ActivityMan_excludeTouchableSub(_this.btRecStop);
	ActivityMan_excludeTouchableSub(_this.btMount);
	ActivityMan_excludeTouchableSub(_this.btEject);
	ActivityMan_excludeTouchableSub(_this.btRecPauseResume);
	
	if (is_paused){
		_this.btRecPauseResume->img_idx_pressed = IMG_REC_PRESSED;
		_this.btRecPauseResume->img_idx_unpress = IMG_REC_UNPRESSED;
		_this.btRecPauseResume->text = getRes(str_mpCam_continuar);
		
	} else {
		_this.btRecPauseResume->img_idx_pressed = IMG_PAUSED_PRESSED;
		_this.btRecPauseResume->img_idx_unpress = IMG_PAUSED_UNPRESSED;
		_this.btRecPauseResume->text = getRes(str_res_pausar);
	}

	bt_lib_2_drawTouchableButton(*_this.btRecStop);
	bt_lib_2_drawTouchableButton(*_this.btRecPauseResume);
	ActivityMan_insertTouchableSub(_this.btRecStop);
	ActivityMan_insertTouchableSub(_this.btRecPauseResume);
	
}

void subActivityGravador_set_phase_wait_recording() {
	clear_sub_space();
	init_loading_task(300, 186);

	ActivityMan_excludeTouchableSub(_this.btRecStart);
	ActivityMan_excludeTouchableSub(_this.btRecStop);
	ActivityMan_excludeTouchableSub(_this.btMount);
	ActivityMan_excludeTouchableSub(_this.btEject);
	ActivityMan_excludeTouchableSub(_this.btRecPauseResume);

	touchable_t bt_message = {};
	setAsButton(&bt_message,
			.text = getRes(str_res_aguardando_gravador),
			.coord_x = 52,
			.coord_y = 172,
			.height = 48,
			.length = 210,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = BLUEBTBAR,
			.border_thickness = 1,
			.onClick = NULL,
			.blink = 0,
			);

	bt_lib_2_drawTouchableButton(bt_message);
}

void subActivityGravador_set_phase_wait_mount() {

	clear_sub_space();
	init_loading_task(300, 186);

	ActivityMan_excludeTouchableSub(_this.btRecStart);
	ActivityMan_excludeTouchableSub(_this.btRecStop);
	ActivityMan_excludeTouchableSub(_this.btMount);
	ActivityMan_excludeTouchableSub(_this.btEject);
	ActivityMan_excludeTouchableSub(_this.btRecPauseResume);

	touchable_t bt_message = {};
	setAsButton(&bt_message,
			.text = getRes(str_res_aguarde_a_montagem_da_mdia),
			.coord_x = 52,
			.coord_y = 172,
			.height = 48,
			.length = 210,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = BLUEBTBAR,
			.border_thickness = 1,
			.onClick = NULL,
			.blink = 0,
			);

	bt_lib_2_drawTouchableButton(bt_message);

}

void subActivityGravador_set_phase_wait_unmount() {

	clear_sub_space();
	init_loading_task(300, 186);

	ActivityMan_excludeTouchableSub(_this.btRecStart);
	ActivityMan_excludeTouchableSub(_this.btRecStop);
	ActivityMan_excludeTouchableSub(_this.btMount);
	ActivityMan_excludeTouchableSub(_this.btEject);
	ActivityMan_excludeTouchableSub(_this.btRecPauseResume);

	touchable_t bt_message = {};
	setAsButton(&bt_message,
			.text = getRes(str_res_aguarde_a_remoo_da_mdia),
			.coord_x = 52,
			.coord_y = 172,
			.height = 48,
			.length = 210,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = BLUEBTBAR,
			.border_thickness = 1,
			.onClick = NULL,
			.blink = 0,
			);

	bt_lib_2_drawTouchableButton(bt_message);

}

void subActivityGravador_set_phase_wait_finish_recording() {

	clear_sub_space();
	init_loading_task(300, 186);

	ActivityMan_excludeTouchableSub(_this.btRecStart);
	ActivityMan_excludeTouchableSub(_this.btRecStop);
	ActivityMan_excludeTouchableSub(_this.btMount);
	ActivityMan_excludeTouchableSub(_this.btEject);
	ActivityMan_excludeTouchableSub(_this.btRecPauseResume);

	touchable_t bt_message = {};
	setAsButton(&bt_message,
			.text = getRes(str_res_aguarde_finalizao_da_gravaon_no_retire_o_pendrive_antes_de_ejetar),
			.coord_x = 52,
			.coord_y = 172,
			.height = 48,
			.length = 210,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = BLUEBTBAR,
			.border_thickness = 1,
			.onClick = NULL,
			.blink = 0,
			);

	bt_lib_2_drawTouchableButton(bt_message);

}

void subActivityGravador_set_phase_mounted() {

	clear_sub_space();
	delete_loading_task();

	ActivityMan_excludeTouchableSub(_this.btRecStart);
	ActivityMan_excludeTouchableSub(_this.btRecStop);
	ActivityMan_excludeTouchableSub(_this.btMount);
	ActivityMan_excludeTouchableSub(_this.btEject);
	ActivityMan_excludeTouchableSub(_this.btRecPauseResume);


	//	bt_lib_2_drawTouchableButton(*_this.btRecStop);
	//	ActivityMan_insertTouchableSub(_this.btMount);
//	bt_lib_2_drawTouchableButton(*_this.btEject);
//	ActivityMan_insertTouchableSub(_this.btEject);
    
    z3Storage_t storage_info;
    ep_getStorageSpace(&storage_info);
    
    char buffer[128];
    
    uint16_t dx = 0;
    
    if (storage_info.size - storage_info.used < 400000){
           
        sprintf(buffer, "A mídia não tem espaço suficiente!\nLibere mais espaço."
        );
        
        dx = 52;
        
    } else {
        
        bt_lib_2_drawTouchableButton(*_this.btRecStart);
        ActivityMan_insertTouchableSub(_this.btRecStart);
    
        sprintf(buffer, "Mídia conectada %.1f GB livres de\n%.1f GB total.", 
            (float) (storage_info.size - storage_info.used) * 1.0 / 1000000L,
            (float) storage_info.size * 1.0 / 1000000L
        );
    
    }
    
    touchable_t bt_message = {};
	setAsButton(&bt_message,
			.text = buffer,
			.coord_x = 105 - dx,
			.coord_y = 172,
			.height = 48,
			.length = 200 + dx,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = BLUEBTBAR,
			.border_thickness = 1,
			.onClick = NULL,
			.blink = 0,
			);

	bt_lib_2_drawTouchableButton(bt_message);

}

void subActivityGravador_set_phase(enum SUBACT_GRAV_PHASE_e phase) {

	if (phase == _this.grav_phase) return;

	_this.grav_phase = phase;
	
	kernel_message(MODE_DEBUG, "Changing to phase: %u\n", phase);

	switch (phase) {
		case SUBACT_GRAV_PHASE_INIT:
		{
			subActivityGravador_set_phase_init();
		}
			break;
		case SUBACT_GRAV_PHASE_MOUNTED:
		{
			subActivityGravador_set_phase_mounted();
		}
			break;
		case SUBACT_GRAV_PHASE_REC:
		{
			subActivityGravador_set_phase_recording(0);
		}
			break;
		case SUBACT_GRAV_PHASE_PAUSED:
		{
			subActivityGravador_set_phase_recording(1);
		}
			break;
		case SUBACT_GRAV_PHASE_WAIT_MOUNT:
		{
			subActivityGravador_set_phase_wait_mount();
		}
			break;
		case SUBACT_GRAV_PHASE_WAIT_RECORDING:
		{
			subActivityGravador_set_phase_wait_recording();
		}
			break;
		case SUBACT_GRAV_PHASE_WAIT_FINISH_RECORDING:
		{
			subActivityGravador_set_phase_wait_finish_recording();
		}
			break;
		case SUBACT_GRAV_PHASE_WAIT_UNMOUNT:
		{
			subActivityGravador_set_phase_wait_unmount();
		}
			break;
		default:
			break;
	}

}

void subActivityGravador_onCreate(void * param) {
	select_recorder();
	clear_sub_space();

	_this.btRecStart = allocAsTouchable;
	setAsButton(_this.btRecStart,
			.text = getRes(str_res_iniciar),
			.coord_x = 52,
			.coord_y = 172,
			.height = 48,
			.length = 50,
			.color = BLUEBTBAR,
			.img_idx_pressed = IMG_REC_PRESSED,
			.img_idx_unpress = IMG_REC_UNPRESSED,
			.reference_txt = CENTER,
			.txt_font = VERDANA8MONO,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = LIGHTBLUEDELIMS,
			.border_thickness = 1,
			.onClick = clickbtRecStart
			);
	center_img_on_button(_this.btRecStart);
	_this.btRecStart->y_txt = 12;
	_this.btRecStart->y_img -= 5;

	_this.btRecStop = allocAsTouchable;
	setAsButton(_this.btRecStop,
			.text = getRes(str_res_parar),
			.coord_x = 52,
			.coord_y = 172,
			.height = 48,
			.length = 50,
			.img_idx_pressed = IMG_STOP_PRESSED,
			.img_idx_unpress = IMG_STOP_UNPRESSED,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = LIGHTBLUEDELIMS,
			.border_thickness = 1,
			.onClick = clickbtRecStop
			);
	center_img_on_button(_this.btRecStop);
	_this.btRecStop->y_txt = 12;
	_this.btRecStop->y_img -= 5;

	_this.btRecPauseResume = allocAsTouchable;
	setAsButton(_this.btRecPauseResume,
			.text = getRes(str_res_pausar),
			.coord_x = 102,
			.coord_y = 172,
			.height = 48,
			.length = 60,
			.color = BLUEBTBAR,
			.img_idx_pressed = IMG_PAUSED_PRESSED,
			.img_idx_unpress = IMG_PAUSED_UNPRESSED,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = LIGHTBLUEDELIMS,
			.border_thickness = 1,
			.onClick = clickbtPauseResume
			);
	center_img_on_button(_this.btRecPauseResume);
	_this.btRecPauseResume->y_txt = 12;
	_this.btRecPauseResume->y_img -= 5;

	_this.btMount = allocAsTouchable;
	setAsButton(_this.btMount,
			.text = getRes(str_res_habilitar_pendrive),
			.coord_x = 52,
			.coord_y = 172,
			.height = 48,
			.length = 150,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = LIGHTBLUEDELIMS,
			.border_thickness = 1,
			.onClick = clickbtMount,
			.img_idx_pressed = IMG_USB_PRESSED,
			.img_idx_unpress = IMG_USB_UNPRESSED,
			);
	center_img_on_button(_this.btMount);
	_this.btMount->x_img -= 50;
	_this.btMount->y_img += 2;
	_this.btMount->x_txt += 15;

	_this.btEject = allocAsTouchable;
	setAsButton(_this.btEject,
			.text = getRes(str_res_ejetar),
			.coord_x = 102,
			.coord_y = 172,
			.height = 48,
			.length = 100,
			.color = BLUEBTBAR,
			.img_idx_pressed = IMG_EJECT_PRESSED,
			.img_idx_unpress = IMG_EJECT_UNPRESSED,
			.txt_font = VERDANA8MONO,
			.reference_txt = CENTER,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.border_color = LIGHTBLUEDELIMS,
			.border_thickness = 1,
			.onClick = clickbtEject
			);
	center_img_on_button(_this.btEject);
	_this.btEject->y_txt = 12;
	_this.btEject->y_img -= 5;

	if (
			_this.grav_phase == SUBACT_GRAV_PHASE_WAIT_MOUNT ||
			_this.grav_phase == SUBACT_GRAV_PHASE_WAIT_RECORDING ||
			_this.grav_phase == SUBACT_GRAV_PHASE_WAIT_UNMOUNT

			) {
		enum SUBACT_GRAV_PHASE_e phase = _this.grav_phase;
		_this.grav_phase = SUBACT_GRAV_PHASE_NONE;
		subActivityGravador_set_phase(phase);
	} else {
		_this.grav_phase = SUBACT_GRAV_PHASE_NONE;
		if (!ep_is_media_mounted()) {
			subActivityGravador_set_phase(SUBACT_GRAV_PHASE_INIT);
		}
	}






	//	bt_lib_2_drawTouchableButton(*_this.btRecStart);
	//	ActivityMan_insertTouchableSub(_this.btRecStart);
	//	bt_lib_2_drawTouchableButton(*_this.btRecStop);
	//	ActivityMan_insertTouchableSub(_this.btRecStop);

}

void subActivityGravador_onDestroy(void * param) {
	desalocTouchable(_this.btRecStop);
	desalocTouchable(_this.btRecStart);
	desalocTouchable(_this.btEject);
	desalocTouchable(_this.btMount);
	delete_loading_task();


}

void subActivityGravador_onTick(void * param) {

	if (ep_is_media_mounted()) {

		if (ep_recording_status() == RECORDING_STARTED) {
			if (_this.grav_phase != SUBACT_GRAV_PHASE_WAIT_UNMOUNT && _this.grav_phase != SUBACT_GRAV_PHASE_WAIT_FINISH_RECORDING){
				if (ep_z3_flags()->f_pausado){
					subActivityGravador_set_phase(SUBACT_GRAV_PHASE_PAUSED);
				} else {
					subActivityGravador_set_phase(SUBACT_GRAV_PHASE_REC);
				}
			}
		}

		if (ep_recording_status() == RECORDING_INITIALIZED || ep_was_safe_stoped()) {
			subActivityGravador_set_phase(SUBACT_GRAV_PHASE_WAIT_RECORDING);
		}

		if (ep_recording_status() == RECORDING_STOPED && !ep_was_safe_stoped()) {
			if (_this.grav_phase != SUBACT_GRAV_PHASE_WAIT_FINISH_RECORDING && _this.grav_phase != SUBACT_GRAV_PHASE_WAIT_UNMOUNT){
				subActivityGravador_set_phase(SUBACT_GRAV_PHASE_MOUNTED);
			} else if (_this.grav_phase == SUBACT_GRAV_PHASE_WAIT_FINISH_RECORDING) {
				board_z3_status_t * flags = ep_z3_flags();
				
				if (flags->recordingTerminated){
					subActivityGravador_set_phase(SUBACT_GRAV_PHASE_MOUNTED);
				}
				
			}
		}

	} else {
		board_z3_status_t * flags = ep_z3_flags();

		if (flags->flashError) {
			subActivityGravador_set_phase(SUBACT_GRAV_PHASE_INIT);
		}

		if (_this.grav_phase != SUBACT_GRAV_PHASE_WAIT_MOUNT) {
			subActivityGravador_set_phase(SUBACT_GRAV_PHASE_INIT);
		}

	}

}

void subActivityVelocidade_onCreate(void * param) {
	select_motor();
	clear_sub_space();

	uint16_t vel_perc = (_parent.velocity - dmd_motor_info()->min_vel)*100L / (dmd_motor_info()->max_vel - dmd_motor_info()->min_vel);

	_this.dbVelocity = allocAsTouchable;
	setAsButton(_this.dbVelocity,
			.border_thickness = 3,
			.blink = 2,
			.color_txt_unpressed = BLUEBTBAR,
			.color = WHITE,
			.color_blink = LIGHTBLUEBG,
			.border_color = LIGHTBLUEBG,
			.border_color_blink = LIGHTRED,
			.coord_x = 105,
			.coord_y = 179,
			.length = DISP_HOR_RESOLUTION - 55 - 105,
			.height = 31,
			.flag = vel_perc,
			.onClick = clickdbVelocityPerc
			);
	bt_lib_2_drawTouchableDragBar(*_this.dbVelocity, _this.dbVelocity->flag);
	ActivityMan_insertTouchableSub(_this.dbVelocity);

	_this.btVelocityUp = allocAsTouchable;
	setAsButton(_this.btVelocityUp,
			.text = "",
			.coord_x = DISP_HOR_RESOLUTION - 52,
			.coord_y = 172,
			.height = 48,
			.length = 50,
			.color = BLUEBTBAR,
			.beep = NO_BEEP,
			.txt_font = VERDANA8MONO,
			.txt_font_height = 13,
			.img_idx_pressed = IMG_UP_ARROW_PRESSED,
			.img_idx_unpress = IMG_UP_ARROW_UNPRESSED,
			.color_blink = CYANBTBAR,
			.reference_txt = CENTER,
			.onClick = clickbtVelocityUpDown,
			.flag = 1,
			);
	center_img_on_button(_this.btVelocityUp);
	bt_lib_2_drawTouchableButton(*_this.btVelocityUp);
	ActivityMan_insertTouchableSub(_this.btVelocityUp);

	_this.btVelocityDown = allocAsTouchable;
	setAsButton(_this.btVelocityDown,
			.text = "",
			.coord_x = 52,
			.coord_y = 172,
			.height = 48,
			.length = 50,
			.color = BLUEBTBAR,
			.txt_font = VERDANA8MONO,
			.txt_font_height = 13,
			.beep = NO_BEEP,
			.reference_txt = CENTER,
			.color_blink = CYANBTBAR,
			.img_idx_pressed = IMG_DOWN_ARROW_PRESSED,
			.img_idx_unpress = IMG_DOWN_ARROW_UNPRESSED,

			.onClick = clickbtVelocityUpDown,
			.flag = 0,
			);
	center_img_on_button(_this.btVelocityDown);
	bt_lib_2_drawTouchableButton(*_this.btVelocityDown);
	ActivityMan_insertTouchableSub(_this.btVelocityDown);


}

void subActivityVelocidade_onDestroy(void * param) {
	desalocTouchable(_this.dbVelocity);
	desalocTouchable(_this.btVelocityUp);
	desalocTouchable(_this.btVelocityDown);
}

void clear_sub_space() {
	SetColor(BLUEBTBAR);
	WAIT_UNTIL_FINISH(Bar(51, 172, 320, 220));
}