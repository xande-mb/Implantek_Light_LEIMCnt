#include "activityHelpIndex.h"
#include "Memory/SerialFlash.h"
#include "CTTKernel/kernel/tasker.h"
#include "UartInterface/UartCommand.h"
#include <ActivityManager/ActivityMan.h>
#include <Application/DisplayControler24/tft_driver.h>
#include <Application/DisplayControler24/display_controller.h>
#include <UserInterface/bt_lib_2.h>
#include <Application/images_list.h>
#include <Application/str_resources.h>
#include <CTTKernel/kernel.h>
#include <UserInterface/Buzzer.h>
#include <Memory/SerialFlash.h>
#include "ActivityMain.h"
#include "activityHelp.h"
#include "ActivityMenu.h"
#include "SubActivitiesMain.h"

void activityHelpIndex_onCreate(void * param);
void activityHelpIndex_onDestroy(void * param);
void activityHelpIndex_onTick(void * param);

#define BTS_MAX_SHOWN 4

#define BTS_TITLE_X 0
#define BTS_TITLE_Y 28
#define BTS_TITLE_LENGTH 140
#define BTS_TITLE_HEIGHT 33
#define BTS_TITLE_MARGIN 2

#define BTS_SUB_TITLE_X (BTS_TITLE_X + 180)
#define BTS_SUB_TITLE_Y       35
#define BTS_SUB_TITLE_LENGTH  120
#define BTS_SUB_TITLE_HEIGHT  25
#define BTS_SUB_TITLE_MARGIN  5


ActivityContext_t activityHelpIndex = newActivity(
		.onCreate = activityHelpIndex_onCreate,
		.onDestroy = activityHelpIndex_onDestroy,
		.onTick = activityHelpIndex_onTick);

typedef enum {
	TAG_TYPE_TITLE,
	TAG_TYPE_SUB_TITLE,
	TAG_TYPE_TEXT,
	TAG_TYPE_NONE,
} tag_type_t;

typedef struct {
	uint8_t add2;
	uint8_t add1;
	uint8_t add0;
} flash_add_t;

typedef struct {
	tag_type_t type;
	flash_add_t add;
	uint16_t size;
	uint16_t array_index;
} help_text_tag_t;

static uint16_t number_of_tags = 0;

help_text_tag_t tags[50] = {};

static touchable_t * bts_titles[BTS_MAX_SHOWN];
static touchable_t * bts_sub_titles[BTS_MAX_SHOWN];
static touchable_t * bt_back;
static touchable_t * bt_up_title;
static touchable_t * bt_down_title;
//static touchable_t * bt_up_sub_title;
//static touchable_t * bt_down_sub_title;

static lateral_bar_t lateral_bar = {

	.x = BTS_TITLE_X + BTS_TITLE_LENGTH + 1,
	.y = 27,
	.length = 10,
	.height = 140,
	.lines_shown = BTS_MAX_SHOWN,

};

static char titles[BTS_MAX_SHOWN][50];
static char sub_titles[BTS_MAX_SHOWN][50];

static uint16_t active_button_index = 0;

static uint16_t offset_title = 0;
static uint16_t offset_sub_title = 0;


void set_active_title_index(uint16_t index);
void create_title_buttons(uint16_t active_title, uint16_t offset_title);
void create_subtitle_buttons(uint16_t active_title, uint16_t offset_sub_title);

void click_btTitle(uint8_t event, uint8_t flag, void* obFlag) {

	if (event == EVENT_RELEASE) {

		set_active_title_index(flag);

	}

}

uint16_t number_of_titles() {
	int ct = 0;
	int i;
	for (i = 0; i < number_of_tags; i++) {
		if (tags[i].type == TAG_TYPE_TITLE) ct++;
	}
	return ct;
}

void number_of_sub_titles_of_title() {

}

