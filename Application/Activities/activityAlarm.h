#ifndef ACTIVITYALARM_H
#define ACTIVITYALARM_H

#ifdef	__cplusplus
extern "C" {
#endif

  #include <ActivityManager/ActivityMan.h>

  extern ActivityContext_t activityAlarm;

  
  typedef uint8_t ( * verify_error_function_t)(void);
  
  void activityAlarm_call(char * title, verify_error_function_t verify_error);
  
#ifdef	__cplusplus
}
#endif

#endif // ACTIVITYALARM_H
