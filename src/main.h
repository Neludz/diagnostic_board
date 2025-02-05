#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "clock_l0xx.h"
#include "stm32l0xx_ll_conf.h"
#include <IO_stm32l0xx.h>

#define BOR_OFF                 ((uint8_t)0x00) /*!< BOR is disabled at power down, the reset is asserted when the VDD
                                             power supply reaches the PDR(Power Down Reset) threshold (1.5V) */
#define BOR_LEVEL1              ((uint8_t)0x08) /*!< BOR Reset threshold levels for 1.7V - 1.8V VDD power supply    */
#define BOR_LEVEL2              ((uint8_t)0x09) /*!< BOR Reset threshold levels for 1.9V - 2.0V VDD power supply    */
#define BOR_LEVEL3              ((uint8_t)0x0A) /*!< BOR Reset threshold levels for 2.3V - 2.4V VDD power supply    */
#define BOR_LEVEL4              ((uint8_t)0x0B) /*!< BOR Reset threshold levels for 2.55V - 2.65V VDD power supply  */
#define BOR_LEVEL5              ((uint8_t)0x0C)
#define BOR_LEVEL               BOR_LEVEL2
#define RDP_VAL                 0xBB

#define IWDG_TIME_X_0_1S        200 // 20s

#define ADC_TIME_MS             20
//#define ADC_DELAY_LEGACY_MS     30
#define ADC_DELAY_MODERN_MS     80
#define UART_TIME_LEGASY_MS     55
#define UART_TIME_MODERN_MS     20

#define SYSTIMER_TICK           1000
#define SYSTIMER_MS_TO_TICK(x)  ((SYSTIMER_TICK * x) / 1000)
#define ADC_TIME                SYSTIMER_MS_TO_TICK(ADC_TIME_MS)

#define DATA_UPDATE_MS          500

#define K_FILTER_VOLT           15     //1..255
#define K_FILTER_B_VOLT         20     //1..255
#define K_FILTER_D_VOLT         50     //1..255
#define K_FILTER_TEMPERATURE    15     //1..255
#define THRESHOLD_LOW_DIGIT     32
#define THRESHOLD_HI_DIGIT      120

// t = A * D_Rt + B
#define TEMPER_SKIP_MASK            0x07
#define TEMPERATURE_MAX             150
#define TEMPERATURE_MIN             -50
#define K_TEMPER_LEGACY_A_X1000     324
#define K_TEMPER_LEGACY_B           (-354)
#define K_TEMPER_PT1000_A_X1000     503
#define K_TEMPER_PT1000_B           (-339)
// table
#define TEMPER_MAX_TABLE            TEMPERATURE_MAX
#define TEMPER_MIN_TABLE            TEMPERATURE_MIN
#define TEMPER_STEP_TABLE           10
#define TEMPER_WHEN_UNDER           (160)
#define TEMPER_WHEN_OVER            (160)
#define TEMPER_TD4A_TABLE           {941, 971, 1001, 1031, 1061, 1092, 1123, 1154, 1185, 1216, 1247, 1278, 1310, 1341, 1372, 1403, 1434, 1465, 1496, 1527, 1557}
#define TEMPER_PT1000_TABLE         {557, 581, 604, 627, 649, 671, 693, 715, 736, 757, 777, 797, 817, 837, 856, 875, 893, 912, 930, 948, 966}

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H_INCLUDED */
