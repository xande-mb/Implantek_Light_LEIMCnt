/*! \file DisplayController.c
    \brief Controle do display e exibição de formas e de texto no mesmo.

    Contém funções de controle do display e de manipulação do conteúdo nele
    exibido, como desenho de formas geométricas e escrita de texto.
 */

#include <p33Exxxx.h>
#include "DisplayController.h"
//#include "GeneralDefinitions.h"
//#include "GeneralFunctions.h"
#include <string.h>
#include <stdio.h>
#include "DisplayFonts.h"
#include "PortManager/portman.h"
#include "PinDefinitions.h"
#include <libpic30.h>

#define dataAsInput() portman_setAsInput(DB0);portman_setAsInput(DB1);portman_setAsInput(DB2);portman_setAsInput(DB3);\
                        portman_setAsInput(DB4);portman_setAsInput(DB5);portman_setAsInput(DB6);portman_setAsInput(DB7);

#define dataAsOutput() portman_setAsOutput(DB0);portman_setAsOutput(DB1);portman_setAsOutput(DB2);portman_setAsOutput(DB3);\
                        portman_setAsOutput(DB4);portman_setAsOutput(DB5);portman_setAsOutput(DB6);portman_setAsOutput(DB7);


unsigned char readByteDisplay(unsigned char displayRS);

static unsigned int is_shutdown = 0;

struct {
    //! Conteúdo do buffer do display
    unsigned char buff[1536];
//    unsigned char buff[256];
    //! Endereço inicial
    unsigned char startAddress;
    //! Passo de incremento
    unsigned char deltaPx;

} displayBuffer;


//! Apaga todo o conteúdo exibido no display.

/*!
    Chama a rotina de esvaziamento do buffer do display,
    atualizando-o em seguida
 */
void clearDisplay() {

    //apaga o conteúdo do buffer
    clearDisplayBuffer();
    //atualiza o display com a memória limpa
    showDisplayBuffer();
}

//! Prepara a inicialização do display.

/*!
    Executa as operações necessária à inicialização do funcionamento do display
 */
void initializeDisplay() {
    portman_setOutput(DRST);
    __delay_ms(10);
    portman_clearOutput(DISP_EN);
    portman_clearOutput(DISP_RS);

    //zera CS1 e CS2
    portman_setOutput(CS1);
    portman_setOutput(CS2);
    __delay_ms(10);

    sendByteDisplay(0, 0x3F); //LCD ON
    __delay_ms(10);

    sendByteDisplay(0, 0xB8); //x0
    __delay_ms(10);

    sendByteDisplay(0, 0x40); //y0
    __delay_ms(10);

    sendByteDisplay(0, 0xC0); //z0
    __delay_ms(10);
}


//! Seleciona a coordenada x para a posição de escrita no display.

/*!
    \param x coordenada x da posição de escrita no display
 */
void setX(unsigned char x) {

    x = x + 0xB8;
    sendByteDisplay(0, x);
    __delay_us(5);
}


//! Seleciona a coordenada y para a posição de escrita no display.

/*!
    \param y coordenada y da posição de escrita no display
 */
void setY(unsigned char y) {

    y = y + 0x40;
    sendByteDisplay(0, y);
    __delay_us(5);
}

void displayDown(){
//    portman_clearOutput(DRST);
    portman_clearOutput(DISP_EN);
    portman_clearOutput(DISP_RS);
    portman_clearOutput(DISP_RW);
    portman_clearOutput(RST);
    portman_clearOutput(CS1);
    portman_clearOutput(CS2);
    portman_clearOutput(DB0);
    portman_clearOutput(DB1);
    portman_clearOutput(DB2);
    portman_clearOutput(DB3);
    portman_clearOutput(DB4);
    portman_clearOutput(DB5);
    portman_clearOutput(DB6);
    portman_clearOutput(DB7);
    
    is_shutdown = 1;
}

void resetDisplay(unsigned int time_us) {
    displayDown();
//    __delay_us(time_us*100);
//        portman_setOutput(DISP_EN);
//    portman_setOutput(DRST);
    __delay_us(100);
    is_shutdown = 0;
    portman_setOutput(RST);
    portman_setOutput(DISP_RS);
//    __delay_us(time_us*1000);
//    __delay_us(time_us*1000);
//    __delay_us(time_us*1000);
    
//    portman_setOutput(DRST);
    __delay_us(50);
    initializeDisplay();
    __delay_us(50);        
    showDisplayBuffer();
}


