#include "main.h"
#include "stm32l0xx.h"
#include <IO_stm32l0xx.h>
#include <stm32l0xx_ll_adc.h>
#include <stdio.h>
volatile uint32_t adc_data[3], i=0, st=0, temp, t=0, test=0, tx=0, val;

uint32_t Tick=0;
uint32_t Delay1, Delay2;

void ADC1_IRQHandler()
{
    if(ADC1->ISR & ADC_ISR_EOC)
    {
        adc_data[i] = ADC1->DR;
        i++;
    }
    if(ADC1->ISR & ADC_ISR_EOS)
    {
        LL_ADC_ClearFlag_EOS(ADC1);
        i = 0;
        st = 0;
        t++;
    }
}

void SysTick_Handler()
{
    Tick++;
}
uint32_t MainTimerGetTick(void)
{
    return Tick;
}

void LPUART1_IRQHandler()
{
    if(LL_LPUART_IsActiveFlag_TXE(LPUART1))
    {
        if (tx < 5)
        {
            LL_LPUART_TransmitData8(LPUART1, 0x03);
            tx++;
        }
        else
        {
            LL_LPUART_DisableIT_TXE(LPUART1);
        }
    }
    if(LL_LPUART_IsActiveFlag_TC(LPUART1))
    {
        LL_LPUART_ClearFlag_TC(LPUART1);
        //LL_LPUART_DisableIT_TC(LPUART1);
       // LL_LPUART_Disable(LPUART1);
    }
}

//-------------------------------------------------------------------------
void IO_delay_ms(uint32_t ms)
{
    volatile uint32_t nCount;
    nCount=(SYSCLK_FREQ/1000)*ms;
    for (; nCount!=0; nCount--);
}
//-------------------------------------------------------------------------
bool Timer_Is_Expired (const uint32_t Timer)
{
    uint32_t TimeTick;
    TimeTick = Tick;
    return ((TimeTick - Timer) < (1UL << 31));
}
//-------------------------------------------------------------------------
uint32_t Main_Timer_Set(const uint32_t AddTime)
{
    uint32_t TimeTick;
    TimeTick = Tick;
    return TimeTick + AddTime;
}

int main(void)
{
    uint32_t delta;
    ClockInit();
    IO_Init();
    printf ( "[ INFO ] Program start now\n" );
    SysTick_Config(SYSCLK_FREQ/1000);
    Delay1 = Main_Timer_Set(60);
    Delay2 = Main_Timer_Set(1000);
    while(1)
    {
        if (Timer_Is_Expired(Delay2))
        {
            Delay2 = Main_Timer_Set(60);
            //LL_LPUART_ClearFlag_TC(LPUART1);
            //tx = 0;
            if(!LL_LPUART_IsEnabledIT_TXE(LPUART1))
            {
//                LL_LPUART_EnableIT_TC(LPUART1);
                  LL_LPUART_EnableIT_TXE(LPUART1);
//                LL_LPUART_Enable(LPUART1);
                tx=0;
            }
        }
        if(st == 0)
        {
            // temperature

//            ADC_Val=(uint16_t)IO_getADCval(i);
//            Adc_Filter_Value[i]=(Adc_Filter_Value[i]*7+ADC_Val)>>3;
//            MBbuf_main[(i+Reg_T_0_Channel)] = calc_temperature(Adc_Filter_Value[i]);
//            if ((int16_t)MBbuf_main[(i+Reg_T_0_Channel)]>=(int16_t)MBbuf_main[(Reg_T_level_Warning)]) MBbuf_main[Reg_T_Warning_bit]|=1<<i;
//            else MBbuf_main[Reg_T_Warning_bit] &=~(1<<i);
//
//            if ((int16_t)MBbuf_main[(i+Reg_T_0_Channel)]>=(int16_t)MBbuf_main[(Reg_T_level_Alarm)]) MBbuf_main[Reg_T_Alarm_bit]|=1<<i;
//            else MBbuf_main[Reg_T_Alarm_bit] &=~(1<<i);
//
//        Adc_Filter_T_MCU=((Adc_Filter_T_MCU*3 + IO_getMCUtemp())>>2);
//        MBbuf_main[Reg_T_MSD]=Adc_Filter_T_MCU;

            if (Timer_Is_Expired(Delay1))
            {
                //test = t;

                Delay1 = Main_Timer_Set(2000);
                printf("adc = %d\n", adc_data[0]);

                test = val/t;
                t = 0;
                val=0;
                printf("val = %d\n", test);
                //printf("count measure = %d\n", test);
            }
            //printf("count measure = %d\n", test);
            val += adc_data[0];
            st = 1;
            LL_ADC_REG_StartConversion(ADC1);
        }
        //IO_delay_ms(1000);
        // printf ( "value = %d \n", IO_GetLineActive(io_addr0)|IO_GetLineActive(io_addr1)<<1|IO_GetLineActive(io_addr2)<<2|IO_GetLineActive(io_addr3)<<3);
        //printf ( "mode = %d \n\n", IO_GetLineActive(io_mode));
        // printf("data_1 %d\n", adc_data[0]);
//        printf("data_2 %d\n", adc_data[1]);
//        printf("data_3 %d\n", adc_data[2]);
        //  printf("tick = %d\n", Tick);
    }
}
