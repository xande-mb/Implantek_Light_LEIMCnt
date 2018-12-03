#include "pidFilter.h"
#include <p33EP64MC206.h>

#define ABS(x) ((x)<0?-(x):(x))

#define BETA_THERM 8.0/10

unsigned char beta_therm;

void pid_zeroize(pid_context_t* pid) {
    pid->prev_error = 0xFFFF;
    pid->int_error = 0;
	pid->controler = 0;
	pid->p_term = 0;
	pid->i_term = 0;
	pid->d_term = 0;

}
int my_debug_counter = 0;

void pid_update(pid_context_t* pid, int process_var, int reference, int dt) {
	int curr_error = reference - process_var;
	int curr_errop = reference/***BETA_THERM**/ - process_var;
    int diff;
    long p_term;
    long i_term;
    long d_term;
    
    long controler;

	if (pid->int_error > pid->windup_guard && pid->windup_guard != 0)
        pid->int_error = pid->windup_guard;
	
	if (pid->int_error < 0)
        pid->int_error = 0;

	if (pid->prev_error == 0xFFFF)
		diff = 0;
	else
		diff = (curr_error - pid->prev_error);

    p_term = __builtin_mulss(pid->p_mul, curr_errop);
    p_term = __builtin_divsd(p_term,pid->p_div);
//    p_term = __builtin_mulss(p_term2, p_term);
	
    i_term = __builtin_mulss(pid->i_mul, curr_error);// pid->i_mul*curr_error;
    i_term = __builtin_divsd(i_term,pid->i_div);

    d_term = __builtin_mulss(pid->d_mul,diff);
    d_term = __builtin_divsd(d_term,pid->d_div);

	pid->p_term = (int)p_term;
	pid->i_term = (int)pid->int_error;
	pid->d_term = (int)d_term;

//	double sum =  p_term + i_term + d_term;
//
//	pid->sum = sum;

//	if (sum > 5*ABS(pid->controler))
//		sum = 4*ABS(pid->controler);
//	if (sum < -5*ABS(pid->controler))
//		sum = -4*ABS(pid->controler);

	controler = p_term + pid->int_error + pid->d_term;

	if (pid->max_out != 0 && controler > pid->max_out )
		controler = pid->max_out;
	if (controler < 0)
		controler = 0;
    
    pid->controler = controler;
	
    pid->prev_error = curr_error;
	pid->int_error += (i_term);

}

void filter_init(filter_t *pFilter, int KFilter){
	pFilter->KFilter = KFilter;
	pFilter->FilterValue = 0;
	pFilter->FilterStateVar = 0;
}

void filter_zerate(filter_t *pFilter){
	pFilter->FilterValue = 0;
	pFilter->FilterStateVar = 0;
}

void filter_update(filter_t *pFilter, long int value){
	pFilter->FilterStateVar += (((long int) value - (long int) pFilter->FilterValue)*(int) (pFilter->KFilter));
	pFilter->FilterValue = (long int) (pFilter->FilterStateVar >> 15);
}

void ramp_init(ramp_t * this, unsigned int initial_value, float slope, unsigned int target){
	this->current_value = initial_value;
	this->slope = slope;
	this->target = target;
}

void ramp_update(ramp_t * this, unsigned int time){

    if (this->current_value < this->target){
    
        unsigned int new_value = this->current_value + this->slope*time;

        if (new_value > this->target)
            new_value = this->target;

        this->current_value = new_value;
        
    } else if (this->current_value > this->target) {
        
        
        unsigned int new_value = (this->current_value - this->slope*time) > 0 ? (this->current_value - this->slope*time) : 0;
        
        if (new_value < this->target)
            new_value = this->target;

        this->current_value = new_value;       
        
    }

}

void ramp_set_target(ramp_t * this, unsigned int target){

	this->target = target;

//	if (this->current_value > this->target){
//		this->current_value = this->target;
//	}
}

void ramp_reset(ramp_t * this,unsigned int initial_value, unsigned int target){
	this->current_value = initial_value;
	this->target = target;
}
