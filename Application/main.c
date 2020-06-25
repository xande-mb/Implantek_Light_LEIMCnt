#include <p33EP64MC206.h>
#include "CTTKernel/kernel.h"
#include <PortManager/portman.h>
#include <Application/system_config.h>
#include <system_check_config.h>
#include <string.h>
#include "ActivityManager/ActivityMan.h"
#include "System/UserInterface/Buzzer.h"
#include "PinDefinitions.h"
#include "Peripherals/adc.h"
#include "DisplayController.h"
#include "Activities/ActivityMain.h"
#include "Models/buttonsModel.h"
#include "Models/step_motor.h"
#include "Peripherals/i2ccom.h"

#define FCY 40000000
#include <libpic30.h>
//#define DEFINE_VARIABLES

#define texto "Implantek Light"

#pragma config WDTPOST = PS2048
#pragma config WDTPRE = PR128

#pragma config WINDIS = OFF
#pragma config FWDTEN = OFF
#pragma config ICS = PGD1

#pragma config GWRP = OFF
#pragma config GCP = OFF
#pragma config JTAGEN = OFF

#pragma config POSCMD = NONE
#pragma config IOL1WAY = OFF
#pragma config OSCIOFNC = ON
#pragma config FCKSM = CSDCMD
#pragma config FNOSC = FRCPLL
#pragma config IESO = OFF
//#pragma config PWMPIN = ON              // Motor Control PWM Module Pin Mode bit (PWM module pins controlled by PORT register at device Reset)
#pragma config PWMLOCK = OFF  


unsigned char read_i2c_board_mem(unsigned char csel, unsigned char add);
void write_i2c_board_mem(unsigned char csel, unsigned char add, unsigned char value);


void task_test(void * param) {
    initializeDisplay();
    clearDisplay();

    writeText(texto, 128 / 2, (64 - 8) / 2,
            VERDANA7, CENTER, SOLID, UPDATE_NOW);

    __delay_ms(250 * 10);
    showDisplayBuffer();

    clearDisplayBuffer();

    sTask_DelayUntil(NULL, 5000 * tMILLIS);
}

void pinConfig(void){
    portman_setAsOutput(DISP_EN);
    portman_setAsOutput(DISP_RS);
    portman_setAsOutput(DISP_RW);
    portman_setAsOutput(DB0);
    portman_setAsOutput(DB1);
    portman_setAsOutput(DB2);
    portman_setAsOutput(DB3);
    portman_setAsOutput(DB4);
    portman_setAsOutput(DB5);
    portman_setAsOutput(DB6);
    portman_setAsOutput(DB7);
    portman_setAsOutput(RST);
    portman_setAsOutput(CS1);
    portman_setAsOutput(CS2);
    portman_setAsOutput(DRST);
    portman_setAsOutput(PRESET);
    
    
    portman_setOutput(DISP_EN);
    portman_clearOutput(DISP_RW);
    portman_clearOutput(DB0);
    portman_clearOutput(DB1);
    portman_clearOutput(DB2);
    portman_clearOutput(DB3);
    portman_clearOutput(DB4);
    portman_clearOutput(DB5);
    portman_clearOutput(DB6);
    portman_clearOutput(DB7);
    portman_setOutput(CS1);
    portman_setOutput(CS2);
    portman_clearOutput(DISP_EN);
    portman_clearOutput(DISP_RS);
    portman_setOutput(RST);
    
    
    portman_clearOutput(HLSEL);
    portman_setAsOutput(HLSEL);
    
    portman_clearOutput(VEEC);
    portman_clearOutput(VEEM);
    portman_setAsOutput(VEEC);
    portman_setAsOutput(VEEM);
    
    
//    portman_setAsInput(SW1);
//    portman_setAsInput(SW2);
//    portman_setAsInput(SW3);
//    portman_setAsInput(SW4);
//    portman_setAsInput(SW5);
//    portman_setAsInput(SW6);
//    
//    portman_setAsInput(FSWF1);
//    portman_setAsInput(FSWF2);
//    portman_setAsInput(FSWF3);
    
    
}

