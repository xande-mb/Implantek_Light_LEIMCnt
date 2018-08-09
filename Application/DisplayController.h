/*! \file DisplayController.h
    \brief Atribuição de pinos aos componentes e periféricos do sistema.

    Contém as definições associadas à atribuição de pinos específicos
    aos diferentes componentes e periféricos do sistema.
 */

#ifndef DISPLAYCONTROLLER_H
#define	DISPLAYCONTROLLER_H

#include "../System/PortManager/portman.h"

// Fontes e seu alinhamento
#define	VERDANA7                0
#define	LEFT					0
#define	CENTER					1
#define	RIGHT					2

#define	SOLID					0
#define	INVERT					1
#define	ERASE					2
#define	TRANSP					3
#define	ONLY_BUFFER				0
#define	UPDATE_NOW				1

// Display
#define	DISPLAY_H_RES				128
#define	DISPLAY_V_RES				64
#define	DISPLAY_H_MIDDLE			(DISPLAY_H_RES / 2)
#define	DISPLAY_H_END				(DISPLAY_H_RES - 1)
#define	DISPLAY_H_QUARTER			(DISPLAY_H_RES / 4)
#define	DISPLAY_H_3QUARTERS			(3 * (DISPLAY_H_RES / 4))

#define	DISPLAY_LINE_1				4
#define	DISPLAY_LINE_2				20
#define	DISPLAY_LINE_3				36
#define	DISPLAY_LINE_4				52

/**** Display *****/

//! Seleciona a coordenada x para a posição de escrita no display.
void setX(unsigned char x);
//! Seleciona a coordenada y para a posição de escrita no display.
void setY(unsigned char y);
//! Prepara a inicialização do display.
void initializeDisplay(void);
//! Esvazia o buffer do display.
void clearDisplayBuffer();
//! Atualiza o display com o conteúdo armazenado em seu buffer.
void showDisplayBuffer();
//! Apaga todo o conteúdo exibido no display.
void clearDisplay();
//! Envia ao display um byte de dado ou de comando.
void sendByteDisplay(unsigned char displayRS, unsigned char displayData);
//! Desenha um retângulo preenchido no display.
void fillRectangle(unsigned char y_i, unsigned char x_i, unsigned char y_f,
        unsigned char x_f, unsigned char colorOption, unsigned char update);
//! Escreve uma linha de texto no display.
void writeTextLine(char txtLine[], unsigned int x, unsigned char y,
        unsigned char font, unsigned char align,
        unsigned char colorOption, unsigned char update);
//! Escreve texto no display.
void writeText(char txt[], unsigned int x, unsigned char y,
        unsigned char font, unsigned char align,
        unsigned char colorOption, unsigned char update);
//! Desenha um pixel no display.
void putPixel(unsigned char x, unsigned char y, unsigned char update);


#endif	/* DISPLAYCONTROLLER_H */

