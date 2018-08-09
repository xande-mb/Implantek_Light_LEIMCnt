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
#define FCY 40000000
#include <libpic30.h>
//#define DEFINE_VARIABLES

#define texto "Teste"

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
    
    portman_setOutput(DISP_EN);
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
    
    portman_setAsInput(SW1);
    portman_setAsInput(SW2);
    portman_setAsInput(SW3);
    portman_setAsInput(SW4);
    portman_setAsInput(SW5);
    portman_setAsInput(SW6);
    
    portman_setAsInput(FSWF1);
    portman_setAsInput(FSWF2);
    portman_setAsInput(FSWF3);
}

int main(void) {
    //configurações do clock (PLL)
    PLLFBD = 63; // FRC 80MHz
    CLKDIVbits.PLLPOST = 0;
    CLKDIVbits.PLLPRE = 1;

    while (OSCCONbits.LOCK != 1);
    while (OSCCONbits.COSC != 0b001);

    kTasker_init();

    kernel_setMode(MODE_PRODUCTION);
    kTask_t * task = kernel_create_task(task_test, NULL, "TesteTouch", 0);
    
    pinConfig();

    initializeDisplay();
    clearDisplay();

    writeText(texto, 128/2, (64-8)/2,
            VERDANA7, CENTER, SOLID, UPDATE_NOW);

    __delay_ms(250 * 10);
    showDisplayBuffer();
    clearDisplayBuffer();
    
    init_buzz();
    sys_buzz(10, 10);

//        kernel_create_task(ActivityMan_Task, NULL, "ActivityManager", 0);

    //    ActivityMan_startActivity(&activityInicial);

    kTasker();

    return 0;
}