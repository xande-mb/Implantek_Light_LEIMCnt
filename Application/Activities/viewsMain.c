#include "viewsMain.h"


#include <Application/DisplayControler24/tft_driver.h>
#include <Application/DisplayControler24/display_controller.h>
#include "ActivityMain.h"
#include <Application/images_list.h>
#include <Application/str_resources.h>
#include <CTTKernel/kernel.h>
#include "Application/Utils/VarWatcher.h"
#include "Application/Utils/model_manager.h"

typedef struct {
	unsigned int top;
	unsigned int left;
	unsigned int right;
	unsigned int bottom;

} view_box_t;


#define IS_TRIPLET (current_model == MODEL_TRIPLET)
#define IS_DUET (current_model == MODEL_DUET)
#define IS_SMART_CAM (current_model == MODEL_SMART_CAM)
#define IS_COOL_LIGHT (current_model == MODEL_COOL_LIGHT)

static models_t current_model = MODEL_TRIPLET;


// FUNÇÔES GENÉRICAS

void draw_border(uint16_t x_pos, uint16_t y_pos, uint16_t length, uint16_t height, WORD color) {

	SetColor(color);
	WAIT_UNTIL_FINISH(Bar(x_pos, y_pos, x_pos + length, y_pos));
	WAIT_UNTIL_FINISH(Bar(x_pos + length, y_pos, x_pos + length, y_pos + height));
	WAIT_UNTIL_FINISH(Bar(x_pos, y_pos + height, x_pos + length, y_pos + height));
	WAIT_UNTIL_FINISH(Bar(x_pos, y_pos, x_pos, y_pos + height));

}

void init_main_view(models_t model) {
	current_model = model;
}

void draw_display_progress_bar(uint16_t x_pos, uint16_t y_pos, uint16_t length, uint16_t percentage) {

	float frac = (length - 5)*1.0 / 100;
	uint16_t height = 9;
	SetColor(LIGHTBLUEBG);
	WAIT_UNTIL_FINISH(Bar(x_pos, y_pos, x_pos + length, y_pos + height));
	SetColor(BLUE);
	WAIT_UNTIL_FINISH(Bar(x_pos, y_pos + 2, x_pos + length - 5, y_pos + height - 2));
	SetColor(WHITE);
	WAIT_UNTIL_FINISH(Bar(x_pos + percentage * frac, y_pos + 3, x_pos + length - 6, y_pos + height - 3));
	SetColor(BLACK);
	WAIT_UNTIL_FINISH(Bar(x_pos + percentage * frac, y_pos, x_pos + percentage * frac + 2, y_pos + height));
	SetColor(YELLOW);
	WAIT_UNTIL_FINISH(Bar(x_pos + percentage * frac + 1, y_pos + 1, x_pos + percentage * frac + 1, y_pos + height - 1));
}

void select_none() {

	borda_light(LIGHTBLUEBG);
	borda_motor(LIGHTBLUEBG);
	borda_cam(LIGHTBLUEBG);
	borda_grav(LIGHTBLUEBG);

}

void draw_bt_menu(touchable_t* touchable, unsigned char isPressed) {
	if (isPressed) {
		SetColor(CYANBTBAR);
		WAIT_UNTIL_FINISH(Bar(0, 172, 50, 219));
		place_disp(9, 177, IMG_NEW_MENU_ON);
		SetColor(WHITE);
		display_set_font(VERDANA7MONO);
		display_print_text(getRes(str_menuPrincipal_menu), 25, 206, CENTER, 0); //, BRIGHTBLUE, FONT_VERDANA7_CENTER_HOR_WHITE_TRANSP);
	} else {
		SetColor(BLUEBTBAR);
		WAIT_UNTIL_FINISH(Bar(0, 172, 50, 219));
		place_disp(9, 177, IMG_NEW_MENU_OFF);
		SetColor(WHITE);
		display_set_font(VERDANA7MONO);
		display_print_text(getRes(str_menuPrincipal_menu), 25, 206, CENTER, 0); //, FONT_VERDANA7_CENTER_HOR_WHITE_TRANSP);
	}
}

void write_title(char * title) {
	
	SetColor(BLUEBTBAR);
	
	WAIT_UNTIL_FINISH(
			Bar(1,
			1,
			IS_TRIPLET ? 198 : DISP_HOR_RESOLUTION - 2,
			25));
	
	SetColor(WHITE);
	display_set_font(VERDANA10NMONO);
	display_print_text(title, 10, 5, LEFT, 0);
	
}

