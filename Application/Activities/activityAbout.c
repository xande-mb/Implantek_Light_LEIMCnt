#include "activityAbout.h"
#include "ActivityMain.h"
#include "SubActivitiesMain.h"
#include "ActivityMenu.h"
#include "CTTKernel/utils/version.h"
#include "CTTKernel/kernel.h"
#include "Application/Utils/model_manager.h"
#include <ActivityManager/ActivityMan.h>
#include <Application/DisplayControler24/tft_driver.h>
#include <Application/DisplayControler24/display_controller.h>
#include <UserInterface/bt_lib_2.h>
#include <Application/images_list.h>
#include <Application/str_resources.h>

void activityAbout_onCreate(void * param);
void activityAbout_onDestroy(void * param);
void activityAbout_onTick(void * param);

#define ABOUT_TEXT_FONT VERDANA10MONO
#define TEXT_X_POSITION 20
#define TEXT_Y_POSITION	40

static char * software_version_format; // = getRes(str_res_verso_do_software_uuc);
static char * kernel_version_format; // = getRes(str_res_verso_do_kernel_uuc);
static char * kernel_buildcode_format; // = getRes(str_res_cdigo_de_compilao_s);
static char * recorder_status_format; // = getRes(str_res_estado_do_gravador_s);
static char * media_status_format; // = getRes(str_res_estado_da_mdia_s);
static char * media_storage_format; // = getRes(str_res_____capacidade_1f_gb_utilizado_1f_gb);

ActivityContext_t activityAbout = newActivity(
        .onCreate = activityAbout_onCreate,
        .onDestroy = activityAbout_onDestroy,
        .onTick = activityAbout_onTick);

static struct {
    touchable_t * bt_back;

} _this;

static void write_title() {

    SetColor(BLUEBTBAR);
    WAIT_UNTIL_FINISH(Bar(0, 0, 319, 26));

    char buffer[100];
    sprintf(buffer, getRes(str_res_sobre_o_triplet));

    SetColor(WHITE);
    display_set_font(VERDANA10NMONO);
    display_print_text(buffer, 10, 5, LEFT, 0);

}

static void click_btBack(uint8_t event, uint8_t flag, void* obFlag) {
    if (event == EVENT_RELEASE) {
        ActivityMan_startActivity(&activityMenu);
    }
}

#define append_information_on_screen(format, ...) {\
	sprintf(buffer, format, __VA_ARGS__);\
	display_print_text(buffer, TEXT_X_POSITION, current_y_pos,LEFT,0);	\
	current_y_pos+=font_get_heigth() + 3;\
}

static void write_information() {
    display_set_font(ABOUT_TEXT_FONT);
    SetColor(BLACK);
    SetBgColor(WHITE);

    char buffer[100];
    uint16_t current_y_pos = TEXT_Y_POSITION;

    append_information_on_screen(software_version_format, 1, 1);
    append_information_on_screen("N�mero de s�rie: %04u", modelManager_getDataReaded()->equip_sn);
    append_information_on_screen(kernel_version_format, CTT_VERSION_MAJOR, CTT_VERSION_MINOR, CTT_VERSION_RELEASE);
    append_information_on_screen(kernel_buildcode_format, kernel_build_code());


    if (
            modelManager_getDataReaded()->equip_model == MODEL_TRIPLET ||
            modelManager_getDataReaded()->equip_model == MODEL_DUET ||
            modelManager_getDataReaded()->equip_model == MODEL_SMART_CAM
            ) {

        recording_status_t recording_stat = ep_recording_status();
        char * recorder_stat =
                recording_stat == RECORDING_STARTED || recording_stat == RECORDING_INITIALIZED
                ?
                getRes(str_res_gravando) :
                ep_is_initialized()
                ?
                getRes(str_res_inicializado) :
                getRes(str_res_aguardando);
        append_information_on_screen(recorder_status_format, recorder_stat);


        if (ep_is_media_mounted()) {
            append_information_on_screen(media_status_format, getRes(str_res_conectada));

            if (recording_stat == RECORDING_STOPED) {
                z3Storage_t storage_info;
                ep_getStorageSpace(&storage_info);
                kernel_message(MODE_DEBUG, "Storage_info %lu %lu %lu \n", storage_info.free, storage_info.size, storage_info.used);
                append_information_on_screen(media_storage_format, (float) storage_info.size * 1.0 / 1000000L, (float) storage_info.used * 1.0 / 1000000L);
            }

        }

    }

}

void activityAbout_onCreate(void * param) {
    // Codigo de inicialização da atividade

    software_version_format = getRes(str_res_verso_do_software_uuc);
    kernel_version_format = getRes(str_res_verso_do_kernel_uuc);
    kernel_buildcode_format = getRes(str_res_cdigo_de_compilao_s);
    recorder_status_format = getRes(str_res_estado_do_gravador_s);
    media_status_format = getRes(str_res_estado_da_mdia_s);
    media_storage_format = getRes(str_res_____capacidade_1f_gb_utilizado_1f_gb);

    ClearDisplay(); // Clear the display

    write_title();

    write_information();

    SetColor(BLUEBTBAR);
    WAIT_UNTIL_FINISH(Bar(0, 168, 319, 219));

    _this.bt_back = allocAsTouchable;
    setAsButton(_this.bt_back,
            .text = getRes(str_voltar),
            .coord_x = 0,
            .coord_y = 172,
            .height = 47,
            .length = 50,
            .color = BLUEBTBAR,
            .img_idx_pressed = IMG_BACK_ARROW_PRESSED,
            .img_idx_unpress = IMG_BACK_ARROW_UNPRESSED,
            .reference_txt = CENTER,
            .txt_font = VERDANA7MONO,
            .txt_font_height = 13,
            .color_blink = CYANBTBAR,
            .onClick = click_btBack
            );

    center_img_on_button(_this.bt_back);
    _this.bt_back->y_txt = 15;
    bt_lib_2_drawTouchableButton(*_this.bt_back);
    ActivityMan_insertTouchable(_this.bt_back);

}

void activityAbout_onDestroy(void * param) {
    // Código a ser executado quando a atividade for finalizada

    desalocTouchable(_this.bt_back);

}

void activityAbout_onTick(void * param) {

    // Código a ser executado ciclicamente durante o ciclo de vida da atividade
}
