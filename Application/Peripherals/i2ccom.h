/**
 * \file: i2ccom.h
 * 
 *  Unidade de software responsável pela comunicação I2C, utilizado via hardware do PIC.
 * 
 */

#ifndef I2CCOM_H
#define	I2CCOM_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
    
//@cond NOT_DOCUMENTED

#define MAX_RETRY	 	1000
#define ONE_BYTE		1
#define TWO_BYTE		2

// EEPROM ADDRESS SIZE
#define ADDRWIDTH		ONE_BYTE

// EEPROM DRIVER COMMAND DEFINITION
#define I2C_IDLE		0
#define I2C_WRITE		1
#define I2C_READ		2
#define I2C_ERR			0xFFFF

// EEPROM DATA OBJECT
typedef struct {
		unsigned char *buff;
		unsigned int n;
		unsigned int addr;
		unsigned int csel;
}I2C1EMEM_DATA;

// EEPROM DRIVER OBJECT
typedef struct {
        unsigned int	cmd;
		I2C1EMEM_DATA	*oData;
        void (*init)(void *);
        void (*tick)(void *);
}I2C1EMEM_DRV;

#define I2C1SEMEM_DRV_DEFAULTS { 0,\
        (I2C1EMEM_DATA *)0,\
        (void (*)(void *))I2C1EMEMinit,\
        (void (*)(void *))I2C1EMEMdrv}

void I2C1EMEMinit(I2C1EMEM_DRV *);

void I2C1EMEMdrv(I2C1EMEM_DRV *);

//@endcond

/**
 * Ajusta um endereço do escravo para o módulo I2C.
 *  
 * @param slave_add Endereço de 8 bits do escravo.
 */
void i2c_setSlave(uint8_t slave_add);

/**
 * Inicializa o módulo i2c.
 */
void i2c_init();

/**
 * Escreve dados no módulo i2c.
 * 
 * @param slave Endereço do escravo.
 * @param add Endereço do registrador a ser escrito.
 * @param csel Seletor de pagina.
 * @param data_size Tamanho dos dados.
 * @param data Vetor de dados a ser enviados.
 * @return Código de erro da i2c.
 */
uint16_t i2c_write(uint8_t slave, uint8_t add, uint8_t csel, uint16_t data_size, uint8_t * data);

/**
 * Lê dados no módulo i2c.
 * 
 * @param slave Endereço do escravo.
 * @param add Endereço do registrador a ser escrito.
 * @param csel Seletor de pagina.
 * @param data_size Tamanho dos dados.
 * @param data Vetor de dados a ser lido.
 * @return Código de erro da i2c.
 */
uint16_t i2c_read(uint8_t slave, uint8_t add, uint8_t csel, uint16_t data_size, uint8_t * data);


#ifdef	__cplusplus
}
#endif

#endif	/* NEWFILE_H */

