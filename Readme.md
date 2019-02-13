# Summary
1. [Basic Introduction](#intro)
2. [Objective](#obj)
3. [Requirement](#req)
4. [Different applications](#diffApp)

# <a name="obj"></a> Introduction
# <a name="intro"></a> Objective
The purpose for this project is to learn how to configure the UART device on the STM32F429ZIT6 MCU.  
There are several exercises to go through in order to understand the way UART device on this MCU works.  
The transmit and receive of data between the UART and PC is host through USB-to-serial converter module (CH340).

# <a name="req"></a> Requirement
The following software are required in order to perform this practical :
    1. STM32CubeMX
    2. SystemWorkBenchForSTM32
    3. TeraTerm
    
The UART5 is configured based on the settings as follow:
    1. Baudrate     : 115,200 bps
    2. Parity       : Odd parity
    3. Data size    : 8 bits data with parity bit
    4. Oversampling : Oversampling by 16
    5. Stop-bit size     : 1 bit
    6. Peripheral clock  : APB1 clock run at 45 MHz
    
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
