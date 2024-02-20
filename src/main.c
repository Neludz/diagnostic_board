#include "main.h"
#include "stm32l0xx.h"
#include <IO_stm32l0xx.h>
#include <stm32l0xx_ll_adc.h>
#include <stdio.h>
#include <string.h>

//-------------------------------------------------------------------------
uint32_t  adc_value_v_mid = 0, adc_value_v_hi = 0;
uint32_t adc_average_v_mid = 0, adc_average_v_hi = 0;
int32_t adc_value_temper = 0;
extern uint16_t adc_data[];
int32_t adc_filtered[ADC_NUMBER];
uint16_t uart_buffer[16];
volatile uint32_t  adc_busy = 0, adc_iteration_count = 0, adc_threshold_start = 0;
uint32_t v_wait_threshold = 0;
uint32_t address = 0, mode = 0;
volatile uint32_t Tick=0;
uint32_t adc_delay = 0, uart_delay = 0, print_delay = 0;
uint32_t tx_count = 0, tx_index, uart_busy = 0;
int32_t temp_temp;
uint32_t print1;
const unsigned char crc_array[256] =
{
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
    0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
    0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0,
    0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d,
    0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5,
    0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58,
    0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6,
    0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
    0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f,
    0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92,
    0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c,
    0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1,
    0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49,
    0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
    0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a,
    0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7,
    0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};
