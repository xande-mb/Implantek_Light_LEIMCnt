/**
 * \file: buttonsModel.h
 * 
 *  Modelo responsável pela detecção de eventos nos botões da mebrana do equipamentos.
 * 
 */

#ifndef BUTTONSMODEL_H
#define BUTTONSMODEL_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>

    /**
     * ENUM para botoes de interface
     */
    typedef enum {
        BUTTON_LEFT_UP,
        BUTTON_LEFT_DOWN,
        BUTTON_DOWN_LEFT,
        BUTTON_DOWN_RIGHT,
        BUTTON_RIGHT_UP,
        BUTTON_RIGHT_DOWN,
        BUTTON_FS_LEFT,
        BUTTON_FS_CENTER,
        BUTTON_FS_RIGHT,
    } front_buttons_t;
    
    /**
     * Enventos dos botoes.
     */
    typedef enum {
        EVENT_NONE,
        EVENT_PRESS,
        EVENT_STILL,
        EVENT_RELEASE,
        EVENT_PRESSED_FROM_START,
    } button_events_t;
    
    /**
     * Tipo que define os dados de um botão.
     */
    typedef struct {
        button_events_t event_emited;
        button_events_t event_last_emited;
        uint8_t current_value;
        uint8_t last_value;
        uint16_t time_event;
    } button_event_data_t;
  
    /**
     * Registra o callback de um botão específico.
     * 
     * @param button
     * @param callback
     */
    void buttons_registerCallback(front_buttons_t button, void (* callback)(button_event_data_t * event));
    
    
    /**
     * Remove um callback para um botao.
     * 
     * @param button Botao cujo callback será removido.
     */
    void buttons_removeCallback(front_buttons_t button);
    
    /**
     * Inicializa botões.
     */
    void buttonsInit();
    
#ifdef	__cplusplus
}
#endif

#endif // BUTTONSMODEL_H
