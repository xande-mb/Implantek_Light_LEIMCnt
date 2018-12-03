#include "adc.h"
#include <p33EP64MC206.h>
#include "../Models/pidFilter.h"

filter_t speedFilter;

typedef struct {
    uint16_t readed;
    uint16_t filtered;
} debug_data_t;

//debug_data_t debug_vals[128];

static volatile uint16_t value_bemf = 0;
static volatile uint16_t value_fmsum = 0;

uint16_t getFilteredSpeed(){
    return speedFilter.FilterValue;
}

void init_adc(){

    
	AD1CON1bits.ADON = 0; //AD Off
	AD1CON1bits.AD12B = 0; //10 Bit Mode
	AD1CON1bits.FORM = 0; //Modo decimal normal
	AD1CON1bits.SSRC = 0b000;//auto-convert
	AD1CON1bits.SSRCG = 0;
	AD1CON1bits.ASAM = 1; //SAMP is auto-set

	AD1CON2bits.VCFG = 0; //AVDD e AVSS
	AD1CON2bits.CSCNA = 1; //Scans inputs for CH0+ during Sample MUXA //verify!
	AD1CON2bits.CHPS = 0; //Converts CH0
	AD1CON2bits.SMPI = 0; //interrupt every 4 cycles
	AD1CON2bits.BUFM = 0; //First half then second half
	AD1CON2bits.ALTS = 0; // MUXA

	AD1CON3bits.ADRC = 0; //ADC clock derived from system clock
	AD1CON3bits.SAMC = 0; // Fsample = FAD/31;
	AD1CON3bits.ADCS = 8; //TP *256 = TAD

	AD1CON4bits.ADDMAEN = 0; // DMA OFF

	AD1CHS0bits.CH0NB = 0;
	AD1CHS0bits.CH0NA = 0;


	//AD1CSSH/AD1CSSL: A/D Input Scan Selection Register
	AD1CSSH = 0;
	AD1CSSL = 0;
    AD1CSSL = 0b0000010000000100;
    
    filter_init(&speedFilter, 30000);

	IFS0bits.AD1IF = 0;			// Clear the A/D interrupt flag bit
	IEC0bits.AD1IE = 1;			// Enable A/D interrupt
	AD1CON1bits.ADON = 0;		// Turn on the A/D converter
	IPC3bits.AD1IP = 1;
    
    
}

uint8_t flag_bemf = 0;

void initAdFMSUM() {
    AD1CSSL = 0b0000010000000000;
    AD1CON1bits.ADON = 1;
    flag_bemf = 0;
}

void initAdBEMF(){
    AD1CSSL = 0b0000000000000100;
    AD1CON1bits.ADON = 1;
    flag_bemf = 1;
}

void __attribute__((__interrupt__, no_auto_psv)) _AD1Interrupt(void) {
    IFS0bits.AD1IF = 0;
    LATBbits.LATB1 = !LATBbits.LATB1;
    if (flag_bemf){
        value_bemf = ADC1BUF0;
        filter_update(&speedFilter, value_bemf);
    } else {
        value_fmsum = ADC1BUF0;
    }
    AD1CON1bits.ADON = 0;
//    static uint16_t ct = 0;
//    if (ct<128){
//        debug_vals[ct].filtered = speedFilter.FilterValue;
//        debug_vals[ct].readed = value;
//        ct++;
//    } else {
//        ct = 0;
//    }
}
