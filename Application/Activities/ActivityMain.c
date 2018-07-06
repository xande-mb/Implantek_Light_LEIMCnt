#include "ActivityMain.h"
#include <ActivityManager/ActivityMan.h>
#include <CTTKernel/kernel.h>
#include <Application/DisplayControler24/tft_driver.h>
#include <Application/DisplayControler24/display_controller.h>
#include <UserInterface/bt_lib_2.h>
#include <Application/images_list.h>
#include <Application/str_resources.h>
#include "SubActivitiesMain.h"
#include "Application/Utils/VarWatcher.h"
#include "ActivityMenu.h"
#include "activityAlarm.h"
#include "viewsMain.h"
#include <Application/Utils/main_message.h>
#include <Application/interfaces/dmd_interface.h>
#include <Application/Utils/model_manager.h>
#include <Application/str_resources.h>

#define _this DraftMain_localc_variables

DraftMain_localc_variables_t _this = {
	.state_led = DEV_STATE_DISABLED
};


#define if_changed(data) {static unsigned int static_remebering = 0xFFFF; static unsigned char first_time = 1; if (static_remebering != data || first_time){static_remebering = (data);first_time = 0;
#define end_if_changed }}

void ActivityMain_onCreate(void * param);
void ActivityMain_onDestroy(void * param);
void ActivityMain_onTick(void * param);

ActivityContext_t ActivityMain = newActivity(
		.onCreate = ActivityMain_onCreate,
		.onDestroy = ActivityMain_onDestroy,
		.onTick = ActivityMain_onTick);


status_message_t message_waiting_recorder = {
	.message = "",
	.type = ST_MESSAGE_ALTERNATING,
	.bg_color = LIGHTBLUEBG,
	.should_bip = 0,
};

status_message_t message_recorder_ok = {
	.message = "",
	.type = ST_MESSAGE_WARNING,
	.bg_color = LIGHTBLUEBG,
	.should_bip = 0,
};
status_message_t message_no_motor = {
	.message = "",
	.type = ST_MESSAGE_ALTERNATING,
	.bg_color = LIGHTBLUEBG,
	.should_bip = 0,
};
status_message_t message_fiber_desconected = {
	.message = "",
	.type = ST_MESSAGE_ALTERNATING, .bg_color = LIGHTBLUEBG,
	.should_bip = 0,
};
status_message_t message_fiber_conected = {
	.message = "",
	.type = ST_MESSAGE_WARNING,
	.bg_color = LIGHTBLUEBG,
	.should_bip = 0,
};
status_message_t message_recording_ok = {
	.message = "",
	.type = ST_MESSAGE_WARNING,
	.bg_color = LIGHTBLUEBG,
	.should_bip = 0,
};
status_message_t message_recording_error = {
	.message = "",
	.type = ST_MESSAGE_WARNING,
	.bg_color = YELLOW,
	.should_bip = 0,
};
status_message_t message_recording_error_memory = {
	.message = "",
	.type = ST_MESSAGE_WARNING,
	.bg_color = YELLOW,
	.should_bip = 0,
};
status_message_t message_recording = {
	.message = "",
	.type = ST_MESSAGE_ALTERNATING,
	.bg_color = LIGHTBLUEBG,
	.should_bip = 0,
};
status_message_t message_torque_limit = {
	.message = "",
	.type = ST_MESSAGE_WARNING,
	.bg_color = YELLOW,
	.should_bip = 0,
};
status_message_t message_motor_error = {
	.message = "",
	.type = ST_MESSAGE_WARNING,
	.bg_color = YELLOW,
	.should_bip = 0,
};
status_message_t message_release_footswitch = {
	.message = "",
	.type = ST_MESSAGE_PERSISTENT,
	.bg_color = YELLOW,
	.should_bip = 0,
};
status_message_t message_release_switch = {
	.message = "",
	.type = ST_MESSAGE_PERSISTENT,
	.bg_color = YELLOW,
	.should_bip = 0,
};
status_message_t message_error_current_led = {
	.message = "",
	.type = ST_MESSAGE_ALTERNATING,
	.bg_color = YELLOW,
	.should_bip = 0,
};
status_message_t message_erro_ventoinha = {
	.message = "",
	.type = ST_MESSAGE_ALTERNATING,
	.bg_color = YELLOW,
	.should_bip = 0,
};
status_message_t message_erro_temperatura = {
	.message = "",
	.type = ST_MESSAGE_ALTERNATING,
	.bg_color = YELLOW,
	.should_bip = 0,
};
status_message_t message_erro_fj_com = {
	.message = "",
	.type = ST_MESSAGE_ALTERNATING,
	.bg_color = YELLOW,
	.should_bip = 0,
};
status_message_t message_camera_off = {
	.message = "",
	.type = ST_MESSAGE_ALTERNATING,
	.bg_color = LIGHTBLUEBG,
	.should_bip = 0,
};
status_message_t message_funcao_janela = {
	.message = "",
	.type = ST_MESSAGE_PERSISTENT,
	.bg_color = LIGHTBLUEBG,
	.should_bip = 0,
};
status_message_t message_reinit_recording = {
	.message = "",
	.type = ST_MESSAGE_WARNING,
	.bg_color = YELLOW,
	.should_bip = 1,
};

