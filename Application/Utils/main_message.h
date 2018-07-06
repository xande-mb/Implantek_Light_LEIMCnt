#ifndef MAIN_MESSAGE_H
#define MAIN_MESSAGE_H

#ifdef	__cplusplus
extern "C" {
#endif


#include <Application/DisplayControler24/tft_driver.h>

    typedef enum STATUS_MESSAGE_TYPE_e {
	ST_MESSAGE_NONE,
	ST_MESSAGE_ALTERNATING, // Fica alternando entre as opções;
	ST_MESSAGE_WARNING, // Mostra uma vez e apaga
	ST_MESSAGE_PERSISTENT, // Fica mostrada até que apagadada
    } status_message_type_t;
    
    typedef struct {
	status_message_type_t type;
	char * message;
	unsigned char f_active;
	WORD bg_color;
	unsigned char should_bip;
    } status_message_t;


    unsigned char status_message_insert(status_message_t * message);

    unsigned char status_message_remove(status_message_t * message);

    void status_message_remove_all();

    void status_message_show(unsigned char index);

    void status_message_man_on_tick();
    
    void init_message_manager();
    
    void stop_message_manager();
    

#ifdef	__cplusplus
}
#endif

#endif // MAIN_MESSAGE_H
