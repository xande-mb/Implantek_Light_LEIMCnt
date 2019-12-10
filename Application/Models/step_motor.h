/**
 * \file: step_motor.h
 * 
 *  Modelo responsável pela rotação do motor de passo.
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
 * Inicia rotação do motor de passo
 * 
 * @param rpm velocidade de rotação desejada
 * @param dir direção de rotação desejada
 */
void stepMotorRun(unsigned int rpm, unsigned int dir);

/**
 * Verifica se o motor de passo está em rotação.
 * 
 * @return Verdadeiro se o motor estiver rotacionando.
 */
unsigned char stepMotorRunning();
  
#ifdef	__cplusplus
}
#endif

#endif // STEP_MOTOR_H
