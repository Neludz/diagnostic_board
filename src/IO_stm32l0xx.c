
#include <stdint.h>
#include <stdbool.h>
#include <IO_stm32l0xx.h>
#include <stm32l0xx_ll_bus.h>
#include <stm32l0xx_ll_adc.h>
#include <stm32l0xx_ll_lpuart.h>
#include <stm32l0xx_ll_dma.h>
#include <main.h>
//------------------------------- prototype -------------------------------------
static void IO_ConfigLine(tGPIO_Line io);
//--------------X macros---------------------------------------------------------
const tGPIO_Line IOs[NUM_IO] =
{
#define X_IO(a,b,c,d,e,f,g)	{b,c,d,e,f,g},
    IO_TABLE
#undef X_IO
};
//--------------------------------- variable -------------------------------------
const uint8_t IO_ADC_number[ADC_NUMBER] = ADC_NUMBER_LIST;
uint16_t adc_data[ADC_NUMBER];
//---------------------------------------------------------------------------------
void IO_SetLine(tIOLine Line, bool State)
{
    if (State)
        IOs[Line].GPIOx->BSRR = 1 << (IOs[Line].GPIO_Pin);
    else
        IOs[Line].GPIOx->BRR = 1 << (IOs[Line].GPIO_Pin);
}
//---------------------------------------------------------------------------------
bool IO_GetLine(tIOLine Line)
{
    if (Line < NUM_IO)
        return (((IOs[Line].GPIOx->IDR) & (1<<(IOs[Line].GPIO_Pin))) != 0);
    else
        return false;
}
//---------------------------------------------------------------------------------
bool IO_GetLineActive(tIOLine Line)
{
    if (Line < NUM_IO)
    {
        bool pin_set = (((IOs[Line].GPIOx->IDR) & (1<<(IOs[Line].GPIO_Pin))) ? true : false);
        return (pin_set == ( IOs[Line].ActiveState ? true : false));
    }
    else
        return false;
}
//---------------------------------------------------------------------------------
void IO_SetLineActive(tIOLine Line, bool State)
{
    if (State ^ IOs[Line].ActiveState)
    {
        IOs[Line].GPIOx->BRR = 1 << (IOs[Line].GPIO_Pin);   //reset
    }
    else
    {
        IOs[Line].GPIOx->BSRR = 1 << (IOs[Line].GPIO_Pin);  //set
    }
}
//---------------------------------------------------------------------------------
void IO_Init(void)
{
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

// Set all pins
    for (int Line = 0; Line < NUM_IO; Line++)
    {
        IO_ConfigLine(IOs[Line]);
    }
}

static void IO_ConfigLine(tGPIO_Line io)
{
    io.GPIOx->MODER &= ~(0x03 << (io.GPIO_Pin * 2));
    io.GPIOx->MODER |= (GET_GPIOx_MODER(io.Mode) << (io.GPIO_Pin * 2));

    io.GPIOx->OTYPER &= ~(0x01 << io.GPIO_Pin);
    io.GPIOx->OTYPER |= (GET_GPIOx_OTYPER(io.Mode) << io.GPIO_Pin);

    io.GPIOx->OSPEEDR &= ~(0x03 << (io.GPIO_Pin * 2));
    io.GPIOx->OSPEEDR |= (GET_GPIOx_OSPEEDR(io.Mode) << (io.GPIO_Pin * 2));

    io.GPIOx->PUPDR &= ~(0x03 << (io.GPIO_Pin * 2));
    io.GPIOx->PUPDR |= (GET_GPIOx_PUPDR(io.Mode) << (io.GPIO_Pin * 2));

    if(io.GPIO_Pin < 8)
    {
        io.GPIOx->AFR[0] &=  ~(0x0F << (io.GPIO_Pin * 4));
        io.GPIOx->AFR[0] |=  io.AF << (io.GPIO_Pin * 4);
    }
    else
    {
        io.GPIOx->AFR[1] &=  ~(0x0F << ((io.GPIO_Pin - 8) * 4));
        io.GPIOx->AFR[1] |=  io.AF << ((io.GPIO_Pin - 8) * 4);
    }

    io.GPIOx->ODR &= ~(1 << io.GPIO_Pin);
    io.GPIOx->ODR |= (io.DefState << io.GPIO_Pin);
}

void IO_DeConfigLine(tIOLine Line)
{

    IOs[Line].GPIOx->PUPDR &= ~(0x03 << (IOs[Line].GPIO_Pin * 2));
    IOs[Line].GPIOx->PUPDR |= (0x02 << (IOs[Line].GPIO_Pin * 2));

}

void IO_ADC_Init(void)
{
    LL_ADC_SetCommonFrequencyMode(ADC1_COMMON, LL_ADC_CLOCK_FREQ_MODE_LOW);  //if adc clock low then 3,5 MHz
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1); // clock enable
    LL_ADC_SetClock(ADC1, LL_ADC_CLOCK_SYNC_PCLK_DIV1); // PCLK prescaler
    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);        // Resolution

    LL_ADC_EnableInternalRegulator(ADC1);
    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_3CYCLES_5);
    //LL_ADC_SAMPLINGTIME_79CYCLES_5

    for(int i = 0; i < ADC_NUMBER; i++)
    {
        LL_ADC_REG_SetSequencerChAdd(ADC1, (1<<IO_ADC_number[i]));
    }
    //LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);
    LL_ADC_StartCalibration(ADC1);
    while(LL_ADC_IsCalibrationOnGoing(ADC1))
    {

    }
    LL_ADC_REG_SetDMATransfer(ADC1,  LL_ADC_REG_DMA_TRANSFER_LIMITED);
    LL_ADC_Enable(ADC1);
}

void IO_UARTC_Init(uint32_t mode)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPUART1);
    LL_APB1_GRP1_EnableClockSleep(LL_APB1_GRP1_PERIPH_LPUART1);
    if (mode == MODE_LEGACY)
        LL_LPUART_SetDataWidth(LPUART1, LL_USART_DATAWIDTH_8B);
    else
        LL_LPUART_SetDataWidth(LPUART1, LL_USART_DATAWIDTH_9B);

    LL_LPUART_SetStopBitsLength(LPUART1, LL_USART_STOPBITS_1);
    LL_LPUART_SetBaudRate(LPUART1, (SYSCLK_FREQ), BAUDRATE);
    LL_LPUART_EnableDirectionTx(LPUART1);
    //LL_LPUART_EnableDMAReq_TX(LPUART1);
    LL_LPUART_EnableIT_TC(LPUART1);
    LL_LPUART_Enable(LPUART1);
    NVIC_SetPriority(LPUART1_IRQn, 1);
    NVIC_EnableIRQ(LPUART1_IRQn);
}

void IO_DMA_Init(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_AHB1_GRP1_EnableClockSleep(LL_AHB1_GRP1_PERIPH_DMA1);

    LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_1, (LL_DMA_DIRECTION_PERIPH_TO_MEMORY |\
                          LL_DMA_MODE_CIRCULAR |\
                          LL_DMA_MEMORY_INCREMENT |\
                          LL_DMA_PERIPH_NOINCREMENT |\
                          LL_DMA_MDATAALIGN_HALFWORD |\
                          LL_DMA_PDATAALIGN_HALFWORD ));

    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMA_REQUEST_0);

    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1, (uint32_t)(&(ADC1->DR)),
                           (uint32_t)adc_data, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, ADC_NUMBER);
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn); /* (1) */
    NVIC_SetPriority(DMA1_Channel1_IRQn,1); /* (2) */
}

