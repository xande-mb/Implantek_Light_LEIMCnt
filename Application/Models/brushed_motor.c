#include "brushed_motor.h"
#include "pidFilter.h"
#include <p33Exxxx.h>

#define FCY 40000000
#define FPWM 1000

#define PRTESTE 500

uint16_t getFilteredSpeed(); //From ADC
pid_context_t pid;

void initTimer3(){
//    T3CON = 0x0020; // internal Tcy/256 clock
    T3CONbits.TCKPS = 1; // internal Tcy/8 clock
	TMR3 = 0;
	PR3 = 650;
	T3CONbits.TON = 0; // turn on timer 3
	T3CONbits.TCS = 0; 
	IPC2bits.T3IP = 1;
    IEC0bits.T3IE = 1;
    IFS0bits.T3IF = 0;
	return;
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0;
    T3CONbits.TON = 0;
    AD1CON1bits.SAMP = 0;
//    LATBbits.LATB1 = 0;
}

void initHighSpeedPwm(){
    
    pid.max_out = 10000;
	pid.min_out = 1;
	pid.p_mul = 15;
	pid.p_div = 1;
	pid.i_mul = 1;
	pid.i_div = 4;
	pid.d_mul = 0;
	pid.d_div = 1;
	pid.windup_guard = 5000;
    
    pid_zeroize(&pid);
    
    TRISBbits.TRISB14 = 0;
    TRISBbits.TRISB15 = 0;
    TRISBbits.TRISB12 = 0;
    TRISBbits.TRISB11 = 0;
    
    PTCON = 0;
    PTCON2 = 0;
    
    IOCON1 = IOCON2 = IOCON3 = 0x0000;
    
    IOCON1bits.PENH = 1; // habilita H
    IOCON1bits.PENL = 1; // Habilita L
    IOCON1bits.POLH = 0; // polaridade active-h
    IOCON1bits.POLL = 0; // polaridade active-h
    IOCON1bits.PMOD = 0b01; // modo 
    IOCON1bits.OVRENH = 0; // polaridade active-h
    IOCON1bits.OVRENL = 1; // polaridade active-h
    IOCON1bits.OVRDAT0 = 0; // polaridade active-h
    IOCON1bits.OVRDAT1 = 0; // polaridade active-h
    
	PTPER = 6000;
    
    IOCON2bits.PENH = 1; // habilita H
    IOCON2bits.PENL = 1; // Habilita L
    IOCON2bits.POLH = 0; // polaridade active-h
    IOCON2bits.POLL = 0; // polaridade active-h
    IOCON2bits.PMOD = 0b01; // modo 
    IOCON2bits.OVRENH = 1; // polaridade active-h
    IOCON2bits.OVRENL = 1; // polaridade active-h
    IOCON2bits.OVRDAT0 = 1; // polaridade active-h
    IOCON2bits.OVRDAT1 = 0; // polaridade active-h
    PTCONbits.SEVTPS = 0;
    PTGCSTbits.PTGEN = 1;
	
	DTR1 = 0;
    
	ALTDTR1 = 0; // dead time
    
    PHASE1 = 0;
    
	PTCON2 = 0b000; // Divide by 1 to generate PWM
    
	PWMCON1 = 0x0000;
    
	PWMCON1bits.TRGIEN = 1;
    
	PDC1 = PRTESTE; // Initialize as 0 voltage
	PDC2 = PRTESTE; // Initialize as 0 voltage
    
	FCLCON1bits.FLTMOD = 3;
	FCLCON2bits.FLTMOD = 3;
    
	PTCON = 0x8000; // start PWM
    
    IPC23bits.PWM1IP = 5;
    IEC5bits.PWM1IE = 1;
    
    TRISBbits.TRISB1 = 0;
    initTimer3();
	return;
    
}

void __attribute__((interrupt, no_auto_psv)) _PWM1Interrupt(void) {
    
    IFS5bits.PWM1IF = 0;
    static int ct = 0;
    
    ct++;
    if (ct == 1){
        initAdBEMF();
    } else if (ct == 3){
        TMR3 = 0;
        IFS0bits.T3IF = 0;
        T3CONbits.TON = 1;
        // Control!!
        PDC1 = 0;
    } else if (ct == 4){
        
        PDC1 = pid.controler;
    } else if (ct == 5) {
        initAdFMSUM();
    } else if (ct == 10) {
        AD1CON1bits.SAMP = 0;
    } else if (ct == 20) {
        uint16_t val = getFilteredSpeed();
        pid_update(&pid,val,0x100,1);
        ct = 0;
    }
    
//    LATBbits.LATB1 = 1;
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