# STM32 USART  
The purpose for this project is to learn how to configure the UART device on the STM32F429ZIT6 development board.  
There are several exercises to go through in order to understand the way UART device on this microcontroller works.  
The transmit and receive of data between the UART and PC is host through USB-to-serial converter module (CH340).

<br/>

## Table of Contents
1. [Requirement](#req)
2. [What is USART/ UART ?](#whatIs)
   * [USART](#usart)
   * [UART](#uart)
   * [Differences between USART and UART](#diffUU)
3. [Important information from datasheet](#importDS)
4. [Discussion](#disc)
    * [Different applications](#diffApp)
        * [1-Byte transmission through UART5](#ex1)
        * [Transmit “Hello World!” through UART5 to USB and display on TeraTerm](#ex2)
        * [Turn on/off LED by using command from USB (transmit) to UART5 (receive)](#ex3)
        * [Blinking LED 4 times per second using receive command from USB to 
           UART5(involve timer interrupt)](#ex4)
    * [Calculations](#calc)
        * [Baudrate](#baud)
        * [Timer2 Interrupt](#interruptTim2)  
5. [References](#refer)          
6. [Appendices](#appdix)   
   * [Tera Term Setup](#tTSetup)  

<br/>



## <a name="req"></a> Requirement
STM32F429ZI microcontroller are used for this project.
The following software are required in order to perform this practical:
```md
    1. STM32CubeMX
    2. SystemWorkBenchForSTM32
    3. TeraTerm
```   
    
The UART5 is configured based on the settings as follow:
```md
    1. Baudrate     : 115,200 bps
    2. Parity       : Odd parity
    3. Data size    : 8 bits data with parity bit
    4. Oversampling : Oversampling by 16
    5. Stop-bit size     : 1 bit
    6. Peripheral clock  : APB1 clock run at 45 MHz
```

The connection between the USB to UART Dongle and the MCU are shown at figure below.
![Connection between USB and UART5](https://github.com/jason9829/stm32-usart/blob/master/resources/images/sketch/connectionBetweenUSBandUART.png)  


<br/>  

## <a name="whatIs"></a> What is USART/ UART ?  
### <a name="usart"></a> USART (Universal Synchronous/Asynchronous Receiver/Trasmitter)  
USART is a serial communication device that can support both synchronous and asynchronous communications. USART comes with an advantage with the additional support of asynchronous communications. USART and UART are both compatible in asynchronous mode which mean that both of the devices can be communicate with each other if the USART is set to transmit data in asynchronous mode. In other words, USART can function as an UART device (if set to asynchronous mode) while UART cannot function act an USART deivce. 

When USART is set up to run in synchronous mode, the sender will generate a clock so that the receiving peripheral can recover from the data stream without knowing the baud rate ahead of time [6] which mean that the sender and receiver share a common clock. Unlike in asynchronous mode, synchronous mode allows the transmission of a block of data (instead of 1 byte at a time) with just one start and stop bit which reduce the overhead issues.

The explanation of asynchronous communication will be located at UART section.



&nbsp;  

### <a name="uart"></a> UART (Universal Asynchronous Receiver/Transmitter)
UART is a computing device used for asynchronous serial communication whereby the data frame and transmission speeds are configurable. An UART is usually an dividual or part of an integrated circuit (IC) which used for serial communications over a peripheral or computer. Multiple UARTs peripherals can be found in most microcontroller today. Another device named USART supports both synchronous and asunchronous operations [5].

![World length programming](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/USART_dataframe_from_RM0090_page_969.png)  
Figure x. Different data frame with different word length configuration  

```
Description of control register 1 to change the data size. (RM0090 page 1011)
Bit 12 M: Word length
This bit determines the word length. It is set or cleared by software.
0: 1 Start bit, 8 Data bits, n Stop bit
1: 1 Start bit, 9 Data bits, n Stop bit
Note: The M bit must not be modified during a data transfer (both transmission and reception)

Description of control register 2 to change the stop bit size. (RM0090 page 1013)
Bits 13:12 STOP: STOP bits
These bits are used for programming the stop bits.
00: 1 Stop bit
01: 0.5 Stop bit
10: 2 Stop bits
11: 1.5 Stop bit
Note: The 0.5 Stop bit and 1.5 Stop bit are not available for UART4 & UART5.
```
&nbsp;  

Figure above shows the different frame structure with different size of data packet. Unlike USART in synchronous mode, the allowed data to be transmitted is either 8 or 9-bit within one start and n stop bit (Based on snippet above).

The changing of data bits can done by writing 0 or 1 to bit 12 of control register 1. Moreover, the stop bit size can be change for both USART and UART. Use of bigger stop bits provides the receiver extra processing time to handle the data.


The transmission speeds of UART is baud rate. Baud rate is the rate of data transmission over a channel in bits. For example, baud rate of *9600* means that the peripherals is able to transmit a maximum of 9600 bits of data over the communication channel. The baud rate can be changed by program the baud rate register (USART_BRR) by using the value calculated by using [this method](#baud).


&nbsp;    

### <a name="diffUU"></a> Differences between USART and UART
Refer from [3] and [8].

| Differences        | USART                                                                                                                                   | UART                                                                                                                      |
|--------------------|-----------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------|
| Baud rate          | Receiver don't have to know the baud rate, it will derived it from the clock signal  and data line from sender.                         | Receiver need to knows the baud rate because there is no incoming clock signal from sender.                               |
| Data line required | Require data and lock line.                                                                                                             | Only require data line.                                                                                                   |
| Data form          | Data is transmitted in block form.                                                                                                      | Data is transmitted each byte at a time.                                                                                  |
| Transmission speed | Transmission speed is faster.                                                                                                           | Slower transmission speed.                                                                                                |
| Functionality      | USART can function as UART.                                                                                                             | UART can function as USART.                                                                                               |
| Complexity         | USART is more complex and can generate data in a form corresponding to many different standard protocols such as IrDA, LIN, Smart Card. | UART is simple and only slight differences from it's base format, such as the number of stop bits and even or odd parity. |  


<br/>

## <a name="importDS"></a> Important information from datasheet 

<br/>  

## <a name="disc"></a> Discussion
### <a name="diffApp"></a> Different application of UART for this project  


#### <a name="ex1"></a> 1-Byte transmission through UART5.  
![Result of oscilloscope no parity](https://github.com/jason9829/stm32-usart/blob/master/resources/images/waveform/0_parity.JPG)  
Figure x. Result of sending decimal `86` via UART5 without parity.    

<br/>  

![Result of oscilloscope parity](https://github.com/jason9829/stm32-usart/blob/master/resources/images/waveform/oneParity.png)  
Figure x. Result of sending decimal `96` via UART5 with parity.    

<br/>

#### <a name="ex2"></a> Transmission of "Hello World!" through UART5.  
![Result of sending Hello World!](https://github.com/jason9829/stm32-usart/blob/master/resources/images/tera%20term/helloWORLD.JPG)   
Figure x. Result of transmitting "Hello World!" using UART5.

<br/>

#### <a name="ex3"></a> Turn on/off LED by command receive from USB.  
```c
void commandLineOperation(GPIORegs *port, GPIOPin pins, char *commandStr){
	if(!(strcasecmp(commandStr, "turn on"))){
		GPIOwritePins(port, pins, PIN_SET);
	}
	else if(!(strcasecmp(commandStr, "turn off"))){
		GPIOwritePins(port, pins, PIN_RESET);
	}
	else if(!(strcasecmp(commandStr, "blink"))){
		configureTimer2Interrupt();
	}
}
```  

<br/>  

#### <a name="ex4"></a> Blink the LED 4 times per second using command receive from USB.   

```c
static int i;
void TIM2_IRQHandler(void){
	if(i < 8){
		GPIOTogglePin(gpioG, GPIOPin13);
		i++;
	}
	else
		i = 0;

	TIM_CLEAR_FLAG(timer2,TIM_UIF);
}
```  


<br/>

## <a name="calc"></a> Calculations  


### <a name="baud"></a> Calculation of USART_DIV and Mantissa for configuration
Based in the equation found on the data sheet,

![alt text](https://github.com/jason9829/stm32-usart/blob/master/resources/images/calculations/BaudRateEquation.JPG)

```md
Constant value:
BaudRate = 115200
OVER8 = 0   // OVER8 = 1 if oversampling by 8
Freq = 45MHz // Clock Frequency

USART_DIV = Fclk / [2* (2-OVER8)* BaudRate]
USART_DIV = 45000000 / [2* (2- 0)* 115200]
USART_DIV = 24.41
Mantissa = 24

Fractional part = 0.41* 16(shift left by 4-bits(*2^4) for 16-bits oversampling, 8-bits oversampling shift left by 3-bits(*2^3)) 
Fractional part = 6.56 = 6

```

### <a name="interruptTim2"></a> Calculation of counter value to allow LED blink 4 timers per second  

```md
Blink 4 times per second,
One cycle of on and off equals to one blink.
Hence, the LED will be toggle on/off for 8 times in a second.

Blink period = 1 / 8 = 125ms

Since timer 2 is located at APB1 Bus, the clock frequency of timer2 is 90MHz.

timer2 period = 1 / [90M / (pre-scaler + 1)]    ; pre-scaler is selected as 29 to ease calculation.
              = 1 / (90M / 30)
              = 3.3333 x 10^-7 s
              
timer2 counter value = Blink period (desired) / timer2 period
                     = 125ms / 3.3333 x 10^-7 s
                     = 375004
                     
# 375004 will be loaded into ARR of timer2, when the counter reaches this value. It will generate an interrupt.

```

<br/>  

## <a name="refer"></a> References  
[1] [[RM0090] STM32F429 Reference Manual](https://github.com/jason9829/stm32-usart/blob/master/resources/pdf/%5BRM0090%5D%20STM32F429%20Reference%20Manual.pdf)  
[2] [[DS9405] Product Specifications](https://github.com/jason9829/stm32-usart/blob/master/resources/pdf/%5BDS9405%5D%20Product%20Specifications.pdf)   
[3] Difference between USART and UART, GeeksforGeeks. Available at: https://www.geeksforgeeks.org/difference-between-usart-and-uart/ (viewed on 4 May 2020)  
[4] Universal synchronous and asynchronous receiver-transmitter, Wikipedia, 2020. Available at: https://en.wikipedia.org/wiki/Universal_synchronous_and_asynchronous_receiver-transmitter (viewed on 4 May 2020)    
[5] Universal asynchronous receiver-transmitter, Wikipedia, 2020. Available at: https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter (viewed on 4 May 2020)  
[6] BASICS OF UART COMMUNICATION, Circuit Basics. Available at: https://www.circuitbasics.com/basics-uart-communication/ (viewed on 4 May 2020)   
[7] USART vs UART: Know the difference, Beningo, 2015. Available at: https://www.edn.com/usart-vs-uart-know-the-difference/ (viewed on 4 May 2020)   
[8] Difference between UART and USART (UART vs USART), Amlendra. Available at: https://aticleworld.com/difference-between-uart-and-usart/ (viewed on 4 May 2020)   


<br/>


## <a name="appdix"></a> Appendices   
### <a name="tTSetup"></a> Tera Term Setup    
1. Create new connection and select the correct COM port (USB-SERIAL CH340).
![Step1](https://github.com/jason9829/stm32-usart/blob/master/resources/images/tera%20term/TerTerm%20step1.png)  
&nbsp;  

2. Goto terminal setup and configure the `New-line` to LF (Line Feed). 
![Step2](https://github.com/jason9829/stm32-usart/blob/master/resources/images/tera%20term/teraTerm%20step3.jpg)  