void write_delims() {
	
	if (IS_TRIPLET || IS_DUET) {

		// delim lateral
		SetColor(LIGHTBLUEDELIMS);
		WAIT_UNTIL_FINISH(Bar
				(
				199, IS_TRIPLET ? 1 : 26, 200, 170
				));
				
		// delim entre luz e cam no triplet
		SetColor(LIGHTBLUEDELIMS);
		WAIT_UNTIL_FINISH(
				Bar(202,
				IS_TRIPLET ? 74 : 26,
				318,
				IS_TRIPLET ? 75 : 27
				));
		
		// delim entre cam e gravador (Triplet e Duet))
		
		SetColor(LIGHTBLUEDELIMS);
		int16_t offset_y = IS_TRIPLET ? 0 : -25;
		WAIT_UNTIL_FINISH(Bar(202, 126 + offset_y, 318, 127 + offset_y));
		
	} else if (IS_SMART_CAM) {
		
		display_box_t box = SQUARE_SC1;
		draw_border(box.left_pos-1, box.top_pos-1, box.length+2, box.height+2, LIGHTBLUEDELIMS);
		box = SQUARE_SC2;
		draw_border(box.left_pos-1, box.top_pos-1, box.length+2, box.height+2, LIGHTBLUEDELIMS);

	}

	// Barra de botões
	SetColor(BLUEBTBAR);
	WAIT_UNTIL_FINISH(Bar(0, 172, 319, 220));

}


// FUNÇÔES MOTOR

void borda_triplet_motor(WORD color) {

	display_box_t box = SQUARE_T1;
	draw_border(box.left_pos, box.top_pos, box.length, box.height, color);

}

void borda_motor(WORD color) {
	if (IS_TRIPLET) borda_triplet_motor(color);
}

void select_motor() {
	borda_light(LIGHTBLUEBG);
	borda_motor(YELLOW);
	borda_cam(LIGHTBLUEBG);
	borda_grav(LIGHTBLUEBG);
}

void draw_velocity(unsigned int vel_to_write, unsigned int velocity_perc) {
	
	char buffer[30];
	SetColor(LIGHTBLUEBG);
	WAIT_UNTIL_FINISH(Bar(30, 55, 155, 75));
	sprintf(buffer, "%u", vel_to_write);
	SetColor(BLACK);
	SetBgColor(LIGHTBLUEBG);
	display_set_font(M39_18MONO);
	display_print_text(buffer, 
			IS_TRIPLET ? 165 : 
				108, 
			55, 
			RIGHT, 80); //, BLACK, FONT_M39_18_RIGHT_HOR_BLACK_TRANSP);

	draw_display_progress_bar(IS_TRIPLET ? 20 : 77, 79, 165, velocity_perc);

}


void place_direction_images(uint16_t img_rev, uint16_t img_osc, uint16_t img_fwrd){
	
	place_disp(IS_TRIPLET ? 15 : 72, 115, img_rev);
	place_disp(IS_TRIPLET ? 75 : 132, 115, img_osc);
	place_disp(IS_TRIPLET ? 135 : 192, 115, img_fwrd);	

}


void draw_direction(direction_of_motor_t direction) {
	
	place_direction_images(	direction == DIRECTION_BACK ? IMG_REVERSE_ON : IMG_REVERSE_OFF,
							direction == DIRECTION_OSC ? IMG_OSCILLATE_ON : IMG_OSCILLATE_OFF,
							direction == DIRECTION_FORW ? IMG_FORWARD_ON : IMG_FORWARD_OFF
		);

}

void write_motor_desconected() {

	write_title(getRes(str_res_motor_desconectado));
}

void write_motor_title(enum motor_type_e type, uint8_t motor_number) {

	kernel_message(MODE_DEBUG, "write motor %u\n", type);

	char* motor_name = type == M_TYPE_MICRO ? getRes(str_res_micromotor) :
			type == M_TYPE_SHAVER ? getRes(str_motor0x40) :
			type == M_TYPE_MINI_SHAVER ? getRes(str_res_mini_shaver) :
			type == M_TYPE_INJECTOR ? getRes(str_res_injetor) :
			type == M_TYPE_PERF ? getRes(str_motor0x31) : getRes(str_motor_desconhecido);
	char* fmt = motor_number == 1 ? getRes(str_res_m1_s) : getRes(str_res_m2_s);
	char buffer[40];
	sprintf(buffer, fmt, motor_name);

	write_title(buffer);

}