//! Atualiza o display com o conteúdo armazenado em seu buffer.

/*!
    Atualiza o display para que o mesmo exiba o conteúdo armazenado no buffer
 */
void showDisplayBuffer() {
    
//    resetDisplay();
    
    if (is_shutdown) return;

    unsigned int i, j;

    portman_setOutput(CS1);
    portman_setOutput(CS2);

    setY(0);

    //percorre as páginas do display
    for (i = 0; i < 8; i++) {

        portman_setOutput(CS1); //tela 1
        portman_clearOutput(CS2);
        setX(i);

        //percorre as colunas da página
        for (j = 0; j < DISPLAY_V_RES; j++)
            sendByteDisplay(1, displayBuffer.buff[DISPLAY_H_RES * i + j]);

        portman_clearOutput(CS1);
        portman_setOutput(CS2); //tela2

        setX(i);

        //percorre as colunas da página
        for (j = 64; j < DISPLAY_H_RES; j++)
            sendByteDisplay(1, displayBuffer.buff[DISPLAY_H_RES * i + j]);
    }
}

/**
 * 
 */
void getDisplayBuffer() {

//    unsigned int i, j;
//
//    portman_setOutput(CS1);
//    portman_setOutput(CS2);
//
//    setY(0);
//
//    //percorre as páginas do display
//    for (i = 0; i < 8; i++) {
//
//        portman_setOutput(CS1); //tela 1
//        portman_clearOutput(CS2);
//        setX(i);
//
//        //percorre as colunas da página
//        unsigned char x = readByteDisplay(1);
//        for (j = 0; j < DISPLAY_V_RES; j++){
////            sendByteDisplay(1, displayBuffer.buff[DISPLAY_H_RES * i + j]);
//            volatile unsigned char data1 = displayBuffer.buff[DISPLAY_H_RES * i + j],
//            data_2 = readByteDisplay(1),ikkk=0;
//            if (data1 != data_2){
//                ikkk++;
//            }
//        }
//        portman_clearOutput(CS1);
//        portman_setOutput(CS2); //tela2
//
//        setX(i);
//
//        //percorre as colunas da página
//        for (j = 64; j < DISPLAY_H_RES; j++)
//            sendByteDisplay(1, displayBuffer.buff[DISPLAY_H_RES * i + j]);
//    }
}


//! Esvazia o buffer do display.

/*!
    Apaga todo o conteúdo do buffer do display
 */
void clearDisplayBuffer() {

    int i;

    for (i = 0; i < 1536; i++)
//    for (i = 0; i < 256; i++)
        displayBuffer.buff[i] = 0;
}


/*!
    Envia ao display um byte de dado ou de comando.

    \param displayRS estado da linha RS do display
    \param displayData byte a ser enviado
 */
void sendByteDisplay(unsigned char displayRS, unsigned char displayData) {
    
    if (is_shutdown) return;

    (displayRS == 0) ? portman_clearOutput(DISP_RS) : portman_setOutput(DISP_RS);
    Nop();

    //escreve os bits do dado em cada porta, de forma paralela
    (displayData & 0b00000001) == 0 ? portman_clearOutput(DB0) : portman_setOutput(DB0);
    ((displayData >> 1) & 0b00000001) == 0 ? portman_clearOutput(DB1) : portman_setOutput(DB1);
    ((displayData >> 2) & 0b00000001) == 0 ? portman_clearOutput(DB2) : portman_setOutput(DB2);
    ((displayData >> 3) & 0b00000001) == 0 ? portman_clearOutput(DB3) : portman_setOutput(DB3);
    ((displayData >> 4) & 0b00000001) == 0 ? portman_clearOutput(DB4) : portman_setOutput(DB4);
    ((displayData >> 5) & 0b00000001) == 0 ? portman_clearOutput(DB5) : portman_setOutput(DB5);
    ((displayData >> 6) & 0b00000001) == 0 ? portman_clearOutput(DB6) : portman_setOutput(DB6);
    ((displayData >> 7) & 0b00000001) == 0 ? portman_clearOutput(DB7) : portman_setOutput(DB7);
    __delay_us(2);

    //finaliza o envio
    portman_setOutput(DISP_EN);
    __delay_us(2);
    portman_clearOutput(DISP_EN);
    __delay_us(2);
}

