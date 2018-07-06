#include <ActivityManager/ActivityMan.h>
#include <CTTKernel/kernel.h>
#include <Application/DisplayControler24/tft_driver.h>
#include <Application/DisplayControler24/display_controller.h>
#include <UserInterface/bt_lib_2.h>
#include <Application/images_list.h>
#include <Application/str_resources.h>
#include <TouchScreen/touchCalib.h>
#include <UserInterface/Buzzer.h>
#include "ActivityMain.h"
#include "ActivityMenu.h"
#include "Application/Utils/model_manager.h"

//@cond NOT_DOCUMENTED
#define TIMEOUT_uS 5000000L

#define TIMEBIP_uS 600000L
//@endcond


void activityInicial_onCreate(void * param);
void activityInicial_onDestroy(void * param);
void activityInicial_onTick(void * param);

static uint64_t entrance_time;

ActivityContext_t activityInicial = newActivity(
		.onCreate = activityInicial_onCreate,
		.onDestroy = activityInicial_onDestroy,
		.onTick = activityInicial_onTick);


static touchable_t * bt_screen;

static unsigned long entered_time = 0;

/**
 * Rotina executada quando a tela é pressionada.
 * 
 * @param event evento de toque.
 * @param flag parâmetro não utilizado.
 * @param obf parâmetro não utilizado.
 */
static void bt_screen_click(unsigned char event, unsigned char flag, void * obf) {

	if (event == EVENT_INVALID) return;

	entered_time = kTime_micros();

	static unsigned long press_time = 0;
	static uint16_t ct_bips;

	if (event == EVENT_PRESS) {
		//        sys_buzz(2, 1);
		press_time = kTime_micros();
		ct_bips = 0;
	} else if (event == EVENT_RELEASE) {
		if (ct_bips == 1) {
			ActivityMan_startActivity(&ActivityMain);
		}
		if (ct_bips == 3) {

		}
		if (ct_bips == 4) {

		}
		if (ct_bips == 2) {

			ActivityContext_t * act_touch_calib = get_sys_activityTouchCalib((uint16_t [4]) {
				25, 24, 25, 24}, VERDANA8MONO, &ActivityMain);

			ActivityMan_startActivity(act_touch_calib);
		}
	} else {
		entrance_time = kTime_micros();
		if (kTime_micros() > press_time + TIMEBIP_uS) {
			sys_buzz(20, 10);
			press_time = kTime_micros();
			ct_bips++;
		}
	}
}

void activityInicial_onCreate(void * param) {
	// Draw image initial
	ClearDisplay();

	models_t model = modelManager_getDataReaded()->equip_model;
	
	uint16_t indx_banner = 
			model == MODEL_TRIPLET ? IMG_BANNER_TRIPLET :
			model == MODEL_SMART_CAM ? IMG_BANNER_SMART_CAM :
			model == MODEL_DUET ? IMG_BANNER_DUET :
			model == MODEL_COOL_LIGHT ? IMG_BANNER_COOL_LIGHT :
			model == MODEL_SHAVER ? IMG_BANNER_SHAVER : IMG_BANNER_TRIPLET;
	
	
	place_disp(0, 88, indx_banner);
	place_disp(8, 207, IMG_ICON_TOUCH);
	place_disp(47, 216, IMG_ICON_BEEP);

	bt_screen = allocAsTouchable;

	setAsButton(bt_screen,
			.blink = 0, //No redraw
			.coord_x = 0,
			.coord_y = 0,
			.length = DISP_HOR_RESOLUTION,
			.height = DISP_VER_RESOLUTION,
			.beep = 0,
			.onClick = bt_screen_click
			);


	ActivityMan_insertTouchable(bt_screen);


	font_set_font(VERDANA7MONO);
	SetColor(LIGHTBLUE);
	disp_multi_line_print("1x\n2x", 38, 213, CENTER, 0);
	disp_multi_line_print(getRes(str_inicial_para_continuar_calibrar), 75, 213, LEFT, 0);
	font_set_font(VERDANA8MONO);
	SetColor(BRIGHTRED);
	display_set_bg_color(TRANSP);
	disp_multi_line_print(getRes(str_ortopedia), 253, 145, CENTER, 0);

	entrance_time = kTime_micros();

}

void activityInicial_onDestroy(void * param) {
	// desaloc resources
}

void activityInicial_onTick(void * param) {
	// wait for timeout to change activity
	if (kTime_micros() - entrance_time > TIMEOUT_uS) {

		ActivityMan_startActivity(&ActivityMain);
		//		ActivityMan_startActivity(&activityMenu);
	}


}