void init_status_messages() {
	message_waiting_recorder.message = getRes(str_res_inicializando_gravador_gravao_indisponvel);
	message_recorder_ok.message = getRes(str_res_gravador_disponvel);
	message_no_motor.message = getRes(str_res_nenhum_motor_conectado);
	message_fiber_desconected.message = getRes(str_res_fibra_tica_desconectada);
	message_fiber_conected.message = "Fibra ótica conectada!";
	message_recording_ok.message = getRes(str_res_gravao_concluida_retire_a_memria_externa);
	message_recording_error.message = getRes(str_res_erro_de_gravao_verificar_conexes);
	message_recording_error_memory.message = getRes(str_res_erro_de_gravao_memria_indisponvel);
	message_recording.message = getRes(str_res_gravao_em_andamento);
	message_torque_limit.message = getRes(str_res_limite_de_torque_excedido_soltar_pedal);
	message_motor_error.message = getRes(str_res_erro_do_motor_soltar_pedal_e_verificar_conexo);
	message_release_footswitch.message = getRes(str_res_por_favor_solte_o_pedal);
	message_release_switch.message = "Por favor, solte o gatilho!";
	message_error_current_led.message = getRes(str_res_erro_de_corrente_fonte_de_luz_indisponvel);
	message_erro_ventoinha.message = getRes(str_res_erro_do_ventilador_fonte_de_luz_indisponvel);
	message_erro_temperatura.message = getRes(str_res_temperatura_muito_alta_fonte_de_luz_indisponvel);
	message_erro_fj_com.message = getRes(str_res_falha_de_comunicao_fonte_de_luz_indisponvel);
	message_camera_off.message = getRes(str_res_cmera_desligada_gravador_desabilitado);
	message_funcao_janela.message = getRes(str_res_funo_janela_ativada);
	message_reinit_recording.message = getRes(str_res_tempo_limite_de_gravao_atingindo_reiniciando);

}

void clickbt_voltar_sub(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		//		//		select_none();
		if ((!dmd_motor_flags()->motor_runing))
			ActivityMan_startSubActivity(&subActivityDefault);
	}
}

void clickbt_menu_voltar(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
        if ((!dmd_motor_flags()->motor_runing))
            ActivityMan_startActivity(&activityMenu);
		//		ActivityMan_startActivity(&activityMenuMenu);
	}
}

void clickbt_select_rec(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		//				select_recorder();
		//		if (is_error())
		//			return;
		kernel_message(MODE_DEBUG, "called rec\n");
		if ((!dmd_motor_flags()->motor_runing)) {
			if (_this.state_recorder != DEV_STATE_DISABLED)
				ActivityMan_startSubActivity(&subActivityGravador);
		}
	}
}