unsigned char readByteDisplay(unsigned char displayRS) {
    
    dataAsInput();
    
    portman_setOutput(DISP_RW);
    
    unsigned char displayData = 0;
    (displayRS == 0) ? portman_clearOutput(DISP_RS) : portman_setOutput(DISP_RS);
    Nop();
    
    __delay_us(2);

    //finaliza o envio
    portman_setOutput(DISP_EN);
    __delay_ms(10);
//    portman_clearOutput(DISP_EN);
//    __delay_us(2);
    
    displayData = (portman_getInput(DB0)) +
                  (portman_getInput(DB1) << 1) +
                  (portman_getInput(DB2) << 2) +
                  (portman_getInput(DB3) << 3) +
                  (portman_getInput(DB4) << 4) +
                  (portman_getInput(DB5) << 5) +
                  (portman_getInput(DB6) << 6) +
                  (portman_getInput(DB7) << 7);

    __delay_us(2);

    //finaliza o envio
//    portman_setOutput(DISP_EN);
//    __delay_us(2);
    portman_clearOutput(DISP_EN);
    __delay_us(2);
    
    dataAsOutput();
    
    portman_clearOutput(DISP_RW);
    
    return displayData;
}


//! Desenha um pixel no display.

/*!
     Desenha um pixel no display.

    \param x coordenada x do pixel
    \param y coordena y do pixel
    \param update UPDATE_NOW se a atualização do display deve ser instantânea;
    ONLY_BUFFER se a alteração deve ser apenas no buffer
 */
void putPixel(unsigned char x, unsigned char y, unsigned char update) {

    //desenha um retângulo de 1x1 pixel, com cor sólida
    fillRectangle(x, y, x, y, SOLID, update);
}


//! Desenha um retângulo preenchido no display.

/*!
    Desenha um retângulo preenchido no display, preenchendo adequadamente
    as colunas de pixels das páginas do display
 
    \param y_i coordenada y do vértice superior esquerdo do retângulo
    \param x_i coordenada x do vértice superior esquerdo do retângulo
    \param y_f coordenada y do vértice inferior direito do retângulo
    \param x_f coordenada x do vértice inferior direito do retângulo
    \param colorOption SOLID para acender os pixels, ERASE para apagá-los
    ou INVERT para inverter seu estado
    \param update UPDATE_NOW se a atualização do display deve ser instantânea
    ou ONLY_BUFFER se a alteração deve ser apenas no buffer
 */
void fillRectangle(unsigned char y_i, unsigned char x_i, unsigned char y_f,
        unsigned char x_f, unsigned char colorOption, unsigned char update) {

    unsigned char page_i, //página inicial a ser alterada
            page_f, //página final a ser alterada
            numPages, //número de páginas a serem alteradas
            fillPattern; //padrão de pixels a ser transferido para uma
    //coluna de uma das páginas

    unsigned char i, j;

    //cada página compreende 8 linhas do display
    page_i = y_i / 8;
    page_f = y_f / 8;
    numPages = page_f - page_i + 1;

    //criação do padrão da região que será preenchida

    //para cada página do display
    for (i = 0; i < numPages; i++) {

        //se é a primeira página
        if (i == 0) {

            //se a página não será inteiramente preenchida
            if ((numPages == 1) && (((y_f + 1) % 8) > 0))
                fillPattern = 0b11111111 << (y_i % 8) &
                (0b11111111 >> (8 - ((y_f + 1) % 8)));

                //se a página será inteiramente preenchida
            else
                fillPattern = 0b11111111 << (y_i % 8);

            //se não é a primeira página
        } else {

            //se é a última página que não é inteiramente preenchida
            if (((i + 1) == numPages) && (((y_f + 1) % 8) > 0))
                fillPattern = 0b11111111 >> (8 - ((y_f + 1) % 8));

            else
                fillPattern = 0b11111111;
        }

        //para cada coluna da página
        for (j = x_i; j < x_f + 1; j++) {

            switch (colorOption) {

                    //acender os pixels da coluna
                case SOLID:
                    displayBuffer.buff[((page_i + i) * DISPLAY_H_RES) + j] |=
                            fillPattern;
                    break;

                    //inverter o estado dos pixels da coluna
                case INVERT:
                    displayBuffer.buff[((page_i + i) * DISPLAY_H_RES) + j] ^=
                            fillPattern;
                    break;

                    //apagar os pixels da coluna
                case ERASE:
                    displayBuffer.buff[((page_i + i) * DISPLAY_H_RES) + j] &=
                            (fillPattern ^ 0b11111111);
                    break;

                default:
                    break;
            }
        }
    }

    if (update == UPDATE_NOW)
        showDisplayBuffer();
}


