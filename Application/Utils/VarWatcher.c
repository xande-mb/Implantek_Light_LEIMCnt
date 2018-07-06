#include "VarWatcher.h"


void uint16_watcher_update(uint16_watcher_t * self, uint16_t value){
	
	self->old_value = self->current_value;
	self->current_value = value;
	self->has_changed = self->current_value != self->old_value;
	
	
}