void clickbt_select_light(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		//		if (is_error())
		//			return;
		kernel_message(MODE_DEBUG, "called light\n");
		if ((!dmd_motor_flags()->motor_runing)) {
			if (_this.state_led != DEV_STATE_DISABLED)
				ActivityMan_startSubActivity(&subActivityLight);
			//					select_light_source();
		}
	}
}

void clickbt_select_motor(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		kernel_message(MODE_DEBUG, "called motor\n");
		if ((dmd_motor_flags()->motor_recognized && !dmd_motor_flags()->motor_runing))
			ActivityMan_startSubActivity(&subActivityVelocidade);
	}
}

void clickbt_select_cam(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		kernel_message(MODE_DEBUG, "called cam\n");
		if (ep_recording_status() == RECORDING_STARTED) {
			return;
		}
		if ((!dmd_motor_flags()->motor_runing)) {
			ActivityMan_startSubActivity(&subActivityCam);
		}
	}
}

static uint16_watcher_t velocity_watcher = NEW_UINT16_WATCHER_INI(0xFFFF);
static uint16_watcher_t led_potency_watcher = NEW_UINT16_WATCHER_INI(0xFFFF);
static uint16_watcher_t recog_watcher = NEW_UINT16_WATCHER_INI(0xFF);
static uint16_watcher_t motor_sel_watcher = NEW_UINT16_WATCHER_INI(0xFF);
static uint16_watcher_t direction_watcher = NEW_UINT16_WATCHER_INI(0xFF);
static uint16_watcher_t pedal_watcher = NEW_UINT16_WATCHER_INI(1);
static uint16_watcher_t pedal_set_watcher = NEW_UINT16_WATCHER_INI(0xFF);
static uint16_watcher_t motor_type_watcher = NEW_UINT16_WATCHER;
static uint16_watcher_t fj_error_watcher = NEW_UINT16_WATCHER;
static uint16_watcher_t fiber_conected_watcher = NEW_UINT16_WATCHER;
static uint16_watcher_t ep_pause_watcher = NEW_UINT16_WATCHER_INI(0xffff);
static uint16_watcher_t ep_state_watcher = NEW_UINT16_WATCHER_INI(0xffff);

