#include "ActivityMain.h"
#include <ActivityManager/ActivityMan.h>
#include "../DisplayController.h"
#include "../Models/buttonsModel.h"
#include "../Models/brushed_motor.h"
#include "Application/Models/step_motor.h"
#include "Application/PinDefinitions.h"
#include "UserInterface/Buzzer.h"
#include "Memory/eeprom_i2c.h"
#include "Application/Peripherals/i2ccom.h"
#include <string.h>
#include <stdio.h>
#include <libpic30.h>

//@cond NOT_DOCUMENTED

void ActivityMain_onCreate(void * param);
void ActivityMain_onDestroy(void * param);
void ActivityMain_onTick(void * param);


unsigned char progs_adds[5] = {0x00, 0x08, 0x10, 0x18, 0x20};

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



#define current_axis_speed (_this.speed_rpm*possible_reductions[_this.reduction][1]/possible_reductions[_this.reduction][0])

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


typedef struct program_values_s {
    uint16_t speed_rpm;
    uint8_t pump_level;
    uint8_t torque;
    uint8_t lam_index;
    uint8_t unused1;
    uint8_t unused2;
    uint8_t unused3;
} program_values_t;


#define prog_def {.speed_rpm = 500, .pump_level = 0, .torque = 5, .lam_index = 0}

program_values_t my_program;

program_values_t my_program_def = prog_def;

void read_program(program_values_t * data, unsigned char add){
    portman_clearOutput(VEEM);
    portman_setOutput(VEEC);
    __delay_ms(10);
    i2c_read(0xA0, add, 0, sizeof(program_values_t), (uint8_t *) data);
    
    if (data->speed_rpm == 0xffff){
        i2c_write(0xA0, add, 0, sizeof(program_values_t), (uint8_t *) &my_program_def);
        __delay_ms(10);
        i2c_read(0xA0, add, 0, sizeof(program_values_t), (uint8_t *) data);
    }
       
    portman_clearOutput(VEEC);
    
    volatile unsigned int i = 0;
    i++;
    
}

void save_program(program_values_t * data, unsigned char add){
    
    portman_clearOutput(VEEM);
    portman_setOutput(VEEC);
    __delay_ms(10);
    i2c_write(0xA0, add, 0, sizeof(program_values_t), (uint8_t *) data);
    __delay_ms(10);
    portman_clearOutput(VEEC);
    
}

void save_current_program(){
    
    my_program.speed_rpm = _this.speed_rpm;
    my_program.pump_level = _this.pump_level;
    my_program.torque = _this.torque;
    my_program.lam_index = _this.reduction;
    
    save_program(&my_program, progs_adds[_this.program-1]);
    __delay_ms(10);
    program_values_t debug = {};
    read_program(&debug, progs_adds[_this.program-1]);
    volatile unsigned int i = 0;
    i++;
    
}

void cache_current_program(){
    
    read_program(&my_program, progs_adds[_this.program-1]);
   
    _this.speed_rpm  = my_program.speed_rpm    ;
    _this.pump_level = my_program.pump_level   ;
    _this.torque     = my_program.torque       ;
    _this.reduction  = my_program.lam_index    ;
    
    writeSpeed(current_axis_speed, _this.is_reverse);
    writePump(_this.pump_level);
    writeTorque(_this.torque);
    brushedMotor_setTorque(_this.torque);
    writeLam(_this.reduction);
    writeProg(_this.program);
    
    showDisplayBuffer();
    
    
}



//@endcond

/**
 * 
 * 
 * @param y_i
 * @param x_i
 * @param y_f
 * @param x_f
 * @param colorOption
 * @param update
 */
void background2line(unsigned char y_i, unsigned char x_i, unsigned char y_f,
        unsigned char x_f, unsigned char colorOption, unsigned char update){
    
    fillRectangle(y_i-1, x_i-1, y_i, x_f+1, SOLID, ONLY_BUFFER);
    fillRectangle(y_i-1, x_i-1, y_f+1, x_i, SOLID, ONLY_BUFFER);
    
    
//    fillRectangle(y_f-1, x_i-1, y_i, x_f+1, SOLID, ONLY_BUFFER);
//    fillRectangle(y_i-1, x_i-1, y_i, x_f+1, SOLID, ONLY_BUFFER);
//    
//    fillRectangle(y_i-1, x_i-1, y_f+1, x_f+1, INVERT, ONLY_BUFFER);
//    fillRectangle(y_i, x_i, y_f, x_f, INVERT, update);
}

