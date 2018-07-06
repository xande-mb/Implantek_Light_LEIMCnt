#include "rtcc_interface.h"
#include <Application/Peripherals/spi.h>
#include <PortManager/portman.h>
#include <UartInterface/UartCommand.h>
#include <string.h>

#define FCY 16000000
#include <libpic30.h>

#define REG_0_READ 0x00
#define REG_0_WRITE 0x80
#define REG_4_WRITE 0x84


static const PORTS_t ss_port = PORT_A;
static const uint16_t ss_bit = 5;

//static void init_commands();

#define SSON portman_clearOutput(ss_port,ss_bit); __delay_us(100);

#define SSOFF __delay_us(20); portman_setOutput(ss_port,ss_bit);

static void init_commands();

//! Converte a representação da data e do horário de BCD para decimal.

/*!
    Converte a representação da data e do horário de BCD para decimal.
    As horas possuem formato de 24 h.

    \param bcdVal representação da data e do horário em BCD
    \return representação da data e do horário em formato decimal
 */
RTCC_t bcdToDecimal(RTCC_BCD_t bcdVal) {

    RTCC_t decVal;

    decVal.SECOND = bcdVal.SECONDS + (bcdVal.SEC10 * 10);
    decVal.MINUTE = bcdVal.MINUTES + (bcdVal.MIN10 * 10);

    //formato de 12 horas
    if (bcdVal.H12_24) // 12-hour format
        decVal.HOUR = bcdVal.HOUR + (bcdVal.HAM_PM * 12) + (bcdVal.H10 * 10);
        //formato de 24 horas
    else
        decVal.HOUR = bcdVal.HOUR + (bcdVal.H20 * 20) + (bcdVal.H10 * 10);

    decVal.DAY = bcdVal.DAY + (bcdVal.DAY10 * 10);
    decVal.MONTH = bcdVal.MONTH + (bcdVal.MONTH10 * 10);
    decVal.YEAR = bcdVal.YEAR + (bcdVal.YEAR10 * 10); // + 2000;

    return decVal;
}

void rtcc_init() {
    portman_setAsOutput(ss_port, ss_bit);
    init_commands();
}

//! Converte a representação da data e do horário de decimal para BCD.

/*!
    Converte a representação da data e do horário de decimal para BCD.
    As horas possuem formato de 24 h.

    \param decVal representação da data e do horário em formato decimal
    \return representação da data e do horário em formato BCD
 */
RTCC_BCD_t decimalToBcd(RTCC_t decVal) {

    RTCC_BCD_t bcdVal;

    bcdVal.SECONDS = decVal.SECOND % 10;
    bcdVal.SEC10 = decVal.SECOND / 10;
    bcdVal.MINUTES = decVal.MINUTE % 10;
    bcdVal.MIN10 = decVal.MINUTE / 10;
    bcdVal.H12_24 = 0; //formato de 24 h
    bcdVal.HOUR = decVal.HOUR % 10;
    bcdVal.H20 = decVal.HOUR / 20;

    if (!bcdVal.H20)
        bcdVal.H10 = decVal.HOUR / 10;
    else
        bcdVal.H10 = 0;

    bcdVal.DAY = decVal.DAY % 10;
    bcdVal.DAY10 = decVal.DAY / 10;
    bcdVal.MONTH = decVal.MONTH % 10;
    bcdVal.MONTH10 = decVal.MONTH / 10;
    bcdVal.YEAR = decVal.YEAR % 10;
    bcdVal.YEAR10 = decVal.YEAR / 10;

    return bcdVal;
}


//! Lê do RTCC e retorna a representação de data e horário no formato BCD.

/*!
    Converte a representação da data e do horário de decimal para BCD.
    As horas possuem formato de 24 h.

    \return representação da data e do horário em formato decimal
 */
RTCC_t rtccRead() {

    RTCC_BCD_t bcdVal;

    unsigned char rtccSend[8] = {REG_0_READ, 0, 0, 0, 0, 0, 0, 0};
    unsigned char rtccReceive[8];

    //	spi1Init(DSRTCC);

    SSON;
    //	uint8_t crc_ok = spi_send_receive_crc(data_send, data_receive, length);
    spi1_send_receive(rtccSend, rtccReceive, 8);
    SSOFF;

    memcpy(bcdVal.payload, &rtccReceive[1], 7);

    return (bcdToDecimal(bcdVal));
}


//! Verifica o data de entrada e escreve no RTCC uma data.

/*!
    \param refDate representação da data em formato decimal
    \return caso a entrada seja correta retorna 1, caso contrário retorna 0.
 */