void ActivityMain_onCreate(void * param) {
	// Codigo de inicializaÃ§Ã£o da atividade

	init_status_messages();
	
	models_t model = modelManager_getDataReaded()->equip_model;

	init_main_view(model);

	ClearDisplay(); // Clear the display

	ClearDisplayColor(LIGHTBLUEBG);

	velocity_watcher = (uint16_watcher_t) NEW_UINT16_WATCHER_INI(0xFFFF);
	led_potency_watcher = (uint16_watcher_t) NEW_UINT16_WATCHER_INI(0xFFFF);
	recog_watcher = (uint16_watcher_t) NEW_UINT16_WATCHER_INI(0xFF);
	motor_sel_watcher = (uint16_watcher_t) NEW_UINT16_WATCHER_INI(0xFF);
	direction_watcher = (uint16_watcher_t) NEW_UINT16_WATCHER_INI(0xFF);
	pedal_watcher = (uint16_watcher_t) NEW_UINT16_WATCHER_INI(1);
	pedal_set_watcher = (uint16_watcher_t) NEW_UINT16_WATCHER_INI(0xFF);
	motor_type_watcher = (uint16_watcher_t) NEW_UINT16_WATCHER_INI(0xFF);
	fj_error_watcher = (uint16_watcher_t) NEW_UINT16_WATCHER;
	fiber_conected_watcher = (uint16_watcher_t) NEW_UINT16_WATCHER;
	ep_pause_watcher = (uint16_watcher_t) NEW_UINT16_WATCHER_INI(0xffff);
	ep_state_watcher = (uint16_watcher_t) NEW_UINT16_WATCHER_INI(0xffff);

	_this.bt_select_light = allocAsTouchable;
	_this.bt_select_cam = allocAsTouchable;
	_this.bt_select_rec = allocAsTouchable;
	_this.bt_menu = allocAsTouchable;
	_this.bt_voltar_sub = allocAsTouchable;
	_this.bt_select_motor = allocAsTouchable;

	if (_this.pwm_perc < 10) {
		_this.pwm_perc = 10;
	}

	write_delims();

	if (model != MODEL_TRIPLET && model != MODEL_SHAVER) {
		write_title(
				model == MODEL_DUET ? getRes(str_res_duet) :
				model == MODEL_COOL_LIGHT ? getRes(str_res_cool_light) :
				model == MODEL_SMART_CAM ? getRes(str_res_smart_cam) :
				getRes(str_res_equipamento_desconhecido)
				);
	}


	if (model == MODEL_TRIPLET || model == MODEL_SHAVER) {
		write_initial_motor_window();

		display_box_t box = model == MODEL_TRIPLET ? SQUARE_T1 : SQUARE_SH1;

		setAsButton(_this.bt_select_motor,
				.text = "",
				.coord_x = box.left_pos,
				.coord_y = box.top_pos,
				.height = box.height,
				.length = box.length,
				.blink = 0,
				.onClick = clickbt_select_motor
				);
		//	bt_lib_2_drawTouchableButton(_this.bt_select_motor);

		ActivityMan_insertTouchable(_this.bt_select_motor);

	}

	if (model == MODEL_TRIPLET ||
			model == MODEL_DUET ||
			model == MODEL_COOL_LIGHT) {

		display_box_t box = model == MODEL_TRIPLET ? SQUARE_T2 :
				model == MODEL_DUET ? SQUARE_D1 : SQUARE_T2
				;

		setAsButton(_this.bt_select_light,
				.text = "",
				.coord_x = box.left_pos,
				.coord_y = box.top_pos,
				.height = box.height,
				.length = box.length,
				.blink = 0,
				.onClick = clickbt_select_light
				);
		//	bt_lib_2_drawTouchableButton(_this.bt_select_light);

		if (model != MODEL_COOL_LIGHT) // No need for selection
			ActivityMan_insertTouchable(_this.bt_select_light);

		draw_light_state(_this.state_led, 1);

		write_light_square();

	}

	if (model == MODEL_TRIPLET ||
			model == MODEL_DUET ||
			model == MODEL_SMART_CAM) {

		display_box_t box = model == MODEL_TRIPLET ? SQUARE_T3 :
				model == MODEL_DUET ? SQUARE_D2 :
				model == MODEL_SMART_CAM ? SQUARE_SC1 : SQUARE_T3
				;

		setAsButton(_this.bt_select_cam,
				.text = "",
				.coord_x = box.left_pos,
				.coord_y = box.top_pos,
				.height = box.height,
				.length = box.length,
				.blink = 0,
				.onClick = clickbt_select_cam
				);
		//	bt_lib_2_drawTouchableButton(*bt_select_cam);
		ActivityMan_insertTouchable(_this.bt_select_cam);

		display_box_t boxr = model == MODEL_TRIPLET ? SQUARE_T4 :
				model == MODEL_DUET ? SQUARE_D3 :
				model == MODEL_SMART_CAM ? SQUARE_SC2 : SQUARE_T4
				;

		setAsButton(_this.bt_select_rec,
				.text = "",
				.coord_x = boxr.left_pos,
				.coord_y = boxr.top_pos,
				.height = boxr.height,
				.length = boxr.length,
				.blink = 0,
				.onClick = clickbt_select_rec
				);
		//	bt_lib_2_drawTouchableButton(*bt_select_rec);
		ActivityMan_insertTouchable(_this.bt_select_rec);

		draw_cam_state(_this.state_camera);

		draw_recorder_state(_this.state_recorder, 0);

		write_cam_square();

		write_rec_square();

	}

	setAsButton(_this.bt_menu,
			.text = "",
			.coord_x = 0,
			.coord_y = 172,
			.height = 45,
			.length = 50,
			.onDraw = draw_bt_menu,
			.onClick = clickbt_menu_voltar
			);
	draw_bt_menu(_this.bt_menu, 0);
	//	bt_lib_2_drawTouchableButton(*bt_menu);
	ActivityMan_insertTouchable(_this.bt_menu);


	setAsButton(_this.bt_voltar_sub,
			.text = getRes(str_voltar),
			.coord_x = 0,
			.coord_y = 172,
			.height = 47,
			.length = 50,
			.color = BLUEBTBAR,
			.img_idx_pressed = IMG_BACK_ARROW_PRESSED,
			.img_idx_unpress = IMG_BACK_ARROW_UNPRESSED,
			.reference_txt = CENTER,
			.txt_font = VERDANA7MONO,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.onClick = clickbt_voltar_sub
			);

	center_img_on_button(_this.bt_voltar_sub);
	_this.bt_voltar_sub->y_img -= 0;
	_this.bt_voltar_sub->y_txt = 15;

	if (model == MODEL_COOL_LIGHT) {
		ActivityMan_startSubActivity(&subActivityLight);
	} else {
		ActivityMan_startSubActivity(&subActivityDefault);
	}
	init_message_manager();

}

