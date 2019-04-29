
#include <p33EP64MC206.h>
#include "i2ccom.h"
#include <stdint.h>

unsigned int j1Done;
static uint8_t device_address = 0xA0;



void i2c_setSlave(uint8_t slave_add){
    device_address = slave_add;
}




I2C1EMEM_DRV i2c1mem = I2C1SEMEM_DRV_DEFAULTS;
I2C1EMEM_DATA w1Data;
I2C1EMEM_DATA r1Data;

void i2c_init(){
    i2c1mem.init(&i2c1mem);
}

uint16_t i2c_write(uint8_t slave, uint8_t add, uint8_t csel, uint16_t data_size, uint8_t * data){
    i2c_setSlave(slave);
    unsigned int erro_ee;
	erro_ee = 0;
	w1Data.buff = data;
	w1Data.n = data_size;
	w1Data.addr = add;
	w1Data.csel = csel;
	i2c1mem.oData = &w1Data;
	i2c1mem.cmd = I2C_WRITE;
	while (i2c1mem.cmd != I2C_IDLE) {
		i2c1mem.tick(&i2c1mem);
		if (i2c1mem.cmd == I2C_ERR) {
			erro_ee = 1; // Sinaliza erro de leitura/escrita
			i2c1mem.cmd = I2C_IDLE; // Encerra comunicação
		}
	}
	return erro_ee;
}


uint16_t i2c_read(uint8_t slave, uint8_t add, uint8_t csel, uint16_t data_size, uint8_t * data){
    i2c_setSlave(slave);
	unsigned int erro_ee = 0;
	r1Data.buff = data;
	r1Data.n = data_size;
	r1Data.addr = add;
	r1Data.csel = csel;
	i2c1mem.oData = &r1Data;
	i2c1mem.cmd = I2C_READ;

	while (i2c1mem.cmd != I2C_IDLE) {
		i2c1mem.tick(&i2c1mem);
		if (i2c1mem.cmd == I2C_ERR) {
			erro_ee = 1; // Sinaliza erro de leitura/escrita
			i2c1mem.cmd = I2C_IDLE; // Encerra comunicação
		}
	}

	return erro_ee;
    
}

/*=============================================================================
I2C1 Master Interrupt Service Routine
=============================================================================*/
void __attribute__((__interrupt__, no_auto_psv)) _MI2C2Interrupt(void) {
	j1Done = 1;
	IFS3bits.MI2C2IF = 0; //Clear the MI2C1 Interrupt Flag;

}

/*=============================================================================
I2C1 Slave Interrupt Service Routine
=============================================================================*/
void __attribute__((__interrupt__, no_auto_psv)) _SI2C2Interrupt(void) {

	IFS3bits.SI2C2IF = 0; //Clear the SI2C1 Interrupt Flag
}

/*=============================================================================
I2C1 Peripheral Initialisation
=============================================================================*/
void I2C1EMEMinit(I2C1EMEM_DRV *i2c1Mem) {
	i2c1Mem->cmd = 0;
	i2c1Mem->oData = 0;

    // ****************************************************88888
	// Configre SCL1/SDA1 pin as open-drain
	ODCBbits.ODCB4 = 1;
	ODCAbits.ODCA8 = 1;

	I2C2CONbits.A10M = 0;
	I2C2CONbits.SCLREL = 1;
	I2C2BRG = 165; // FSCL = 400KHz

	I2C2ADD = 0;
	I2C2MSK = 0;

	IPC12bits.MI2C2IP = 4;
	I2C2CONbits.I2CEN = 1;
	IEC3bits.MI2C2IE = 1;
	IFS3bits.MI2C2IF = 0;
}

