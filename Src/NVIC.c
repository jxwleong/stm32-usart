#include "NVIC.h"


/**
 * */
void nvicEnableInterrupt(int interruptNumber){
	int n, bit;
	n = interruptNumber/32;
	bit = interruptNumber%32;
	nvic-> ISER[n] = 1 << bit;
}


void nvicDisableInterrupt(int interruptNumber){
	int n, bit;
	n = interruptNumber/32;
	bit = interruptNumber%32;
	nvic-> ICER[n] = 1 << bit ;
}


void nvicEnablePendingInterrupt(int interruptNumber){
	int n, bit;
	n = interruptNumber/32;
	bit = interruptNumber%32;
	nvic-> ISPR[n] = 1 << bit ;
}



void nvicDisablePendingInterrupt(int interruptNumber){
	int n, bit;
	n = interruptNumber/32;
	bit = interruptNumber%32;
	nvic-> ICPR[n] = 1 << bit ;
}



/**
 * Find out if interrupt is active.
 *
 * Return :
 * 	0			means not active
 * 	non-zero 	means active*/
int nvicIsInterruptActive(int interruptNumber){
	int n, bit;
	n = interruptNumber/32;
	bit = interruptNumber%32;

	return (nvic-> IABR[n]) << bit;
}
