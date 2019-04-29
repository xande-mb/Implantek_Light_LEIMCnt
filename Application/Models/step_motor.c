#include "step_motor.h"
#include "../PinDefinitions.h"
#include "PortManager/portman.h"
#include <p33Exxxx.h>

static uint16_t step_motor_enable = 0;

void stepMotorInit() {
    portman_setAsOutput(PRESET);
    portman_setAsOutput(PSTEP);
    portman_setAsOutput(PDIR);
    portman_setAsDigital(PSLEEP);
    portman_setAsOutput(PSLEEP);
    portman_setOutput(PSLEEP);
    
    RPOR9bits.RP120R = 0b010000;
    OC1CON1 = 0; /* It is a good practice to clear off the control bits initially */
    OC1CON2 = 0;
    OC1CON1bits.OCTSEL = 0b010; /* This selects the timer 2 as the clock input to the OC*/
    OC1CON1bits.TRIGMODE = 0; 
    OC1CON2bits.SYNCSEL = 0b11111;
    OC1R = 1000; /* This is just a typical number, user must calculate based on the
    waveform requirements and the system clock */
    OC1CON1bits.OCM = 0; /* This selects and starts the toggle mode */ 
    T4CON = 0x0020; // internal Tcy/256 clock
	TMR4 = 0;
	PR4 = 0xffff;
	T4CONbits.TON = 1; // turn on timer 3
    IFS1bits.T4IF = 0;
    IPC6bits.T4IP = 1;
}


/**
 * Calculo feito da seguinte forma:
 * 
 * OCRS = (FCY/TIMER_DIV) * (MOTOR_STEP_DEG/360) * (1/MICROSTEPS) * (60/VEL_RPM)
 * 
 * Utilizado: 
 *   FCY = 70MHz;
 *   TIMER_DIV = 64;
 *   MOTOR_STEP_DEG = 1.8deg; (retirado do datasheet)
 *   MICROSTEPS = 32; (modo do controlador, todos os jumpers em alto)
 */
#define RPM2VEL(rpm) (10254/rpm)

void stepMotorRun(unsigned int rpm, unsigned int dir) {
    
    if (rpm>0) {
        unsigned int value = RPM2VEL(rpm);
        OC1R = value/2;
        OC1RS = value;
        OC1CON1bits.OCM = 6;
        portman_setOutput(PRESET);
        step_motor_enable = 1;
    } else {
        OC1CON1bits.OCM = 0;
        portman_clearOutput(PRESET);// = 0;
        step_motor_enable = 0;
    }
    
    if (dir%2){
        portman_setOutput(PDIR);
    } else {
        portman_clearOutput(PDIR);
    }
}

unsigned char stepMotorRunning(){
    return step_motor_enable;
}


