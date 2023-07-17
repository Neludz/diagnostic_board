#ifndef _IO_H
#define _IO_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32l0xx.h"
//----------------------------------------------
// ALL IN ONE REG,  USE -> (OUT | PP | SP_VHI | NO_PULL)
// GPIOx_MODER -> ((X >> 0) & 0x03)
#define GET_GPIOx_MODER(x)  (x & 0x03)
#define IN          (0x00)
#define OUT         (0x01)
#define ALT         (0x02)
#define AI          (0x03)
// GPIOx_OTYPER -> ((X >> 2) & 0x01)
#define GET_GPIOx_OTYPER(x)  ((x >> 2) & 0x01)
#define PP          (0x00)
#define OD          (0x04)
//GPIOx_OSPEEDR -> ((X >> 3) & 0x03)
#define GET_GPIOx_OSPEEDR(x)  ((x >> 3) & 0x03)
#define SP_LOW      (0x00)
#define SP_MED      (0x08)
#define SP_HI       (0x10)
#define SP_VHI      (0x18)
//GPIOx_PUPDR -> ((X >> 5) & 0x03)
#define GET_GPIOx_PUPDR(x)  ((x >> 5) & 0x03)
#define NO_PULL     (0x00)
#define PULL_UP     (0x20)
#define PULL_DOWN   (0x40)
//----------------------------------------------

// GPIOx_AFRL
#define AF_0   (0x00)
#define AF_1   (0x01)
#define AF_2   (0x02)
#define AF_3   (0x03)
#define AF_4   (0x04)
#define AF_5   (0x05)
#define AF_6   (0x06)
#define AF_7   (0x07)

typedef struct
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
    uint8_t Mode;
    uint8_t AF;
    uint8_t DefState;
    uint8_t ActiveState;
} tGPIO_Line;

typedef enum
{
    OFF = 0,
    ON = 1,
    LOW = 0,
    HIGH =1,
} tIOState;

#define ADC_NUMBER              3

#define	ADC_TEMP_CHANNEL        4
#define	ADC_V_MID_CHANNEL       7
#define	ADC_V_HI_CHANNEL        6
#define ADC_NUMBER_LIST        {ADC_TEMP_CHANNEL, ADC_V_MID_CHANNEL, ADC_V_HI_CHANNEL}
#define MV_ADC  				3300
#define ADC_COUNTS  			(1<<12)
#define MODE_LEGACY             0   //0 or 1
#define BAUDRATE                38400

enum
{
    ADC_TEMP_NUM,
    ADC_V_HI_NUM,
    ADC_V_MID_NUM,
};

//          NAME    GPIOx   GPIO_Pin    MODE    AF  DefState  ActiveState
#define IO_TABLE\
	X_IO(io_addr0,			GPIOB,  3,      (IN | PULL_UP),	0,		0,  	LOW)	\
	X_IO(io_addr1,			GPIOB,  5,      (IN | PULL_UP),	0,		0,  	LOW)	\
    X_IO(io_addr2,			GPIOB,  4,      (IN | PULL_UP),	0,		0,  	LOW)	\
    X_IO(io_addr3,			GPIOA,  15,     (IN | PULL_UP),	0,		0,  	LOW)	\
    X_IO(io_mode,			GPIOA,  10,     (IN | PULL_UP),	0,		0,  	LOW)	\
    X_IO(io_adc_temp_I4,	GPIOA,  4,      (AI | NO_PULL),	0,		0,  	LOW)	\
    X_IO(io_adc_volt_mid_I7,GPIOA,  7,      (AI | NO_PULL),	0,		0,  	LOW)	\
    X_IO(io_adc_volt_hi_I6, GPIOA,  6,      (AI | NO_PULL),	0,		0,  	LOW)	\
    X_IO(io_uart_tx,        GPIOB,  6,      (ALT | PP | SP_VHI | NO_PULL),  AF_6,   0,  LOW)	\
    X_IO(io_mid_error,      GPIOA,  9,      (IN | NO_PULL),	0,		0,  	LOW)	\
    X_IO(io_hi_error,       GPIOA,  8,      (IN | NO_PULL),	0,		0,  	LOW)	\



typedef enum
{
#define X_IO(a,b,c,d,e,f,g)	a,
    IO_TABLE
#undef X_IO
    NUM_IO		//count
} tIOLine;

void IO_Init(void);
bool IO_GetLineActive(tIOLine Line);
void IO_UARTC_Init(uint32_t mode);
void IO_DeConfigLine(tIOLine Line);

#endif /* _IO_H */

