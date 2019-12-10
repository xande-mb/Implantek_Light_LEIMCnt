/**
 * \file: spi.h
 * 
 *  Periférico responsável pela comunicação SPI do processador com outros CIs.
 * 
 */



#ifndef SPI_H
#define SPI_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

    /**
     * Inicializa a SPI
     */
    void spi_init();

    /**
     * Envia e recebe um byte na rede SPI
     * 
     * @param data_out byte a ser enviado
     * @return byte lido
     */
    unsigned char spi1_send_receive_byte(unsigned char data_out);

    /**
     * Envia e recebe uma sequencia de bytes utilizando CRC para consistencia dos dados.
     * 
     * @param output Sequencia de bytes a ser enviada
     * @param input Sequencia de bytes recebida.
     * @param length Tamanho da sequencia de bytes.
     * @return 1 caso o crc tenha sido ok.
     */
    uint8_t spi_send_receive_crc(uint8_t * output,  uint8_t * input, uint16_t length);
    
    /**
     * 
     * 
     * @param output Sequencia de bytes a ser enviada
     * @param input Sequencia de bytes recebida.
     * @param length Tamanho da sequencia de bytes.
     * @return Sempre 0 para compatibilidade.
     */
    unsigned char spi1_send_receive(uint8_t * output,  uint8_t * input, uint16_t length);
    
#ifdef	__cplusplus
}
#endif


#endif