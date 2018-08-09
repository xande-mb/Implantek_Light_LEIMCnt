
#include <p33EP64MC206.h>
#define FCY 40000000
#include <libpic30.h>

#include "Utils/crc16.h"

volatile unsigned int spi1_wait = 0;

void __attribute__((__interrupt__, no_auto_psv)) _SPI1Interrupt(void) {
	IFS0bits.SPI1IF = 0;
	spi1_wait = 0;
	SPI1STATbits.SPIROV = 0;
}

void spi_init() {

	IEC0bits.SPI1IE = 0;
	SPI1STATbits.SPIEN = 0;
	SPI1STATbits.SPISIDL = 0;
	IFS0bits.SPI1IF = 0;
	IPC2bits.SPI1IP = 0x03;
	SPI1CON1bits.PPRE0 = 1; // 8 Mbps
	SPI1CON1bits.PPRE1 = 1; //
	SPI1CON1bits.SPRE0 = 0; //
	SPI1CON1bits.SPRE1 = 1; //
	SPI1CON1bits.SPRE2 = 1; //
	SPI1CON1bits.MSTEN = 1; // Master
	SPI1CON1bits.CKP = 1;
	SPI1CON1bits.SSEN = 0;
	SPI1CON1bits.CKE = 0;
	SPI1CON1bits.SMP = 1;
	SPI1CON1bits.MODE16 = 0;
	SPI1CON1bits.DISSDO = 0;
	SPI1CON1bits.DISSCK = 0;
	SPI1CON2bits.FRMEN = 0;
	SPI1CON2bits.SPIFSD = 0;
	IEC0bits.SPI1IE = 1;
	SPI1STATbits.SPIEN = 1;

}

unsigned char spi1_send_receive_byte(unsigned char data_out) {
	spi1_wait = 1;
	SPI1BUF = data_out;
	while (spi1_wait);
	return SPI1BUF;
}

unsigned char spi1_send_receive(uint8_t * output,  uint8_t * input, uint16_t length) {
	int i;
//	uint8_t * inptr = input;

	for (i = 0; i< length; i++){
		*input = spi1_send_receive_byte(*output);
//		kernel_message(MODE_DEBUG,"Sent: %02X Rec: %02X\n", *output,*input)
		input++;
		output++;
		__delay_us(500);
	}
	
	return 0;
}


#include <CTTKernel/kernel.h>

uint8_t spi_send_receive_crc(uint8_t * output,  uint8_t * input, uint16_t length){
	
	int i;
	uint8_t * inptr = input;
//	uint8_t * outptr = output;
	
	append_CRC(output,length-2);
	for (i = 0; i< length; i++){
		*input = spi1_send_receive_byte(*output);
//		kernel_message(MODE_DEBUG,"Sent: %02X Rec: %02X\n", *output,*input)
		input++;
		output++;
		__delay_us(500);
	}
	
	uint8_t fok = verify_crc((unsigned char *) inptr, (unsigned int) length);
	
	if (!fok){
//		kernel_message(MODE_PRE_PRODUCTION, "CRC error!\n");
//		kernel_message(MODE_PRE_PRODUCTION, "CRC error!\n");
	}
	return fok;
	
}