void click_btSubTitle(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {


		help_text_tag_t * tag = obFlag;

		init_help_activity_by_tag_index(tag->array_index);

		//		char text_buffer[1024];
		//		serialflash_read(tag->add.add2, tag->add.add1, tag->add.add0, text_buffer, tag->size);
		//		activityHelp_put_text(text_buffer);
		//		activityHelp_setTitleSubTitle(titles[active_button_index], sub_titles[flag]);
		//		ActivityMan_startActivity(&activityHelp);

	}
}

void init_help_activity_by_tag_index(uint16_t index) {

	help_text_tag_t * tag_text = &tags[index];
	help_text_tag_t * tag_sub_title = &tags[index - 1];
//	help_text_tag_t * tag_title = &tags[index - 1];

	char text_buffer[1024];
	static char text_buffer_subtitle[128];
	static char text_buffer_title[128];
	int i;
	for (i = index; i >= 0; i--) {
		if (tags[i].type == TAG_TYPE_TITLE) {
			serialflash_read(tags[i].add.add2, tags[i].add.add1, tags[i].add.add0, (uint8_t *) text_buffer_title, tags[i].size);
            __builtin_nop();
            __builtin_nop();
            __builtin_nop();
			break;
		}
	}

	serialflash_read(tag_text->add.add2, tag_text->add.add1, tag_text->add.add0, (uint8_t *) text_buffer, tag_text->size);
	serialflash_read(tag_sub_title->add.add2, tag_sub_title->add.add1,
			tag_sub_title->add.add0, (uint8_t *) text_buffer_subtitle, tag_sub_title->size);
	activityHelp_put_text(text_buffer);
	activityHelp_setTitleSubTitle(text_buffer_title, text_buffer_subtitle);
	uint16_t next = -1, prev = -1;
	if (tags[index - 2].type == TAG_TYPE_TEXT && tags[index - 3].type == TAG_TYPE_SUB_TITLE) prev = index - 2;
	if (tags[index + 2].type == TAG_TYPE_TEXT && tags[index + 1].type == TAG_TYPE_SUB_TITLE) next = index + 2;

	activityHelp_setNextPrev(next, prev);
	ActivityMan_startActivity(&activityHelp);

}

uint16_t has_next_sub(uint16_t index) {
	//	if 
	return 0;
}

uint16_t has_prev_sub(uint16_t index) {
	
	return 0;

}

static void click_btBack(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {
		ActivityMan_startActivity(&activityMenu);
		//				ActMan_startActivity(&activityHelpIndex);



	}
}

static void click_btDown(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {

		sys_buzz(0, 0);

		if (offset_title + BTS_MAX_SHOWN < number_of_titles()) {
			offset_title += 1;
			if (active_button_index < offset_title) active_button_index = offset_title;
			set_active_title_index(active_button_index);
		}

	}
}

static void click_btUp(uint8_t event, uint8_t flag, void* obFlag) {
	if (event == EVENT_RELEASE) {

		sys_buzz(0, 0);
		if (offset_title > 0) {
			offset_title -= 1;
			if (active_button_index > BTS_MAX_SHOWN - 1) active_button_index = BTS_MAX_SHOWN - 1 + offset_title;
			set_active_title_index(active_button_index);
		}

	}
}

void set_active_title_index(uint16_t index) {

	active_button_index = index;

	create_title_buttons(active_button_index, offset_title);

	create_subtitle_buttons(active_button_index, offset_sub_title);

}

// TODO: Passar essa função para o system junto com o log manager

static void add_sum(flash_add_t * add, uint8_t bytes) {

	uint16_t new_add0 = (add->add0 + bytes) % 256;
	uint16_t new_add1 = (add->add1 + (add->add0 + bytes) / 256) % 256;
	uint16_t new_add2 = (add->add2 + (add->add1 + (add->add0 + bytes) / 256) / 256) % 256;

	add->add0 = new_add0;
	add->add1 = new_add1;
	add->add2 = new_add2;

}