unsigned char rtccWriteDate(RTCC_t refDate) {

    if (refDate.MONTH <= 12 && refDate.MONTH != 0) {
        if (refDate.DAY <= numberOfDaysInMonth(refDate) && refDate.DAY != 0) {
            RTCC_BCD_t bcdVal;
            bcdVal = decimalToBcd(refDate);

            unsigned char rtccSend[4] = {REG_4_WRITE, 0, 0, 0};
            unsigned char rtccReceive[4];

            memcpy(&rtccSend[1], &bcdVal.payload[4], 3);


            SSON;
            spi1_send_receive(rtccSend, rtccReceive, 4);
            SSOFF;
            return 1;
        }
    }
    return 0;
}


//! Escreve no RTCC um horário.

/*!
    \param refTime representação do horário em formato decimal.
 */
void rtccWriteTime(RTCC_t refTime) {

    RTCC_BCD_t bcd_val;
    bcd_val = decimalToBcd(refTime);

    unsigned char rtccSend[4] = {REG_0_WRITE, 0, 0, 0};
    unsigned char rtccReceive[4];

    memcpy(&rtccSend[1], bcd_val.payload, 3);

    SSON;
    spi1_send_receive(rtccSend, rtccReceive, 4);
    SSOFF;
}


//! Retorna o número de dias desde o início do calendário gregoriano até o início de um dado ano.

/*!
    Leva em consideração anos bissextos, mas não a passagem do calendário
    juliano ao calendário gregoriano, ocorrida no século XVI.
    Faz-se uma extrapolação do calendário gregoriano a um hipotético "ano zero".

    \param year ano de interesse
    \return número de dias desde o início do calendário gregoriano até o início do ano de interesse
 */
long leap(long year) {

    return year * 365 + (year / 4) - (year / 100) + (year / 400);
}


//! Retorna um número representando o dia da semana de uma dada data.

/*!
    Retorna um número representando o dia da semana de uma dada data
    (1 = segunda-feira, 7 = domingo).

    \param year ano da data de interesse
    \param month mês da data de interesse
    \param day dia da data de interesse
    \return número representando o dia da semana da data fornecida (1 = segunda-feira, 7 = domingo)
 */
long dayOfWeek(long year, long month, long day) {

    long zeller; //Congruência de Zeller

    year += ((month + 9) / 12) - 1;
    month = (month + 9) % 12;

    zeller = leap(year) + month * 30 + ((6 * month + 5) / 10) + day + 1;

    return (zeller % 7) + 1;
}


//! Retorna o número de dias de um dado mês.

/*!
    \param refDate representação de data com o mês de interesse
    \return número de dias do mês de interesse
 */
unsigned char numberOfDaysInMonth(RTCC_t refDate) {

    unsigned char numDays = 31;

    if (refDate.MONTH == 4 || refDate.MONTH == 6 || refDate.MONTH == 9 || refDate.MONTH == 11)
        numDays = 30;

    else if (refDate.MONTH == 2) {

        if (refDate.YEAR % 4 == 0)
            numDays = 29; //ano varia de 2000 a 2099. Século I é idêntico ao século XXI

        else
            numDays = 28;
    }

    return numDays;
}


char * weekDayStr[7];


//! Retorna o nome do dia da semana representado por uma dada data.

/*!
    \param refDate representação da data de interesse
    \return string contendo o nome do dia da semana da data de interesse
 */
char* dayOfWeekName(RTCC_t refDate) {

    weekDayStr[0] = "Segunda";
    weekDayStr[1] = "Terça";
    weekDayStr[2] = "Quarta";
    weekDayStr[3] = "Quinta";
    weekDayStr[4] = "Sexta";
    weekDayStr[5] = "Sabado";
    weekDayStr[6] = "Domingo";

    return weekDayStr[dayOfWeek(refDate.YEAR, refDate.MONTH, refDate.DAY) - 1];
}

static void cmd_set_time(char **args, unsigned char n_args) {

    if (n_args == 6) {

        RTCC_t rtcw;
        rtcw.YEAR = atoi(args[0]);
        rtcw.MONTH = atoi(args[1]);
        rtcw.DAY = atoi(args[2]);
        rtcw.HOUR = atoi(args[3]);
        rtcw.MINUTE = atoi(args[4]);
        rtcw.SECOND = atoi(args[5]);
        rtccWriteDate(rtcw);
        rtccWriteTime(rtcw);
        serial_printf("<OK>\n");
        return;
    }

    serial_printf("<ERR>\n");

}

static void cmd_get_time(char **args, unsigned char n_args) {


    RTCC_t rtcr = rtccRead();

    serial_printf("<OK %u %u %u %u %u %u>\n",
            rtcr.YEAR,
            rtcr.MONTH,
            rtcr.DAY,
            rtcr.HOUR,
            rtcr.MINUTE,
            rtcr.SECOND
        );
    
}

static command_t comand_set_time = {
    .cmd_identifier = "set_time",
    .handler = cmd_set_time,
};

static command_t comand_get_time = {
    .cmd_identifier = "get_time",
    .handler = cmd_get_time,
};

static void init_commands() {
    uart_add_comand(&comand_set_time);
    uart_add_comand(&comand_get_time);
}

