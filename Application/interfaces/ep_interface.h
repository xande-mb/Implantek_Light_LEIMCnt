#ifndef EP_INTERFACE_H
#define EP_INTERFACE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

    typedef enum recording_status_e {
	RECORDING_STARTED,
	RECORDING_STOPED,
	RECORDING_INITIALIZED
    } recording_status_t;

    //! Atributos do meio de armazenamento de gravações

    typedef struct {
	unsigned long size;
	unsigned long used;
	unsigned long free;

    } z3Storage_t;

    typedef struct {
	unsigned f_zdvr_ok : 1;
	unsigned f_flash_ok_mounted : 1;
	unsigned f_flash_con_remove : 1;
	unsigned f_record_comand_sent : 1;
	unsigned f_gravando : 1;
	unsigned f_pausado : 1;
	unsigned file_renamed : 1;
	unsigned record_comand_sent : 1;
	unsigned unused1 : 1;
	//! Erro de gravação
	unsigned recordingError : 1;
	//! Gravação encerrada
	unsigned recordingTerminated : 1;
	//! Erro do pendrive
	unsigned flashError : 1;
	//! Erro de remoção do pendrive
	unsigned unmountingError : 1;
	//! Tempo de gravação decorrido, em segundos
	unsigned long recordingTime;
	unsigned unused2 : 5;
    } board_z3_status_t;

    void ep_init();

    uint8_t ep_get_version();

    uint8_t ep_start(char * fname, uint16_t continuation);

    uint8_t ep_stop();

    void ep_start_task();

    void ep_stop_task();

    uint8_t ep_recording_status();

    uint8_t ep_is_initialized();
    
    uint8_t ep_is_media_mounted();
    
    uint8_t ep_unmountStorageDevice();
    
    uint8_t ep_mountStorageDevice();
    
    uint8_t ep_getStorageSpace(z3Storage_t *spaceInfo);
    
    uint16_t ep_recording_time();

    board_z3_status_t * ep_z3_flags();
    
    uint8_t ep_was_safe_stoped();

#ifdef	__cplusplus
}
#endif

#endif // EP_INTERFACE_H
