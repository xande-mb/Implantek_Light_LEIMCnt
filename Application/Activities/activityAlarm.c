#include "activityAlarm.h"
#include "UserInterface/Buzzer.h"
#include "ActivityMain.h"
#include <ActivityManager/ActivityMan.h>
#include <Application/DisplayControler24/tft_driver.h>
#include <Application/DisplayControler24/display_controller.h>
#include <UserInterface/bt_lib_2.h>
#include <Application/images_list.h>
#include <Application/str_resources.h>

void activityAlarm_onCreate(void * param);
void activityAlarm_onDestroy(void * param);
void activityAlarm_onTick(void * param);

#define TIME_ERROR_MIN_decis 5*10


ActivityContext_t activityAlarm = newActivity(
					.onCreate = activityAlarm_onCreate,
					.onDestroy = activityAlarm_onDestroy,
					.onTick = activityAlarm_onTick);


struct {
  
	char * error_title;
	verify_error_function_t verify_error;
	uint32_t ct_100ms;
	
} _this = {
	
	
	
};

void activityAlarm_setVariables(char * title, verify_error_function_t verify_error);


void activityAlarm_call(char * title, verify_error_function_t verify_error){
	
	activityAlarm_setVariables(title,verify_error);
	ActivityMan_startActivity(&activityAlarm);
	
}

void activityAlarm_setVariables(char * title, verify_error_function_t verify_error){
	_this.error_title = title;
	_this.verify_error = verify_error;
}


void activityAlarm_onCreate(void * param){
  // Codigo de inicialização da atividade
  
	ClearDisplay(); // Clear the display
	SetColor(YELLOW);
	Bar(0,0,DISP_HOR_RESOLUTION-1,DISP_VER_RESOLUTION-1);

	_this.ct_100ms = 0;
	//place_picture();
	
	SetColor(BLUEBTBAR);
	display_set_font(VERDANA12NMONO);
	display_print_text(_this.error_title,DISP_HOR_RESOLUTION/2,DISP_VER_RESOLUTION/2-50,CENTER,0);
	
}

void activityAlarm_onDestroy(void * param){
	// Código a ser executado quando a atividade for finalizada
	
}

void activityAlarm_onTick(void * param){

	
	_this.ct_100ms++;
	
	
	
	if ((_this.ct_100ms % 10) == 0 && (_this.ct_100ms % 100) < 50){
		sys_buzz(10,5);
		sys_buzz(20,5);
	}
	
	if (!_this.verify_error() && _this.ct_100ms > TIME_ERROR_MIN_decis){
		
		ActivityMan_getBack();
		
	}	
}
