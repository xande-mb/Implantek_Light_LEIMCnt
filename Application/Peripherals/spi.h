


#ifndef SPI_H
#define SPI_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

    void spi_init();

    unsigned char spi1_send_receive_byte(unsigned char data_out);

    uint8_t spi_send_receive_crc(uint8_t * output,  uint8_t * input, uint16_t length);
    
    unsigned char spi1_send_receive(uint8_t * output,  uint8_t * input, uint16_t length);
    
#ifdef	__cplusplus
}
#endif


#endif