void write_initial_motor_window() {

	SetColor(BLACK);
	display_set_font(VERDANA8MONO);
	display_print_text(getRes(str_res_velocidade), 
			IS_TRIPLET ? 20 : 77, 35, LEFT, 0);

	//		draw_velocity(5);
	display_print_text(getRes(str_res_rpm), IS_TRIPLET ? 140 : 83, 65, RIGHT, 0);

	display_print_text(getRes(str_res_sentido), IS_TRIPLET ? 20 : 77, 100, LEFT, 0);

	place_disp(IS_TRIPLET ? 15 : 72, 115, IMG_REVERSE_OFF);
	place_disp(IS_TRIPLET ? 75 : 132, 115, IMG_OSCILLATE_OFF);
	place_disp(IS_TRIPLET ? 135 : 192, 115, IMG_FORWARD_OFF);

}


// FUNÇÕES LED

void borda_triplet_light(WORD color) {

	display_box_t box = SQUARE_T2;
	draw_border(box.left_pos, box.top_pos, box.length, box.height, color);

}

void borda_duet_light(WORD color) {

	display_box_t box = SQUARE_D1;
	draw_border(box.left_pos, box.top_pos, box.length, box.height, color);

}

void borda_light(WORD color) {

	if (IS_TRIPLET) borda_triplet_light(color);
	if (IS_DUET) borda_duet_light(color);

}

void select_light_source() {
	borda_light(YELLOW);
	borda_motor(LIGHTBLUEBG);
	borda_cam(LIGHTBLUEBG);
	borda_grav(LIGHTBLUEBG);
}

void place_light_state(uint16_t sq_led_img, uint16_t light_source_img) {

	if (IS_TRIPLET) {

		place_disp(211, 20, light_source_img);
		place_disp(285, 3, sq_led_img);

	} else if (IS_DUET) {

		place_disp(35, 75, light_source_img);

	} else if (IS_COOL_LIGHT) {

		place_disp(35 + 57, 75, light_source_img);

	}

}

void draw_light_state(state_of_device_t state, uint8_t force) {

	static uint16_watcher_t led_state = NEW_UINT16_WATCHER;

	if (force) {
		led_state = (uint16_watcher_t) NEW_UINT16_WATCHER_INI(0xFF);
	}

	uint16_watcher_update(&led_state, state);

	if (led_state.has_changed) {

		if (state == DEV_STATE_ON) {
			place_light_state(IMG_SQUARED_LED_ON, IMG_LIGHT_SOURCE_ON);
			//			place_disp(211, 20, IMG_LIGHT_SOURCE_ON);
			//			place_disp(285, 3, IMG_SQUARED_LED_ON);
		} else if (state == DEV_STATE_OFF) {
			place_light_state(IMG_SQUARED_LED_OFF, IMG_LIGHT_SOURCE_OFF);
			//			place_disp(211, 20, IMG_LIGHT_SOURCE_OFF);
			//			place_disp(285, 3, IMG_SQUARED_LED_OFF);
		} else if (state == DEV_STATE_DISABLED) {
			place_light_state(IMG_SQUARED_LED_DIS, IMG_LIGHT_SOURCE_OFF);
			//			place_disp(211, 20, IMG_LIGHT_SOURCE_OFF);
			//			place_disp(285, 3, IMG_SQUARED_LED_DIS);
		}

	}
}

void draw_light_perc_duet(unsigned char perc_to_write) {

	char buffer[30];
	//	SetColor(LIGHTBLUEBG);
	//	WAIT_UNTIL_FINISH(Bar(30, 55, 155, 75));
	sprintf(buffer, "%u", perc_to_write);
	SetColor(BLACK);
	SetBgColor(LIGHTBLUEBG);
	display_set_font(M39_18MONO);
	display_print_text(buffer, 320 - 155, 55 + 40, RIGHT, 80); //, BLACK, FONT_M39_18_RIGHT_HOR_BLACK_TRANSP);

	draw_display_progress_bar(20, 79 + 50, 165, perc_to_write);

}

