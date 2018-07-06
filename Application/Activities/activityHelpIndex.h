#ifndef ACTIVITYHELPINDEX_H
#define ACTIVITYHELPINDEX_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <ActivityManager/ActivityMan.h>

#include <stdint.h>


    extern ActivityContext_t activityHelpIndex;
    
    void init_help_activity_by_tag_index(uint16_t index);

#ifdef	__cplusplus
}
#endif

#endif // ACTIVITYHELPINDEX_H
