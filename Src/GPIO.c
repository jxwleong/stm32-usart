#include "GPIO.h"


uint8_t bitPos[] = {
		-1,		// b'0000' (undefined)
		 0,		// b'0001'
		 1,		// b'0010'
		-2,		// b'0011' (undefined)
		 2,		// b'0100'
		-2,		// b'0101' (undefined)
		-2,		// b'0110' (undefined)
		-2,		// b'0011' (undefined)
		 3,		// b'1000'
		-2,		// b'1001' (undefined)
		-2,		// b'1010' (undefined)
		-2,		// b'1011' (undefined)
		-2,		// b'1100' (undefined)
		-2,		// b'1101' (undefined)
		-2,		// b'1110' (undefined)
		-2,		// b'1111' (undefined)

};
void GPIOConfigurePin(GPIORegs *port, GPIOPin pins, int configuration){
	uint32_t i, pinMask, tempMask;
	uint32_t mode;
	uint32_t driver;
	uint32_t outSpeed;
	uint32_t pullType;

	pinMask = 0x1;
	mode = configuration & 0x3;
	driver = (configuration & 0x4) >> 2;
	outSpeed = (configuration & 0x18) >> 3;
	pullType = (configuration & 0x60) >> 5;

	for(i = 0; i <16 ; i++){
		tempMask = pinMask & pins;
		if(tempMask){
			port->mode &= ~(3 << (i * 2));
			port->mode |= mode << (i * 2);

			port->driverType &= ~(1 << (i * 1)) ;
			port->driverType |= driver << (i * 1);;

			port->outSpeed &= ~(3 << (i * 2));
			port->outSpeed |= outSpeed  << (i * 2);

			port->pullType &= ~(3 << (i * 2));
			port->pullType |= pullType << (i * 2);

		}
		pinMask = pinMask << 1;
	}

}

void GPIOwritePins(GPIORegs *port, GPIOPin pins, GpioPinState pinState){
		if(pinState == PIN_SET){
			port-> outBits = pins;
		}
		else if(pinState == PIN_RESET){
			port-> outBits = pins << 16;
		}
}

void GPIOTogglePin(GPIORegs *port, GPIOPin pins){
	if(port->outData & pins){
		GPIOwritePins(port, pins, PIN_RESET);
	}
	else{
		GPIOwritePins(port, pins, PIN_SET);
	}
}

/*return GPIO pins in terms of 0, 1, 2 ,3 .... instead of enum*/
int GPIOPinVal(GPIOPin pin){
	switch(pin){
	case 	GPIOPin0	: return 0;	break;
	case	GPIOPin1	: return 1;	break;
	case	GPIOPin2	: return 2;	break;
	case	GPIOPin3	: return 3;	break;
	case	GPIOPin4	: return 4;	break;
	case	GPIOPin5	: return 5;	break;
	case	GPIOPin6	: return 6;	break;
	case	GPIOPin7	: return 7;	break;
	case	GPIOPin8	: return 8;	break;
	case	GPIOPin9	: return 9;	break;
	case	GPIOPin10	: return 10;break;
	case	GPIOPin11	: return 11;break;
	case	GPIOPin12	: return 12;break;
	case	GPIOPin13	: return 13;break;
	case	GPIOPin14	: return 14;break;
	case	GPIOPin15	: return 15;break;
	}
}

/*Configure alternate function of one ports*/
void GPIOConfigureAltFunc(GPIORegs *port, GPIOPin pin, int altFunc){
	IORegister *altReg = &(port->altFuncLow);
	int GpioPin;
	GpioPin = GPIOPinVal(pin);

	if(GpioPin > 7){
		altReg++;			// Move the altFuncHigh
		GpioPin -= 8;
	}

	(*altReg) = (altFunc << (4 * GpioPin));
	/*Shift to correct bits*/
}


