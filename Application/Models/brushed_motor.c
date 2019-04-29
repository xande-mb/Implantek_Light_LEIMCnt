#include "brushed_motor.h"
#include "pidFilter.h"
#include "Application/Peripherals/adc.h"
#include "../PinDefinitions.h"
#include <p33Exxxx.h>

#define FCY 40000000
#define FPWM 1000

#define PRTESTE 500

uint16_t getFilteredSpeed(); //From ADC
pid_context_t pid;
uint16_t safe_stop = 0;

static uint16_t velocity_control = 0;
static uint16_t rpm_speed = 0;
static int ct = 0;
static uint16_t is_rev = 0;
static ramp_t motor_ramp;

/**
 * 4 -> 500
 * 29 -> 1760
 * @param rpm
 * @return 
 */
static uint16_t rpm2bmf(uint16_t rpm){
    return 10 + (rpm-240)/35;
}

static uint16_t rpm2bmf_low(uint16_t rpm){
    return 22 + (rpm-240)/35;
}

void initTimer3(){

    //    T3CON = 0x0020; // internal Tcy/256 clock
    T3CONbits.TCKPS = 1; // internal Tcy/8 clock
	TMR3 = 0;
	PR3 = 650;
	T3CONbits.TON = 0; // turn on timer 3
	T3CONbits.TCS = 0; 
	IPC2bits.T3IP = 5;
    IEC0bits.T3IE = 1;
    IFS0bits.T3IF = 0;
	return;
    
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    
    
    IFS0bits.T3IF = 0;
    T3CONbits.TON = 0;
    AD1CON1bits.SAMP = 0;
    

}

void initHighSpeedPwm(){
    
    ct = 0;
     
    pid.max_out = 5000;
	pid.min_out = 1;
	pid.p_mul = 10;
	pid.p_div = 1;
	pid.i_mul = 1;
	pid.i_div = 1;
	pid.d_mul = 0;
	pid.d_div = 1;
	pid.windup_guard = 5000;
    
    pid_zeroize(&pid);
    
//    TRISBbits.TRISB14 = 0;
//    TRISBbits.TRISB15 = 0;
//    TRISBbits.TRISB12 = 0;
//    TRISBbits.TRISB11 = 0;
    
    PTCON = 0;
    PTCON2 = 0;
    
    IOCON1 = IOCON2 = IOCON3 = 0x0000;
    
    IOCON1bits.PENH = 1; // habilita H
    IOCON1bits.PENL = 1; // Habilita L
    IOCON1bits.POLH = 0; // polaridade active-h
    IOCON1bits.POLL = 0; // polaridade active-h
    IOCON1bits.PMOD = 0b01; // modo 
    if (is_rev){
        IOCON1bits.OVRENH = 0; // polaridade active-h
        IOCON1bits.OVRENL = 1; // polaridade active-h
        IOCON1bits.OVRDAT0 = 1; // polaridade active-h
        IOCON1bits.OVRDAT1 = 0; // polaridade active-h
    } else {
        IOCON1bits.OVRENH = 1; // polaridade active-h
        IOCON1bits.OVRENL = 1; // polaridade active-h
        IOCON1bits.OVRDAT0 = 0; // polaridade active-h
        IOCON1bits.OVRDAT1 = 0; // polaridade active-h   
    }
    
	PTPER = 6000;
    PWMCON1 = 0x0000;
    PWMCON2 = 0x0000;
    
    IOCON2bits.PENH = 1; // habilita H
    IOCON2bits.PENL = 1; // Habilita L
    IOCON2bits.POLH = 0; // polaridade active-h
    IOCON2bits.POLL = 0; // polaridade active-h
    IOCON2bits.PMOD = 0b01; // modo 
    if (is_rev){
        IOCON2bits.OVRENH = 1; // polaridade active-h
        IOCON2bits.OVRENL = 1; // polaridade active-h
        IOCON2bits.OVRDAT0 = 0; // polaridade active-h
        IOCON2bits.OVRDAT1 = 0; // polaridade active-h
        PWMCON1bits.TRGIEN = 1;
    } else {
        IOCON2bits.OVRENH = 0; // polaridade active-h
        IOCON2bits.OVRENL = 1; // polaridade active-h
        IOCON2bits.OVRDAT0 = 1; // polaridade active-h
        IOCON2bits.OVRDAT1 = 0; // polaridade active-h
        PWMCON2bits.TRGIEN = 1;
    }
    
    PTCONbits.SEVTPS = 0;
    PTGCSTbits.PTGEN = 1;
	
	DTR1 = 0;
    
	ALTDTR1 = 0; // dead time
    
    PHASE1 = 0;
    
	PTCON2 = 0b000; // Divide by 1 to generate PWM
	
    
//	PDC1 = PRTESTE; // Initialize as 0 voltage
//	PDC2 = PRTESTE; // Initialize as 0 voltage
    
	FCLCON1bits.FLTMOD = 3;
	FCLCON2bits.FLTMOD = 3;
    
	PTCON = 0x8000; // start PWM
    
    if (is_rev){
        IPC23bits.PWM1IP = 5;
        IEC5bits.PWM1IE = 1;
        IEC5bits.PWM2IE = 0;
    } else {
        IPC23bits.PWM2IP = 5;
        IEC5bits.PWM2IE = 1;       
        IEC5bits.PWM1IE = 0;       
    }
    initTimer3();
	return;
    
}