#include "Application/Utils/VarWatcher.h"
#include "UserInterface/Buzzer.h"

void verify_and_update_motor() {
	motor_flags_t * mflags;
	board_flags_t * bflags;

	bflags = dmd_board_flags();
	mflags = dmd_motor_flags();
	mem_data_exchange_t * info;
	info = dmd_motor_info();

	//	mflags.motor_recognized

	uint16_watcher_update(&recog_watcher, (uint16_t) mflags->motor_recognized);
	uint16_watcher_update(&motor_type_watcher, (uint16_t) info->type);
	uint16_watcher_update(&motor_sel_watcher, (uint16_t) bflags->selected_motor);

	if (recog_watcher.has_changed || motor_sel_watcher.has_changed) {
        _this.safe_disable_til_next = 0;
		if (mflags->motor_recognized) {
			write_motor_title(info->type, bflags->selected_motor);
			_this.velocity = info->min_vel;
		} else {
			//			write_motor_title(0xFF, 1);
			write_motor_desconected();
			_this.velocity = 0;
			dmd_hab_motor(info->min_vel, 0, 0);
		}
	}

	// Escrever motor quando modificado o tipo
	if (motor_type_watcher.has_changed) {
		if (mflags->motor_recognized) {
			write_motor_title(info->type, dmd_board_flags()->selected_motor);
			_this.velocity = info->min_vel;
            
		}

	}
    
	uint16_watcher_update(&velocity_watcher, (uint16_t) _this.velocity);
    
	if (velocity_watcher.has_changed) {
        
		uint16_t perc = _this.velocity == 0 ? 0 : (_this.velocity - info->min_vel)*100L / (info->max_vel - info->min_vel);
		draw_velocity(_this.velocity, perc);
        
	}
    
	enum DMD_HAB_DIRECTION_e {
		FOWARD, BACK, OSCILANT, WINDOW, NOT_RUNNING
	};
    
	uint16_t direction_to_run = NOT_RUNNING;

	uint16_t pedal_to_run = 0;

	if (!info->has_gat) {

		flags_pedal_t * fpedal = fj_flags_pedal();

		if (!fpedal->pedal_botao) {
			// Modo para set de velocidade
			uint16_watcher_update(&pedal_set_watcher, fpedal->pedal_direito + (fpedal->pedal_esquerdo << 1));

			if (pedal_set_watcher.has_changed) {

				if (fpedal->pedal_direito) {
					clickbtVelocityUpDown(EVENT_PRESS, 1, (void*) 0);
				} else if (fpedal->pedal_esquerdo) {
					clickbtVelocityUpDown(EVENT_PRESS, 0, (void*) 0);
				}

			}
			if (fpedal->pedal_direito) {
				clickbtVelocityUpDown(EVENT_STILLPRESS, 1, (void*) 0);
			} else if (fpedal->pedal_esquerdo) {
				clickbtVelocityUpDown(EVENT_STILLPRESS, 0, (void*) 0);
			}
		}

		uint16_watcher_update(&pedal_watcher, (uint16_t) ((fpedal->pedal_botao && (fpedal->pedal_central |
				fpedal->pedal_esquerdo |
				fpedal->pedal_direito)) |
				(!fpedal->pedal_botao && (fpedal->pedal_central))));

		pedal_to_run = pedal_watcher.current_value;
        

		uint16_watcher_update(&direction_watcher, (uint16_t) !pedal_to_run ? NOT_RUNNING : // se o pedal não está requerendo ligar não interessa
				(fpedal->pedal_botao ? //pedal botão em modo de controle
				(fpedal->pedal_direito ? FOWARD : (fpedal->pedal_esquerdo ? BACK : OSCILANT)) :
				WINDOW)
				);

		static uint16_t lock_footswitch = 0;

		if (lock_footswitch) {
			pedal_to_run = 0;
			if (!pedal_watcher.current_value) {
				lock_footswitch = 0;
			}
		} else {

			if (direction_watcher.has_changed && pedal_watcher.current_value && !pedal_watcher.has_changed) {
				sys_buzz(100, 10);
				status_message_insert(&message_release_footswitch);
				pedal_to_run = 0;
				lock_footswitch = 1;
			} else {
				status_message_remove(&message_release_footswitch);
			}

		}

		direction_to_run = direction_watcher.current_value;

	} else {
        static unsigned int counter_error = 0;
        
        if ((mflags->dir_active || mflags->rev_active) && !mflags->motor_runing){
            counter_error++;
            
            if (counter_error > 2){
//                sys_buzz(100, 10);
				
                status_message_insert(&message_release_switch);
            }
        } else {
            counter_error = 0;
            status_message_remove(&message_release_switch);
        }
    }


	// Limita velocidade de oscilação

	static unsigned int return_velocity = 0;

	if (!pedal_to_run && return_velocity) {

		if (return_velocity != _this.velocity) {
			_this.velocity = return_velocity;
		}
		return_velocity = 0;
	}
    
    if (
            !(info->id == M_ID_MINISHAVER || info->id == M_ID_SHAVER_PORTESCAP || info->id == M_ID_SHAVER)
                                        &&  
            (direction_to_run == OSCILANT || direction_to_run == WINDOW )
            )
    {
        pedal_to_run = 0;
    }


	if (direction_to_run == OSCILANT && pedal_to_run && _this.velocity > 3000) {
		return_velocity = _this.velocity;
		_this.velocity = 3000;
	}
    

	dmd_hab_motor(_this.velocity,
			(mflags->motor_recognized && info->has_gat && !_this.safe_disable_til_next)  | (pedal_to_run),
			direction_to_run);



	enum MOTOR_DIRECTION_e direct = pedal_to_run ? direction_to_run + 1 : DIRECTION_NONE;

	if (info->has_gat) {
		direct = dmd_motor_flags()->dir_active == 1 ?
				(dmd_motor_flags()->rev_active == 1 ? DIRECTION_OSC : DIRECTION_FORW) :
				(dmd_motor_flags()->rev_active == 1 ? DIRECTION_BACK : DIRECTION_NONE);
	}

	if_changed((mflags->motor_runing << 3) + direct) {

		if (!mflags->motor_runing) {
			direct = DIRECTION_NONE;
		}

		draw_direction(direct);
	}
	end_if_changed;

	if (mflags->motor_runing && ActivityMain.curr_subactivity != &subActivityDefault) {

		ActivityMan_startSubActivity(&subActivityDefault);

	}

	// Habilitar motores com gatilho
	//	if_changed(info.has_gat && mflags.motor_recognized) {
	//		
	//		if (mflags.motor_recognized && info.has_gat) {	
	//			dmd_hab_motor(info.min_vel,1,0);
	//			
	//		}
	//	}
	//	end_if_changed;


}