//-------------------------------------------------------------------------
void DMA1_Channel1_IRQHandler()
{
    if (LL_DMA_IsActiveFlag_TC1(DMA1) || LL_DMA_IsActiveFlag_TE1(DMA1))
    {
        adc_busy = 0;
        adc_iteration_count++;
        WRITE_REG(DMA1->IFCR, DMA_IFCR_CTEIF1 | DMA_IFCR_CTCIF1);
    }
}
//-------------------------------------------------------------------------
void SysTick_Handler()
{
    Tick++;
}
//-------------------------------------------------------------------------
uint32_t MainTimerGetTick(void)
{
    return Tick;
}
//-------------------------------------------------------------------------
void LPUART1_IRQHandler()
{
    if(LL_LPUART_IsActiveFlag_TXE(LPUART1))
    {
        if (tx_index < tx_count)
        {
            LPUART1->TDR = uart_buffer[tx_index] & 0x1FFUL;
            tx_index++;
        }
        else
        {
            LL_LPUART_DisableIT_TXE(LPUART1);
        }
    }
    if(LL_LPUART_IsActiveFlag_TC(LPUART1))
    {
        LL_LPUART_ClearFlag_TC(LPUART1);
        uart_busy = 0;
    }
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
void data_update(void)
{
    if (mode == MODE_LEGACY)
    {
        temp_temp = (((int32_t)(((R_TEMPER_DIVIDER * adc_filtered[ADC_TEMP_NUM])/(ADC_COUNTS - adc_filtered[ADC_TEMP_NUM])) - R_TEMPER_LEGACY_R0)*\
                      (100000/R_TEMPER_LEGACY_R0) + (K_TEMPER_A_INVERSE_X100>>1))/K_TEMPER_A_INVERSE_X100);
    }
    else
    {
        temp_temp = (((int32_t)(((R_TEMPER_DIVIDER * adc_filtered[ADC_TEMP_NUM])/(ADC_COUNTS- adc_filtered[ADC_TEMP_NUM])) - R_TEMPER_MODERN_R0)*\
                      (100000/R_TEMPER_MODERN_R0) + (K_TEMPER_A_INVERSE_X100>>1))/K_TEMPER_A_INVERSE_X100);

        adc_value_v_mid = (adc_average_v_mid * K_VOLT_MV + 500)/1000; //((adc_value_v_mid * 63) + (adc_average_v_mid * K_VOLT_MV/1000)) >> 6; //
        adc_value_v_hi =  (adc_average_v_hi * K_VOLT_MV + 500)/1000; //((adc_value_v_hi * 63) + (adc_average_v_hi * K_VOLT_MV/1000)) >> 6; //
    }
    if (temp_temp >= TEMPERATURE_MAX || temp_temp <= TEMPERATURE_MIN)
        adc_value_temper = TEMPERATURE_MAX;
    else
        adc_value_temper = (adc_value_temper * 3 + temp_temp) >> 2;

    LL_IWDG_ReloadCounter(IWDG);
}
//-------------------------------------------------------------------------
void adc_processing(void)
{
    static int32_t intermediate_v_mid = 0, intermediate_v_hi = 0;
    static uint32_t intermediate_t = 0;
    static int32_t b_v_mid = 0, b_v_hi = 0;
    static int32_t adc_sum_v_mid = 0, adc_sum_v_hi = 0, adc_state = 0;
    uint32_t value_temp_hi, value_temp_mid;
    switch (adc_state)
    {
    case 0:
        if(!adc_busy)
            adc_state = 1;  //fall
        else
            break;
    case 1:
        // temperature
        intermediate_t += (adc_data[ADC_TEMP_NUM] - (uint32_t)adc_filtered[ADC_TEMP_NUM]);
        adc_filtered[ADC_TEMP_NUM] = (uint32_t)((intermediate_t * K_FILTER_TEMPERATURE) >> 8);
        if (mode == MODE_LEGACY) //double smoothing
        {
            // v_hi
            intermediate_v_hi = adc_filtered[ADC_V_HI_NUM];
            adc_filtered[ADC_V_HI_NUM]=((K_FILTER_B_VOLT*adc_data[ADC_V_HI_NUM])+((256-K_FILTER_B_VOLT)*(intermediate_v_hi+b_v_hi)))/256;
            b_v_hi = (K_FILTER_D_VOLT*(adc_filtered[ADC_V_HI_NUM] - intermediate_v_hi) + ((256-K_FILTER_D_VOLT)*b_v_hi))/256;
            adc_sum_v_hi += adc_filtered[ADC_V_HI_NUM];
            // v_mid
            intermediate_v_mid = adc_filtered[ADC_V_MID_NUM];
            adc_filtered[ADC_V_MID_NUM]=(K_FILTER_B_VOLT*adc_data[ADC_V_MID_NUM]+((256-K_FILTER_B_VOLT)*(intermediate_v_mid+b_v_mid)))/256;
            b_v_mid = (K_FILTER_D_VOLT*(adc_filtered[ADC_V_MID_NUM] - intermediate_v_mid) + (256-K_FILTER_D_VOLT)*b_v_mid)/256;
            adc_sum_v_mid += adc_filtered[ADC_V_MID_NUM];
        }
        else // simple
        {
            //hi
            intermediate_v_hi += (adc_data[ADC_V_HI_NUM] - adc_filtered[ADC_V_HI_NUM]);
            adc_filtered[ADC_V_HI_NUM] = (intermediate_v_hi * K_FILTER_VOLT) >> 8;
            adc_sum_v_hi += adc_filtered[ADC_V_HI_NUM];
            //mid
            intermediate_v_mid += (adc_data[ADC_V_MID_NUM] - adc_filtered[ADC_V_MID_NUM]);
            adc_filtered[ADC_V_MID_NUM] = (intermediate_v_mid * K_FILTER_VOLT) >> 8;
            adc_sum_v_mid += adc_filtered[ADC_V_MID_NUM];
        }
        if (Timer_Is_Expired(adc_delay))
        {
            adc_state = 2; //fall
        }
        else
        {
            adc_state = 0;
            adc_busy = 1;
            LL_ADC_REG_StartConversion(ADC1);
            break;
        }
    case 2:
        if (adc_sum_v_hi > 0)   //if use double smoothing ADC filtering value can be < 0
            value_temp_hi = adc_sum_v_hi;
        else
            value_temp_hi = 0;
        if (adc_sum_v_mid > 0) //if use double smoothing ADC filtering value can be < 0
            value_temp_mid = adc_sum_v_mid;
        else
            value_temp_mid = 0;
        // v_hi
        adc_average_v_hi = (adc_average_v_hi * 3 + (value_temp_hi / adc_iteration_count)) >> 2;//(adc_sum_v_hi / adc_iteration_count);//
        // v_mid
        adc_average_v_mid = (adc_average_v_mid * 3 + (value_temp_mid / adc_iteration_count)) >> 2;// (adc_sum_v_mid / adc_iteration_count);
        data_update();
        adc_sum_v_hi = 0;
        adc_sum_v_mid = 0;
        print1 = adc_iteration_count;
        adc_iteration_count = 0;
        if (adc_threshold_start)
        {
            adc_threshold_start = 0;
            adc_delay = Main_Timer_Set(ADC_TIME);
            adc_state = 0;
            adc_busy = 1;
            LL_ADC_REG_StartConversion(ADC1);
        }
        else
        {
            adc_delay = Main_Timer_Set(ADC_DELAY_MODERN_MS);
            LL_ADC_DisableInternalRegulator(ADC1);
            LL_ADC_Disable(ADC1);
            adc_state = 3;
            adc_filtered[ADC_V_MID_NUM] = 0;
            adc_filtered[ADC_V_HI_NUM] = 0;
        }
        break;
    case 3:
        if (adc_threshold_start == 1 || Timer_Is_Expired(adc_delay))
        {
            adc_delay = Main_Timer_Set(ADC_TIME);
            adc_state = 0;
            adc_threshold_start = 0;
            LL_ADC_EnableInternalRegulator(ADC1);
            LL_ADC_Enable(ADC1);
            adc_busy = 1;
            LL_ADC_REG_StartConversion(ADC1);
        }
        break;
    default :
        break;
    }
}
//-------------------------------------------------------------------------
uint32_t fill_buffer_legacy()
{
    uint32_t delta, i;
    uint8_t crc = 0;
    int32_t span;
    // addr
    uart_buffer[0] = address << 3;
    // delta
    if (!adc_average_v_hi)
        adc_average_v_hi++;
    delta = ((adc_average_v_mid * 0xFF)+(adc_average_v_hi>>1)) / adc_average_v_hi;
    //delta = delta >> 4; // 8bit
    if (delta >= 0xFF)
        delta = 0xFE;

    uart_buffer[1] = (delta >> 3) | 1;
    uart_buffer[2] = ((delta & 0xF) << 1) | 1;
    //temperature
    if (adc_value_temper >= TEMPERATURE_MAX || (int32_t)adc_value_temper <= (int32_t)TEMPERATURE_MIN)
    {
        uart_buffer[3] = 110;
    }
    else
    {
        span = ((adc_value_temper - (TEMPERATURE_MIN))*100 + (TEMPERATURE_SPAN >> 1))/TEMPERATURE_SPAN;
        uart_buffer[3] = span;
    }
    // CRC
    for (i=0; i < 4; i++)
    {
        crc = crc ^ (uart_buffer[i] & 0xFF);
    }
    crc = crc^ 0x80;
    uart_buffer[4] = crc;
    return 5;
}
//-------------------------------------------------------------------------
unsigned char dallas_crc8(const unsigned int size)
{
    unsigned char crc = 0;
    for ( unsigned int i = 0; i < size; ++i )
    {
        crc = crc_array[(0xFF & uart_buffer[i]) ^ crc];
    }
    return crc;
}
//-------------------------------------------------------------------------
uint32_t fill_buffer_modern()
{
    uint32_t delta, i;
    int16_t temperature;
    uint8_t crc;
    // addr
    uart_buffer[0] = address << 4;
    // delta
    if (adc_value_v_mid > (1<<12))
        delta = (0xFFF);
    else
        delta = adc_value_v_mid;
    uart_buffer[1] = (((delta >> 7) & 0x7F) << 1) | 0x01;
    uart_buffer[2] = ((delta & 0x7F) << 1) | 0x01;

    if (adc_value_v_hi > adc_value_v_mid && adc_value_v_hi < (1<<12))
        delta = adc_value_v_hi - adc_value_v_mid;
    else
        delta = 0;
    uart_buffer[3] = (((delta >> 7) & 0x7F) << 1) | 0x01;
    uart_buffer[4] = ((delta & 0x7F) << 1) | 0x01;

    // temperature
    if (adc_value_temper >= TEMPERATURE_MAX || adc_value_temper <= TEMPERATURE_MIN)
    {
        temperature = 224;
        uart_buffer[0] |= (0x01 << 3);
    }
    else
        temperature = adc_value_temper + 60;
    uart_buffer[5] = temperature | 0x01;
    uart_buffer[0] |= ((temperature & 0x01) << 2);
    // CRC
    crc = dallas_crc8(6);
    uart_buffer[6] = crc | 0x100;
    return 7;
}
//-------------------------------------------------------------------------
uint32_t threshold_update(void)
{
    static uint32_t v_hi_threshold_hi = 0;

    if (adc_filtered[ADC_V_HI_NUM] >= THRESHOLD_HI_DIGIT)
    {
        v_hi_threshold_hi = 1;
    }
    else if(adc_filtered[ADC_V_HI_NUM] < THRESHOLD_LOW_DIGIT && v_hi_threshold_hi == 1)
    {
        v_hi_threshold_hi = 0;
        return 1;
    }
    return 0;
}
//-------------------------------------------------------------------------
void uart_processing(void)
{
    static uint32_t uart_state = 0;

    switch (uart_state)
    {
    case 0:
        if ( Timer_Is_Expired(uart_delay))
            uart_state = 1; //fall
        else
            break;
    case 1:
        if (mode == MODE_LEGACY)
        {
            adc_threshold_start = 1;
            uart_state = 2;     //fall
        }
        else
        {
            uart_state = 3;
            break;
        }
    case 2: //legacy
        if (threshold_update())
        {
            uart_delay = Main_Timer_Set(SYSTIMER_MS_TO_TICK(UART_TIME_LEGASY_MS));
            uart_state = 0;
            if(uart_busy == 0)
            {
                tx_count = fill_buffer_legacy();
                tx_index = 0;
                uart_busy = 1;
                LL_LPUART_EnableIT_TXE(LPUART1);
            }
        }
        break;
    case 3: //modern
        uart_delay = Main_Timer_Set(SYSTIMER_MS_TO_TICK(UART_TIME_MODERN_MS));
        uart_state = 0;
        if(uart_busy == 0)
        {
            tx_count = fill_buffer_modern();
            tx_index = 0;
            uart_busy = 1;
            LL_LPUART_EnableIT_TXE(LPUART1);
        }
        break;
    default:
        break;
    }
}
//-------------------------------------------------------------------------
void print(void)
{
    if (Timer_Is_Expired(print_delay))
    {
        print_delay = Main_Timer_Set(SYSTIMER_MS_TO_TICK(2000));
        //printf("adc_average_v_hi = %d \n", adc_average_v_hi);
        //printf("adc_average_v_mid = %d \n", adc_average_v_mid);
        //printf("adc_filtered[ADC_TEMP_NUM] = %d \n", adc_filtered[ADC_TEMP_NUM]);
        printf("COUNT____ = %d \n", print1);
    }
}
//-------------------------------------------------------------------------
void flash_btock(void)
{
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
//-------------------------------------------------------------------------
void wd_init(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_WWDG);
    LL_WWDG_SetPrescaler(WWDG, LL_WWDG_PRESCALER_1);
    LL_WWDG_SetCounter(WWDG, WWDG_COUNTER);
    LL_WWDG_SetWindow(WWDG, WWDG_COUNTER);
    LL_WWDG_Enable(WWDG);
}
//-------------------------------------------------------------------------
void iwdg_init(uint8_t x0_1s_time)
{
    LL_IWDG_Enable(IWDG);
    LL_IWDG_EnableWriteAccess(IWDG);
    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_256);
    LL_IWDG_SetReloadCounter(IWDG, (144 * x0_1s_time)/10);
    while(!LL_IWDG_IsReady(IWDG))
    {
        // Wait reary flag
    }
    LL_IWDG_ReloadCounter(IWDG);
}
//-------------------------------------------------------------------------
int main(void)
{
    uint32_t i;
    ClockInit();
    SysTick_Config(SYSCLK_FREQ/SYSTIMER_TICK);
#ifndef DEBU_USER
    iwdg_init(IWDG_TIME_X_0_1S);
#endif

    print_delay = Main_Timer_Set(SYSTIMER_MS_TO_TICK(400));
    while (!Timer_Is_Expired(print_delay))
    {
        __WFI();
    }
#ifndef DEBU_USER
    flash_btock();
#endif
    IO_Init();
    print_delay = Main_Timer_Set(SYSTIMER_MS_TO_TICK(100));
    while (!Timer_Is_Expired(print_delay))
    {
        __WFI();
    }
    address = IO_GetLineActive(io_addr0) | IO_GetLineActive(io_addr1) << 1 | IO_GetLineActive(io_addr2) << 2 | IO_GetLineActive(io_addr3) << 3;
    mode = IO_GetLineActive(io_mode);
    IO_UARTC_Init(mode);
//    IO_DeConfigLine(io_addr0);
//    IO_DeConfigLine(io_addr1);
//    IO_DeConfigLine(io_addr2);
//    IO_DeConfigLine(io_addr3);
//    IO_DeConfigLine(io_mode);
#ifdef DEBU_USER
    printf ( "[ INFO ] Program start now\n" );
#endif
    for (i =0; i<ADC_NUMBER; i++)
    {
        adc_filtered[i] = 0;
        adc_data[i] = 0;
    }
    uart_delay = Main_Timer_Set(SYSTIMER_MS_TO_TICK(100));

    while(1)
    {
#ifdef DEBU_USER
        print();
#endif
        //  __WFI();
        __WFE();
        uart_processing();
        adc_processing();
    }
}
//-------------------------------------------------------------------------
