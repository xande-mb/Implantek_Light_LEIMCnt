#ifndef ACTIVITYHELP_H
#define ACTIVITYHELP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <ActivityManager/ActivityMan.h>

#include <stdint.h>
    
    typedef struct lateral_bar_s {
	uint16_t x;
	uint16_t y;
	uint16_t length;
	uint16_t height;
	uint16_t lines_total;
	uint16_t lines_shown;
    } lateral_bar_t;

    extern ActivityContext_t activityHelp;

    void activityHelp_put_text(char * text);
    
    void activityHelp_setNextPrev(int16_t next_id, int16_t prev_id);

    void activityHelp_setTitleSubTitle(char * title, char * sub_title);

    void draw_lateral_bar(lateral_bar_t * lateral_bar, uint16_t offset);

#ifdef	__cplusplus
}
#endif

#endif // ACTIVITYHELP_H