void ActivityMain_onDestroy(void * param) {
	// CÃ³digo a ser executado quando a atividade for finalizada

	desalocTouchable(_this.bt_menu);
	desalocTouchable(_this.bt_select_cam);
	desalocTouchable(_this.bt_select_light);
	desalocTouchable(_this.bt_select_motor);
	desalocTouchable(_this.bt_select_rec);
	desalocTouchable(_this.bt_voltar_sub);

	stop_message_manager();

}

void verify_and_update_led() {

	models_t model = modelManager_getDataReaded()->equip_model;

	uint16_watcher_update(&led_potency_watcher, (uint16_t) _this.pwm_perc);

	if (led_potency_watcher.has_changed) {
		draw_light_perc(_this.pwm_perc);
	}

	unsigned int ledon = fj_is_led_on();

	//	if_changed(ledon) {

	if (ledon) {
		_this.state_led = DEV_STATE_ON;
	} else {
		_this.state_led = DEV_STATE_OFF;
	}
	//		draw_light_state(_this.state_led, 0);

	if (!fj_is_initialized() || !fj_is_fiber_conn()) {
		_this.state_led = DEV_STATE_DISABLED;
	}

	//	}
	//	end_if_changed

	fj_error_flags_t fj_error = fj_get_error_flags();

	//	uint16_watcher_update(&fj_error_watcher, *((uint16_t*) &fj_error));
	//	
	//	if (fj_error_watcher.has_changed){

	if (fj_is_initialized() && fj_is_in_error()) { //error fj

		_this.state_led = DEV_STATE_DISABLED;
		//			draw_light_state(_this.state_led, 0);

		if (fj_error.error_com) {
			status_message_insert(&message_erro_fj_com);
		} else {
			status_message_remove(&message_erro_fj_com);
		}

		if (fj_error.error_current_led) {
			status_message_insert(&message_error_current_led);
		} else {
			status_message_remove(&message_error_current_led);
		}

		if (fj_error.error_fan) {
			status_message_insert(&message_erro_ventoinha);
		} else {
			status_message_remove(&message_erro_ventoinha);
		}

		if (fj_error.error_temperature_led) {
			//				status_message_insert(&message_erro_temperatura);
			activityAlarm_call(getRes(str_res_erro_de_temperatura_do_driver), fj_is_in_error);
		} else {
			//				status_message_remove(&message_erro_temperatura);
		}

		//			if (fj_error.error_temperature_led){
		//				status_message_insert(&message_erro_temperatura);
		//			} else {
		//				status_message_remove(&message_erro_temperatura);
		//			}

	} else {

		status_message_remove(&message_erro_fj_com);
		status_message_remove(&message_erro_ventoinha);
		status_message_remove(&message_error_current_led);
		status_message_remove(&message_erro_temperatura);

	}

	//	}

	if (!fj_is_in_error() && fj_is_initialized()) {

		uint8_t is_fiber_conn = fj_is_fiber_conn();

		uint16_watcher_update(&fiber_conected_watcher, is_fiber_conn);

		if (fiber_conected_watcher.has_changed) {

			if (!is_fiber_conn) {
				fj_set_led_on_off(0);
				status_message_insert(&message_fiber_desconected);

				if (ActivityMain.curr_subactivity == &subActivityLight) {
					ActivityMan_startSubActivity(&subActivityDefault);
				}

			} else {

				status_message_remove(&message_fiber_desconected);
				status_message_insert(&message_fiber_conected);
				if (model == MODEL_COOL_LIGHT) {
					if (ActivityMain.curr_subactivity != &subActivityLight) {
						ActivityMan_startSubActivity(&subActivityLight);
					}
				}

			}

		}
	}

	draw_light_state(_this.state_led, 0);
}

