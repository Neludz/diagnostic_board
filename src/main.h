#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "clock_l0xx.h"
#include "stm32l0xx_ll_conf.h"

#define BOR_OFF         ((uint8_t)0x00) /*!< BOR is disabled at power down, the reset is asserted when the VDD
                                             power supply reaches the PDR(Power Down Reset) threshold (1.5V) */
#define BOR_LEVEL1      ((uint8_t)0x08) /*!< BOR Reset threshold levels for 1.7V - 1.8V VDD power supply    */
#define BOR_LEVEL2      ((uint8_t)0x09) /*!< BOR Reset threshold levels for 1.9V - 2.0V VDD power supply    */
#define BOR_LEVEL3      ((uint8_t)0x0A) /*!< BOR Reset threshold levels for 2.3V - 2.4V VDD power supply    */
#define BOR_LEVEL4      ((uint8_t)0x0B) /*!< BOR Reset threshold levels for 2.55V - 2.65V VDD power supply  */
#define BOR_LEVEL5      ((uint8_t)0x0C)
#define BOR_LEVEL       BOR_LEVEL2
#define RDP_VAL         0xBB

#define WWDG_COUNTER    0x7F

#define ADC_TIME_MS             60
#define UART_TIME_1_MS          55
#define UART_TIME_2_MS          60

#define SYSTIMER_TICK           1000
#define SYSTIMER_MS_TO_TICK(x)  ((SYSTIMER_TICK * x) / 1000)
#define ADC_TIME                SYSTIMER_MS_TO_TICK(ADC_TIME_MS)


#define MV_ADC  				3300
#define R_LOW_DEV_OHM_ADC 		200
#define R_HIGH_DEV_OHM_ADC 		70500
#define ADC_COUNTS  			(1<<10)



#ifdef __cplusplus
}
#endif

#endif /* MAIN_H_INCLUDED */
