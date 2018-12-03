
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PID_FILTER_H
#define	PID_FILTER_H


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

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
	} pid_context_t;

	typedef struct {
		int KFilter;
		long int FilterValue;
		long int FilterStateVar;
	} filter_t;

	typedef struct {
		unsigned int target;
		float slope;
		unsigned int current_value;
	} ramp_t;

	void pid_update(pid_context_t* pid, int process_var, int reference, int dt);

	void pid_zeroize(pid_context_t* pid);

	void filter_init(filter_t *pFilter, int KFilter);

	void filter_zerate(filter_t *pFilter);

	void filter_update(filter_t *pFilter, long int value);

	void ramp_init(ramp_t * this, unsigned int initial_value, float slope, unsigned int target);

	void ramp_update(ramp_t * this, unsigned int time);

	void ramp_set_target(ramp_t * this, unsigned int target);

	void ramp_reset(ramp_t * this,unsigned int initial_value, unsigned int target);
    

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

