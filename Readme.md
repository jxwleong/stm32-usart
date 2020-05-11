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
   * [Flow of Transmit and Receive Data](#flowTRX)
   * [Alternate Functions](#altFunc)
   * [GPIO Registers](#gpioReg)
   * [USART Registers](#usartReg)
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
| Functionality      | USART can function as UART.                                                                                                             | UART cannot function as USART.                                                                                               |
| Complexity         | USART is more complex and can generate data in a form corresponding to many different standard protocols such as IrDA, LIN, Smart Card. | UART is simple and only slight differences from it's base format, such as the number of stop bits and even or odd parity. |  


<br/>

## <a name="importDS"></a> Important information from datasheet 
### <a name="flowTRX"></a> Flow of Transmit and Receive Data  
![TxRx Flow](https://github.com/jason9829/stm32-usart/blob/master/resources/images/sketch/Practical2_txrx.png)  

Figure x. The flow of transmit and receive data for USART/ UART in STM32F429ZI.
&nbsp;  

The meaning of the acronym can be found in Table 147 in [1]. The flow of receive and transmit data the as follow.

Firstly, the RXNE (read data register not empty) flag status is checked. If the flag indicates that the receive data register (RxReg) is empty, this means that either the data received are still shifting or there are not incoming data. When the RXNE flag shows the the RxReg is not empty (data is ready to be read), the data in RxReg will be copied into USART_DR (Data register). The data can be now readed by accessing the USART_DR register. This is the process of receiving data.

To transmit the data, the data must be loaded into USART_DR register described at previous paragraph. The TXE (transmit data register empty) flag is checked. Only if the transmit data register (TxReg) is empty, the data in USART_DR register will be loaded into the transmit data register. Then, the data in transmit data register will be shifted out.


<br/>  

### <a name="altFunc"></a> Alternate Functions  
There are many peripherals in the microcontroller which require I/O but the number of I/O are limited. To solve this problem, alternate functions were used. Alternate functions means that the same I/O pin can be connect to different peripherals using multiplexer. By changing the selector pins on the multiplexer, the I/O pin can be shared with multiple peripherals. USART in this MCU are shared with GPIO. Thus, GPIO registers are needed to configured to allow USART to access to the I/O pin. The circuitry of the I/O pin is shown at figure below. 

![GPIO pin circuit](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/gpioPinCircuit.PNG)
Figure x. The I/O pin circuitry from page 268 in [1].  

&nbsp;
![Alternate fuunction mux](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/altFuncMux.PNG)  
Figure x. The circuitry for different alternate function from page 273 in [1].  

&nbsp;  
![Alternate function mapping for UART5](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/GPIO_ALT_FUNC.png)  
Figure x. The alternate function pin for UART5 from page 78 in [2].
&nbsp;  

For this project, UART5 was used for transmit and receive the data. Based on the information shown at figure above, pin `PC12` is used for transmission of UART5 and pin `PD2` is used for receiving data.


<br/>   

### <a name="gpioReg"></a> GPIO Registers  
Only used GPIO registers will be discussed. 

1. **GPIO port mode register (GPIOx_MODER) (x = A..I/J/K)**  
    &nbsp;   
    This register is used to configure the I/O direction for each GPIO ports.  
    ![GPIOx_MODER](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/GPIOx_MODER.PNG)  
    Figure x. The register map for GPIOx_MODER from page 281 at [1].  
    &nbsp;   
    ```
    Bits 2y:2y+1 MODERy[1:0]: Port x configuration bits (y = 0..15)
	These bits are written by software to configure the I/O direction mode.
	00: Input (reset state)
	01: General purpose output mode
	10: Alternate function mode
	11: Analog mode
    ```  
    > For the use of USART5, GPIO port C (PC12) and D (PD2) was used based on Figure x. In other words, GPIOC_MODER's bit 25:24          (MODER12) are set       to binary 10 and GPIOD_MODER's bit 5:4 (MODER2) is also set to binary 10 for the user of transmitting and receiving data using I/Os.
       
    &nbsp;    
2. **GPIO port output type register (GPIOx_OTYPER) (x = A..I/J/K)**  
    &nbsp;  
    This register is used to configure the output type of each GPIO ports.  
    ![GPIOx_OTYPER](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/GPIOx_OTYPER.PNG) 
    Figure x. The register map for GPIOx_OTYPER from page 281 at [1].  
    &nbsp;   
    ```
    Bits 31:16 Reserved, must be kept at reset value.
	Bits 15:0 OTy: Port x configuration bits (y = 0..15)
	These bits are written by software to configure the output type of the I/O port.
	0: Output push-pull (reset state)
	1: Output open-drain
    ```
    
         
    &nbsp;  
3. **GPIO port output speed register (GPIOx_OSPEEDR) (x = A..I/J/K)**  
    &nbsp;  
    This register is used to configure the I/O output speed for each GPIO ports.  
    ![GPIOx_OSPEEDR](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/GPIOx_OSPEEDR.PNG)  
    Figure x. The register map for GPIOx_OSPEEDR from page 282 at [1].  
    &nbsp;  
    ```
    Bits 2y:2y+1 OSPEEDRy[1:0]: Port x configuration bits (y = 0..15)
	These bits are written by software to configure the I/O output speed.
	00: Low speed
	01: Medium speed
	10: High speed
	11: Very high speed
	Note: Refer to the product datasheets for the values of OSPEEDRy bits versus VDD
	range and external load.
    ```
    
    &nbsp; 
4. **GPIO port pull-up/pull-down register (GPIOx_PUPDR) (x = A..I/J/K)**  
    &nbsp;  
    This register is used to configure I/O to be pull-up or pull-down.  
    ![GPIOx_PUPDR](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/GPIOx_PUPDR.PNG)   
    Figure x. The register map for GPIOx_PUPDR from page 282 at [1]. 
    &nbsp;  
    ```
    Bits 2y:2y+1 PUPDRy[1:0]: Port x configuration bits (y = 0..15)
	These bits are written by software to configure the I/O pull-up or pull-down
	00: No pull-up, pull-down
	01: Pull-up
	10: Pull-down
	11: Reserved
    ```
        
    &nbsp; 
5. **GPIO alternate function low register (GPIOx_AFRL) (x = A..I/J/K)**  
    &nbsp;  
    This register is usedto configure the alternate function I/Os.
    ![GPIOx_AFRL](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/GPIOx_AFRL.PNG)   
    Figure x. The register map for GPIOx_AFRL from page 285 at [1]. 
    &nbsp;   
    ```
    Bits 31:0 AFRLy: Alternate function selection for port x bit y (y = 0..7)
	These bits are written by software to configure alternate function I/Os
	AFRLy selection:
	0000: AF0         1000: AF8
	0001: AF1         1001: AF9
	0010: AF2         1010: AF10
	0011: AF3         1011: AF11
	0100: AF4         1100: AF12
	0101: AF5         1101: AF13
	0110: AF6         1110: AF14
	0111: AF7         1111: AF15

    ```  
    > GPIOD_AFRL's AFRL2 was loaded with bit 1000 (AF8) so that the correct alternate function will be chosen using the multiplexer shown in Figure x.
    
    &nbsp;  
6. **GPIO alternate function high register (GPIOx_AFRH) (x = A..I/J)**  
    &nbsp;  
    This register is usedto configure the alternate function I/Os.
    ![GPIOx_AFRH](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/GPIOx_AFRH.PNG)   
    Figure x. The register map for GPIOx_AFRH from page 286 at [1]. 
    &nbsp;  
    ```
    Bits 31:0 AFRHy: Alternate function selection for port x bit y (y = 8..15)
	These bits are written by software to configure alternate function I/Os
	AFRHy selection:
	0000: AF0         1000: AF8
	0001: AF1         1001: AF9
	0010: AF2         1010: AF10
	0011: AF3         1011: AF11
	0100: AF4         1100: AF12
	0101: AF5         1101: AF13
	0110: AF6         1110: AF14
	0111: AF7         1111: AF15
    ```  
    > GPIOD_AFRH's AFRH12 was loaded with bit 1000 (AF8) so that the correct alternate function will be chosen using the multiplexer shown in Figure x.
   
    &nbsp;  



<br/>   

### <a name="usartReg"></a> USART Registers  
1. **Status register (USART_SR)**  
    &nbsp;  
    The register will used to show the status of the USART (flag or error).   
    ![USART_SR](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/USART_SR.PNG)  
    Figure x. The register map for USART_SR from page 1007 at [1].  
    &nbsp;  
    ```
    Bits 31:10 Reserved, must be kept at reset value
	Bit 9 CTS: CTS flag
		This bit is set by hardware when the CTS input toggles, if the CTSE bit is set. It is cleared by
		software (by writing it to 0). An interrupt is generated if CTSIE=1 in the USART_CR3
		register.
		0: No change occurred on the CTS status line
		1: A change occurred on the CTS status line
		Note: This bit is not available for UART4 & UART5.
	Bit 8 LBD: LIN break detection flag
		This bit is set by hardware when the LIN break is detected. It is cleared by software (by
		writing it to 0). An interrupt is generated if LBDIE = 1 in the USART_CR2 register.
		0: LIN Break not detected
		1: LIN break detected
		Note: An interrupt is generated when LBD=1 if LBDIE=1
	Bit 7 TXE: Transmit data register empty
		This bit is set by hardware when the content of the TDR register has been transferred into
		the shift register. An interrupt is generated if the TXEIE bit =1 in the USART_CR1 register. It
		is cleared by a write to the USART_DR register.
		0: Data is not transferred to the shift register
		1: Data is transferred to the shift register)
		Note: This bit is used during single buffer transmission.
	Bit 6 TC: Transmission complete
		This bit is set by hardware if the transmission of a frame containing data is complete and if
		TXE is set. An interrupt is generated if TCIE=1 in the USART_CR1 register. It is cleared by
		a software sequence (a read from the USART_SR register followed by a write to the
		USART_DR register). The TC bit can also be cleared by writing a '0' to it. This clearing
		sequence is recommended only for multibuffer communication.
		0: Transmission is not complete
		1: Transmission is complete
	Bit 5 RXNE: Read data register not empty
		This bit is set by hardware when the content of the RDR shift register has been transferred
		to the USART_DR register. An interrupt is generated if RXNEIE=1 in the USART_CR1
		register. It is cleared by a read to the USART_DR register. The RXNE flag can also be
		cleared by writing a zero to it. This clearing sequence is recommended only for multibuffer
		communication.
		0: Data is not received
		1: Received data is ready to be read.
	Bit 4 IDLE: IDLE line detected
		This bit is set by hardware when an Idle Line is detected. An interrupt is generated if the
		IDLEIE=1 in the USART_CR1 register. It is cleared by a software sequence (an read to the
		USART_SR register followed by a read to the USART_DR register).
		0: No Idle Line is detected
		1: Idle Line is detected
		Note: The IDLE bit will not be set again until the RXNE bit has been set itself (i.e. a new idle
		line occurs).
	Bit 3 ORE: Overrun error
		This bit is set by hardware when the word currently being received in the shift register is
		ready to be transferred into the RDR register while RXNE=1. An interrupt is generated if
		RXNEIE=1 in the USART_CR1 register. It is cleared by a software sequence (an read to the
		USART_SR register followed by a read to the USART_DR register).
		0: No Overrun error
		1: Overrun error is detected
		Note: When this bit is set, the RDR register content will not be lost but the shift register will be
		overwritten. An interrupt is generated on ORE flag in case of Multi Buffer
		communication if the EIE bit is set.
	Bit 2 NF: Noise detected flag
		This bit is set by hardware when noise is detected on a received frame. It is cleared by a
		software sequence (an read to the USART_SR register followed by a read to the
		USART_DR register).
		0: No noise is detected
		1: Noise is detected
		Note: This bit does not generate interrupt as it appears at the same time as the RXNE bit
		which itself generates an interrupting interrupt is generated on NF flag in case of Multi
		Buffer communication if the EIE bit is set.
		Note: When the line is noise-free, the NF flag can be disabled by programming the ONEBIT
		bit to 1 to increase the USART tolerance to deviations (Refer to Section 30.3.5: USART
		receiver tolerance to clock deviation on page 988).
	Bit 1 FE: Framing error
		This bit is set by hardware when a de-synchronization, excessive noise or a break character
		is detected. It is cleared by a software sequence (an read to the USART_SR register
		followed by a read to the USART_DR register).
		0: No Framing error is detected
		1: Framing error or break character is detected
		Note: This bit does not generate interrupt as it appears at the same time as the RXNE bit
		which itself generates an interrupt. If the word currently being transferred causes both
		frame error and overrun error, it will be transferred and only the ORE bit will be set.
		An interrupt is generated on FE flag in case of Multi Buffer communication if the EIE bit
		is set.
	Bit 0 PE: Parity error
		This bit is set by hardware when a parity error occurs in receiver mode. It is cleared by a
		software sequence (a read from the status register followed by a read or write access to the
		USART_DR data register). The software must wait for the RXNE flag to be set before
		clearing the PE bit.
		An interrupt is generated if PEIE = 1 in the USART_CR1 register.
		0: No parity error
		1: Parity error
    
    ```
    
    &nbsp;  
2. **Baud rate register (USART_BRR)**  
    &nbsp;  
    This register is used to configure the baud rate of USART.  
    ![USART_BRR](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/USART_BRR.PNG)  
    Figure x. The register map for USART_BRR from page 1010 at [1]. 
    &nbsp;  
    ```
    Bits 31:16 Reserved, must be kept at reset value
	Bits 15:4 DIV_Mantissa[11:0]: mantissa of USARTDIV
		These 12 bits define the mantissa of the USART Divider (USARTDIV)
	Bits 3:0 DIV_Fraction[3:0]: fraction of USARTDIV
		These 4 bits define the fraction of the USART Divider (USARTDIV). When OVER8=1, the
		DIV_Fraction3 bit is not considered and must be kept cleared.
    ```
    
    
    &nbsp; 
3. **Control register 1 (USART_CR1)**  
    &nbsp;  
    This register is used to configure the USART.  
    ![USART_CR1](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/USART_CR1.PNG)  
    Figure x. The register map for USART_BRR from page 1010 at [1]. 
    &nbsp;  
    ```
    Bits 31:16 Reserved, must be kept at reset value
	Bit 15 OVER8: Oversampling mode
		0: oversampling by 16
		1: oversampling by 8
		Note: Oversampling by 8 is not available in the Smartcard, IrDA and LIN modes: when
		SCEN=1,IREN=1 or LINEN=1 then OVER8 is forced to ‘0 by hardware.
	Bit 14 Reserved, must be kept at reset value
	Bit 13 UE: USART enable
		When this bit is cleared, the USART prescalers and outputs are stopped and the end of the
		current byte transfer in order to reduce power consumption. This bit is set and cleared by
		software.
		0: USART prescaler and outputs disabled
		1: USART enabled
	Bit 12 M: Word length
		This bit determines the word length. It is set or cleared by software.
		0: 1 Start bit, 8 Data bits, n Stop bit
		1: 1 Start bit, 9 Data bits, n Stop bit
		Note: The M bit must not be modified during a data transfer (both transmission and reception)
	Bit 11 WAKE: Wakeup method
		This bit determines the USART wakeup method, it is set or cleared by software.
		0: Idle Line
		1: Address Mark
	Bit 10 PCE: Parity control enable
		This bit selects the hardware parity control (generation and detection). When the parity
		control is enabled, the computed parity is inserted at the MSB position (9th bit if M=1; 8th bit
		if M=0) and parity is checked on the received data. This bit is set and cleared by software.
		Once it is set, PCE is active after the current byte (in reception and in transmission).
		0: Parity control disabled
		1: Parity control enabled
	Bit 9 PS: Parity selection
		This bit selects the odd or even parity when the parity generation/detection is enabled (PCE
		bit set). It is set and cleared by software. The parity will be selected after the current byte.
		0: Even parity
		1: Odd parity
	Bit 8 PEIE: PE interrupt enable
		This bit is set and cleared by software.
		0: Interrupt is inhibited
		1: An USART interrupt is generated whenever PE=1 in the USART_SR register
	Bit 7 TXEIE: TXE interrupt enable
		This bit is set and cleared by software.
		0: Interrupt is inhibited
		1: An USART interrupt is generated whenever TXE=1 in the USART_SR register
	Bit 6 TCIE: Transmission complete interrupt enable
		This bit is set and cleared by software.
		0: Interrupt is inhibited
		1: An USART interrupt is generated whenever TC=1 in the USART_SR register
	Bit 5 RXNEIE: RXNE interrupt enable
		This bit is set and cleared by software.
		0: Interrupt is inhibited
		1: An USART interrupt is generated whenever ORE=1 or RXNE=1 in the USART_SR
		register
	Bit 4 IDLEIE: IDLE interrupt enable
		This bit is set and cleared by software.
		0: Interrupt is inhibited
		1: An USART interrupt is generated whenever IDLE=1 in the USART_SR register
	Bit 3 TE: Transmitter enable
		This bit enables the transmitter. It is set and cleared by software.
		0: Transmitter is disabled
		1: Transmitter is enabled
		Note: During transmission, a “0” pulse on the TE bit (“0” followed by “1”) sends a preamble
		(idle line) after the current word, except in smartcard mode.
		When TE is set, there is a 1 bit-time delay before the transmission starts.
	Bit 2 RE: Receiver enable
		This bit enables the receiver. It is set and cleared by software.
		0: Receiver is disabled
		1: Receiver is enabled and begins searching for a start bit
	Bit 1 RWU: Receiver wakeup
		This bit determines if the USART is in mute mode or not. It is set and cleared by software
		and can be cleared by hardware when a wakeup sequence is recognized.
		0: Receiver in active mode
		1: Receiver in mute mode
		Note: Before selecting Mute mode (by setting the RWU bit) the USART must first receive a
		data byte, otherwise it cannot function in Mute mode with wakeup by Idle line detection.
		In Address Mark Detection wakeup configuration (WAKE bit=1) the RWU bit cannot be
		modified by software while the RXNE bit is set.
	Bit 0 SBK: Send break
		This bit set is used to send break characters. It can be set and cleared by software. It should
		be set by software, and will be reset by hardware during the stop bit of break.
		0: No break character is transmitted
		1: Break character will be transmitted	
    ```
    
    &nbsp;   
4. **Control register 2 (USART_CR2)**  
    &nbsp;  
    This register is used to configure the USART.  
    ![USART_CR2](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/USART_CR2.PNG)  
    Figure x. The register map for USART_BRR from page 1013 at [1]. 
    &nbsp;  
    ```
    Bits 31:15 Reserved, must be kept at reset value
	Bit 14 LINEN: LIN mode enable
		This bit is set and cleared by software.
		0: LIN mode disabled
		1: LIN mode enabled
		The LIN mode enables the capability to send LIN Synch Breaks (13 low bits) using the SBK bit in
		the USART_CR1 register, and to detect LIN Sync breaks.
	Bits 13:12 STOP: STOP bits
		These bits are used for programming the stop bits.
		00: 1 Stop bit
		01: 0.5 Stop bit
		10: 2 Stop bits
		11: 1.5 Stop bit
		Note: The 0.5 Stop bit and 1.5 Stop bit are not available for UART4 & UART5.
	Bit 11 CLKEN: Clock enable
		This bit allows the user to enable the CK pin.
		0: CK pin disabled
		1: CK pin enabled
		This bit is not available for UART4 & UART5.
	Bit 10 CPOL: Clock polarity
		This bit allows the user to select the polarity of the clock output on the CK pin in synchronous mode.
		It works in conjunction with the CPHA bit to produce the desired clock/data relationship
		0: Steady low value on CK pin outside transmission window.
		1: Steady high value on CK pin outside transmission window.
		This bit is not available for UART4 & UART5.
	Bit 9 CPHA: Clock phase
		This bit allows the user to select the phase of the clock output on the CK pin in synchronous mode.
		It works in conjunction with the CPOL bit to produce the desired clock/data relationship (see figures
		308 to 309)
		0: The first clock transition is the first data capture edge
		1: The second clock transition is the first data capture edge
		Note: This bit is not available for UART4 & UART5.
	Bit 8 LBCL: Last bit clock pulse
		This bit allows the user to select whether the clock pulse associated with the last data bit
		transmitted (MSB) has to be output on the CK pin in synchronous mode.
		0: The clock pulse of the last data bit is not output to the CK pin
		1: The clock pulse of the last data bit is output to the CK pin
		Note: 1: The last bit is the 8th or 9th data bit transmitted depending on the 8 or 9 bit format selected
		by the M bit in the USART_CR1 register.
		2: This bit is not available for UART4 & UART5.
	Bit 7 Reserved, must be kept at reset value
	Bit 6 LBDIE: LIN break detection interrupt enable
		Break interrupt mask (break detection using break delimiter).
		0: Interrupt is inhibited
		1: An interrupt is generated whenever LBD=1 in the USART_SR register
	Bit 5 LBDL: lin break detection length
		This bit is for selection between 11 bit or 10 bit break detection.
		0: 10-bit break detection
		1: 11-bit break detection
	Bit 4 Reserved, must be kept at reset value
	Bits 3:0 ADD[3:0]: Address of the USART node
		This bit-field gives the address of the USART node.
		This is used in multiprocessor communication during mute mode, for wake up with address mark
		detection.
	Note: These 3 bits (CPOL, CPHA, LBCL) should not be written while the transmitter is enabled.	
    
    ```
    
    &nbsp;     
5. **Control register 3 (USART_CR3)**  
    &nbsp;  
    This register is used to configure the USART.  
    ![USART_CR3](https://github.com/jason9829/stm32-usart/blob/master/resources/images/reference%20manual/USART_CR3.PNG)  
    Figure x. The register map for USART_BRR from page 1014 at [1]. 
    &nbsp; 
    ```
    Bits 31:12 Reserved, must be kept at reset value
	Bit 11 ONEBIT: One sample bit method enable
		This bit allows the user to select the sample method. When the one sample bit method is
		selected the noise detection flag (NF) is disabled.
		0: Three sample bit method
		1: One sample bit method
		Note: The ONEBIT feature applies only to data bits. It does not apply to START bit.
	Bit 10 CTSIE: CTS interrupt enable
		0: Interrupt is inhibited
		1: An interrupt is generated whenever CTS=1 in the USART_SR register
		Note: This bit is not available for UART4 & UART5.
	Bit 9 CTSE: CTS enable
		0: CTS hardware flow control disabled
		1: CTS mode enabled, data is only transmitted when the CTS input is asserted (tied to 0). If
		the CTS input is deasserted while a data is being transmitted, then the transmission is
		completed before stopping. If a data is written into the data register while CTS is
		deasserted, the transmission is postponed until CTS is asserted.
		Note: This bit is not available for UART4 & UART5.
	Bit 8 RTSE: RTS enable
		0: RTS hardware flow control disabled
		1: RTS interrupt enabled, data is only requested when there is space in the receive buffer.
		The transmission of data is expected to cease after the current character has been
		transmitted. The RTS output is asserted (tied to 0) when a data can be received.
		Note: This bit is not available for UART4 & UART5.
	Bit 7 DMAT: DMA enable transmitter
		This bit is set/reset by software
		1: DMA mode is enabled for transmission.
		0: DMA mode is disabled for transmission.
	Bit 6 DMAR: DMA enable receiver
		This bit is set/reset by software
		1: DMA mode is enabled for reception
		0: DMA mode is disabled for reception
	Bit 5 SCEN: Smartcard mode enable
		This bit is used for enabling Smartcard mode.
		0: Smartcard Mode disabled
		1: Smartcard Mode enabled
	Note: This bit is not available for UART4 & UART5.
		Bit 4 NACK: Smartcard NACK enable
		0: NACK transmission in case of parity error is disabled
		1: NACK transmission during parity error is enabled
		Note: This bit is not available for UART4 & UART5.
	Bit 3 HDSEL: Half-duplex selection
		Selection of Single-wire Half-duplex mode
		0: Half duplex mode is not selected
		1: Half duplex mode is selected
	Bit 2 IRLP: IrDA low-power
		This bit is used for selecting between normal and low-power IrDA modes
		0: Normal mode
		1: Low-power mode
	Bit 1 IREN: IrDA mode enable
		This bit is set and cleared by software.
		0: IrDA disabled
		1: IrDA enabled
	Bit 0 EIE: Error interrupt enable
		Error Interrupt Enable Bit is required to enable interrupt generation in case of a framing
		error, overrun error or noise flag (FE=1 or ORE=1 or NF=1 in the USART_SR register) in
		case of Multi Buffer Communication (DMAR=1 in the USART_CR3 register).
		0: Interrupt is inhibited
		1: An interrupt is generated whenever DMAR=1 in the USART_CR3 register and FE=1 or
		ORE=1 or NF=1 in the USART_SR register.
	
    ```
    
    &nbsp; 

<br/>  

## <a name="disc"></a> Discussion
### <a name="diffApp"></a> Different application of UART for this project  


#### <a name="ex1"></a> 1-Byte transmission through UART5.  
![Result of oscilloscope no parity](https://github.com/jason9829/stm32-usart/blob/master/resources/images/waveform/0_parity.JPG)  
Figure x. Result of sending decimal `86` via UART5 without parity.    

&nbsp;    



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





