#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdint.h>
    
    void init_log_manager(uint8_t initial_page, uint8_t initial_add1, uint8_t last_page);
    
  
#ifdef	__cplusplus
}
#endif

#endif // LOGMANAGER_H
