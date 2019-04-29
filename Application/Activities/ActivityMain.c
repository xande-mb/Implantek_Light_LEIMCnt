#include "ActivityMain.h"
#include <ActivityManager/ActivityMan.h>
#include "../DisplayController.h"
#include "../Models/buttonsModel.h"
#include "../Models/brushed_motor.h"
#include "Application/Models/step_motor.h"
#include "UserInterface/Buzzer.h"
#include <string.h>
#include <stdio.h>

void ActivityMain_onCreate(void * param);
void ActivityMain_onDestroy(void * param);
void ActivityMain_onTick(void * param);


#define RETURN_IF_RUNNING if(brushedMotor_isRunning()) return;

static uint16_t possible_reductions[6][2] = {
    {1,1},{2,1},{4,1},{8,1},{16,1},{20,1}
};

#define V_LINE_x                75
#define H_LINE_y                34
#define V_LINE_1_x              48
#define V_LINE_2_x              90

#define getCenter(y,x,y2,x2)    ((x)/2+(x2)/2),((y)/2+(y2)/2)

ActivityContext_t ActivityMain = newActivity(
					.onCreate = ActivityMain_onCreate,
					.onDestroy = ActivityMain_onDestroy,
					.onTick = ActivityMain_onTick);


#define speed_pace 500
#define torque_pace 5

static struct {
    uint16_t reduction;
    uint16_t speed_rpm;
    uint16_t is_reverse;
    uint16_t pump_level;
    uint16_t program;
    uint16_t torque;
    uint16_t off_pump_counter;
} _this = {
    .reduction = 0,
    .speed_rpm = 500,
    .is_reverse = 0,
    .pump_level = 0,
    .program = 1,
    .torque = 5,
    .off_pump_counter = 0
};


#define current_axis_speed (_this.speed_rpm*possible_reductions[_this.reduction][1]/possible_reductions[_this.reduction][0])

void drawPumpLevel(uint16_t x, uint16_t y, uint16_t color, uint8_t level){
    
    int i;
    uint16_t sizes[7] = {1, 2, 3, 5, 7, 10, 14};
    for (i = 0; i< 7 ; i++){
        fillRectangle(y+10,x + i*3,y+10,x+i*3+1,color,ONLY_BUFFER);
        if (i < level){
            fillRectangle(y+8 - sizes[i],x + i*3,y+8,x+i*3+1,color,ONLY_BUFFER);
        }
    }
    
}

void writeSpeed(uint16_t rpm, uint8_t is_rev){
    
    fillRectangle( 1, 1, H_LINE_y - 1, V_LINE_x - 1, ERASE, ONLY_BUFFER);
    
    writeText("Velocidade:", 2, 2,
            VERDANA7, LEFT, SOLID, ONLY_BUFFER);
    
    char buffer[50];
    
    if (is_rev)
        snprintf(buffer,50,"< %u rpm <", rpm);
    else
        snprintf(buffer,50,"> %u rpm >", rpm);
    
    writeText(buffer, getCenter(1, 1, H_LINE_y - 1, V_LINE_x - 1),
            VERDANA7, CENTER, SOLID, ONLY_BUFFER);  
    
    fillRectangle( 1, 1, H_LINE_y - 1, V_LINE_x - 1, INVERT, ONLY_BUFFER);
    
}

void writePump(uint8_t pump){
    
    fillRectangle(           1,   V_LINE_x + 1, H_LINE_y - 1,          126, ERASE, ONLY_BUFFER);
    
    writeText("Bomba:", V_LINE_x + 2, 2,
            VERDANA7, LEFT, SOLID, ONLY_BUFFER); 
    
    uint16_t coords[2] = {getCenter( 1, V_LINE_x + 1, H_LINE_y - 1, 126)};
    
    drawPumpLevel(coords[0]-10,coords[1]+3, SOLID, pump);
//    char * possibilities[4] = {"l        l","l 0     l","l 00   l","l 000 l"};
//    
//    writeText(possibilities[pump%4], getCenter( 1, V_LINE_x + 1, H_LINE_y - 1, 126),
//            VERDANA7, CENTER, SOLID, ONLY_BUFFER);  
    
    fillRectangle( 1, V_LINE_x + 1, H_LINE_y - 1, 126, INVERT, ONLY_BUFFER);
    
    
}

