#include "main_message.h"
#include "Application/DisplayControler24/display_controller.h"
#include "CTTKernel/kernel/tasker.h"
#include "CTTKernel/kernel.h"
#include <string.h>
#include <Application/Activities/ActivityMain.h>
#include <UserInterface/Buzzer.h>

#define number_of_simultaneous_messages 8
#define bip() sys_buzz(10,10);


status_message_t * active_status_message[number_of_simultaneous_messages] = {};
status_message_t * showing_message = NULL;
kTask_t * task_message_manager = NULL;
unsigned char idx_showing_message = 0;
unsigned int time_showing;

void status_message_task(void * param);

static unsigned char get_next_message(unsigned char index) {
	unsigned char i = 1;
	while (i <= 8 && active_status_message[(index + i) % number_of_simultaneous_messages] == NULL) {
		i++;
	}
	return (index + i) % number_of_simultaneous_messages;

}

void init_message_manager(){
	if (task_message_manager == NULL){
		task_message_manager = kernel_create_task(status_message_task, NULL, "StatusMessageTask",0);
//		kernel_message(MODE_DEBUG, "iNITIALIZATION WAS CALLED!\n");
	}
}

void stop_message_manager(){
	if (task_message_manager != NULL){
		sTask_delete(task_message_manager);
		task_message_manager = NULL;
	}
}

unsigned char status_message_is_inserted(status_message_t * message) {
	unsigned char i = 0;
	while (active_status_message[i] != message && i < number_of_simultaneous_messages)
		i++;
	//	if (i<number_of_simultaneous_messages)
	//		active_status_message[i] = NULL;

	return i<number_of_simultaneous_messages;
}

unsigned char status_message_is_showing(status_message_t * message) {
	return showing_message == message;
}

unsigned char status_message_insert(status_message_t * message) {
	unsigned char i = 0;

	if (status_message_is_inserted(message))
		return 1;

	while (active_status_message[i] != NULL && i < number_of_simultaneous_messages)
		i++;
	if (i < number_of_simultaneous_messages) {
		active_status_message[i] = message;
		if (showing_message == NULL || showing_message->type == ST_MESSAGE_ALTERNATING)
			status_message_show(i);
	}

	return i<number_of_simultaneous_messages;
}

unsigned char status_message_remove(status_message_t * message) {
	unsigned char i = 0;
	while (active_status_message[i] != message && i < number_of_simultaneous_messages)
		i++;
	if (i < number_of_simultaneous_messages)
		active_status_message[i] = NULL;

	//	if (showing_message == message)
	//		showing_message = NULL;

	return i<number_of_simultaneous_messages;
}

void status_message_remove_all() {
	unsigned char i;
	for (i = 0; i < number_of_simultaneous_messages; i++) {
		active_status_message[i] = NULL;
	}
}

void status_message_show(unsigned char index) {
	
	if (active_status_message[index] != NULL) {
		idx_showing_message = index;
		showing_message = active_status_message[index];
		time_showing = 0;
		SetColor(showing_message->bg_color);
		WAIT_UNTIL_FINISH(Bar(0, DISP_VER_RESOLUTION - 19, DISP_HOR_RESOLUTION, DISP_VER_RESOLUTION));
		SetColor(BLACK);
		display_set_font(VERDANA8MONO);
		display_print_text(showing_message->message, 20, DISP_VER_RESOLUTION - 18, LEFT, 0);
	} else {
		SetColor(LIGHTBLUEBG);
		WAIT_UNTIL_FINISH(Bar(0, DISP_VER_RESOLUTION - 19, DISP_HOR_RESOLUTION, DISP_VER_RESOLUTION));
	}
}

void status_message_force_show() {
	status_message_show(idx_showing_message);
}

/**
 * Fazer a cada 500ms
 */
void status_message_man_on_tick() {

	if (showing_message != NULL)
		time_showing++;
	else
		time_showing = 1000;

	if (time_showing > 6) {
		if (showing_message->type == ST_MESSAGE_WARNING) {
			status_message_remove(showing_message);
			showing_message = NULL;
		}
		if (showing_message->type != ST_MESSAGE_PERSISTENT) {
			status_message_show(get_next_message(idx_showing_message));
		} else {
			showing_message = active_status_message[idx_showing_message];
			if (showing_message->should_bip){
				bip();
				bip();
				bip();
			}
		}
	}
}


void status_message_task(void * param){
	
	
//	if (kTask_isInitialization(NULL)){
//		kernel_message(MODE_DEBUG, "Initializing status message task!\n");
//	}
	
	status_message_man_on_tick();
	
	sTask_DelayUntil(NULL,500*tMILLIS);
	return;
}



