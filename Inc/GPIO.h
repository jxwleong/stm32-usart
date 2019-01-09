#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>

// GPIO mode is in bit 0 and 1
typedef enum{
	GPIO_INPUT = 0,
	GPIO_OUTPUT,
	GPIO_ALT_FUNC,
	GPIO_ANALOG
}	GpioMode;

// GPIO output driver type is in bit 2
typedef enum{
	GPIO_PUSH_PULL = 0,
	GPIO_OPEN_DRAIN = 1 << 2,	// use bit 3 bcz bit 1 and bit 2 used in GpioMode
}	GpioDriverType;

// GPIO output speed is in bits 3 and 4
typedef enum{
	GPIO_LOW_SPEED = 0,
	GPIO_MED_SPEED = 1 << 3,
	GPIO_HI_SPEED = 2 << 3,
	GPIO_VERY_HI_SPEED = 3 << 3,
}	GpioOutputSpeed;

// GPIO pull type is in bits 5 and 6
typedef enum{
	GPIO_NO_PULL = 0,
	GPIO_PULL_UP = 1 << 5,
	GPIO_PULL_DOWN = 2 << 5,
}	GpioPullType;

typedef enum{
	PIN_RESET = 0,
	PIN_SET,
}GpioPinState;

typedef enum{
	GPIOPin0  = 0x0001,
	GPIOPin1  = 0x0002,
	GPIOPin2  = 0x0004,
	GPIOPin3  = 0x0008,
	GPIOPin4  = 0x0010,
	GPIOPin5  = 0x0020,
	GPIOPin6  = 0x0040,
	GPIOPin7  = 0x0080,
	GPIOPin8  = 0x0100,
	GPIOPin9  = 0x0200,
	GPIOPin10 = 0x0400,
	GPIOPin11 = 0x0800,
	GPIOPin12 = 0x1000,
	GPIOPin13 = 0x2000,
	GPIOPin14 = 0x4000,
	GPIOPin15 = 0x8000,
}	GPIOPin;

typedef volatile uint32_t IORegister ;

typedef struct GPIORegs GPIORegs;
struct GPIORegs {
	IORegister mode;
	IORegister driverType;
	IORegister outSpeed;
	IORegister pullType;
	IORegister inData;
	IORegister outData;
	IORegister outBits;
	IORegister pinLock;
	IORegister altFuncLow;
	IORegister altFuncHi;
};

#define gpioA	((GPIORegs *)0x40020000)
#define gpioB	((GPIORegs *)0x40020400)
#define gpioC	((GPIORegs *)0x40020800)
#define gpioD	((GPIORegs *)0x40020c00)
#define gpioE	((GPIORegs *)0x40021000)
#define gpioF	((GPIORegs *)0x40021400)
#define gpioG	((GPIORegs *)0x40021800)
#define gpioH	((GPIORegs *)0x40021c00)
#define gpioI	((GPIORegs *)0x40022000)
#define gpioJ	((GPIORegs *)0x40022400)
#define gpioK	((GPIORegs *)0x40022800)


/*AF pins*/
typedef enum{
	AF0, AF1, AF2, AF3, AF4, AF5, AF6, AF7,
	AF8, AF9, AF10, AF11, AF12, AF13, AF14, AF15,

};
/*AF pins from bit 8 to 11*/
/*
#define AF0		(0 << 8)	// Port x Pin 0
#define AF1		(1 << 8)
#define AF2		(2 << 8)
#define AF3		(3 << 8)
#define AF4		(4 << 8)
#define AF5		(5 << 8)
#define AF6		(6 << 8)
#define AF7		(7 << 8)
#define AF8		(8 << 8)
#define AF9		(9 << 8)
#define AF10	(10 << 8)
#define AF11	(11 << 8)
#define AF12	(12 << 8)
#define AF13	(13 << 8)
#define AF14	(14 << 8)
#define AF15	(15 << 8)
*/
/*Functions*/

void GPIOConfigurePin(GPIORegs *port, GPIOPin pins, int configuration);
void GPIOConfigureAltFunc(GPIORegs *port, GPIOPin pin, int altFunc);
void GPIOwritePins(GPIORegs *port, GPIOPin pins, GpioPinState pinState);
void GPIOTogglePin(GPIORegs *port, GPIOPin pins);


#endif // __GPIO_H__
