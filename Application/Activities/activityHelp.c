#include "activityHelp.h"
#include <ActivityManager/ActivityMan.h>
#include <Application/DisplayControler24/tft_driver.h>
#include <Application/DisplayControler24/display_controller.h>
#include <UserInterface/bt_lib_2.h>
#include <Application/images_list.h>
#include <Application/str_resources.h>
#include <CTTKernel/kernel.h>
#include "ActivityMain.h"
#include "SubActivitiesMain.h"
#include "CTTKernel/kernel/tasker.h"
#include "UserInterface/Buzzer.h"
#include "activityHelpIndex.h"

#define HELP_TEXT_FONT VERDANA10MONO
#define MAX_TEXT_SIZE 1024

void activityHelp_onCreate(void * param);
void activityHelp_onDestroy(void * param);
void activityHelp_onTick(void * param);

#define help_text_max_x			300
#define help_text_min_x			10
#define help_text_max_y			167
#define help_text_min_y			26
#define help_text_margin_top	8
#define help_text_margin_bot	3
#define help_text_margin_right  2

ActivityContext_t activityHelp = newActivity(
		.onCreate = activityHelp_onCreate,
		.onDestroy = activityHelp_onDestroy,
		.onTick = activityHelp_onTick);

static struct {
	touchable_t* btUp;
	touchable_t* btDown;
	touchable_t* btLeft;
	touchable_t* btRight;
	touchable_t* btBack;
	unsigned int help_chapter_to_show;
	unsigned int help_item_to_show;
	uint16_t line_count;
	uint16_t lines_shown;
	uint16_t current_offset;
	int16_t next_id;
	int16_t prev_id;
	lateral_bar_t bar;
	char * title, * sub_title;
	char text[MAX_TEXT_SIZE];
} _this = {

};

static void write_page(char * text, uint8_t line_offset);

void initialize_text(char * text);

void activityHelp_put_text(char * text) {
	initialize_text(text);
}

void activityHelp_setTitleSubTitle(char * title, char * sub_title) {
	
	_this.title = title;
	_this.sub_title = sub_title;
	
}


void activityHelp_setNextPrev(int16_t next_id, int16_t prev_id) {
	
	_this.next_id = next_id;
	_this.prev_id = prev_id;
	
}

void activityHelp_putTag(uint16_t tag_index){
	
}

uint16_t linearize_text(char * text, uint16_t size_max, FONTS_AVAILABLE_T font) {

	int i;
	int current_line_size = 0;
	int until_last_space_size = 0;
	int index_last_space = 0;
	int line_count = 1;
	init_txt_vectors(font);
	for (i = 0; i < strlen(text); i++) {

		char current_char = text[i];

		if (current_char == ' ') {
			index_last_space = i;
			until_last_space_size = current_line_size;
		}

		//		kernel_message(MODE_DEBUG,"%u\n",current_line_size);

		uint16_t size_char = display_font_char_len(current_char, font);
		current_line_size += size_char;

//		kernel_message(MODE_DEBUG, "Char %c %u was measured: %u, total_line is %u\n", current_char, (unsigned int) current_char, size_char, current_line_size);
		if (current_char == '\n') {
			line_count++;
			until_last_space_size = 0;
			current_line_size = 0;
		} else {
			
		}


		if (current_line_size >= size_max) {

			text[index_last_space] = '\n';
			line_count++;
//			kernel_message(MODE_DEBUG, "Text was broken on: %c on index %u, size_was %u is %u, next line start with: %u\n",
//					text[index_last_space - 1],
//					index_last_space,
//					until_last_space_size, current_line_size,
//					current_line_size - until_last_space_size
//					);

			current_line_size -= until_last_space_size;
			until_last_space_size = 0;
		}



	}

	return line_count;

}

static void click_btDown(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {

		sys_buzz(0, 0);
		if (_this.current_offset + _this.lines_shown < _this.line_count) {
			_this.current_offset = _this.current_offset + 1;
			draw_lateral_bar(&_this.bar, _this.current_offset);
			write_page(_this.text, _this.current_offset);
		}

	}
}

static void click_btUp(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {

		sys_buzz(0, 0);
		if (_this.current_offset > 0) {
			_this.current_offset = _this.current_offset - 1;
			draw_lateral_bar(&_this.bar, _this.current_offset);
			write_page(_this.text, _this.current_offset);
		}

	}
}