void verify_and_update_cam_recorder() {

	unsigned int is_zdvr_init = (ep_is_initialized() << 1) + fj_cam_is_on();

	if_changed(is_zdvr_init) {

		if (ep_is_initialized()) {
			status_message_remove(&message_waiting_recorder);

			if (fj_cam_is_on()) {
				status_message_insert(&message_recorder_ok);
				status_message_remove(&message_camera_off);
				_this.state_recorder = DEV_STATE_OFF;
			} else {
				status_message_insert(&message_camera_off);
				_this.state_recorder = DEV_STATE_DISABLED;
			}
		} else {
			status_message_insert(&message_waiting_recorder);

			_this.state_recorder = DEV_STATE_DISABLED;
		}

	}
	end_if_changed

	uint16_t recording_status = ep_recording_status();

	if_changed(recording_status) {

		if (recording_status == RECORDING_INITIALIZED) {
			_this.state_recorder = DEV_STATE_PARCIAL;
		} else if (recording_status == RECORDING_STARTED) {
			_this.state_recorder = DEV_STATE_ON;
		} else if (_this.state_recorder != DEV_STATE_DISABLED) {
			_this.state_recorder = DEV_STATE_OFF;
		}

	}

	end_if_changed

	board_z3_status_t * flags = ep_z3_flags();

	uint16_watcher_update(&ep_pause_watcher, flags->f_pausado);
	uint16_watcher_update(&ep_state_watcher, _this.state_recorder);

	if (ep_pause_watcher.has_changed || ep_state_watcher.has_changed) {
		draw_recorder_state(_this.state_recorder, flags->f_pausado);
	}

	static uint16_t ct_update = 0;
	if (ct_update % 2 == 0) {
		if (ep_recording_status() == RECORDING_STARTED) {
			draw_rec_time(DEV_STATE_ON, ep_recording_time());
		}
	}

	ct_update++;

	if_changed(_this.state_camera) {

		draw_cam_state(_this.state_camera);

	}

	end_if_changed

	if (ep_was_safe_stoped()) {
		kernel_message(MODE_DEBUG, "SAFE STOPED DETECTED!\n");
		//		if (!status_message_is_inserted(&message_reinit_recording)) {
		status_message_insert(&message_reinit_recording);
		//		}
	} else {
		status_message_remove(&message_reinit_recording);
	}


}