static void write_title() {

	SetColor(BLUEBTBAR);
	WAIT_UNTIL_FINISH(Bar(0, 0, 319, 26));

	char buffer[100];
	sprintf(buffer, getRes(str_res_ndice));

	SetColor(WHITE);
	display_set_font(VERDANA10NMONO);
	display_print_text(buffer, 10, 5, LEFT, 0);

}

#define DEBUG_PREV_TAG {if (number_of_tags > 0){ \
		kernel_message(MODE_DEBUG, "ended tag %u type %u size %u add (%u %u %u)\n",\
						number_of_tags-1,\
						tags[number_of_tags-1].type,\
						tags[number_of_tags-1].size,\
						tags[number_of_tags-1].add.add2,\
						tags[number_of_tags-1].add.add1,\
						tags[number_of_tags-1].add.add0\
						)}};

void init_help_tags(flash_add_t initial_add) {
	int i, j;
	const unsigned int max_read = 32;
	number_of_tags = 0;
	char last_chars[3] = {0, 0, 0};
	tags[0].size = 0;
	volatile flash_add_t current_add = initial_add;
	for (i = 0; i < 1000; i++) {
		char buff[max_read + 10];
		serialflash_read(current_add.add2, current_add.add1, current_add.add0, (uint8_t *) buff, max_read);
		for (j = 0; j < max_read; j++) {
			last_chars[0] = last_chars[1];
			last_chars[1] = last_chars[2];
			last_chars[2] = buff[j];
			if (number_of_tags > 0) {
				tags[number_of_tags - 1].size++;
			}


			if (memcmp(last_chars, "\0$$", 3) == 0) {
				DEBUG_PREV_TAG;
				tags[number_of_tags].add.add0 = current_add.add0;
				tags[number_of_tags].add.add1 = current_add.add1;
				tags[number_of_tags].add.add2 = current_add.add2;
				add_sum(&tags[number_of_tags].add, j + 1);
				tags[number_of_tags].type = TAG_TYPE_SUB_TITLE;
				tags[number_of_tags].size = 0;
				tags[number_of_tags].array_index = number_of_tags;
				number_of_tags++;
			}

			if (last_chars[0] == '\0' && last_chars[1] == '$' && last_chars[2] != '$') {
				DEBUG_PREV_TAG;
				tags[number_of_tags].add.add0 = current_add.add0;
				tags[number_of_tags].add.add1 = current_add.add1;
				tags[number_of_tags].add.add2 = current_add.add2;
				add_sum(&tags[number_of_tags].add, j);
				tags[number_of_tags].type = TAG_TYPE_TITLE;
				tags[number_of_tags].size = 1;
				tags[number_of_tags].array_index = number_of_tags;
				number_of_tags++;
			}

			if (last_chars[1] == '\0' && last_chars[2] != '$') {
				DEBUG_PREV_TAG;
				tags[number_of_tags].add.add0 = current_add.add0;
				tags[number_of_tags].add.add1 = current_add.add1;
				tags[number_of_tags].add.add2 = current_add.add2;
				add_sum(&tags[number_of_tags].add, j + 1);
				tags[number_of_tags].type = TAG_TYPE_TEXT;
				tags[number_of_tags].size = 0;
				tags[number_of_tags].array_index = number_of_tags;
				number_of_tags++;
			}

			if (memcmp(last_chars, "\0\0\0", 3) == 0) {
				DEBUG_PREV_TAG;
				return;
			}

		}
		add_sum((flash_add_t *) &current_add, max_read);
		//		kernel_message(MODE_DEBUG,"Summed to: %u %u %u\n", current_add.add0, current_add.add1, current_add.add2);
	}
}

