
#include <stdint.h>
#include <stdbool.h>
#include <IO_stm32l0xx.h>
#include <stm32l0xx_ll_bus.h>
#include <stm32l0xx_ll_adc.h>
#include <stm32l0xx_ll_lpuart.h>
#include <main.h>
//------------------------------- prototype -------------------------------------
static void IO_ADC_Init(void);
static void IO_ConfigLine(tGPIO_Line io);
static void IO_UARTC_Init(void);
//--------------X macros---------------------------------------------------------
const tGPIO_Line IOs[NUM_IO] =
{
#define X_IO(a,b,c,d,e,f,g)	{b,c,d,e,f,g},
    IO_TABLE
#undef X_IO
};
//--------------------------------- variable -------------------------------------
const uint8_t IO_ADC_number[ADC_NUMBER] = ADC_NUMBER_LIST;
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
        IO_ConfigLine(IOs[Line]);
    }
    IO_ADC_Init();
    IO_UARTC_Init();
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

static void IO_ADC_Init(void)
{
    LL_ADC_SetCommonFrequencyMode(ADC1_COMMON, LL_ADC_CLOCK_FREQ_MODE_LOW);  //if adc clock low then 3,5 MHz
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1); // clock enable
    LL_ADC_SetClock(ADC1, LL_ADC_CLOCK_SYNC_PCLK_DIV1); // PCLK prescaler
    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_10B);        // Resolution

    LL_ADC_EnableInternalRegulator(ADC1);
    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_1CYCLE_5);
    //LL_ADC_SAMPLINGTIME_79CYCLES_5


    for(int i = 0; i < ADC_NUMBER; i++)
    {
        LL_ADC_REG_SetSequencerChAdd(ADC1, (1<<IO_ADC_number[i]));
    }
    LL_ADC_StartCalibration(ADC1);
    while(LL_ADC_IsCalibrationOnGoing(ADC1))
    {

    }
    LL_ADC_Enable(ADC1);
    LL_ADC_EnableIT_EOC(ADC1);
    LL_ADC_EnableIT_EOS(ADC1);
    NVIC_SetPriority(ADC1_IRQn, 1);
    NVIC_EnableIRQ(ADC1_IRQn);

}

static void IO_UARTC_Init(void)
{

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPUART1);

    LL_LPUART_SetDataWidth(LPUART1, LL_USART_DATAWIDTH_8B);
    LL_LPUART_SetStopBitsLength(LPUART1, LL_USART_STOPBITS_1);
    LL_LPUART_SetBaudRate(LPUART1, SYSCLK_FREQ, BAUDRATE);
    LL_LPUART_EnableDirectionTx(LPUART1);

    LL_LPUART_EnableIT_TC(LPUART1);
    // LL_LPUART_EnableIT_TXE(LPUART1);
    LL_LPUART_Enable(LPUART1);
    NVIC_SetPriority(LPUART1_IRQn, 1);
    NVIC_EnableIRQ(LPUART1_IRQn);
}

