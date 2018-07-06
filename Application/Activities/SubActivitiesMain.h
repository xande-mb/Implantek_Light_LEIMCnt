#ifndef SUBACTIVITIESMAIN_H
#define SUBACTIVITIESMAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <ActivityManager/ActivityMan.h>
    
extern SubActivityContext_t subActivityLight;
extern SubActivityContext_t subActivityDefault;
extern SubActivityContext_t subActivityCam;
extern SubActivityContext_t subActivityGravador;
extern SubActivityContext_t subActivityVelocidade;

void center_img_on_button(touchable_t * touchable);

void clickbtVelocityUpDown(uint8_t event, uint8_t flag, void* obFlag);

#ifdef	__cplusplus
}
#endif

#endif // SUBACTIVITIESMAIN_H