/**
 * Desenha o nível da bomba peristaltica.
 * 
 * @param x coordenada horizontal
 * @param y coordenada vertical
 * @param color cor
 * @param level nível
 */
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

/**
 * Escreve a velocidade na tela.
 * 
 * @param rpm Velocidade.
 * @param is_rev Modo reverso.
 */
void writeSpeed(uint16_t rpm, uint8_t is_rev){
    
    fillRectangle( 1, 1, H_LINE_y - 1, V_LINE_x - 1, ERASE, ONLY_BUFFER);
    
    writeText("Velocidade:", 2, 2+1,
            VERDANA7, LEFT, SOLID, ONLY_BUFFER);
    
    char buffer[50];
    
    if (is_rev)
        snprintf(buffer,50,"< %u rpm <", rpm);
    else
        snprintf(buffer,50,"> %u rpm >", rpm);
    
    writeText(buffer, getCenter(1, 1, H_LINE_y - 1, V_LINE_x - 1),
            VERDANA7, CENTER, SOLID, ONLY_BUFFER);  
    
//    background2line( 1, 1, H_LINE_y - 1, V_LINE_x - 1, INVERT, ONLY_BUFFER);
    
}

/**
 * Escreve quadrado da bomba peristáltica.
 * 
 * @param pump Nível da bomba.
 */
void writePump(uint8_t pump){
    
    fillRectangle(           1,   V_LINE_x + 1, H_LINE_y - 1,          126, ERASE, ONLY_BUFFER);
    
    writeText("Bomba:", V_LINE_x + 2, 2 +1,
            VERDANA7, LEFT, SOLID, ONLY_BUFFER); 
    
    uint16_t coords[2] = {getCenter( 1, V_LINE_x + 1, H_LINE_y - 1, 126)};
    
    drawPumpLevel(coords[0]-10,coords[1]+3, SOLID, pump);
//    char * possibilities[4] = {"l        l","l 0     l","l 00   l","l 000 l"};
//    
//    writeText(possibilities[pump%4], getCenter( 1, V_LINE_x + 1, H_LINE_y - 1, 126),
//            VERDANA7, CENTER, SOLID, ONLY_BUFFER);  
    
//    background2line( 1, V_LINE_x + 1, H_LINE_y - 1, 126, INVERT, ONLY_BUFFER);
    
    
}

/**
 * Escreve o torque na tela.
 * 
 * @param torq Valor do torque.
 */
void writeTorque(uint16_t torq){
    
    fillRectangle(H_LINE_y + 1, 1, 62, V_LINE_1_x-1, ERASE, ONLY_BUFFER);
    writeText("Torque:", 2, H_LINE_y + 2 +1,
            VERDANA7, LEFT, SOLID, ONLY_BUFFER);
    char buffer[50];
    snprintf(buffer,50,"%u N.cm", torq);
    writeText(buffer, getCenter( H_LINE_y + 1, 1, 62, V_LINE_1_x-1),
            VERDANA7, CENTER, SOLID, ONLY_BUFFER);  

//    background2line(H_LINE_y + 1, 1, 62, V_LINE_1_x-1, INVERT, ONLY_BUFFER);

  
}

/**
 * Escreve a redução na tela.
 * 
 * @param reduction Redução.
 */
void writeLam(uint16_t reduction){
    reduction = reduction;
    fillRectangle(H_LINE_y + 1, V_LINE_1_x + 1, 62, V_LINE_2_x-1, ERASE, ONLY_BUFFER);
    char buffer[50];
    snprintf(buffer,50,"%u : %u ", possible_reductions[reduction][0], possible_reductions[reduction][1]);
    writeText("Lam.:", V_LINE_1_x + 2, H_LINE_y + 2+1,
            VERDANA7, LEFT, SOLID, ONLY_BUFFER);  
    writeText(buffer, getCenter(H_LINE_y + 1, V_LINE_1_x + 1, 62, V_LINE_2_x-1),
            VERDANA7, CENTER, SOLID, ONLY_BUFFER);  
//    background2line(H_LINE_y + 1, V_LINE_1_x + 1, 62, V_LINE_2_x, INVERT, ONLY_BUFFER);
//    fillRectangle(H_LINE_y + 1, V_LINE_1_x + 1, 62, V_LINE_2_x-1, INVERT, ONLY_BUFFER);
}

