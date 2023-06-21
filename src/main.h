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

#define ADC_TIME_MS             20
#define ADC_DELAY_LEGACY_MS     30
#define ADC_DELAY_MODERN_MS     80
#define UART_TIME_LEGASY_MS     55
#define UART_TIME_MODERN_MS     20

#define SYSTIMER_TICK           1000
#define SYSTIMER_MS_TO_TICK(x)  ((SYSTIMER_TICK * x) / 1000)
#define ADC_TIME                SYSTIMER_MS_TO_TICK(ADC_TIME_MS)

#define R_LOW_DEV_OHM_ADC 		200
#define R_HIGH_DEV_OHM_ADC 		70500
//#define K_SHEME_X100            141
#define K_VOLT_MV               ((MV_ADC * (R_LOW_DEV_OHM_ADC + R_HIGH_DEV_OHM_ADC))/(R_LOW_DEV_OHM_ADC * ADC_COUNTS))
#define K_FILTER_VOLT           43      //1..255
#define K_FILTER_TEMPERATURE    1       //1..255
#define THRESHOLD_LOW_DIGIT     7
#define THRESHOLD_HI_DIGIT      30

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H_INCLUDED */
