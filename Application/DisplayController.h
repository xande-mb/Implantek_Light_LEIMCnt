/*! \file DisplayController.h
    \brief Atribui��o de pinos aos componentes e perif�ricos do sistema.

    Cont�m as defini��es associadas � atribui��o de pinos espec�ficos
    aos diferentes componentes e perif�ricos do sistema.
 */

#ifndef DISPLAYCONTROLLER_H
#define	DISPLAYCONTROLLER_H

#include "../System/PortManager/portman.h"

//@cond NOT_DOCUMENTED

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

//@endcond


/**
 * Seleciona a coordenada x para a posi��o de escrita no display.
 * 
 * @param x coordenada horizontal.
 */
void setX(unsigned char x);

/**
 * Seleciona a coordenada y para a posi��o de escrita no display.
 * 
 * @param y Coordenada vertical.
 */
void setY(unsigned char y);

/**
 * Prepara a inicializa��o do display.
 */
void initializeDisplay(void);

/**
 * Esvazia o buffer do display.
 */
void clearDisplayBuffer();

/**
 * Atualiza o display com o conte�do armazenado em seu buffer.
 */
void showDisplayBuffer();

/**
 * Apaga todo o conte�do exibido no display.
 */
void clearDisplay();

/**
 * Envia ao display um byte de dado ou de comando.
 * 
 * @param displayRS Registrador do display.
 * @param displayData Byte a ser enviado.
 */
void sendByteDisplay(unsigned char displayRS, unsigned char displayData);

/**
 * Desenha um ret�ngulo preenchido no display.
 * 
 * @param y_i Coordenada vertical inicial.
 * @param x_i Coordenada horizontal inicial.
 * @param y_f Coordenada vertical final.
 * @param x_f Coordenada horizontal final.
 * @param colorOption Op��o de cor.
 * @param update Flag que indica se deve ser feito o update no display logo.
 */
void fillRectangle(unsigned char y_i, unsigned char x_i, unsigned char y_f,
        unsigned char x_f, unsigned char colorOption, unsigned char update);

/**
 * Escreve uma linha de texto no display.
 * 
 * @param txtLine Linha de texto.
 * @param x Posi��o horizontal.
 * @param y Posi��o vertical.
 * @param font Fonte.
 * @param align Alinhamento.
 * @param colorOption Op�c�o de cor
 * @param update Flag que indica se deve ou n�o fazer update no display.
 */
void writeTextLine(char txtLine[], unsigned int x, unsigned char y,
        unsigned char font, unsigned char align,
        unsigned char colorOption, unsigned char update);

/**
 * Escreve texto no display.
 * 
 * @param txt Texto.
 * @param x Coordenada horizontal.
 * @param y Coordenada vertical.
 * @param font Fonte.
 * @param align Alinhamento.
 * @param colorOption Op��o de cor.
 * @param update Flag que indica se a atualiza��o deve ser feita logo.
 */
void writeText(char txt[], unsigned int x, unsigned char y,
        unsigned char font, unsigned char align,
        unsigned char colorOption, unsigned char update);

/**
 * Desenha um pixel no display.
 * 
 * @param x Coordenada horizontal.
 * @param y Coordenada vertical.
 * @param update Flag que indica se deve ser feito update logo.
 */
void putPixel(unsigned char x, unsigned char y, unsigned char update);


#endif	/* DISPLAYCONTROLLER_H */