void writeTorque(uint16_t torq){
    
    fillRectangle(H_LINE_y + 1, 1, 62, V_LINE_1_x-1, ERASE, ONLY_BUFFER);
    writeText("Torque:", 2, H_LINE_y + 2,
            VERDANA7, LEFT, SOLID, ONLY_BUFFER);
    char buffer[50];
    snprintf(buffer,50,"%u N.cm", torq);
    writeText(buffer, getCenter( H_LINE_y + 1, 1, 62, V_LINE_1_x-1),
            VERDANA7, CENTER, SOLID, ONLY_BUFFER);  

    fillRectangle(H_LINE_y + 1, 1, 62, V_LINE_1_x-1, INVERT, ONLY_BUFFER);

  
}

void writeLam(uint16_t reduction){
    reduction = reduction;
    fillRectangle(H_LINE_y + 1, V_LINE_1_x + 1, 62, V_LINE_2_x-1, ERASE, ONLY_BUFFER);
    char buffer[50];
    snprintf(buffer,50,"%u : %u ", possible_reductions[reduction][0], possible_reductions[reduction][1]);
    writeText("Lam.:", V_LINE_1_x + 2, H_LINE_y + 2,
            VERDANA7, LEFT, SOLID, ONLY_BUFFER);  
    writeText(buffer, getCenter(H_LINE_y + 1, V_LINE_1_x + 1, 62, V_LINE_2_x-1),
            VERDANA7, CENTER, SOLID, ONLY_BUFFER);  
    fillRectangle(H_LINE_y + 1, V_LINE_1_x + 1, 62, V_LINE_2_x-1, INVERT, ONLY_BUFFER);
}

void writeProg(uint16_t prog){
    
    fillRectangle(H_LINE_y + 1, V_LINE_2_x + 1, 62, 126, ERASE, ONLY_BUFFER);
    char buffer[30];
    snprintf(buffer,30,"%u", prog);
    writeText("Prog.:", V_LINE_2_x + 2, H_LINE_y + 2,
            VERDANA7, LEFT, SOLID, ONLY_BUFFER);
    writeText(buffer, getCenter(H_LINE_y + 1, V_LINE_2_x + 1, 62, 126),
            VERDANA7, CENTER, SOLID, ONLY_BUFFER);  
    fillRectangle(H_LINE_y + 1, V_LINE_2_x + 1, 62, 126, INVERT, ONLY_BUFFER);

}

void callbackPump(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS ||
           (event->event_emited == EVENT_STILL && event->time_event > 10 && event->time_event%8 == 0) ){
        sys_buzz(5,1);
        _this.pump_level = (_this.pump_level+1)%8;
//        stepMotorRun(50*_this.pump_level, 0);
        writePump(_this.pump_level);
        showDisplayBuffer();
    }
}

void callbackTorque(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS ){
        sys_buzz(5,1);
        _this.torque += torque_pace;
        if(_this.torque > 80) _this.torque = 5;
        writeTorque(_this.torque);
        brushedMotor_setTorque(_this.torque);
        showDisplayBuffer();
    } else if (event->event_emited == EVENT_STILL && event->time_event > 10 && event->time_event%2 == 0){
        
        _this.torque += torque_pace;
        if(_this.torque > 80) _this.torque = 80; else sys_buzz(5,1);
        writeTorque(_this.torque);
        brushedMotor_setTorque(_this.torque);
        showDisplayBuffer();        
    }
}

void callbackProg(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS){
        sys_buzz(5,1);
        _this.program = _this.program+1;
        if (_this.program > 5) _this.program = 1;
        writeProg(_this.program);
        showDisplayBuffer();
    }
}

void callbackLam(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS){
//        sys_buzz(5,1);
        _this.reduction = _this.reduction+1;
        if (_this.reduction > 5){ _this.reduction = 0; }else{ sys_buzz(5,1);}
        writeLam(_this.reduction);
        writeSpeed(current_axis_speed, _this.is_reverse);
        showDisplayBuffer();
    }
}

void callbackSpeedUp(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS || 
            (event->event_emited == EVENT_STILL && event->time_event > 8)){
//        sys_buzz(5,1);
        if (_this.speed_rpm < speed_pace){ _this.speed_rpm = 0; }
        _this.speed_rpm = _this.speed_rpm + speed_pace;
        if (_this.speed_rpm > 30000) _this.speed_rpm = 30000; else{ sys_buzz(10,1);}
        writeSpeed(current_axis_speed, _this.is_reverse);
        showDisplayBuffer();
    }
}

