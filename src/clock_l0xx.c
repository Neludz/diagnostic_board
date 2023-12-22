#include "clock_l0xx.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx.h"
#include "main.h"

// Config

#ifndef POWER_VOLTAGE
#define POWER_VOLTAGE   LL_PWR_REGU_VOLTAGE_SCALE1
#endif

#if !defined(USE_HSE_BYPASS) && !defined(USE_HSE) && !defined(USE_HSI_4) && !defined(USE_HSI)
#define USE_HSE_BYPASS
#endif

#if !defined(PLL_MUL)
#define PLL_MUL    LL_RCC_PLL_MUL_3
#endif

#if !defined(PLL_DIV)
#define PLL_DIV    LL_RCC_PLL_DIV_4
#endif

#if !defined(FLASH_LATENCY)
#define FLASH_LATENCY    LL_FLASH_LATENCY_1
#endif

#ifndef SYSCLK_FREQ
#error SYSCLK_FREQ!!! Check this!
#endif

#if defined(USE_HSE_BYPASS) && defined(USE_HSE)
#error HSE or HSE_BYPASS!!! Check this!
#endif

#if defined(USE_HSI) && defined(USE_HSI_4)
#error HSI or HSI/4!!! Check this!
#endif

void ClockInit(void)
{
    /* Configure Buffer cache, Flash prefetch,  Flash preread */
#ifdef BUFFER_CACHE_DISABLE
    LL_FLASH_DisableBuffers();
#endif /* BUFFER_CACHE_DISABLE */

#ifdef PREREAD_ENABLE
    LL_FLASH_EnablePreRead();
#endif /* PREREAD_ENABLE */

#ifdef PREFETCH_ENABLE
    LL_FLASH_EnablePrefetch();
#endif /* PREFETCH_ENABLE */

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* Configure voltage*/
    LL_PWR_SetRegulVoltageScaling(POWER_VOLTAGE);

#if defined USE_HSE_BYPASS
    LL_RCC_HSE_EnableBypass();
    LL_RCC_HSE_Enable();
    while(!LL_RCC_HSE_IsReady ())
    {
        // Wait HSE
    }
#elif defined USE_HSE
    LL_RCC_HSE_Enable();
    while(!LL_RCC_HSE_IsReady ())
    {
        // Wait HSE
    }
#elif defined USE_HSI_4
    LL_RCC_HSI_EnableDivider();
    LL_RCC_HSI_Enable();
    while(!LL_RCC_HSI_IsReady ())
    {
        // Wait
    }
#elif defined USE_HSI
    LL_RCC_HSI_Enable();
    while(!LL_RCC_HSI_IsReady ())
    {
        // Wait
    }
#endif

#ifdef USE_PLL

#if defined(USE_HSE_BYPASS) || defined(USE_HSE)
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, PLL_MUL, PLL_DIV);
#else
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, PLL_MUL, PLL_DIV);
#endif
    LL_RCC_PLL_Enable();
    while(!LL_RCC_PLL_IsReady())
    {
        // Wait PLL
    }
#endif

    LL_FLASH_SetLatency(FLASH_LATENCY);
    while (LL_FLASH_GetLatency() != FLASH_LATENCY)
    {
        // Wait latency
    }
    LL_RCC_SetAHBPrescaler(AHB_PRE_DEV);

#if defined USE_PLL
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {
        // Wait ClkSource
    }
#elif defined(USE_HSE_BYPASS) || defined(USE_HSE)
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE)
    {
        // Wait ClkSource
    }
#else
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
    {
        // Wait ClkSource
    }
#endif

    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
}
