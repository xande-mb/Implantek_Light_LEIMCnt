/**
 * \file: pidFilter.h
 * 
 *  Unidade de software responsável por abstrair o PID e os filtros de aquisição de sinal.
 * 
 */


// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PID_FILTER_H
#define	PID_FILTER_H


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    /**
     * Estrutura que abstrai um módulo PID.
     */
	typedef struct {
		long windup_guard;
		int p_term;
		int i_term;
		int d_term;
		int prev_error;
		long int int_error;
		int controler;
		int max_out;
		int min_out;
		int p_mul;
		int i_mul;
		int d_mul;
		int p_div;
		int i_div;
		int d_div;
		int sum;
        long int int_error_reminder;
	} pid_context_t;

    /**
     * Estrutura que abstrai um módulo de filtro FIR.
     */
	typedef struct {
		int KFilter;
		long int FilterValue;
		long int FilterStateVar;
	} filter_t;

    /**
     * Estrutura que abstrai uma rampa digital.
     */
	typedef struct {
		unsigned int target;
		float slope;
		unsigned int current_value;
	} ramp_t;

    /**
     * Atualiza o PID
     * 
     * @param pid Ponteiro para o PID.
     * @param process_var Variável de processo do pid
     * @param reference Alvo do pid
     * @param dt Delta temporal
     */
	void pid_update(pid_context_t* pid, int process_var, int reference, int dt);
    
    /**
     * Zera o PID
     * 
     * @param pid Ponteiro para o PID
     */
	void pid_zeroize(pid_context_t* pid);

    /**
     * Inicializa um filtro IIR.
     * 
     * @param pFilter ponteiro para o filtro.
     * @param KFilter Constante de amortecimento do filtro.
     */
	void filter_init(filter_t *pFilter, int KFilter);

    /**
     * Zera os parametros internos do filtro.
     * 
     * @param pFilter ponteiro para o filtro.
     */
	void filter_zerate(filter_t *pFilter);
    

    /**
     * Faz atualização do filtro.
     * 
     * @param pFilter Ponteiro para o filtro.
     * @param value Valor a ser atualizado.
     */
	void filter_update(filter_t *pFilter, long int value);

   
    /**
     * Inicializa uma rampa.
     * 
     * @param this Ponteiro para a rampa
     * @param initial_value Valor inicial da rampa.
     * @param slope Derivada da rampa.
     * @param target Valor alvo da rampa.
     */
	void ramp_init(ramp_t * this, unsigned int initial_value, float slope, unsigned int target);

    /**
     * Faz atualização da rampa.
     * 
     * @param this Ponteiro para a rampa.
     * @param time Delta de tempo.
     */
	void ramp_update(ramp_t * this, unsigned int time);

    /**
     * Modifica o valor alvo da rampa.
     * 
     * @param this Ponteiro para a rampa.
     * @param target valor alvo da rampa.
     */
	void ramp_set_target(ramp_t * this, unsigned int target);

    /**
     * Reinicia uma rampa.
     * 
     * @param this Ponteiro para a rampa.
     * @param initial_value Valor inicial da rampa.
     * @param target Valor alvo da rampa.
     */
	void ramp_reset(ramp_t * this,unsigned int initial_value, unsigned int target);
    

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

