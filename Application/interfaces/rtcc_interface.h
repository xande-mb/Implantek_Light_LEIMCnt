#ifndef RTCC_INTERFACE_H
#define RTCC_INTERFACE_H

#ifdef	__cplusplus
extern "C" {
#endif


	//! Representa��o de data e hor�rio em formato BCD
	typedef union {
		unsigned char payload[7];

		struct {

			unsigned SECONDS : 4;
			unsigned SEC10 : 3;
			unsigned : 1;
			unsigned MINUTES : 4;
			unsigned MIN10 : 3;
			unsigned : 1;
			unsigned HOUR : 4;
			unsigned H10 : 1;
			unsigned H20 : 1;
			unsigned H12_24 : 1;
			unsigned : 1;
			unsigned WEEKDAY : 3;
			unsigned : 5;
			unsigned DAY : 4;
			unsigned DAY10 : 2;
			unsigned : 2;
			unsigned MONTH : 4;
			unsigned MONTH10 : 1;
			unsigned : 3;
			unsigned YEAR : 4;
			unsigned YEAR10 : 4;
		};

		struct {

			unsigned : 8;
			unsigned : 8;
			unsigned : 5;
			unsigned HAM_PM : 1;
		};

	} RTCC_BCD_t;


	//! Representa��o de data e hor�rio em formato decimal
	typedef struct {

		unsigned char SECOND;
		unsigned char MINUTE;
		unsigned char HOUR;
		unsigned char DAY;
		unsigned char MONTH;
		unsigned char YEAR;
		
	} RTCC_t;

	
//! Converte a representa��o da data e do hor�rio de BCD para decimal.
RTCC_t bcdToDecimal(RTCC_BCD_t bcdVal);
//! Converte a representa��o da data e do hor�rio de decimal para BCD.
RTCC_BCD_t decimalToBcd(RTCC_t decVal);
//! L� do RTCC e retorna a representa��o de data e hor�rio no formato BCD.
RTCC_t rtccRead();
//! Escreve no RTCC uma data.
unsigned char  rtccWriteDate(RTCC_t refDate);
//! Escreve no RTCC um hor�rio.
void rtccWriteTime(RTCC_t refTime);
//! Retorna o n�mero de dias desde o in�cio do calend�rio gregoriano at� o in�cio de um dado ano.
long leap(long year);
//! Retorna um n�mero representando o dia da semana de uma dada data.
long dayOfWeek(long year, long month, long day);
//! Retorna o n�mero de dias de um dado m�s.
unsigned char numberOfDaysInMonth(RTCC_t refDate);
//! Retorna o nome do dia da semana representado por uma dada data.
char* dayOfWeekName(RTCC_t refDate);

void rtcc_init();
    
  
#ifdef	__cplusplus
}
#endif

#endif // RTCC_INTERFACE_H