/**
 * Escreve o valor de programa.
 * 
 * @param prog Valor do programa.
 */
void writeProg(uint16_t prog){
    
    fillRectangle(H_LINE_y + 1, V_LINE_2_x + 1, 62, 126, ERASE, ONLY_BUFFER);
    char buffer[30];
    snprintf(buffer,30,"%u", prog);
    writeText("Prog.:", V_LINE_2_x + 2, H_LINE_y + 2+1,
            VERDANA7, LEFT, SOLID, ONLY_BUFFER);
    writeText(buffer, getCenter(H_LINE_y + 1, V_LINE_2_x + 1, 62, 126),
            VERDANA7, CENTER, SOLID, ONLY_BUFFER);  
//    background2line(H_LINE_y + 1, V_LINE_2_x + 1, 62, 126, INVERT, ONLY_BUFFER);

}

/**
 * Rotina de callback para quando o botão da bomba é pressionado.
 * 
 * @param event Evento do botão.
 */
void callbackPump(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS ||
           (event->event_emited == EVENT_STILL && event->time_event > 10 && event->time_event%8 == 0) ){
        sys_buzz(5,1);
        _this.pump_level = (_this.pump_level+1)%8;
//        stepMotorRun(50*_this.pump_level, 0);
        writePump(_this.pump_level);
        showDisplayBuffer();
    } else if (event->event_emited == EVENT_RELEASE){
//        displayDown();
//        __delay_ms(500);
//        resetDisplay(50);
    }
}

/**
 * Rotina de callback para quando o botão de seleção de torque é pressionado.
 * 
 * @param event Evento do botão.
 */
void callbackTorque(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS ){
        sys_buzz(5,1);
        _this.torque += torque_pace;
        if(_this.torque > 55) _this.torque = 5;
        writeTorque(_this.torque);
        brushedMotor_setTorque(_this.torque);
        showDisplayBuffer();
    } else if (event->event_emited == EVENT_STILL && event->time_event > 10 && event->time_event%2 == 0){
        
        _this.torque += torque_pace;
        if(_this.torque > 55) _this.torque = 55; else sys_buzz(5,1);
        writeTorque(_this.torque);
        brushedMotor_setTorque(_this.torque);
        showDisplayBuffer();        
    }
}

/**
 * Rotina de callback do botão de seleção de programação.
 * 
 * @param event Evento do botão.
 */
void callbackProg(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS){
        save_current_program();
        sys_buzz(5,1);
        _this.program = _this.program+1;
        if (_this.program > 5) _this.program = 1;
        writeProg(_this.program);
        showDisplayBuffer();
        
        cache_current_program();
    } else if (event->event_emited == EVENT_RELEASE){
//        resetDisplay(1);
    }
}

/**
 * Rotina de callback do botão de seleção de torque.
 * 
 * @param event Evento do botão.
 */
void callbackLam(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS){
//        sys_buzz(5,1);
        _this.reduction = _this.reduction+1;
        if (_this.reduction > 5){ _this.reduction = 0; }else{ sys_buzz(5,1);}
        writeLam(_this.reduction);
        writeSpeed(current_axis_speed, _this.is_reverse);
        showDisplayBuffer();
//        save_current_program();
    } else if (event->event_emited == EVENT_RELEASE){
//        resetDisplay(10);
    }
}


/**
 * Rotina de callback do botão de aumentar velocidade.
 * 
 * @param event Evento do botão.
 */
void callbackSpeedUp(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS || 
            (event->event_emited == EVENT_STILL && event->time_event > 8)){
//        sys_buzz(5,1);
        if (_this.speed_rpm < speed_pace){ _this.speed_rpm = 0; }
        _this.speed_rpm = _this.speed_rpm + speed_pace;
        if (_this.speed_rpm > 30000) _this.speed_rpm = 30000; else{ sys_buzz(10,1);}
        writeSpeed(current_axis_speed, _this.is_reverse);
//        save_current_program();
        showDisplayBuffer();
    }
}


