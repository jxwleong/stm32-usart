#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

typedef volatile uint32_t TimerRegister;
typedef struct TimerRegs TimerRegs;
struct TimerRegs{			//  Offset	// Description
	TimerRegister cr1;		//	0x00	// control register 1
	TimerRegister cr2;		//	0x04	// control register 2
	TimerRegister smcr;		//	0x08	// slave mode control register
	TimerRegister dier;		//	0x0c	// DMA/ Interrupt enable register
	TimerRegister sr;		//	0x10	// status register
	TimerRegister egr;		//	0x14	// event generation register
	TimerRegister ccmr1;	//	0x18	// capture/ compare mode register 1
	TimerRegister ccmr2;	//	0x1c	// capture/ compare mode register 2
	TimerRegister ccer;		//	0x20	// capture/ compare enable register
	TimerRegister cnt;		//	0x24	// counter
	TimerRegister psc;		//	0x28	// pre-scaler
	TimerRegister arr;		//	0x2c	// auto-reload register
	TimerRegister rev1;		//	0x30	// reversed space 1
	TimerRegister ccr1;		//	0x34	// capture/compare register 1
	TimerRegister ccr2;		//	0x38	// capture/compare register 2
	TimerRegister ccr3;		//	0x3c	// capture/compare register 3
	TimerRegister ccr4;		//	0x40	// capture/compare register 4
	TimerRegister rev2;		//	0x44	// reversed space 2
	TimerRegister dcr;		//	0x48	// DMA control register
	TimerRegister dmar;		//	0x4c	// DMA address for full transfer
	TimerRegister or;		//	0x50	// option register
};


// CR1
#define TIM_CEN			1

// EGR
#define TIM_UG			1

// CCER
#define OC_NORMAL_POLARITY 		0
#define OC_INVERSE_POLARITY 	1

// SR (refer to data sheet for status register [SR] page 633)
#define	TIM_UIF			(1<<0)
#define	TIM_CC1IF		(1<<1)		// shift to left 1 means 2
#define	TIM_CC2IF		(1<<2)		// shift to left 2 means 4 and so on...
#define	TIM_CC3IF		(1<<3)
#define	TIM_CC4IF		(1<<4)
#define	TIM_TIF			(1<<6)
#define	TIM_CC1OF		(1<<9)
#define	TIM_CC2OF		(1<<10)
#define	TIM_CC3OF		(1<<11)
#define	TIM_CC4OF		(1<<12)

// DIER
#define	TIM_UIE			1
#define	TIM_CC1IE		(1<<1)		// shift to left 1 means 2
#define	TIM_CC2IE		(1<<2)		// shift to left 2 means 4 and so on...
#define	TIM_CC3IE		(1<<3)
#define	TIM_CC4IE		(1<<4)
#define	TIM_TIE			(1<<6)
#define	TIM_CC1DE		(1<<9)
#define	TIM_CC2DE		(1<<10)
#define	TIM_CC3DE		(1<<11)
#define	TIM_CC4DE		(1<<12)
// CCMR
// Compare Mode (bit 6:4)
#define	OC_FROZEN				(0 << 4)
#define OC_ACTIV_ON_MATCH		(1 << 4)
#define OC_INACTIV_ON_MATCH		(2 << 4)
#define OC_TOGGLE				(3 << 4)
#define OC_FORCE_ACTIV			(4 << 4)
#define OC_FORCE_INACTIV		(5 << 4)
#define OC_PWM1					(6 << 4)
#define OC_PWM2					(7 << 4)

// bit 3
#define CC_PRELOAD_EN			(1 << 3)

// Compare capture 1/3 selection (bit 1:0)
#define CC_SELECT_OC			0
#define CC_SELECT_IC_DIRECT		1
#define CC_SELECT_IC_INDIRECT	2
#define CC_SELECT_IC_TRC		3


/*********************************Timer Macros********************************/
#define TIM_SET_COUNTER(tim, value)					(tim)->cnt = (value)
#define TIM_SET_OC_VALUE(tim, ch, value)			*(&((tim)->ccr1) + (ch) - 1) = (value)		// Start with 0 so ch -1 (user insert channel 1)
#define TIM_SET_OC_POLARITY(tim, ch, polarity)							\
							do{								 /*times 4 bcoz refer to page 649 bits*/			\
							(tim)->ccer &= (~2 << (((ch) - 1) * 4));			\
							(tim)->ccer |= (polarity << (((ch) - 1) * 4) + 1);	\
							}while(0)