void click_btRightLeft(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		init_help_activity_by_tag_index(flag);
	}	
}

static void click_btBack(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
//		ActivityMan_getBack();
				ActivityMan_startActivity(&activityHelpIndex);
	}
}

void initialize_text(char * text) {
	strncpy(_this.text, text, MAX_TEXT_SIZE - 1);
	_this.line_count = linearize_text(_this.text, help_text_max_x - help_text_margin_right - help_text_min_x, HELP_TEXT_FONT);
}

void draw_lateral_bar(lateral_bar_t * lateral_bar, uint16_t offset) {

	SetColor(GRAY0);
	Bar(lateral_bar->x, lateral_bar->y,
			lateral_bar->x + lateral_bar->length, lateral_bar->y + lateral_bar->height);
	SetColor(GRAY1);

	uint16_t sub_bar_size = lateral_bar->lines_shown * lateral_bar->height / lateral_bar->lines_total;

	if (sub_bar_size > lateral_bar->height) {
		sub_bar_size = lateral_bar->height;
	}

	uint16_t sub_bar_start = offset * lateral_bar->height / lateral_bar->lines_total;

	kernel_message(MODE_DEBUG, "SH %u SS %u H%u LT %u LS %u\n", sub_bar_size, sub_bar_start, lateral_bar->height, lateral_bar->lines_total, lateral_bar->lines_shown);

	Bar(lateral_bar->x, lateral_bar->y + sub_bar_start,
			lateral_bar->x + lateral_bar->length, lateral_bar->y + sub_bar_start + sub_bar_size);

}

static void write_title() {

	SetColor(BLUEBTBAR);
	WAIT_UNTIL_FINISH(Bar(0, 0, 319, 25));

	char buffer[100];
	sprintf(buffer, "%s > %s", _this.title, _this.sub_title);

	SetColor(WHITE);
	display_set_font(VERDANA10NMONO);
	display_print_text(buffer, 10, 5, LEFT, 0);

}

