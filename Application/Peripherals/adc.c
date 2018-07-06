#include "adc.h"
#include <p33EP64MC206.h>


void init_adc(){
	
	AD1CON1bits.ADON = 0;
	AD1CON1bits.ASAM = 1;
	AD1CON1bits.SSRC = 0b111;
	
	AD1CON2bits.CSCNA = 1;
	AD1CON2bits.SMPI = 1;
	
	
	
//	AD1CON1 = 0x080E0; // Turn on, auto-convert
//	AD1CON2bits. = 0; // AVdd, AVss, int every conversion, MUXA only
	AD1CON3 = 0x1FFF; // 31 Tad auto-sample, Tad = 256*Tcy
	AD1CSSL = 0b0000000000000000; // No scanned inputs
	AD1CSSH = 0b0000000000000101;
	IEC0bits.AD1IE = 1;
	IFS0bits.AD1IF = 0;
	AD1CON1bits.ADON = 1;
}

void __attribute__((__interrupt__, no_auto_psv)) _ADC1Interrupt(void) {
    IFS0bits.AD1IF = 0;
}