void callbackSpeedDown(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS || 
            (event->event_emited == EVENT_STILL && event->time_event > 8)){
//        sys_buzz(5,1);
        if (_this.speed_rpm <= speed_pace){ _this.speed_rpm = 240;}
        else{ _this.speed_rpm = _this.speed_rpm - speed_pace; sys_buzz(10,1);}
        
        writeSpeed(current_axis_speed, _this.is_reverse);
        showDisplayBuffer();
    }
}

void callbackReverse(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS){
        sys_buzz(5,1);
        _this.is_reverse = !_this.is_reverse;
        writeSpeed(current_axis_speed, _this.is_reverse);
        showDisplayBuffer();
    } else if (event->event_emited == EVENT_RELEASE){
        
    }
}

void callbackCenterFS(button_event_data_t * event){
    
    if (event->event_emited == EVENT_PRESS){
//        writePump(_this.pump_level);
        if (_this.pump_level){
            _this.off_pump_counter = 0;
            stepMotorRun( 30+35*(_this.pump_level-1), 1);
        }
        showDisplayBuffer();
        brushedMotor_run(_this.speed_rpm,_this.is_reverse);
    } else if (event->event_emited == EVENT_RELEASE){
        _this.off_pump_counter = 20;
        if (_this.pump_level) stepMotorRun(200, 0);
        brushedMotor_stop();
        sys_clear_buzz();
//        brushedMotor_run(0,0);
    }    
}

void ActivityMain_onCreate(void * param) {
    // Codigo de inicialização da atividade
    
    clearDisplayBuffer();
    
//    sys_buzz(1000,0);
    
    writeSpeed(current_axis_speed, _this.is_reverse);
    writePump(_this.pump_level);
    writeTorque(_this.torque);
    brushedMotor_setTorque(_this.torque);
    writeLam(_this.reduction);
    writeProg(_this.program);
    
    showDisplayBuffer();
    
    buttons_registerCallback(BUTTON_RIGHT_UP, callbackPump);
    buttons_registerCallback(BUTTON_DOWN_LEFT, callbackTorque);
    buttons_registerCallback(BUTTON_RIGHT_DOWN, callbackProg);
    buttons_registerCallback(BUTTON_DOWN_RIGHT, callbackLam);
    buttons_registerCallback(BUTTON_LEFT_UP, callbackSpeedUp);
    buttons_registerCallback(BUTTON_LEFT_DOWN, callbackSpeedDown);

    buttons_registerCallback(BUTTON_FS_CENTER, callbackCenterFS);
    buttons_registerCallback(BUTTON_FS_LEFT, callbackPump);
    buttons_registerCallback(BUTTON_FS_RIGHT, callbackReverse);
    
    
}

void ActivityMain_onDestroy(void * param) {
  // Código a ser executado quando a atividade for finalizada
    
    buttons_removeCallback(BUTTON_RIGHT_UP);
    buttons_removeCallback(BUTTON_RIGHT_DOWN);
    buttons_removeCallback(BUTTON_DOWN_LEFT);
    buttons_removeCallback(BUTTON_DOWN_RIGHT);
    buttons_removeCallback(BUTTON_LEFT_UP);
    buttons_removeCallback(BUTTON_LEFT_DOWN);
    buttons_removeCallback(BUTTON_FS_CENTER);
    buttons_removeCallback(BUTTON_FS_LEFT);
    buttons_removeCallback(BUTTON_FS_RIGHT);
}



void ActivityMain_onTick(void * param){
  // Código a ser executado ciclicamente durante o ciclo de vida da atividade
    
    if (_this.off_pump_counter){
        
        if (_this.off_pump_counter == 1) stepMotorRun(0,0);
        _this.off_pump_counter --;
        
    } else {
        
    }
    
    if (brushedMotor_isStopped()){
        
        sys_buzz(5,0);
    } else {
        sys_clear_buzz();
    }
    
//    writeProg(brushedMotor_getVel()*32/10);
//    writeProg(brushedMotor_getControl());
    writeProg(brushedMotor_getCurrent());
    showDisplayBuffer();
    
}
