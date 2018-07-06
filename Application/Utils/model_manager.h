#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

    typedef enum {
	MODEL_TRIPLET,
	MODEL_DUET,
	MODEL_COOL_LIGHT,
	MODEL_SMART_CAM,
	MODEL_SHAVER
    } models_t;

    typedef enum {
	CAMERA_SD,
	CAMERA_HD,
	NO_CAMERA
    } camera_type_t;

    typedef struct {
	models_t equip_model;
	camera_type_t cam_type;
	uint16_t equip_sn;
	uint16_t camera_sn;

    } model_data_t;

    void modelManager_init();
    
    void modelManager_read();

    model_data_t * modelManager_getDataReaded();


#ifdef	__cplusplus
}
#endif

#endif // MODEL_MANAGER_H
