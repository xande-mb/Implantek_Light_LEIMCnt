#ifndef VIEWSMAIN_H
#define VIEWSMAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <Application/DisplayControler24/tft_driver.h>
#include <Application/Utils/model_manager.h>
#include "ActivityMain.h"
#include <UserInterface/bt_lib_2.h>
    
    
    typedef struct {	
	uint16_t left_pos;
	uint16_t top_pos;
	uint16_t height;
	uint16_t length;
    } display_box_t;
    
#define SQUARE_T1 ((display_box_t) {.left_pos = 0, .top_pos = 26, .height = 145, .length = 198})
#define SQUARE_T2 ((display_box_t) {.left_pos = 201, .top_pos = 0, .height = 73, .length = 118})
#define SQUARE_T3 ((display_box_t) {.left_pos = 201, .top_pos = 76, .height = 49, .length = 118})
#define SQUARE_T4 ((display_box_t) {.left_pos = 201, .top_pos = 128, .height = 43, .length = 118})
    
#define SQUARE_D1 SQUARE_T1
#define SQUARE_D2 ((display_box_t) {.left_pos = 201, .top_pos = 28, .height = 72, .length = 118})
#define SQUARE_D3 ((display_box_t) {.left_pos = 201, .top_pos = 103, .height = 68, .length = 118})
    
#define SQUARE_SC1 ((display_box_t) {.left_pos = 28, .top_pos = 64, .height = 70, .length = 118})
#define SQUARE_SC2 ((display_box_t) {.left_pos = 174, .top_pos = 64, .height = 70, .length = 118})
    
#define SQUARE_SH1 ((display_box_t) {.left_pos = 57, .top_pos = 26, .height = 145, .length = 198})

    void init_main_view(models_t model);

    void borda_motor(WORD color);

    void borda_cam(WORD color);

    void borda_grav(WORD color);

    void borda_light(WORD color);

    void select_source();

    void select_recorder();

    void select_cam();

    void select_motor();

    void select_none();

    void draw_bt_menu(touchable_t* touchable, unsigned char isPressed);

    void draw_light_state(state_of_device_t state, uint8_t force);

    void draw_cam_state(state_of_device_t state);

    void draw_rec_time(state_of_device_t state, uint16_t rec_time);

    void draw_recorder_state(state_of_device_t state, uint16_t is_paused);

    void draw_light_perc(unsigned char perc_to_write);

    void draw_velocity(unsigned int vel_to_write, unsigned int velocity_perc);

    void draw_direction(direction_of_motor_t direction);

    void write_title(char * title);

    void write_motor_desconected();

    void write_motor_title(enum motor_type_e type, uint8_t motor_number);

    void write_initial_motor_window();

    void write_light_square();

    void write_cam_square();

    void write_rec_square();

    void write_delims();


#ifdef	__cplusplus
}
#endif

#endif // VIEWSMAIN_H
