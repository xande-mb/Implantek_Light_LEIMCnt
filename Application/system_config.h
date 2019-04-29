/* 
 * File:   system_config.h
 * Author: DMC
 *
 * Created on 14 de Outubro de 2016, 11:13
 */

#ifndef SYSTEM_CONFIG_H
#define	SYSTEM_CONFIG_H


#ifdef	__cplusplus
extern "C" {
#endif

	/**
	 * Configurações gerais do hardware - evitar dependências do System e Kernel
	 * Parâmetros ainda travados/não configurados:
	 *		- Cores em 16 bpp
	 *		- Epson S1D13517
	 *		- Memória RAM
	 *		- Clock uC = 80 MHz (para timer do Touch)
	 *
	 */

	//TODO: Remover isso!
//#ifndef _SUPPRESS_PLIB_WARNING
//#define _SUPPRESS_PLIB_WARNING
//#endif

	/**
	 * Definir clock do processador
	 */
#define SYSTEM_CLOCK 40000000ul

	/**
	 * Definir fator de divisão do clock do processador para o clock dos periféricos
	 * */
#define PERIPHERAL_CLOCK_DIV (1 << OSCCONbits.PBDIV)

#define EQUIP_NAME "<dmc:Triplet>\n"

	/**
	 *  Definir módulos utilizadas pelo sistema aqui
	 */

	/**
	 * Utilizar criptografia
	 */
//#define	SYSTEM_USES_CRYPTO

	/**
	 * Utilizar Memória Flash Serial
	 */
#define	SYSTEM_USES_MEMORY_FLASH
#define SERIAL_FLASH_SS_PORT PORT_C,15 // Desabilitar quaisquer periféricos compartilhados na porta

//#define SYSTEM_USES_INTERFACE_BT_LIB
    
	/**
	 * Utilizar Buzzer
	 */
#define	SYSTEM_USES_BUZZER


#define BUZZER_PORT PORT_C,0

//#define BUZZER_USES_TIMER 2

#define SYSTEM_USES_ACTIVITY_MANAGER

//#define SYSTEM_USES_DISPLAY_CONTROLER

//#define SYSTEM_USES_FAN

#define SYSTEM_USES_MEMORY_EEPROM

#define SYSTEM_USES_UART_INTERFACE
    
//#define SYSTEM_USES_CALIBRATION_EEPROM
    
//#define SYSTEM_USES_DEFAULT_TOUCH_CALIB_ACTIVITY

#define SYSTEM_USES_PORTMAN

	/**
	 Define tamanho do HEAP a ser alocado
	 */
#define D_ALLOC_HEAP 2048

	/**
	 Define alinhamento em bytes do HEAP
	 */
#define D_ALLOC_ALIGNMENT 8

	/**
	 * Escolher timer para kernel
	 */
#define SYSTEM_KERNEL_TIMER 1

	/**
	 * Escolher Uart para system
	 */
#define SYSTEM_UART 2

	/**
	 * Escolher timer para touch
	 */
//#define SYSTEM_USES_TOUCH
//#define SYSTEM_TOUCH_TIMER 4
    
//#define TOUCH_YPOS_PORT PORT_C,4
//#define TOUCH_XPOS_PORT PORT_G,7
//#define TOUCH_YNEG_PORT PORT_A,2
//#define TOUCH_XNEG_PORT PORT_A,1
//    
//#define TOUCH_YPOS_PORT PORT_A,2
//#define TOUCH_XPOS_PORT PORT_A,1
//#define TOUCH_YNEG_PORT PORT_C,4
//#define TOUCH_XNEG_PORT PORT_G,7
    
    
    // configurações alternativas:
    
//    
//#define ALTERNATIVE_INTERFACE_FONTS "Application/DisplayControler24/fonts_images_legacy.h"
//#define ALTERNATIVE_PICTURE_MANAGER "Application/DisplayControler24/fonts_images_legacy.h"
//#define ALTERNATIVE_DISPLAY_CONTROLER "Application/DisplayControler24/fonts_images_legacy.h"
     
	/**
	 * Configuração das portas utilizadas pelo touch
	 * ADC deve ser configurado para ler constantemente as portas
	 * utilizadas e executar função adc_update_touch(leitura_x, leitura_y) a
	 * cada interrupção após a leitura das portas x e y. AD_IS_RUNNING deve ser definido após
	 * a inicialização do AD.
	 * A calibração deve ser realizada pela aplicação (inclusive gravação em memória)
	 * e os valores de calibração informados através da função:
	 * touch_set_calibration(calXMin, calXMax, calYMin, calYMax)
	 */

//#define TOUCH_YPOS_PORT PORT_B,8
//#define TOUCH_XPOS_PORT PORT_B,9
//#define TOUCH_YNEG_PORT PORT_A,9
//#define TOUCH_XNEG_PORT PORT_A,10

	/**
	 * Hardware - definir portas utilizadas
	 */

	/**
	 * Ajuste de brilho do backlight
	 */
//#define BACKLIGHT_PWM_PORT PORT_C,3

	/**
	 * Enable do backlight
	 */
//#define BACKLIGHT_EN_PORT PORT_A,0
    
    
//#define BACKLIGHT_EN_ACTIVE_LOW
    
    
//#define BACKLIGHT_EN_ACTIVE_HIGH

	/**
	 * Reset do controlador do display
	 */
//#define DISPC_RESET_PORT PORT_C,1

	/**
	 * Data/Command
	 */
//#define DISPC_RS_PORT PORT_C,2

	/**
	 * ChipSelect
	 */
//#define DISPC_CS_PORT PORT_D,10

	// Frame/Display utilizado
	//#define LCDFrame_A
	//#define LCDFrame_B
	//#define LCDFrame_C
//#define LCDFrame_D

	// Seleciona controlador de brilho do backlight
//#define BACKLIGHT_PWM_PIC
	//#define BACKLIGHT_PWM_EPSON

	// Utilizar pino PWM como enable do display
//#define BACKLIGHT_USES_PWM_AS_ENABLE


	/**
	 * Definição de FONTES instaladas
	 */

//#define SYSTEM_USES_FONT_VERDANA_18
//#define SYSTEM_USES_FONT_VERDANA_18_N
//#define SYSTEM_USES_FONT_VERDANA_22
//#define SYSTEM_USES_FONT_VERDANA_22_N
//#define SYSTEM_USES_FONT_VERDANA_26
//#define SYSTEM_USES_FONT_VERDANA_28
//#define SYSTEM_USES_FONT_VERDANA_28_N
//#define SYSTEM_USES_FONT_VERDANA_36
//#define SYSTEM_USES_FONT_VERDANA_36_N
//#define SYSTEM_USES_FONT_VERDANA_48
	//#define SYSTEM_USES_FONT_KLUNSTER_36

//#define SYSTEM_FONT_DEFAULT VERDANA_18

	//#define FONTS_INSTALLED_T FONTS_AVAILABLE_T


	/**
	 * Definição do tema padrão
	 */

	/*#define SYSTEM_INTERFACE_DEFAULT_THEME_DEFINITIONS \
				.font_title = VERDANA_22, \
				.font_subtitle = VERDANA_18, \
				.font_dialog = VERDANA_18, \
				.font_big_button = VERDANA_22, \
				.font_small_button = VERDANA_18, \
				.font_big_number = VERDANA_22, \
	 */

//#define SYSTEM_USES_CRYPTO

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_CONFIG_H */