void resetDisplay_Task(void * context) {
	
    resetDisplay(1);;
	sTask_Delay(NULL, 5000*tMILLIS);

	return;
}


int main(void) {
    //configura��es do clock (PLL)
//    PLLFBD = 63; // FRC 80M
//    CLKDIVbits.PLLPOST = 0;
//    CLKDIVbits.PLLPRE = 1;
//
//    while (OSCCONbits.LOCK != 1);
//    while (OSCCONbits.COSC != 0b001);
	PLLFBD = 74; // M=76
	CLKDIVbits.FRCDIV = 0;
	CLKDIVbits.PLLPOST = 0; // N1=2
	CLKDIVbits.PLLPRE = 0; // N2=2
	OSCTUN = 0;
	__builtin_write_OSCCONH(0x01);
	__builtin_write_OSCCONL(OSCCON | 0x01);
	while (OSCCONbits.COSC != 0b001);
	// Wait for PLL to lock
            
	while (OSCCONbits.LOCK != 1);
    kTasker_init();
    
    kernel_setMode(MODE_DEBUG);
//    kTask_t * task = kernel_create_task(task_test, NULL, "TesteTouch", 0);
//  
    ANSELB = 0x0000;    
	ANSELC = 0x0000;
//	ANSELD = 0x0000;
	ANSELE = 0x0000;
    
    ANSELBbits.ANSB0 = 1;
    ANSELAbits.ANSA0 = 1;
    ANSELAbits.ANSA12 = 1;
    portman_setAsInput(PORT_A,0);
    portman_setAsInput(PORT_B,0);
    portman_setAsInput(PORT_A,12);
    
    pinConfig();
    stepMotorInit();
    initializeDisplay();
    clearDisplay();
//    init_adc();
//    initHighSpeedPwm();
    LATA;
    
    writeText(texto, 128/2, (64-8)/2 - 10,
            VERDANA7, CENTER, SOLID, UPDATE_NOW);
    
    writeText("v 1.0", 128/2, (64-8)/2+10,
            VERDANA7, CENTER, SOLID, UPDATE_NOW);
    
    
//    fillRectangle(0,0,10,15,SOLID,UPDATE_NOW);
    
    __delay_ms(250 * 10);
    showDisplayBuffer();
    clearDisplayBuffer();
    
    init_buzz();
    sys_buzz(10, 10);
    
    kernel_create_task(ActivityMan_Task, NULL, "ActivityManager", 0);
    
    kernel_create_task(resetDisplay_Task, NULL, "display_reset", 0);
    
    ActivityMan_startActivity(&ActivityMain);
        
    buttonsInit();
    
    i2c_init();
    
//    uint8_t data[64] = {};
//    
//    memset(data, 0xff, 64);
//    portman_setOutput(VEEC);;
//        volatile unsigned int res_w = i2c_write(0xA0, 32, 0, 16, data);
//    __delay32(10000);
//    
//    volatile unsigned char data_read[64] = {};
//    
//    volatile unsigned int res = i2c_read(0xA0, 32, 0, 16, data_read);

//    volatile int i =0;
//    i++;
    
    kTasker();
    
    return 0;
}


unsigned char read_i2c_board_mem(unsigned char add, unsigned char csel){
    portman_setOutput(VEEC);
    volatile unsigned char data_read[4] = {};
    volatile unsigned int res = i2c_read(0xA0, add, csel, 1, data_read);
    __delay32(10000);
//    portman_clearOutput(VEEC);
    return data_read[0];
}

void write_i2c_board_mem(unsigned char add, unsigned char csel, unsigned char value) {
    portman_setOutput(VEEC);
    volatile unsigned char data[4] = {value, 0, 0, 0};
    volatile unsigned int res_w = i2c_write(0xA0, add, csel, 1, data);
    __delay32(10000);
//    portman_clearOutput(VEEC);
}