uint16_t brushedMotor_isRunning(){
    return velocity_control>0;
}

void brushedMotor_run(uint16_t speed, uint16_t direction){
    velocity_control = rpm2bmf(speed);
    rpm_speed = speed;
    if (speed< 1000){
        
        velocity_control = rpm2bmf_low(speed);
        portman_clearOutput(HLSEL);
    } else {
        velocity_control = rpm2bmf(speed);
        portman_setOutput(HLSEL);
    }
    is_rev = direction;
    ramp_init(&motor_ramp, 0, 1, velocity_control);
    if (speed > 0){
        init_adc();
        initHighSpeedPwm();
    } else {
        
    }
}

uint16_t brushedMotor_getVel(){
    
    return getFilteredSpeed();
    
}

uint16_t brushedMotor_getCurrent(){
    
    return getFilteredCurr();
    
}

uint16_t brushedMotor_getControl(){
    return pid.controler;
}

void brushedMotor_stop(){
    velocity_control = 0;
    safe_stop = 0;
//    IOCON1bits.OVRENH = 1; // polaridade active-h
//    IOCON1bits.OVRENL = 1; // polaridade active-h
//    IOCON1bits.OVRDAT0 = 0; // polaridade active-h
//    IOCON1bits.OVRDAT1 = 0; // polaridade active-h
//
//    IOCON2bits.OVRENH = 1; // polaridade active-h
//    IOCON2bits.OVRENL = 1; // polaridade active-h
//    IOCON2bits.OVRDAT0 = 0; // polaridade active-h
//    IOCON2bits.OVRDAT1 = 0; // polaridade active-h
//    IEC5bits.PWM1IE = 0;    
}

static unsigned int curr_lim = 6;

void brushedMotor_setTorque(unsigned int value){
    curr_lim = value*2;//6 + (value-5)*6/5;
}

void verify_current_limit(){
    if (brushedMotor_getCurrent() > curr_lim + rpm_speed/1500){
        velocity_control = 0;
        safe_stop = 1;
    }
}

uint16_t brushedMotor_isStopped(){
    return safe_stop;
}

void doControl(){
    
    ct++;
    if (ct == 1){
        static uint16_t tf = 0;
        
        if (tf)
            ramp_update(&motor_ramp,1);
        
        tf = !tf;
        
        PDC1 = 0;
        PDC2 = 0;
    } else if (ct == 2){
        
        // Control!!
        PDC1 = 0;
        PDC2 = 0;
    } else if (ct == 5){
            
         TMR3 = 0;
        IFS0bits.T3IF = 0;
        T3CONbits.TON = 1;
//        LATBbits.LATB1 = 1;
        initAdBEMF(is_rev);       
    } else if (ct == 6){
        
        if (velocity_control){
            if (pid.controler < 10) pid.controler = 10;
            if (pid.controler > 5000) pid.controler = 5000;
            PDC1 = pid.controler;
            PDC2 = pid.controler;
        }
    } else if (ct == 7) {
        initAdFMSUM();
    } else if (ct == 9) {
        AD1CON1bits.SAMP = 0;
    } else if (ct == 20) {
        verify_current_limit();
        uint16_t val = getFilteredSpeed();
        pid_update(&pid,val,motor_ramp.current_value,1);
        ct = 0;
    }
}

void __attribute__((interrupt, no_auto_psv)) _PWM1Interrupt(void) {
    
    IFS5bits.PWM1IF = 0;

    doControl();
    
}

void __attribute__((interrupt, no_auto_psv)) _PWM2Interrupt(void) {
    
    IFS5bits.PWM2IF = 0;

    doControl();
    
}


//void initHighSpeedPwm(){
//    
//    TRISBbits.TRISB14 = 0;
//    TRISBbits.TRISB15 = 0;
//    
///* Set PWM Period on Primary Time Base */
//PTPER = 2000;
///* Set Phase Shift */
//PHASE1 = 0;
//PHASE2 = 0;
//PHASE3 = 0;
///* Set Duty Cycles */
//PDC1 = 900;
//PDC2 = 900;
//PDC3 = 900;
///* Set Dead Time Values */
//DTR1 = DTR2 = DTR3 = 0;
//ALTDTR1 = ALTDTR2 = ALTDTR3 = 0;
///* Set PWM Mode to Push-Pull */
//IOCON1 = IOCON2 = IOCON3 = 0xC000;
///* Set Primary Time Base, Edge-Aligned Mode and Independent Duty Cycles */
//PWMCON1 = PWMCON2 = PWMCON3 = 0x0000;
///* Configure Faults */
//FCLCON1 = FCLCON2 = FCLCON3 = 0x0003;
///* 1:1 Prescaler */
//PTCON2 = 0x0000;
///* Enable PWM Module */
//PTCON = 0x8000;
//    
//    
//}