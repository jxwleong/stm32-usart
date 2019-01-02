#include "Timer.h"


Status_TypeDef TIM_BASE_START(TIM_Handle_Type *tim){
	tim->State = TIM_STATE_BUSY;
    tim->Instance->cr1 |= 1;
	tim->State = TIM_STATE_READY;

	return Ok;
}


void Timer2init(TIM_Handle_Type *timer){
	timer->Instance = timer2;
}

/**
 * Set the timer output compare channel to the specified mode*
 * Input :
 * 	mode		is the mode of the output compare. They can be anyone below:
 * 				OC_FROZEN;
 * 				OC_ACTIV_ON_MATCH
 * 				OC_INACTIV_ON_MATCH
 * 				OC_TOGGLE
 * 				OC_FORCE_ACTIV
 * 				OC_FORCE_INACTIV
 * 				OC_PWM1
 * 				OC_PWM2
 **/
void timerOcSetMode(TimerRegs *tim, int channel, int mode){
	int numOfShifts = 0;
	TimerRegister *ccmr = &(tim->ccmr1);

	if(((channel - 1) & 1) == 1)				// channel 1 = 00
		numOfShifts = 8;						// channel 2 = 01 \ if & with 1 = 1
	if(channel > 2){
		ccmr++;
	}
	*ccmr = mode << numOfShifts;
}

/**
 * Set the timer output compare channel to the specified mode*
 * Input :
 * 	mode		is the mode of the output compare. They can be anyone below:
 * 				OC_FROZEN;
 * 				OC_ACTIV_ON_MATCH
 * 				OC_INACTIV_ON_MATCH
 * 				OC_TOGGLE
 * 				OC_FORCE_ACTIV
 * 				OC_FORCE_INACTIV
 * 				OC_PWM1
 * 				OC_PWM2
 *	outPolarity is either 0 = normal or 1 = inverse
 **/
void timerInitOutputCompare(TimerRegs *tim, int channel, int mode, int outPolarity, int compareValue){
	timerOcSetMode(tim, channel, mode);
	TIM_SET_OC_VALUE(tim, channel, compareValue);
	TIM_SET_OC_POLARITY(tim, channel, outPolarity);

}


//void TIMER_CCxChannelCmd(GeneralTimerRegs* TIMx, uint32_t Channel, uint32_t ChannelState)
//{
//	 uint32_t tmp = 0U;


//	 tmp = TIM_CCER_CC1E << Channel;

	 /* Reset the CCxE Bit */
//	 TIMx->ccer &= ~tmp;

	 /* Set or reset the CCxE Bit */
//	 TIMx->ccer |= (uint32_t)(ChannelState << Channel);
//}