#define TIM_CAPTURE_COMPARE_ENABLE(tim, ch)			(tim)->ccer |= 1 << (((ch) - 1) * 4);
#define TIM_CAPTURE_COMPARE_DISABLE(tim, ch)		(tim)->ccer &= ~(1 << (((ch) - 1) * 4));

// 	Clear SR register of Timer, whichFlag should be one of the following:
//	TIM_UIF				// update interrupt flag
//	TIM_CC1IF			// capture/ compare 1 interrupt flag
//	TIM_CC2IF			// capture/ compare 2 interrupt flag
//	TIM_CC3IF			// capture/ compare 3 interrupt flag
//	TIM_CC4IF			// capture/ compare 4 interrupt flag
//	TIM_TIF				// trigger interrupt flag
//	TIM_CC1OF			// capture/ compare 1 overcapture flag
//	TIM_CC2OF			// capture/ compare 2 overcapture flag
//	TIM_CC3OF			// capture/ compare 3 overcapture flag
//	TIM_CC4OF			// capture/ compare 4 overcapture flag
#define TIM_CLEAR_FLAG(tim, whichFlag)		(tim)->sr = ~(whichFlag)		// rc_0 no clear '&' bcoz set or reset has no effect
#define TIM_GET_FLAGS(tim, whichFlag)		(tim)->sr & (whichFlag)
#define TIM_IS_SET_FLAGS(tim, whichFlag)	(tim)->sr & (whichFlag)? 1:0
#define TIM_COUNTER_ENABLE(tim)				(tim)->cr1 |= TIM_CEN;
#define TIM_COUNTER_DISABLE(tim)			(tim)->cr1 &= ~TIM_CEN;

#define TIME_UPDATE_GEN						(tim)->egr |= TIM_EGR;

#define TIM_INTERRUPT_ENABLE(tim, bit)		(tim)->dier |= bit;

#define TIM_ENABLE_DMA(tim, bit)	\
		do{								\
			(tim)->dier &= ~(bit);		\
			(tim)->dier |= bit;		\
		}while(0)

typedef enum
{
  TIM_STATE_RESET             = 0x00U,    /*!< Peripheral not yet initialized or disabled  */
  TIM_STATE_READY             = 0x01U,    /*!< Peripheral Initialized and ready for use    */
  TIM_STATE_BUSY              = 0x02U,    /*!< An internal process is ongoing              */
  TIM_STATE_TIMEOUT           = 0x03U,    /*!< Timeout state                               */
  TIM_STATE_ERROR             = 0x04U     /*!< Reception process is ongoing                */
}TIM_State_TypeDef;

typedef enum
{
  Ok       = 0x00U,
  Error    = 0x01U,
  Busy     = 0x02U,
  Timeout  = 0x03U
} Status_TypeDef;


typedef struct TIM_Handle_Type TIM_Handle_Type;
struct TIM_Handle_Type{
	TimerRegs *Instance;
	volatile TIM_State_TypeDef State;
};


#define TIMER_CHANNEL_1                      0x00000000U
#define TIMER_CHANNEL_2                      0x00000004U
#define TIMER_CHANNEL_3                      0x00000008U
#define TIMER_CHANNEL_4                      0x0000000CU
#define TIMER_CHANNEL_ALL                    0x00000018U

#define TIMER_CCx_ENABLE                  	 0x00000001U
#define TIMER_CCx_DISABLE                    0x00000000U
#define TIMER_CCxN_ENABLE                  	 0x00000004U
#define TIMER_CCxN_DISABLE                 	 0x00000000U

#define timer1   ((TimerRegs*)0x40010000)
#define timer2   ((TimerRegs*)0x40000000)
#define timer3   ((TimerRegs*)0x40000400)
#define timer4   ((TimerRegs*)0x40000800)
#define timer5   ((TimerRegs*)0x40000c00)
#define timer6   ((TimerRegs*)0x40001000)
#define timer7   ((TimerRegs*)0x40001400)
#define timer8   ((TimerRegs*)0x40010400)
#define timer9   ((TimerRegs*)0x40014000)
#define timer10  ((TimerRegs*)0x40014400)
#define timer11  ((TimerRegs*)0x40014800)
#define timer12  ((TimerRegs*)0x40001800)
#define timer13  ((TimerRegs*)0x40001c00)
#define timer14  ((TimerRegs*)0x40002000)



Status_TypeDef TIM_BASE_START(TIM_Handle_Type *tim);
void Timer2init(TIM_Handle_Type *timer);
void timerInitOutputCompare(TimerRegs *tim, int channel, int mode, int outPolarity, int compareValue);

#endif // __TIMER_H__
