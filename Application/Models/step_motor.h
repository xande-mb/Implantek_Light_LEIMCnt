/**
 * \file: step_motor.h
 * 
 *  Modelo respons�vel pela rota��o do motor de passo.
 * 
 */


#ifndef STEP_MOTOR_H
#define STEP_MOTOR_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Inicializa o motor de passo
 */
void stepMotorInit();

/**
 * Inicia rota��o do motor de passo
 * 
 * @param rpm velocidade de rota��o desejada
 * @param dir dire��o de rota��o desejada
 */
void stepMotorRun(unsigned int rpm, unsigned int dir);

/**
 * Verifica se o motor de passo est� em rota��o.
 * 
 * @return Verdadeiro se o motor estiver rotacionando.
 */
unsigned char stepMotorRunning();
  
#ifdef	__cplusplus
}
#endif

#endif // STEP_MOTOR_H