void activityHelp_onCreate(void * param) {
	// Codigo de inicialização da atividade

	ClearDisplay(); // Clear the display


	SetColor(BLUEBTBAR);
	WAIT_UNTIL_FINISH(Bar(0, 168, 319, 219));

	_this.btUp = allocAsTouchable;
	setAsButton(_this.btUp,
			.text = "",
			.coord_x = 220,
			.coord_y = 172,
			.height = 43,
			.length = 50,
			.color = BLUEBTBAR,
			.beep = BEEP_ON_PRESS,
			.txt_font = VERDANA8MONO,
			.txt_font_height = 13,
			.img_idx_pressed = IMG_UP_ARROW_PRESSED,
			.img_idx_unpress = IMG_UP_ARROW_UNPRESSED,
			.color_blink = CYANBTBAR,
			.reference_txt = CENTER,
			.onClick = click_btUp
			);
	center_img_on_button(_this.btUp);
	bt_lib_2_drawTouchableButton(*_this.btUp);
	ActivityMan_insertTouchable(_this.btUp);

	_this.btDown = allocAsTouchable;
	setAsButton(_this.btDown,
			.text = "",
			.coord_x = 270,
			.coord_y = 172,
			.height = 43,
			.length = 50,
			.color = BLUEBTBAR,
			.beep = BEEP_ON_PRESS,
			.txt_font = VERDANA8MONO,
			.txt_font_height = 13,
			.img_idx_pressed = IMG_DOWN_ARROW_PRESSED,
			.img_idx_unpress = IMG_DOWN_ARROW_UNPRESSED,
			.color_blink = CYANBTBAR,
			.reference_txt = CENTER,
			.onClick = click_btDown
			);
	center_img_on_button(_this.btDown);
	bt_lib_2_drawTouchableButton(*_this.btDown);
	ActivityMan_insertTouchable(_this.btDown);

	_this.btBack = allocAsTouchable;
	setAsButton(_this.btBack,
			.text = getRes(str_res_ndice),
			.coord_x = 0,
			.coord_y = 172,
			.height = 47,
			.length = 50,
			.color = BLUEBTBAR,
			.img_idx_pressed = IMG_INDEX_PRESSED,
			.img_idx_unpress = IMG_INDEX_UNPRESSED,
			.reference_txt = CENTER,
			.txt_font = VERDANA7MONO,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.onClick = click_btBack
			);
	center_img_on_button(_this.btBack);
	_this.btBack->y_txt = 15;
	_this.btBack->y_img -= 7;
	_this.btBack->x_img += 2;
	bt_lib_2_drawTouchableButton(*_this.btBack);
	ActivityMan_insertTouchable(_this.btBack);

	_this.btLeft = allocAsTouchable;
	setAsButton(_this.btLeft,
			.text = getRes(str_res_anterior),
			.coord_x = 50,
			.coord_y = 172,
			.height = 47,
			.length = 70,
			.color = BLUEBTBAR,
			.img_idx_pressed = IMG_BACK_ARROW_PRESSED,
			.img_idx_unpress = IMG_BACK_ARROW_UNPRESSED,
			.reference_txt = CENTER,
			.txt_font = VERDANA7MONO,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.flag = _this.prev_id,
			.onClick = click_btRightLeft
			);
	center_img_on_button(_this.btLeft);
	_this.btLeft->y_txt = 15;
	_this.btLeft->y_img -= 5;
	if (_this.prev_id > -1){
		bt_lib_2_drawTouchableButton(*_this.btLeft);
		ActivityMan_insertTouchable(_this.btLeft);
	}
	_this.btRight = allocAsTouchable;
	setAsButton(_this.btRight,
			.text = getRes(str_res_prximo),
			.coord_x = 120,
			.coord_y = 172,
			.height = 47,
			.length = 70,
			.color = BLUEBTBAR,
			.img_idx_pressed = IMG_NEXT_ARROW_PRESSED,
			.img_idx_unpress = IMG_NEXT_ARROW_UNPRESSED,
			.reference_txt = CENTER,
			.txt_font = VERDANA7MONO,
			.txt_font_height = 13,
			.color_blink = CYANBTBAR,
			.flag = _this.next_id,
			.onClick = click_btRightLeft
			);
	center_img_on_button(_this.btRight);
	_this.btRight->y_txt = 15;
	_this.btRight->y_img -= 7;
	_this.btRight->x_img += 2;
	if (_this.next_id > -1){
		bt_lib_2_drawTouchableButton(*_this.btRight);
		ActivityMan_insertTouchable(_this.btRight);
	}
	_this.bar.x = 300;
	_this.bar.y = help_text_min_y;
	_this.bar.length = 19;
	_this.bar.height = help_text_max_y - help_text_min_y;
	_this.bar.lines_total = _this.line_count;
	display_set_font(HELP_TEXT_FONT);
	_this.lines_shown = (_this.bar.height - help_text_margin_top - help_text_margin_bot) / display_font_get_height();
	_this.bar.lines_shown = _this.lines_shown;

	_this.current_offset = 0;

	write_title();

	draw_lateral_bar(&_this.bar, 0);

	write_page(_this.text, 0);

}

void activityHelp_onDestroy(void * param) {
	// Código a ser executado quando a atividade for finalizada
	desalocTouchable(_this.btBack);
	desalocTouchable(_this.btDown);
	desalocTouchable(_this.btUp);
	desalocTouchable(_this.btLeft);
	desalocTouchable(_this.btRight);

}

void activityHelp_onTick(void * param) {

	// Código a ser executado ciclicamente durante o ciclo de vida da atividade
}

void copy_lines(char *to, const char* from, uint16_t start_line, uint16_t max_lines) {

//	char *start_char, *end_char;
	uint16_t line_count = 0;
	
	while (*from) {
		if (*from == '\n') line_count++;

		if (line_count >= start_line && line_count < start_line + max_lines) {
			*to++ = *from;
			*to = 0;
		}
		if (line_count >= start_line + max_lines) {
			break;
		}
		from++;
	}

}

static void write_page(char * text, uint8_t line_offset) {

	SetColor(WHITE);
	Bar(help_text_min_x, help_text_min_y, help_text_max_x - 1, help_text_max_y);

	char text_bufer[512];

	copy_lines(text_bufer, text, line_offset, _this.bar.lines_shown);

	//	linearize_text(text, max_x - min_x - 5, HELP_TEXT_FONT);
	display_set_font(HELP_TEXT_FONT);
	SetColor(BLACK);
	SetBgColor(WHITE);
	disp_multi_line_print(text_bufer, help_text_min_x, help_text_min_y + help_text_margin_top, LEFT, 0);



}