//! Escreve texto no display.

/*!
    Escreve texto no display.
 
    \param txt string contendo o texto a ser escrito
    \param x coordenada x da posição inicial do texto
    \param y coordenada y da posição inicial do texto
    \param font fonte a ser utilizada
    \param align alinhamento do texto: RIGHT, CENTER ou LEFT
    \param colorOption SOLID para acender os pixels, ERASE para apagá-los
    ou INVERT para inverter seu estado
    \param update UPDATE_NOW se a atualização do display deve ser instantânea
    ou ONLY_BUFFER se a alteração deve ser apenas no buffer
 */
void writeText(char txt[], unsigned int x, unsigned char y,
        unsigned char font, unsigned char align,
        unsigned char colorOption, unsigned char update) {

    int i = 0;
    char txtBuffer[strlen(txt)];

    strcpy(txtBuffer, txt);

    char * pch;
    //obtém a primeira linha do texto
    pch = strtok(txtBuffer, "\n");

    //para cada linha do texto
    for (i = 0; pch != NULL; i++) {

        writeTextLine(pch, x, y + (i * 16), font, align,
                colorOption, update);
        //obtém a próxima linha
        pch = strtok(NULL, "\n");
    }
}


//! Escreve uma linha de texto no display.

/*!
    Escreve uma linha de texto no display.

    \param txtLine string contendo a linha de texto a ser escrita
    \param x coordenada x da posição inicial da linha
    \param y coordenada y da posição inicial da linha
    \param font fonte a ser utilizada
    \param align alinhamento do texto: RIGHT, CENTER ou LEFT
    \param colorOption SOLID para acender os pixels, ERASE para apagá-los
    ou INVERT para inverter seu estado
    \param update UPDATE_NOW se a atualização do display deve ser instantânea
    ou ONLY_BUFFER se a alteração deve ser apenas no buffer
 */
