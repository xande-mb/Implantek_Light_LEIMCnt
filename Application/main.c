#include <p33EP64MC206.h>
#include "CTTKernel/kernel.h"
#include <PortManager/portman.h>
#include <Application/system_config.h>
#include <system_check_config.h>
#include <string.h>
#include "ActivityManager/ActivityMan.h"
#include "System/UserInterface/Buzzer.h"
//#define FCY 16000000
#include <libpic30.h>
//#define DEFINE_VARIABLES

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


void task_test(void * param){
    static uint8_t flag;
    
    if(flag == 0){
        portman_setOutput(PORT_A, 10);
        flag = 1;
    } else{
        portman_clearOutput(PORT_A, 10);
        flag = 0;
    }
    
	sTask_DelayUntil(NULL, 1000 * tMILLIS);
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
    
    portman_setAsOutput(PORT_A, 10);
    
    init_buzz();
    sys_buzz(10, 10);
    
    kTask_t * task = kernel_create_task(task_test,NULL,"TesteTouch",0);
//    kernel_create_task(ActivityMan_Task, NULL, "ActivityManager", 0);
    
//    ActivityMan_startActivity(&activityInicial);

	kTasker();

	return 0;
}