# STM32 USART  
The purpose for this project is to learn how to configure the UART device on the STM32F429ZIT6 development board.  
There are several exercises to go through in order to understand the way UART device on this microcontroller works.  
The transmit and receive of data between the UART and PC is host through USB-to-serial converter module (CH340).

<br/>

## Table of Contents
1. [Requirement](#req)
2. [What is USART/ UART ?](#whatIs)
3. [Discussion](#disc)
    * [Different applications](#diffApp)
        * [1-Byte transmission through UART5](#ex1)
        * [Transmit “Hello World!” through UART5 to USB and display on TeraTerm](#ex2)
        * [Turn on/off LED by using command from USB (transmit) to UART5 (receive)](#ex3)
        * [Blinking LED 4 times per second using receive command from USB to 
           UART5(involve timer interrupt)](#ex4)
    * [Calculations](#calc)
        * [Baudrate](#baud)
        * [Timer2 Interrupt](#interruptTim2)  
4. [References](#refer)          
5. [Appendices](#appdix)   
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
### USART (Universal Synchronous/Asynchronous Receiver/Trasmitter)
![World length programming](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/USART_dataframe_from_RM0090_page_969.png)  
Figure x. Different data frame with different word length configuration  

&nbsp;  
![Configurable stop bits](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/USART_dataframe_from_RM0090_page_971.png)  
Figure x. Different data frames for different stop bits configuration from [1] page 971.

&nbsp;

### UART (Universal Asynchronous Receiver/Transmitter)
UART is a computing device used for asynchronous serial communication whereby the data format and transmission speeds are configurable. An UART is usually an dividual or part of an integrated circuit (IC) which used for serial communications over a peripheral or computer. Multiple UARTs peripherals can be found in most microcontroller today. Another device named USART supports both synchronous and asunchronous operations [4].

The transmission speeds of UART is baud rate. Baud rate is the rate of data transmission over a channel in bits. For example, baud rate of *9600* means that the peripherals is able to transmit a maximum of 9600 bits of data over the communication channel.

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
![Function to toggle LED](https://github.com/jason9829/stm32-usart/blob/master/resources/images/function%20snippet/commandlineBlink.JPG)    

<br/>  

#### <a name="ex4"></a> Blink the LED 4 times per second using command receive from USB.   
![Interrupt Service Routine for blinking the LED](https://github.com/jason9829/stm32-usart/blob/master/resources/images/function%20snippet/InterruptServiceRoutine.JPG)  

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
[2] Difference between USART and UART, GeeksforGeeks. Available at: https://www.geeksforgeeks.org/difference-between-usart-and-uart/ (viewed on 4 May 2020)  
[3] Universal synchronous and asynchronous receiver-transmitter, Wikipedia, 2020. Available at: https://en.wikipedia.org/wiki/Universal_synchronous_and_asynchronous_receiver-transmitter (viewed on 4 May 2020)    
[4] Universal asynchronous receiver-transmitter, Wikipedia, 2020. Available at: https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter (viewed on 4 May 2020)  
[5] BASICS OF UART COMMUNICATION, Circuit Basics. Available at: https://www.circuitbasics.com/basics-uart-communication/ (viewed on 4 May 2020)    



<br/>


## <a name="appdix"></a> Appendices   
### <a name="tTSetup"></a> Tera Term Setup    
1. Create new connection and select the correct COM port (USB-SERIAL CH340).
![Step1](https://github.com/jason9829/stm32-usart/blob/master/resources/images/tera%20term/TerTerm%20step1.png)  
&nbsp;  

2. Goto terminal setup and configure the `New-line` to LF (Line Feed). 
![Step2](https://github.com/jason9829/stm32-usart/blob/master/resources/images/tera%20term/teraTerm%20step3.jpg)  





