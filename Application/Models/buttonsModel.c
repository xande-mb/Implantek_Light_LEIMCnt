#include "buttonsModel.h"
#include "CTTKernel/kernel.h"
#include "PortManager/portman.h"

#define NUMBER_FRONT_BUTTONS 9

typedef struct {
    fast_port_t port;
    button_event_data_t event;
    void (* callback)(button_event_data_t * event);
} button_t;

#define NULL ((void*)0)

button_t front_buttons_array[NUMBER_FRONT_BUTTONS] = {};
kTask_t * bts_task;

void buttonsThread(void * data);

static void init_button(front_buttons_t btl, PORTS_t port, uint16_t bt){
    portman_setAsDigital(port, bt);
    portman_setAsInput(port, bt);
    portman_get_fast_port(port, bt, &front_buttons_array[btl].port);
    front_buttons_array[btl].callback = NULL;
    front_buttons_array[btl].event.current_value = portman_fast_read(&(front_buttons_array[BUTTON_LEFT_UP].port));
    front_buttons_array[btl].event.last_value = front_buttons_array[BUTTON_LEFT_UP].event.current_value;
    front_buttons_array[btl].event.time_event = 0;
    
}

void buttonsInit() {
    
    init_button(BUTTON_LEFT_UP    , PORT_A,7);
    init_button(BUTTON_LEFT_DOWN  , PORT_G,7);

    init_button(BUTTON_DOWN_LEFT  , PORT_G,6);
    init_button(BUTTON_DOWN_RIGHT , PORT_E,14);

    init_button(BUTTON_RIGHT_DOWN , PORT_E,13);
    init_button(BUTTON_RIGHT_UP   , PORT_E,12);
    
    init_button(BUTTON_FS_RIGHT   , PORT_C,15);
    init_button(BUTTON_FS_LEFT    , PORT_C,12);
    init_button(BUTTON_FS_CENTER  , PORT_B,11);
    
    bts_task = kernel_create_task(buttonsThread, NULL, "buttons_task", 0);
}

void buttons_registerCallback(front_buttons_t button, void (* callback)(button_event_data_t * event)){
    front_buttons_array[button].callback = callback;
}

void buttons_removeCallback(front_buttons_t button){
    front_buttons_array[button].callback = NULL;
}

static void btLoop(button_t * button) {
    
    button->event.last_value = button->event.current_value;
    button->event.event_last_emited = button->event.event_emited;
    button->event.event_emited = EVENT_NONE;
    
    button->event.current_value = portman_fast_read(&(button->port));
    
    if (button->event.current_value && !button->event.last_value){
        button->event.event_emited = EVENT_PRESS;
    } else if (button->event.current_value && button->event.last_value && 
            (button->event.event_last_emited == EVENT_PRESS || button->event.event_last_emited == EVENT_STILL)
            ){
        button->event.event_emited = EVENT_STILL;
    } else if (button->event.current_value && button->event.last_value && button->event.event_last_emited != EVENT_PRESS){
        button->event.event_emited = EVENT_PRESSED_FROM_START;
    } else if (!button->event.current_value && button->event.last_value){
        button->event.event_emited = EVENT_RELEASE;
    }
    
    if (button->event.event_emited == button->event.event_last_emited){
        button->event.time_event ++;
    } else {
        button->event.time_event = 0;
    }
    
    if (button->event.event_emited != EVENT_NONE && button->callback != NULL){
        button->callback(&button->event);
    }
}

void buttonsThread(void * data){
    
    unsigned int i;
    for (i = 0; i < NUMBER_FRONT_BUTTONS; i++){
        btLoop(&front_buttons_array[i]);
    }
    
    sTask_DelayUntil(NULL, 100*tMILLIS);
}
