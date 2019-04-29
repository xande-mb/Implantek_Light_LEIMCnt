/*! \file PinDefinitions.h
    \brief Atribuição de pinos aos componentes e periféricos do sistema.

    Contém as definições associadas à atribuição de pinos específicos
    aos diferentes componentes e periféricos do sistema.
 */

#ifndef PINMAP_H
#define	PINMAP_H

// Clock
#define	FCY					40000000

#include "../System/PortManager/portman.h"

/**** Display ****/
#define DISP_RS			PORT_A,9
#define DISP_RW			PORT_C,3
#define DISP_EN			PORT_C,4
#define DB0				PORT_C,5
#define DB1				PORT_D,8
#define DB2				PORT_B,5
#define DB3				PORT_C,13
#define DB4				PORT_B,9
#define DB5				PORT_C,6
#define DB6				PORT_C,7
#define DB7				PORT_C,8
#define RST				PORT_F,0
#define CS1				PORT_D,5
#define CS2				PORT_D,6

#define DRST            PORT_A,4

#define HLSEL           PORT_B,10


#define VEEC            PORT_E,15
#define VEEM            PORT_A,10


/**** Push buttons ****/
//#define SW1             PORT_A,7 
//#define SW2             PORT_G,7
//#define SW3             PORT_G,6
//#define SW4             PORT_E,14
//#define SW5             PORT_E,13
//#define SW6             PORT_E,12

/**** Pedal ****/
//#define FSWF1             PORT_F,1 
//#define FSWF2             PORT_B,10
//#define FSWF3             PORT_B,11

/******* Step Motor *********/

#define PDIR              PORT_G,9
#define PRESET            PORT_B,1
#define PSTEP             PORT_G,8
#define PSLEEP            PORT_A,1


#endif	/* PINMAP_H */