void activityHelpIndex_onCreate(void * param) {
	// Codigo de inicialização da atividade
	flash_add_t initial_add = {
		.add0 = 0, .add1 = 0, .add2 = 0xd
	};

	ClearDisplay(); // Clear the display

	SetColor(BLUEBTBAR);
	WAIT_UNTIL_FINISH(Bar(0, 168, 319, 219));

	init_help_tags(initial_add);

	lateral_bar.lines_total = number_of_titles();

	//	create_title_buttons();

	int i;
	for (i = 0; i < BTS_MAX_SHOWN; i++) {

		bts_titles[i] = allocAsTouchable;
		setAsButton(bts_titles[i],
				.text = "",
				.coord_x = BTS_TITLE_X,
				.coord_y = BTS_TITLE_Y + i * (BTS_TITLE_HEIGHT + BTS_TITLE_MARGIN),
				.height = BTS_TITLE_HEIGHT,
				.length = BTS_TITLE_LENGTH,
				.color = BLUEBTBAR,
				.beep = BEEP_ON_PRESS,
				.txt_font = VERDANA8MONO,
				.txt_font_height = 13,
				.color_blink = CYANBTBAR,
				.reference_txt = CENTER,
				.onClick = click_btTitle,
				.flag = i,
				.blink = 1,
				);

	}

	//	active_button_index = 0;

	//	create_title_buttons();

	for (i = 0; i < BTS_MAX_SHOWN; i++) {

		bts_sub_titles[i] = allocAsTouchable;
		setAsButton(bts_sub_titles[i],
				.text = "",
				.coord_x = BTS_SUB_TITLE_X,
				.coord_y = BTS_SUB_TITLE_Y + i * (BTS_SUB_TITLE_HEIGHT + BTS_SUB_TITLE_MARGIN),
				.height = BTS_SUB_TITLE_HEIGHT,
				.length = BTS_SUB_TITLE_LENGTH,
				.color = BLUEBTBAR,
				.beep = BEEP_ON_PRESS,
				.txt_font = VERDANA8MONO,
				.txt_font_height = 13,
				.color_blink = CYANBTBAR,
				.reference_txt = CENTER,
				.onClick = click_btSubTitle,
				.flag = i,
				.obFlag = bts_sub_titles[i],
				.blink = 1,
				);
	}

	set_active_title_index(active_button_index);

	bt_back = allocAsTouchable;
	setAsButton(bt_back,
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
	center_img_on_button(bt_back);
	bt_back->y_txt = 15;
	bt_lib_2_drawTouchableButton(*bt_back);
	ActivityMan_insertTouchable(bt_back);

	bt_up_title = allocAsTouchable;
	setAsButton(bt_up_title,
			.text = "",
			.coord_x = 55,
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
	center_img_on_button(bt_up_title);
	bt_lib_2_drawTouchableButton(*bt_up_title);
	ActivityMan_insertTouchable(bt_up_title);

	bt_down_title = allocAsTouchable;
	setAsButton(bt_down_title,
			.text = "",
			.coord_x = 105,
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
	center_img_on_button(bt_down_title);
	bt_lib_2_drawTouchableButton(*bt_down_title);
	ActivityMan_insertTouchable(bt_down_title);

	write_title();


}

void create_title_buttons(uint16_t active_title, uint16_t offset_title) {

	int i;
	for (i = 0; i < BTS_MAX_SHOWN; i++) {
		bt_lib_2_eraseTouchable(*bts_titles[i], WHITE);
		ActivityMan_excludeTouchable(bts_titles[i]);
	}

	int ct_titles = 0;

	for (i = 0; i < number_of_tags; i++) {
		if (tags[i].type == TAG_TYPE_TITLE) {
			kernel_message(MODE_DEBUG, "Found title on tag %u / %u, titles: %u, offset: %u\n", i, number_of_tags, ct_titles, offset_title);
			if (ct_titles >= offset_title) {
				char buffer[512] = {};
				serialflash_read(tags[i].add.add2,
						tags[i].add.add1,
						tags[i].add.add0,
						(uint8_t *) buffer,
						tags[i].size + 5 > 511 ? 511 : tags[i].size + 5
						);
				kernel_message(MODE_DEBUG, "%u %.*s\n", tags[i].type, 25, buffer);
				strcpy(titles[ct_titles - offset_title], buffer);
				bts_titles[ct_titles - offset_title]->text = titles[ct_titles - offset_title];
				if (active_title == ct_titles) bts_titles[ct_titles - offset_title]->color = CYANBTBAR;
				else bts_titles[ct_titles - offset_title]->color = BLUEBTBAR;
				bts_titles[ct_titles - offset_title]->flag = ct_titles;
				bt_lib_2_drawTouchableButton(*bts_titles[ct_titles - offset_title]);
				ActivityMan_excludeTouchable(bts_titles[ct_titles - offset_title]);
				ActivityMan_insertTouchable(bts_titles[ct_titles - offset_title]);
			}
			ct_titles++;
			if ((int) (ct_titles - offset_title) >= (int) (BTS_MAX_SHOWN)) {
				kernel_message(MODE_DEBUG, "Breaking %u %u\n", ct_titles, offset_title);
				break;
			}

		}
	}

	draw_lateral_bar(&lateral_bar, offset_title);


}

void create_subtitle_buttons(uint16_t active_title, uint16_t offset_sub_title) {

	int i;
	for (i = 0; i < BTS_MAX_SHOWN; i++) {
		bt_lib_2_eraseTouchable(*bts_sub_titles[i], WHITE);
		ActivityMan_excludeTouchable(bts_sub_titles[i]);
	}

	i = 0;
	int ct_sub_titles = 0;
	int ct_titles = 0;
	while (ct_titles <= active_title) {
		if (tags[i].type == TAG_TYPE_TITLE) ct_titles++;
		i++;
	}

	for (i = i; i < number_of_tags; i++) {

		if (tags[i].type == TAG_TYPE_SUB_TITLE) {

			kernel_message(MODE_DEBUG, "Accessing %u subtitle on tag %u\n", ct_sub_titles, i);

			char buffer[512] = {};
			serialflash_read(tags[i].add.add2, tags[i].add.add1, tags[i].add.add0,
					(uint8_t *)buffer, tags[i].size + 5 > 511 ? 511 : tags[i].size + 5);
			strcpy(sub_titles[ct_sub_titles], buffer);
			bts_sub_titles[ct_sub_titles]->text = sub_titles[ct_sub_titles];
			bts_sub_titles[ct_sub_titles]->obFlag = &(tags[i + 1]);
			bts_sub_titles[ct_sub_titles]->flag = ct_sub_titles;
			bt_lib_2_drawTouchableButton(*bts_sub_titles[ct_sub_titles]);
			ActivityMan_excludeTouchable(bts_sub_titles[ct_sub_titles]);
			ActivityMan_insertTouchable(bts_sub_titles[ct_sub_titles]);
			ct_sub_titles++;
			if (ct_sub_titles >= BTS_MAX_SHOWN) {
				break;
			}
		}

		if (tags[i].type == TAG_TYPE_TITLE) break;
	}

	kernel_message(MODE_DEBUG, "found %u subtitles\n", ct_sub_titles);

}

void activityHelpIndex_onDestroy(void * param) {
	// CÃ³digo a ser executado quando a atividade for finalizada

	int i;
	for (i = 0; i < BTS_MAX_SHOWN; i++) {

		desalocTouchable(bts_titles[i]);
		desalocTouchable(bts_sub_titles[i]);

	}

	desalocTouchable(bt_back);
	desalocTouchable(bt_up_title);
	desalocTouchable(bt_down_title);
//	desalocTouchable(bt_up_sub_title);
//	desalocTouchable(bt_down_sub_title);
}

void activityHelpIndex_onTick(void * param) {



	// CÃ³digo a ser executado ciclicamente durante o ciclo de vida da atividade
}
