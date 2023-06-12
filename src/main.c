#include "main.h"
#include "stm32l0xx.h"
#include <IO_stm32l0xx.h>
#include <stm32l0xx_ll_adc.h>
#include <stdio.h>
#include <string.h>
#include <ntc_sensor.h>

uint32_t  i, adc_value_v_mid = 0, adc_value_v_hi = 0;
extern uint16_t adc_data[];
uint32_t adc_filtered[ADC_NUMBER];
uint8_t uart_buffer[10];
int16_t adc_value_temper = 0;
volatile uint32_t  adc_busy = 0, adc_iteration_count = 0, adc_channel_count = 0;
uint32_t Address = 0, Mode = 0;
volatile uint32_t Tick=0;
uint32_t adc_delay, uart_delay;
uint32_t tx = 0;
//void ADC1_IRQHandler()
//{
//    if(ADC1->ISR & ADC_ISR_EOC)
//    {
//        adc_data[adc_channel_count] = ADC1->DR;
//        adc_channel_count++;
//    }
//    if(ADC1->ISR & ADC_ISR_EOS)
//    {
//
//        LL_ADC_ClearFlag_EOS(ADC1);
//        adc_channel_count = 0;
//        adc_busy = 0;
//        adc_iteration_count++;
//    }
//}
void DMA1_Channel1_IRQHandler()
{
    if (LL_DMA_IsActiveFlag_TC1(DMA1)||LL_DMA_IsActiveFlag_TE1(DMA1))
    {
        adc_busy = 0;
        adc_iteration_count++;
        LL_DMA_ClearFlag_TC1(DMA1);
        LL_DMA_ClearFlag_TE1(DMA1);
        LL_DMA_ClearFlag_GI1(DMA1);
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
            LPUART1->TDR = 0x101;
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
    }
}