void writeTextLine(char txtLine[], unsigned int x, unsigned char y,
        unsigned char font, unsigned char align,
        unsigned char colorOption, unsigned char update) {

    struct {
        unsigned char length, line_tt, column_tt, ascii_val, j,
        k, l, lim_px, p2_px, x, y;
        unsigned int line_sz, i, pixel_tt;
    } Text;

    struct {
        unsigned int voffsetx[160], i;
        unsigned char vxsize[160], aux;
    } Font;

    Text.length = strlen(txtLine);

    switch (font) {

        case VERDANA7:

            y -= 2;
            Text.line_tt = 12; //10;
            Text.line_sz = 143; //bytes por linha

            Font.vxsize[0] = horizontalLengthsVerdana7[0];
            Font.voffsetx[0] = 0;

            for (Text.i = 1; Text.i < 160; Text.i++) {

                Font.vxsize[Text.i] = horizontalLengthsVerdana7[Text.i];
                Font.voffsetx[Text.i] = Font.voffsetx[Text.i - 1] +
                        (horizontalLengthsVerdana7[Text.i - 1] / 8);

                if ((horizontalLengthsVerdana7[Text.i - 1] % 8) > 0)
                    Font.voffsetx[Text.i]++;
            }
            break;

//        case VERDANA10:
//
//            Text.line_tt = 15;
//            Text.line_sz = 208; //bytes por linha
//
//            Font.vxsize[0] = horizontalLengthsVerdana10[0];
//            Font.voffsetx[0] = 0;
//
//            for (Text.i = 1; Text.i < 160; Text.i++) {
//
//                Font.vxsize[Text.i] = horizontalLengthsVerdana10[Text.i];
//                Font.voffsetx[Text.i] = Font.voffsetx[Text.i - 1] +
//                        (horizontalLengthsVerdana10[Text.i - 1] / 8);
//
//                if ((horizontalLengthsVerdana10[Text.i - 1] % 8) > 0)
//                    Font.voffsetx[Text.i]++;
//            }
//            break;
//
//        case VERDANA10NI:
//
//            Text.line_tt = 15;
//            Text.line_sz = 237; //bytes por linha
//
//            Font.vxsize[0] = horizontalLengthsVerdana10BoldItalic[0];
//            Font.voffsetx[0] = 0;
//
//            for (Text.i = 1; Text.i < 160; Text.i++) {
//
//                Font.vxsize[Text.i] = horizontalLengthsVerdana10BoldItalic[Text.i];
//                Font.voffsetx[Text.i] = Font.voffsetx[Text.i - 1] +
//                        (horizontalLengthsVerdana10BoldItalic[Text.i - 1] / 8);
//
//                if ((horizontalLengthsVerdana10BoldItalic[Text.i - 1] % 8) > 0)
//                    Font.voffsetx[Text.i]++;
//            }
//            break;

        default:
            break;
    }

    Text.pixel_tt = 0;

    //cálculo do comprimento da linha (em pixels)
    for (Text.i = 0; Text.i < Text.length; Text.i++) {

        if (txtLine[Text.i] == 'ñ')
            txtLine[Text.i] = 'ò';

        Text.ascii_val = txtLine[Text.i];

        //ajuste para caracteres especiais
        if (Text.ascii_val > 0xBF)
            Text.ascii_val -= 0x40;

        Text.pixel_tt += Font.vxsize[(Text.ascii_val - 0x20)];
    }

    switch (align) {

        case CENTER:

            x = (x) - (Text.pixel_tt / 2);
            break;

        case RIGHT:

            x = (DISPLAY_H_END + 1 - x) - (Text.pixel_tt);
            break;

        default:
            break;
    }

    //calcula o endereço inicial da coluna
    displayBuffer.startAddress = x / 8;
    displayBuffer.deltaPx = x;

    //para cada caracter

    for (Text.i = 0; Text.i < Text.length; Text.i++) {

        Text.ascii_val = txtLine[Text.i];

        //ajuste para caracteres especiais
        if (Text.ascii_val > 0xBF)
            Text.ascii_val -= 0x40;

        //caracter - primeiro elemento
        Font.i = (Font.voffsetx[(Text.ascii_val - 0x20)]);

        Text.column_tt = Font.vxsize[(Text.ascii_val - 0x20)] / 8;

        if ((Font.vxsize[(Text.ascii_val - 0x20)] % 8) > 0)
            Text.column_tt++;

        //linhas da fonte
        for (Text.j = 0; Text.j < Text.line_tt; Text.j++) {

            //colunas da fonte
            for (Text.k = 0; Text.k < Text.column_tt; Text.k++) {

                if ((Text.column_tt - 1) == Text.k)
                    Text.lim_px = Font.vxsize[(Text.ascii_val - 0x20)] % 8;

                else
                    Text.lim_px = 8;

                if (Text.lim_px == 0)
                    Text.lim_px = 8;

                for (Text.l = 0; Text.l < Text.lim_px; Text.l++) {

                    Text.p2_px = 0b00000001 << (7 - Text.l);

                    switch (font) {

                        case VERDANA7:

                            Font.aux = fontVerdana7[Font.i + Text.k];
                            break;

//                        case VERDANA10:
//
//                            Font.aux = fontVerdana10[Font.i + Text.k];
//                            break;
//
//                        case VERDANA10NI:
//
//                            Font.aux = fontVerdana10BoldItalic[Font.i + Text.k];
//                            break;

                        default:
                            break;
                    }

                    Text.x = displayBuffer.deltaPx + Text.l + (Text.k * 8);
                    Text.y = Text.j + y;

                    if ((Text.x < DISPLAY_H_END + 2) & (Text.y < 65)) {

                        //pixel do caracter
                        if ((Font.aux & Text.p2_px) == Text.p2_px)
                            fillRectangle(Text.y, Text.x, Text.y, Text.x,
                                SOLID, ONLY_BUFFER);

                        else if (colorOption == SOLID)
                            fillRectangle(Text.y, Text.x, Text.y, Text.x,
                                ERASE, ONLY_BUFFER);
                    }
                }
            }

            Font.i += Text.line_sz;
        }

        displayBuffer.deltaPx += Font.vxsize[(Text.ascii_val - 0x20)];
    }

    if (update)
        showDisplayBuffer();
}