void draw_light_perc_triplet(unsigned char perc_to_write) {
	char buffer[30];
	SetColor(LIGHTBLUEBG);
	WAIT_UNTIL_FINISH(Bar(DISP_HOR_RESOLUTION - 50, 16, DISP_HOR_RESOLUTION - 5, 30));
	sprintf(buffer, "%u%%", perc_to_write);
	SetColor(BLACK);
	display_set_font(VERDANA8MONO);
	display_print_text(buffer, 5, 16, RIGHT, 0); //, BLACK, FONT_VERDANA8_RIGHT_HOR_BLACK_TRANSP);
}

void draw_light_perc_cool_light(unsigned char perc_to_write) {

	char buffer[30];
	sprintf(buffer, "%u", perc_to_write);
	SetColor(BLACK);
	SetBgColor(LIGHTBLUEBG);
	display_set_font(M39_18MONO);
	display_print_text(buffer, 320 - 155 - 57, 55 + 40, RIGHT, 80); //, BLACK, FONT_M39_18_RIGHT_HOR_BLACK_TRANSP);

	draw_display_progress_bar(20 + 57, 79 + 50, 165, perc_to_write);

}

void draw_light_perc(unsigned char perc_to_write) {
	if (IS_TRIPLET) draw_light_perc_triplet(perc_to_write);
	if (IS_DUET) draw_light_perc_duet(perc_to_write);
	if (IS_COOL_LIGHT) draw_light_perc_cool_light(perc_to_write);
}

void write_light_square() {

	display_set_font(VERDANA8MONO);
	SetColor(LIGHTBLUEDELIMS);

	if (IS_TRIPLET) {

		display_print_text(getRes(str_res_fonte_de_luz), 205, 3, LEFT, 0);

	} else if (IS_DUET) {

		display_print_text(getRes(str_res_fonte_de_luz), 25, 45, LEFT, 0);
		SetColor(BLACK);
		display_print_text("%", 320 - 180, 65 + 40, RIGHT, 0);

	} else if (IS_COOL_LIGHT) {
		SetColor(BLACK);
		display_print_text(getRes(str_res_fonte_de_luz), 25 + 57, 45, LEFT, 0);

		display_print_text("%  ", 320 - 180 - 57, 65 + 40, RIGHT, 0);

	}

}

// FUNÇÕES CAMERA

void borda_triplet_cam(WORD color) {

	display_box_t box = SQUARE_T3;
	draw_border(box.left_pos, box.top_pos, box.length, box.height, color);

}

void borda_duet_cam(WORD color) {

	display_box_t box = SQUARE_D2;
	draw_border(box.left_pos, box.top_pos, box.length, box.height, color);

}

void borda_scam_cam(WORD color) {

	display_box_t box = SQUARE_SC1;
	draw_border(box.left_pos, box.top_pos, box.length, box.height, color);

}

void borda_cam(WORD color) {

	if (IS_TRIPLET) borda_triplet_cam(color);
	if (IS_DUET) borda_duet_cam(color);
	if (IS_SMART_CAM) borda_scam_cam(color);

}

void select_cam() {
	borda_light(LIGHTBLUEBG);
	borda_motor(LIGHTBLUEBG);
	borda_cam(YELLOW);
	borda_grav(LIGHTBLUEBG);
}

void write_cam_square() {

	display_set_font(VERDANA8MONO);
	SetColor(LIGHTBLUEDELIMS);

	uint16_t text_x =
			IS_TRIPLET ? 205 :
			IS_DUET ? 210 :
			35; // smart_cam

	uint16_t text_y =
			IS_TRIPLET ? 76 :
			IS_DUET ? 34 :
			72; // smart_cam

	display_print_text(getRes(str_res_cmera),
			text_x,
			text_y,
			LEFT,
			0
			);

}

static void place_cam_imgs(uint16_t cam_img, uint16_t sq_img) {

	place_disp(IS_TRIPLET ? 285 :
			IS_DUET ? 285 :
			110,
			IS_TRIPLET ? 78 :
			IS_DUET ? 36 :
			74,
			sq_img);

	place_disp(
			IS_TRIPLET ? 205 :
			IS_DUET ? 210 :
			35,
			IS_TRIPLET ? 98 :
			IS_DUET ? 58 :
			98,
			cam_img);

}

void draw_cam_state(state_of_device_t state) {

	uint16_t cam_img = state == DEV_STATE_ON ? IMG_CAMERA_ON : IMG_CAMERA_OFF;
	uint16_t sq_img = state == DEV_STATE_ON ? IMG_SQUARED_LED_ON : IMG_SQUARED_LED_OFF;

	place_cam_imgs(cam_img, sq_img);

}