/**
 * Rotina de callback do botão de diminuição de velocidade.
 * 
 * @param event Evento do botão.
 */
void callbackSpeedDown(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS || 
            (event->event_emited == EVENT_STILL && event->time_event > 8)){
//        sys_buzz(5,1);
        if (_this.speed_rpm <= speed_pace){ _this.speed_rpm = 240;}
        else{ _this.speed_rpm = _this.speed_rpm - speed_pace; sys_buzz(10,1);}
        
        writeSpeed(current_axis_speed, _this.is_reverse);
//        save_current_program();
        showDisplayBuffer();
    }
}

/**
 * Rotina de callback do botao de reverso no pedal.
 * 
 * @param event Evento do botão.
 */
void callbackReverse(button_event_data_t * event){
    RETURN_IF_RUNNING
    if (event->event_emited == EVENT_PRESS){
        sys_buzz(5,1);
        _this.is_reverse = !_this.is_reverse;
        writeSpeed(current_axis_speed, _this.is_reverse);
        showDisplayBuffer();
//        resetDisplay(100);
        
    } else if (event->event_emited == EVENT_RELEASE){
//        displayDown();
//        __delay_ms(100);
//        resetDisplay(100);
    }
}

/**
 * Rotina de callback do botao central do pedal.
 * 
 * @param event Evento do botão.
 */
void callbackCenterFS(button_event_data_t * event){
    
    if (event->event_emited == EVENT_PRESS){
//        writePump(_this.pump_level);
        if (_this.pump_level){
            _this.off_pump_counter = 0;
            stepMotorRun( 30+35*(_this.pump_level-1), 1);
        }
        showDisplayBuffer();
        brushedMotor_run(_this.speed_rpm,_this.is_reverse);
        
//        resetDisplay(500);
    } else if (event->event_emited == EVENT_RELEASE){
        _this.off_pump_counter = 20;
        if (_this.pump_level) stepMotorRun(200, 0);
        brushedMotor_stop();
        sys_clear_buzz();
        
//        brushedMotor_run(0,0);
    }
}

/**
 * Rotina de incialização da atividade.
 * 
 * @param param Parâmetro opcional.
 */
void ActivityMain_onCreate(void * param) {
    // Codigo de inicialização da atividade
    
    clearDisplayBuffer();
    
    fillRectangle(0, 0, DISPLAY_V_RES, DISPLAY_H_RES, SOLID, ONLY_BUFFER);
    
//    sys_buzz(1000,0);
    
    cache_current_program();
    
//    writeSpeed(current_axis_speed, _this.is_reverse);
//    writePump(_this.pump_level);
//    writeTorque(_this.torque);
//    brushedMotor_setTorque(_this.torque);
//    writeLam(_this.reduction);
//    writeProg(_this.program);
    
    showDisplayBuffer();
    
    buttons_registerCallback(BUTTON_RIGHT_UP, callbackPump);
    buttons_registerCallback(BUTTON_DOWN_LEFT, callbackSpeedDown);
    buttons_registerCallback(BUTTON_RIGHT_DOWN, callbackProg);
    buttons_registerCallback(BUTTON_DOWN_RIGHT, callbackSpeedUp);
    buttons_registerCallback(BUTTON_LEFT_UP, callbackLam);
    buttons_registerCallback(BUTTON_LEFT_DOWN, callbackTorque);

    buttons_registerCallback(BUTTON_FS_CENTER, callbackCenterFS);
    buttons_registerCallback(BUTTON_FS_LEFT, callbackPump);
    buttons_registerCallback(BUTTON_FS_RIGHT, callbackReverse);
    
    
    getDisplayBuffer();
    
}

/**
 * Rotina de finalização da atividade.
 * 
 * @param param Parametro opcional.
 */
void ActivityMain_onDestroy(void * param) {
  // CÃ³digo a ser executado quando a atividade for finalizada
    
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


/**
 * Rotina periódica executada durante 
 * 
 * @param param Parametro opcional.
 */
void ActivityMain_onTick(void * param){
  // CÃ³digo a ser executado ciclicamente durante o ciclo de vida da atividade
    
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
//    writeProg(getFilteredSpeed());
    showDisplayBuffer();
    
    static unsigned int _ct = 0;
    
    if ((_ct++)%20 == 0){
//        resetDisplay(50);
    }
    
}
