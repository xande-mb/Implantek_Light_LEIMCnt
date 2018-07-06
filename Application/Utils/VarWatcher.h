#ifndef VARWATCHER_H
#define VARWATCHER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
    typedef struct uint32_watcher_s {
	
	uint32_t current_value;
	uint32_t old_value;
	
    } uint32_watcher_t;
    
    
#define NEW_UINT16_WATCHER {0,0,0}
#define NEW_UINT16_WATCHER_INI(val) {.has_changed = 0, .current_value = val,.old_value = 0}
    
    typedef struct uint16_watcher_s {
	uint8_t has_changed;
	uint16_t current_value;
	uint16_t old_value;
	
    } uint16_watcher_t;
    
    typedef struct byte_array_watcher_s {
	
	uint16_t size;
	uint8_t *current_value;
	uint8_t *old_value;
	
    } byte_array_watcher_t;
    

    void uint16_watcher_update(uint16_watcher_t * self, uint16_t value);
  
#ifdef	__cplusplus
}
#endif

#endif // VARWATCHER_H