// FUNÇÕES GRAVADOR

void borda_triplet_grav(WORD color) {

	display_box_t box = SQUARE_T4;
	draw_border(box.left_pos, box.top_pos, box.length, box.height, color);

}

void borda_duet_grav(WORD color) {

	display_box_t box = SQUARE_D3;
	draw_border(box.left_pos, box.top_pos, box.length, box.height, color);


}

void borda_scam_grav(WORD color) {

	display_box_t box = SQUARE_SC2;
	draw_border(box.left_pos, box.top_pos, box.length, box.height, color);

}

void borda_grav(WORD color) {

	if (IS_TRIPLET) borda_triplet_grav(color);
	if (IS_DUET) borda_duet_grav(color);
	if (IS_SMART_CAM) borda_scam_grav(color);

}

void select_recorder() {
	borda_light(LIGHTBLUEBG);
	borda_motor(LIGHTBLUEBG);
	borda_cam(LIGHTBLUEBG);
	borda_grav(YELLOW);
}

void put_text_grav(char * text) {
	SetColor(BLACK);
	display_set_font(VERDANA8MONO);
	display_print_text(text, 
			IS_TRIPLET ? 250 :
			IS_DUET ? 250 :
				228,
			IS_TRIPLET ? 150 :
			IS_DUET ? 140 :
				101,
			LEFT, 50); //, BLACK, FONT_VERDANA8_RIGHT_HOR_BLACK_TRANSP);
}


// TODO para outros modelos

void erase_text_grav() {
	
	uint16_t x = IS_TRIPLET ? 250 :
			IS_DUET ? 250 :
				228;
	uint16_t y = IS_TRIPLET ? 150 :
			IS_DUET ? 140 :
				101;
	
	SetColor(LIGHTBLUEBG);
	Bar(
			x,
			y,
			x+50, 
			y+10);
}

void draw_rec_time(state_of_device_t state, uint16_t rec_time) {
	if (state == DEV_STATE_ON) {
		uint16_t h = rec_time / 60 / 60, m = (rec_time / 60) % 60, s = rec_time % 60;
		char buffer[30];
		SetBgColor(LIGHTBLUEBG);
		sprintf(buffer, "%02u:%02u:%02u", h, m, s);
		put_text_grav(buffer);
	} else {
		erase_text_grav();
	}
}

void place_recorder_imgs(uint16_t img_sq_led, uint16_t img_rec_pause) {

	place_disp(
			IS_TRIPLET ? 285 :
			IS_DUET ? 285 :
			258,
			IS_TRIPLET ? 130 :
			IS_DUET ? 113 :
			74,
			img_sq_led
			);

	place_disp(
			IS_TRIPLET ? 208 :
			IS_DUET ? 213 :
			186,
			IS_TRIPLET ? 150 :
			IS_DUET ? 140 :
			101,
			img_rec_pause
			);

}

void draw_recorder_state(state_of_device_t state, uint16_t is_paused) {
	if (state == DEV_STATE_ON) {
		if (!is_paused)
			place_recorder_imgs(IMG_SQUARED_LED_ON, IMG_REC_PAUSE_REC);
		else
			place_recorder_imgs(IMG_SQUARED_LED_ON, IMG_REC_PAUSE_PAUSE);

	} else if (state == DEV_STATE_OFF) {
		place_recorder_imgs(IMG_SQUARED_LED_OFF, IMG_REC_PAUSE_OFF);
		draw_rec_time(state, 0);
	} else if (state == DEV_STATE_DISABLED) {
		place_recorder_imgs(IMG_SQUARED_LED_DIS, IMG_REC_PAUSE_OFF);
		draw_rec_time(state, 0);
	} else if (state == DEV_STATE_PARCIAL) {
		place_recorder_imgs(IMG_SQUARED_LED_ORANGE, IMG_REC_PAUSE_OFF);
		draw_rec_time(state, 0);
	}
}

void write_rec_square() {

	display_set_font(VERDANA8MONO);

	SetColor(LIGHTBLUEDELIMS);
	display_print_text(getRes(str_res_gravador),
			IS_TRIPLET ? 205 :
			IS_DUET ? 210 :
			183,
			IS_TRIPLET ? 131 :
			IS_DUET ? 111 :
			72,
			LEFT,
			0);

}
