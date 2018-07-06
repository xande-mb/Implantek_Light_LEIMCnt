#ifndef DMD_INTERFACE_H
#define DMD_INTERFACE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

    enum motor_cmd_e {
	/**
	 * Não implementado
	 */
	CMD_LEITURA_PEDAL = 0x01,
	/**
	 * Não implementado
	 */
	CMD_GRAVA_NS = 0x02,
	/**
	 * Não implementado
	 */
	CMD_GRAVA_TUSO = 0x03,
	/**
	 * Não implementado
	 */
	CMD_DEFINE_CALIBRACAO = 0x04,
	/**
	 * Modifica o motor selecionado, verificar flags para confirmar
	 * SEND_DATA: MOT(byte);
	 * RECV_DATA: DUMMY;
	 */
	CMD_ALTERA_MOTOR = 0x05,
	/**
	 * Não implementado
	 */
	CMD_INFO_MOTOR_I = 0x0A,
	/**
	 * Não implementado
	 */
	CMD_INFO_MOTOR_II = 0x0B,
	/**
	 * Não implementado
	 */
	CMD_INFO_MOTOR = 0x0C,
	/**
	 * Retorna a versão da placa:
	 * SEND_DATA: DUMMY;
	 * RECV_DATA: MAJOR, MINOR, LETTER;
	 */
	CMD_VERSAO_PLACA = 0x0D,
	/**
	 * Retorna as flags e velocidade do motor ativo:
	 * SEND_DATA: DUMMY;
	 * RECV_DATA: board_flags_t, motor_flags_t, velocity(int little_endian);
	 */
	CMD_GET_FLAGS = 0x0E,
	/**
	 * Retorna as flags e velocidade do motor ativo:
	 * SEND_DATA: DUMMY;
	 * RECV_DATA: motor_mem_t;
	 */
	CMD_GET_INFO_MEM = 0x0F,
	/**
	 * Habilita o motor selecionado em caso de motor com gatilho:
	 * SEND_DATA: HAB(byte), VEL_MAX(int little_endian), DIR(byte);
	 * RECV_DATA: DUMMY;
	 */
	CMD_HABILIT_MOTOR = 0x10,
	/**
	 * Grava o motor selecionado de acordo com o id enviado
	 * SEND_DATA: MOTOR_ID;
	 * RECV_DATA: DUMMY;
	 */
	CMD_GRAVA_MOTOR = 0x11,
    };

    typedef struct {
	/**
	 * Indica estado da maquina segundo ENUM states_e
	 */
	unsigned state : 3;
	/**
	 * Indica motor selecionado
	 * (NULO indica que nenhum motor está selecionado)
	 */
	unsigned selected_motor : 2;
	/**
	 * Indica erro no teste do FET
	 * (Não Implementado)
	 */
	unsigned fet_error : 1;
	/**
	 * Indica motor 1 conectado
	 */
	unsigned motor1_con : 1;
	/**
	 * Indica motor 2 conectado
	 */
	unsigned motor2_con : 1;
	/**
	 * Indica se o software vai ser controlado por gatilho
	 */
	unsigned gat_control : 1;
	unsigned unused : 7;
    } board_flags_t;

    typedef struct {
	/**
	 * Indica motor rodando
	 */
	unsigned motor_runing : 1;
	/**
	 * Indica que motor foi parado e não pode rodar por algum motivo
	 * (Por enquanto indica apenas gatilho pressionado)
	 */
	unsigned stopped : 1;
	/**
	 * @deprecated
	 * Indica que os gatilhos foram calibrados
	 */
	unsigned calibrated : 1;
	/**
	 * Indica que o gatilho direito está ativo
	 */
	unsigned dir_active : 1;
	/**
	 * Indica que o gatilho revcerso está ativo
	 */
	unsigned rev_active : 1;
	/**
	 * Indica que o motor está conectado
	 */
	unsigned motor_present : 1;
	/**
	 * Indica que a memória foi lida e reconhecido
	 */
	unsigned motor_recognized : 1;
	/**
	 * Indica que o motor está habilitado a rodar
	 * (só é utilizado no caso de motores que possuem gatilho)
	 */
	unsigned motor_habilita : 1;
	/**
	 * Motor rodou por muito tempo acima da velocidade determinada
	 *
	 */
	unsigned motor_overspeed : 1;
	/**
	 * Limite de corrente excedido
	 */
	unsigned motor_over_current : 1;
	/**
	 * Motor não rodou
	 * (!) Geralmente indica falta de alimentação
	 */
	unsigned motor_stalled : 1;
	/**
	 * Motor não atingiu a velocidade requisitada
	 * (!) Em geral indica baixa tensão
	 */
	unsigned motor_underspeed : 1;
	/**
	 * Erro de sequência de comutação
	 * (!) Em geral indica problema em um dos sensores hall
	 */
	unsigned motor_sector_error : 1;
	/**
	 * Erro de direção
	 * (!) Indica halls ou fases montados de forma errada
	 */
	unsigned motor_direction_error : 1;

	unsigned unused : 2;
    } motor_flags_t;

    typedef struct osc_config_s {
	unsigned int min_vel;

	unsigned int max_vel;

	unsigned int display_min_vel;

	unsigned int display_max_vel;

	unsigned int freq_max;

	unsigned int freq_min;

	unsigned int freq_def;

	unsigned int slope_factor;

    } osc_config_t;

    typedef struct motor_config_s {
	unsigned int min_vel;

	unsigned int max_vel;

	unsigned int display_min_vel;

	unsigned int display_max_vel;

	unsigned int ph_sec_of_cw;

	unsigned int ph_sec_of_ccw;

	int p_mul;

	int i_mul;

	int d_mul;

	int p_div;

	int i_div;

	int d_div;

	unsigned char alg;

	unsigned char break_type;

	unsigned int slope_factor;

    } motor_config_t;

    typedef struct mem_data_exchange_s {
	uint8_t manufacturer;
	uint8_t type;
	uint8_t application;
	uint8_t id;
	uint16_t sn;
	uint8_t can_window;
	uint8_t can_oscilate;
	uint16_t max_vel;
	uint16_t min_vel;
	uint8_t has_gat;
    } mem_data_exchange_t;

    enum motor_aplication_e {
	APLIC_ODONTO,
	APLIC_MEDICA
    };

    enum motor_maufact_e {
	MANUF_PORTESCAP,
	MANUF_MAXON,
	MANUF_FAULHABER
    };

    enum motor_type_e {
	M_TYPE_MICRO,
	M_TYPE_SHAVER,
	M_TYPE_MINI_SHAVER,
	M_TYPE_PERF,
	M_TYPE_INJECTOR
    };

    enum motor_id_e {
	M_ID_MICROMOTOR20 = 0x20,
	M_ID_MINISHAVER = 0x21,
	M_ID_SHAVER = 0x30,
	M_ID_PERFURADOR_MAXON = 0x31,
	M_ID_SHAVER_PORTESCAP = 0x40,
	M_ID_MICROMOTOR41 = 0x41,
	M_ID_MINISHAVER_PORTESCAP = 0x42,
	M_ID_INJETOR_PORTESCAP = 0x43,
	M_ID_PERFURADOR_PORTESCAP = 0x44,
    };


//    uint8_t dmd_get_flags(motor_flags_t * motor_flags, board_flags_t * board_flags);
//
//    uint8_t dmd_get_mem_info(mem_data_exchange_t * motor_mem);

    void dmd_init();

    uint8_t dmd_hab_motor(uint16_t velocity, uint8_t flag, uint8_t direction);
    
    motor_flags_t * dmd_motor_flags();

    board_flags_t * dmd_board_flags();

    mem_data_exchange_t * dmd_motor_info();
    
    uint8_t dmd_alternate_motor();

#ifdef	__cplusplus
}
#endif

#endif // DMD_INTERFACE_H
