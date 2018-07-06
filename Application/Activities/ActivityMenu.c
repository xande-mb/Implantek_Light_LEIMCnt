#include "ActivityMenu.h"
#include "CTTKernel/kernel.h"
#include <ActivityManager/ActivityMan.h>
#include <Application/DisplayControler24/tft_driver.h>
#include <Application/DisplayControler24/display_controller.h>
#include "UserInterface/bt_lib_2.h"
#include "ActivityMain.h"
#include "activityHelp.h"
#include "Application/images_list.h"
#include "activityHelpIndex.h"
#include "activityAbout.h"
#include "SubActivitiesMain.h"
#include <Application/str_resources.h>


#define UP_TOP		90
#define HEIGHT		80

#define BUTTON_LENGTH		80
#define BUTTON_HEIGHT		80

static touchable_t * bt_information;
//static touchable_t * bt_logs;
static touchable_t * bt_help;
static touchable_t * bt_back;

void bt_back_click(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		ActivityMan_startActivity(&ActivityMain);
	}
}

void bt_help_click(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		ActivityMan_startActivity(&activityHelpIndex);
	}
}


void bt_about_click(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		ActivityMan_startActivity(&activityAbout);
	}
}


void actMenu_onCreate(void * param);


void actMenu_onTick(void * param);


void actMenu_onDestroy(void * param);

ActivityContext_t activityMenu = newActivity(
        .onCreate = actMenu_onCreate,
        .onDestroy = actMenu_onDestroy,
        .onTick = actMenu_onTick);


void actMenu_onCreate(void * param){
	
	ClearDisplay(); // Clear the display
	
	ClearDisplayColor(LIGHTBLUEBG);
	
	SetColor(BLUEBTBAR);
	WAIT_UNTIL_FINISH(Bar(0, UP_TOP, 319, UP_TOP + HEIGHT));

	bt_back = allocAsTouchable;
	setAsButton(bt_back,
			.text = getRes(str_voltar),
			.coord_x = 0,
			.coord_y = UP_TOP,
			.height = BUTTON_HEIGHT,
			.length = BUTTON_LENGTH,
			.color = BLUEBTBAR,
			.img_idx_pressed = IMG_BACK_ARROW_PRESSED,
			.img_idx_unpress = IMG_BACK_ARROW_UNPRESSED,
			.reference_txt = CENTER,
			.txt_font = VERDANA7MONO,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.onClick = bt_back_click
			);
	center_img_on_button(bt_back);
	bt_back->y_img -= 10;
	bt_back->x_img -= 3;
	bt_back->y_txt = 25;
	
	ActivityMan_insertTouchable(bt_back);
	bt_lib_2_drawTouchableButton(*bt_back);

	bt_information = allocAsTouchable;
	setAsButton(bt_information,
			.text = getRes(str_res_sobre),
			.coord_x = BUTTON_LENGTH + 20,
			.coord_y = UP_TOP,
			.height = BUTTON_HEIGHT,
			.length = BUTTON_LENGTH,
			.color = BLUEBTBAR,
			.img_idx_pressed = IMG_INFO_PRESSED,
			.img_idx_unpress = IMG_INFO_UNPRESSED,
			.reference_txt = CENTER,
			.txt_font = VERDANA7MONO,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.onClick = bt_about_click
			);
	center_img_on_button(bt_information);
	bt_information->y_img -= 10;
	bt_information->x_img += 3;
	bt_information->y_txt = 25;
	
	ActivityMan_insertTouchable(bt_information);
	bt_lib_2_drawTouchableButton(*bt_information);
	

//	bt_logs = allocAsTouchable;
//	setAsButton(bt_logs,
//			.text = "Registros",
//			.coord_x = 2*BUTTON_LENGTH,
//			.coord_y = UP_TOP,
//			.height = BUTTON_HEIGHT,
//			.length = BUTTON_LENGTH,
//			.color = BLUEBTBAR,
//			.img_idx_pressed = IMG_LOGS_PRESSED,
//			.img_idx_unpress = IMG_LOGS_UNPRESSED,
//			.reference_txt = CENTER,
//			.txt_font = VERDANA7MONO,
//			.txt_font_height = 13,
//			.color_blink = CYANBTBAR,
//			.onClick = bt_back_click
//			);
//	center_img_on_button(bt_logs);
//	bt_logs->y_img -= 10;
//	bt_logs->x_img += 3;
//	bt_logs->y_txt = 25;
//	ActivityMan_insertTouchable(bt_logs);
//	bt_lib_2_drawTouchableButton(*bt_logs);

	bt_help = allocAsTouchable;
	setAsButton(bt_help,
			.text = getRes(str_res_ajuda),
			.coord_x = 2*BUTTON_LENGTH + 40,
			.coord_y = UP_TOP,
			.height = BUTTON_HEIGHT,
			.length = BUTTON_LENGTH,
			.color = BLUEBTBAR,
			.img_idx_pressed = IMG_HELP_PRESSED,
			.img_idx_unpress = IMG_HELP_UNPRESSED,
			.reference_txt = CENTER,
			.txt_font = VERDANA7MONO,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.onClick = bt_help_click
			);
	center_img_on_button(bt_help);
	bt_help->y_img -= 10;
	bt_help->x_img += 3;
	bt_help->y_txt = 25;
	
	ActivityMan_insertTouchable(bt_help);
	bt_lib_2_drawTouchableButton(*bt_help);
	
	
}
	


void actMenu_onTick(void * param){
	
}


void actMenu_onDestroy(void * param){
	desalocTouchable(bt_help);
//	desalocTouchable(bt_logs);
	desalocTouchable(bt_information);
	desalocTouchable(bt_back);
}