/*=============================================================================
I2C1 Serial EEPROM, STATE-MACHINE BASED DRIVER
=============================================================================*/
void I2C1EMEMdrv(I2C1EMEM_DRV *i2c1Mem) {

	static int state1 = 0, cntr1 = 0, rtrycntr1 = 0;

	switch (state1) {
		case 0:
			if ((i2c1Mem->cmd == I2C_WRITE) || (i2c1Mem->cmd == I2C_READ))
				state1 = 1;
			break;

		/*==================================*/
		/* Control/Address Phase			*/
		/*==================================*/
		case 1:
			// Start Condition
			I2C2CONbits.SEN = 1;
			state1 = state1 + 1;
			break;
		case 2:
			// Start Byte with device select id
			if (j1Done == 1) {
				j1Done = 0;
				state1 = state1 + 1;
				I2C2TRN = (device_address) | (((i2c1Mem->oData->csel)&0x7) << 1);
			}
			break;
		case 3:
			// Send address byte 1, if ack is received. Else Retry
			if (j1Done == 1) {
				j1Done = 0;
				if (I2C2STATbits.ACKSTAT == 1) // Ack Not received, Retry
				{
					if (rtrycntr1 < MAX_RETRY){
						state1 = 18;
					} else {
						state1 = 16; // Flag error and exit
					}
				} else {
					rtrycntr1 = 0;
#if ADDRWIDTH==TWO_BYTE
					I2C2TRN = ((i2c1Mem->oData->addr)&0xFF00) >> 8;
					state1 = state1 + 1;
#endif
#if ADDRWIDTH==ONE_BYTE
					I2C2TRN = ((i2c1Mem->oData->addr));
					state1 = state1 + 2;
#endif
				}
			}
			break;
		case 4:
			// Send address byte 2, if ack is received. Else Flag error and exit
			if (j1Done == 1) {
				j1Done = 0;
				if (I2C2STATbits.ACKSTAT == 1) // Ack Not received, Flag error and exit
				{
					state1 = 16;
				} else {
#if ADDRWIDTH==TWO_BYTE
					I2C2TRN = ((i2c1Mem->oData->addr)&0x00FF);
#endif
					state1 = state1 + 1;
				}
			}
			break;
		case 5:
			// Read or Write
			if (j1Done == 1) {
				j1Done = 0;
				if (I2C2STATbits.ACKSTAT == 1) // Ack Not received, Flag error and exit
				{
					state1 = 16;
				} else {
					if (i2c1Mem->cmd == I2C_WRITE)state1 = state1 + 1;
					if (i2c1Mem->cmd == I2C_READ)state1 = 8;
				}
			}
			break;

			/*==================================*/
			/* Write Data Phase			        */
			/*==================================*/
		case 6:
			// Send data
			I2C2TRN = *(i2c1Mem->oData->buff + cntr1);
			state1 = state1 + 1;
			cntr1 = cntr1 + 1;
			break;
		case 7:
			// Look for end of data or no Ack
			if (j1Done == 1) {
				j1Done = 0;
				state1 = state1 - 1;
				if (I2C2STATbits.ACKSTAT == 1) // Ack Not received, Flag error and exit
				{
					state1 = 16;
				} else {
					if (cntr1 == i2c1Mem->oData->n)state1 = 14; // Close the Frame
				}
			}
			break;

			/*==================================*/
			/* Read Data Phase			        */
			/*==================================*/
		case 8:
			// Repeat Start
			I2C2CONbits.RSEN = 1;
			state1 = state1 + 1;
			break;
		case 9:
			// Re-send control byte with W/R=R
			if (j1Done == 1) {
				j1Done = 0;
				state1 = state1 + 1;
				I2C2TRN = (device_address|0b1) | (((i2c1Mem->oData->csel)&0x7) << 1);
			}
			break;
		case 10:
			// Check, if control byte went ok
			if (j1Done == 1) {
				j1Done = 0;
				state1 = state1 + 1;
				if (I2C2STATbits.ACKSTAT == 1)state1 = 16; // Ack Not received, Flag error and exit
			}
			break;
		case 11:
			// Receive Enable
			I2C2CONbits.RCEN = 1;
			state1++;
			break;
		case 12:
			// Receive data
			if (j1Done == 1) {
				j1Done = 0;
				state1 = state1 + 1;
				*(i2c1Mem->oData->buff + cntr1) = I2C2RCV;
				cntr1++;
				if (cntr1 == i2c1Mem->oData->n) {
					I2C2CONbits.ACKDT = 1; // No ACK
				} else {
					I2C2CONbits.ACKDT = 0; // ACK
				}
				I2C2CONbits.ACKEN = 1;
			}
			break;
		case 13:
			if (j1Done == 1) {
				j1Done = 0;
				if (cntr1 == i2c1Mem->oData->n)state1 = state1 + 1;
				else state1 = state1 - 2;
			}
			break;

			/*==================================*/
			/* Stop Sequence			        */
			/*==================================*/
		case 14:
			I2C2CONbits.PEN = 1;
			state1++;
			break;
		case 15:
			if (j1Done == 1) {
				j1Done = 0;
				state1 = 0;
				cntr1 = 0;
				i2c1Mem->cmd = 0;
			}
			break;

			/*==================================*/
			/* Set Error     			        */
			/*==================================*/
		case 16:
			I2C2CONbits.PEN = 1;
			state1++;
			break;
		case 17:
			if (j1Done == 1) {
				j1Done = 0;
				state1 = 0;
				rtrycntr1 = 0;
				cntr1 = 0;
				i2c1Mem->cmd = 0xFFFF;
			}
			break;

			/*==================================*/
			/* Retry         			        */
			/*==================================*/
		case 18:
			I2C2CONbits.PEN = 1;
			state1++;
			rtrycntr1++;
			break;
		case 19:
			if (j1Done == 1) {
				j1Done = 0;
				state1 = 0;
				cntr1 = 0;
			}
			break;
	}
}


