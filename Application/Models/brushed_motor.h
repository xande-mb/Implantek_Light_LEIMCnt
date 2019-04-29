#ifndef BRUSHED_MOTOR_H
#define BRUSHED_MOTOR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
/**
 * Roda o motor com escovas
 * 
 * @param speed Velocidade de rota��o.
 * @param direction Dire��o da rota��o.
 */
void brushedMotor_run(uint16_t speed, uint16_t direction);
  
/**
 * Interrompe a rota��o do motor.
 */
void brushedMotor_stop();

/**
 * Verifica se o motor est� rotacionando.
 * 
 * @return Verdadeiro se o motor estiver rotacionando.
 */
uint16_t brushedMotor_isRunning();

/**
 * Verifica a velocidade de rota��o do motor.
 * 
 * @return Velocidade de rota��o do motor. 
 */
uint16_t brushedMotor_getVel();

/**
 * Verifica a corrente do motor durante a rota��o.
 * 
 * @return Corrente do motor.
 */
uint16_t brushedMotor_getCurrent();

/**
 * Ajusta o torque para o motor.
 * 
 * @param value Torque m�ximo do motor.
 */
void brushedMotor_setTorque(unsigned int value);


/**
 * Verifica se o motor foi travado por excesso de torque.
 * 
 * @return Verdadeiro se o motor foi parado por excesso de torque.
 */
uint16_t brushedMotor_isStopped();
        
#ifdef	__cplusplus
}
#endif

#endif // BRUSHED_MOTOR_H
