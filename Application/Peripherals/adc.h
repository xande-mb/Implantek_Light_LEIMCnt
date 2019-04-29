#ifndef ADC_H
#define ADC_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
    
    /**
     * Inicializa o ADC
     */
    void init_adc();
    
    /**
     * Recupera o valor filtrado da corrente.
     * 
     * @return Valor filtrado da corrente.
     */
    uint16_t getFilteredCurr();
    
    /**
     * Inicializa o ad da backemf
     * 
     * @param is_rev 1 se o motor estiver rotacionando em reverso.
     */
    void initAdBEMF(uint16_t is_rev);
    
    /**
     * Inicia o AD para corrente no motor.
     */
    void initAdFMSUM();


#ifdef	__cplusplus
}
#endif


#endif