//-------------------------------------------------------------------------
void IO_delay_ms(uint32_t ms)
{
    volatile uint32_t nCount;
    nCount=(SYSCLK_FREQ/SYSTIMER_TICK)*ms;
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
//-------------------------------------------------------------------------
void adc_processing(void)
{
    static uint32_t intermediate_v_mid = 0, intermediate_v_hi = 0;
    if(!adc_busy)
    {
        adc_filtered[ADC_TEMP_NUM] = (adc_filtered [ADC_TEMP_NUM] * 7 + adc_data[ADC_TEMP_NUM]) >> 3;
        //adc_filtered[ADC_V_MID_NUM] += (adc_filtered [ADC_V_MID_NUM] * 3 + adc_data[ADC_V_MID_NUM]) >> 2;
        //adc_filtered[ADC_V_HI_NUM] = (adc_filtered [ADC_V_HI_NUM] * 3 + adc_data[ADC_V_HI_NUM]) >> 2;
        intermediate_v_mid += (adc_data[ADC_V_MID_NUM] - adc_filtered[ADC_V_MID_NUM]);
        //  filtered[ADC_V_MID_NUM] = ((intermediate_v_mid * filter_ratio]) >> 8);
        if (Timer_Is_Expired(adc_delay))
        {
            adc_delay = Main_Timer_Set(ADC_TIME);


            adc_iteration_count = 0;

        }
        adc_busy = 1;
        LL_ADC_REG_StartConversion(ADC1);
    }
}
/*
Intermediate_I += ((Inow-OFFSET_I) - Filtered_I);	//фильтрация и смещение дискрет с АЦП (с датчика половина опорного->ноль)
                Filtered_I = (( Intermediate_I * MBbuf_main[Reg_Cur_Filter_Ratio]) >> 8);
                Isum+= (Filtered_I)*(Filtered_I);
                lastVCross = checkVCross;
                if (Filtered_I > OFFSET_I_Plus) checkVCross = true;
                if (Filtered_I < OFFSET_I_Minus) checkVCross = false;
                if (Number_Of_Samples==1) lastVCross = checkVCross;
                if (lastVCross != checkVCross) Cross_Count++;
            }
            if(!(ulNotifiedValue&ADC_CURRENT_FIN))
            {
                ADC_Current_Start();
            }
        }
        while(!(ulNotifiedValue&ADC_CURRENT_FIN));

        Filter_Ratio = ADC_COUNTS * MBbuf_main[Reg_Cur_Sens_Hall_Ratio]; //divider
        Irms = (((sqrt(Isum / Number_Of_Samples)) *MBbuf_main[Reg_Cur_Scale] * Dot_count[MBbuf_main[Reg_Cur_Dot]&0x03]*mV_ADC)+(Filter_Ratio>>1))/Filter_Ratio;

        Filter_Ratio = ((MBbuf_main[Reg_Cur_Zero_Level]*Dot_count[MBbuf_main[Reg_Cur_Dot]&0x03]*MBbuf_main[Reg_Cur_Scale])/1000);
        if (Irms < Filter_Ratio) Irms=0;                                    //zero level

        MBbuf_main[Reg_Cur_Cross_Count] = (uint16_t)Cross_Count;
        MBbuf_main[Reg_Cur_RMS_W1] = (uint16_t)(Irms & 0xFFFF);
        MBbuf_main[Reg_Cur_RMS_W2] = (uint16_t)((Irms >>16) & 0xFFFF);
        MBbuf_main[Reg_Cur_N_Measure_W1] =	(uint16_t) Number_Of_Samples & 0xFFFF;
        MBbuf_main[Reg_Cur_N_Measure_W2] = (uint16_t)((Number_Of_Samples >>16) & 0xFFFF);
        Itotal=Irms;
        vTaskDelay((199/portTICK_RATE_MS));

*/



void data_update(void)
{


    adc_value_temper = calc_temperature(adc_filtered[ADC_TEMP_NUM]);
    adc_value_v_mid = (adc_filtered[ADC_V_MID_NUM] * R_HIGH_DEV_OHM_ADC)/200;
    adc_value_v_hi = 0;


}

void uart_processing(void)
{
    if (Timer_Is_Expired(uart_delay))
    {
        uart_delay = Main_Timer_Set(SYSTIMER_MS_TO_TICK(60));

        if(!LL_LPUART_IsEnabledIT_TXE(LPUART1))
        {
            tx = 0;
            LL_LPUART_EnableIT_TXE(LPUART1);
        }
//            printf("adc = %d\n", adc_data[0]);
//            printf("val = %d\n", test);
//            printf("count_measure = %d\n", measure);
//                     printf("data_1 %d\n", adc_data[0]);
//        printf("data_2 %d\n", adc_data[1]);
//        printf("data_3 %d\n", adc_data[2]);
    }

}

void flash_btock(void)
{
//----------
    uint8_t rdp_level = READ_BIT(FLASH->OPTR, FLASH_OPTR_RDPROT);
    if (rdp_level != RDP_VAL)
    {
        if(READ_BIT(FLASH->PECR, FLASH_PECR_OPTLOCK))
        {
            /* Unlocking FLASH_PECR register access*/
            if(READ_BIT(FLASH->PECR, FLASH_PECR_PELOCK))
            {
                /* Unlocking FLASH_PECR register access*/
                WRITE_REG(FLASH->PEKEYR, 0x89ABCDEFU);
                WRITE_REG(FLASH->PEKEYR, 0x02030405U);
            }
            /* Unlocking the option bytes block access */
            WRITE_REG(FLASH->OPTKEYR, 0xFBEAD9C8U);
            WRITE_REG(FLASH->OPTKEYR, 0x24252627U);
        }
//----------
        uint32_t tmp1, tmp2;
        tmp1 = (uint32_t)(OB->RDP & ((~FLASH_OPTR_RDPROT) & 0x0000FFFF));
        /* Calculate the option byte to write */
        tmp1 |= (uint32_t)(RDP_VAL);
        tmp2 = (uint32_t)(((uint32_t)((uint32_t)(~tmp1) << 16U)) | tmp1);
        /* Wait for last operation to be completed */
        while((FLASH->SR) & (FLASH_SR_BSY));
        /* program read protection level */
        OB->RDP = tmp2;
        /* Wait for last operation to be completed */
        while((FLASH->SR) & (FLASH_SR_BSY));
//----------
        tmp2 = 0;
        /* Get the User Option byte register */
        tmp1 = OB->USER & ((~FLASH_OPTR_BOR_LEV) >> 16U);
        /* Calculate the option byte to write - [0xFF | nUSER | 0x00 | USER]*/
        tmp2 = (uint32_t)~((BOR_LEVEL | tmp1)) << 16U;
        tmp2 |= (BOR_LEVEL | tmp1);

        while((FLASH->SR) & (FLASH_SR_BSY));
        /* Write the BOR Option Byte */
        OB->USER = tmp2;
        while((FLASH->SR) & (FLASH_SR_BSY));
//----------
        SET_BIT(FLASH->PECR, FLASH_PECR_OPTLOCK);
    }
}


void wd_init(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_WWDG);
    LL_WWDG_SetPrescaler(WWDG, LL_WWDG_PRESCALER_1);
    LL_WWDG_SetCounter(WWDG, WWDG_COUNTER);
    LL_WWDG_SetWindow(WWDG, WWDG_COUNTER);
    LL_WWDG_Enable(WWDG);
}

//-------------------------------------------------------------------------
int main(void)
{
    ClockInit();
    //flash_btock();
    wd_init();
    IO_Init();
    printf ( "[ INFO ] Program start now\n" );
    SysTick_Config(SYSCLK_FREQ/SYSTIMER_TICK);

    for (i =0; i<ADC_NUMBER; i++)
    {
        adc_filtered[i] = 0;
        adc_data[i] = 0;
    }
    uart_delay = Main_Timer_Set(1000);
    while(1)
    {
        adc_processing();
        uart_processing();
        LL_WWDG_SetCounter(WWDG, WWDG_COUNTER);
       // __WFI();
    }
}
