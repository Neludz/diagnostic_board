#ifndef CLOCK_L0XX_H_INCLUDED
#define CLOCK_L0XX_H_INCLUDED

// config: use stm32 ll driver
//HSI only with dev 4 and without PLL
//#define USE_HSE
//#define USE_PLL
//#define USE_HSE_BYPASS
//#define HSE_VALUE             8000000U
#define USE_HSI_4
#define SYSCLK_FREQ             4000000U
#define  PREFETCH_ENABLE
#define  PREREAD_ENABLE
//#define  BUFFER_CACHE_DISABLE
#define FLASH_LATENCY       LL_FLASH_LATENCY_0
#define PLL_MUL             LL_RCC_PLL_MUL_3
#define PLL_DIV             LL_RCC_PLL_DIV_4

#define AHB_PRE_DEV         LL_RCC_SYSCLK_DIV_1

//01: 1.8 V (range 1)   LL_PWR_REGU_VOLTAGE_SCALE1
//10: 1.5 V (range 2)   LL_PWR_REGU_VOLTAGE_SCALE2
//11: 1.2 V (range 3)   LL_PWR_REGU_VOLTAGE_SCALE3
#define POWER_VOLTAGE       LL_PWR_REGU_VOLTAGE_SCALE3

void ClockInit(void);

#endif /* CLOCK_L0XX_H_INCLUDED */