void ActivityMain_onTick(void * param) {

	//	scheduler_tick(&_this.my_scheduler);

	models_t model = modelManager_getDataReaded()->equip_model;

	if (model != MODEL_COOL_LIGHT) {

		if_changed((unsigned int) ActivityMain.curr_subactivity) {

			if (ActivityMain.curr_subactivity != &subActivityDefault) {
				kernel_message(MODE_DEBUG, "Its not default!\n");
				ActivityMan_excludeTouchable(_this.bt_menu);
				ActivityMan_excludeTouchable(_this.bt_voltar_sub);
				bt_lib_2_drawTouchableButton(*_this.bt_voltar_sub);
				ActivityMan_insertTouchable(_this.bt_voltar_sub);
			} else {
				ActivityMan_excludeTouchable(_this.bt_menu);
				ActivityMan_excludeTouchable(_this.bt_voltar_sub);
				draw_bt_menu(_this.bt_menu, 0);
				ActivityMan_insertTouchable(_this.bt_menu);
			}
		}

		end_if_changed

	}


	if (model == MODEL_TRIPLET ||
			model == MODEL_SHAVER)
		verify_and_update_motor();

	if (model == MODEL_TRIPLET ||
			model == MODEL_COOL_LIGHT ||
			model == MODEL_DUET)
		verify_and_update_led();

	if (model == MODEL_TRIPLET ||
			model == MODEL_SMART_CAM ||
			model == MODEL_DUET)
		verify_and_update_cam_recorder();

}

void scope_watch_and_draw() {



}

