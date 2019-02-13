# Summary
1. [Basic Introduction](#intro)
    1. [Objective](#obj)
    2. [Requirement](#req)
    3. [Different applications](#diffApp)
2. [Calculations](#calc)

# <a name="obj"></a> Introduction
# <a name="intro"></a> Objective
The purpose for this project is to learn how to configure the UART device on the STM32F429ZIT6 MCU.  
There are several exercises to go through in order to understand the way UART device on this MCU works.  
The transmit and receive of data between the UART and PC is host through USB-to-serial converter module (CH340).

# <a name="req"></a> Requirement
STM32F429ZI microcontroller are used for this project.
The following software are required in order to perform this practical :
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

# <a name="diffApp"></a> Different application of UART for this project
1. [1-Byte transmission through UART5](#ex1)
2. [Transmit “Hello World!” through UART5 to USB and display on TeraTerm](#ex2)
3. [Turn on/off LED by using command from USB (transmit) to UART5 (receive)](#ex3)
4. [Blinking LED 4 times per second using receive command from USB to 
    UART5(involve timer interrupt)](#ex4)

# <a name="ex1"></a> 1-Byte transmission through UART5.
# <a name="ex2"></a> Transmission of "Hello Wrold!" through UART5.
# <a name="ex3"></a> Turn on/off LED by command receive from USB.
# <a name="ex4"></a> Blink the LED 4 times per second using command receive from USB.

# <a name="calc"></a> Calculations
1. [Baudrate](#baud)
2. [Timer2 Interrupt](#interruptTim2)

# <a name="baud"></a> Calculation of USART_DIV and Mantissa for configuration
Based in the equation found on the data sheet,

![alt text](https://github.com/jason9829/STM32_USART/blob/master/Practical2AssignmentPic/BaudRateEquation.JPG)

```md
Constant value:
BaudRate = 115200
OVER8 = 0   // OVER8 = 1 if oversampling by 8
Freq = 45MHz // Clock Frequency

USART_DIV = Fclk / [2* (2-OVER8)* BaudRate]
```

# <a name="interruptTim2"></a> Calculation of counter value to allow LED blink 4 timers per second


