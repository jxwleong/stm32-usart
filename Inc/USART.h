#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>

typedef volatile uint32_t IoRegister;
typedef struct USARTRegs USARTRegs;
struct USARTRegs{
	IoRegister SR;
	IoRegister DR;
	IoRegister BRR;
	IoRegister CR1;
	IoRegister CR2;
	IoRegister CR3;
	IoRegister GTPR;
};

#define USART4  		(USARTRegs *)(0x40004C00)
#define USART5  		(USARTRegs *)(0x40005000)

// USART registers configuration settings
// STATUS register
// Parity error
#define USART_PE_DISABLE		0			// Disable parity error
#define USART_ENABLE			(1 << 0)	// Enable parity error

// Framing error
#define USART_FE_DISABLE		(0 << 1)	// Disable framing error
#define USART_FE_ENABLE			(1 << 1)	// Enable framing error

// Noise detected flag
#define USART_NF_DISABLE		(0 << 2)	// Disable noise flag
#define USART_NF_ENABLE			(1 << 2)	// Enable noise flag

// Overrun error
#define USART_ORE_NOT_DETECTED	(0 << 3)	// No overrun error detected
#define USART_ORE_DETECTED		(1 << 3)	// Overrun error detected

// IDLE line detected
#define USART_iDLE_NOT_DETECTED	(0 << 4)	// No idle line detected
#define USART_IDLE_DETECTED		(1 << 4)	// Idle line  detected

// Read data register not empty
#define USART_RXNE_NOT_READY		(0 << 5)	// Data is not received
#define USART_RXNE_READY 			(1 << 5)	// Received data is ready to be read

// Transmission complete
#define USART_TC_NOT_COMPLETE		(0 << 6)	// Transmission is complete
#define USART_TC_COMPLETE			(1 << 6)	// Transmission is not complete

// Transmit data register empty
#define USART_TXE_NOT_TRANSFERRED		(0 << 7)	// Data is not transferred to the shift register
#define USART_TXE_TRANSFERRED			(1 << 7)	// Data is transferred to the shift register

// LIN break detection flag
#define USART_LBD_NOT_DETECTED		(0 << 8)	// LIN Break not detected
#define USART_LBD_DETECTED			(1 << 8)	// LIN Break detected

// CTS flag
#define USART_CTS_NO_CHANGE			(0 << 9)
#define USART_CTS_CHANGE			(1 << 9)


// CONTROL register 1
// Send break
#define USART_SBK_DISABLE			0			// No break character is transmitted
#define USART_SBK_ENABLE			(1 << 0)	// Break character will be transmitted

// Receiver wakeup
#define USART_RWU_DISABLE			(0 << 1)	// Receiver in active mode
#define USART_RWU_ENABLE			(1 << 1)	// Receiver in mute mode

// Receiver enable
#define USART_RE_DISABLE			(0 << 2)	// Disable receiver
#define USART_RE_ENABLE				(1 << 2)	// Enable receiver

// Transmitter enable
#define USART_TE_DISABLE			(0 << 3)	// Disable transmitter
#define USART_TE_ENABLE				(1 << 3)	// Enable transmitter

// IDLE interrupt enable
#define USART_IDLEIE_DISABLE			(0 << 4)	// Disable IDLE interrupt
#define USART_IDLEIE_ENABLE				(1 << 4)	// Enable IDLE interrupt

// RXNE interrupt enable
#define USART_RXNEIE_DISABLE			(0 << 5)	// Disable RXNEIE interrupt
#define USART_RXNEIE_ENABLE				(1 << 5)	// Enable RXNEIE interrupt

// Transmission complete interrupt enable
#define USART_TCIE_DISABLE				(0 << 6)	// Disable TCIE interrupt
#define USART_TCIE_ENABLE				(1 << 6)	// Enable TCIE interrupt

// TXE Iinterrupt enable
#define USART_TXEIE_DISABLE				(0 << 7)	// Disable TXEIE interrupt
#define USART_TXEIE_ENABLE				(1 << 7)	// Enable TXEIE interrupt

// PE interrupt enable
#define USART_TXEIE_DISABLE				(0 << 8)	// Disable PEIE interrupt
#define USART_TXEIE_ENABLE				(1 << 8)	// Enable PEIE interrupt

// Parity selection
#define USART_PARITY_EVEN				(0 << 9)	// Parity select even
#define USART_PARITY_ODD				(1 << 9)	// Parity select odd

// Parity control enable
#define USART_PC_DISABLE				(0 << 10)	// Disable parity control
#define USART_PC_ENABLE					(1 << 10)	// Enable parity control

// Wakeup method
#define USART_WU_IDLEL					(0 << 11)	// Idle Line wake up method
#define USART_WU_AddressM				(1 << 11)	// Address mark wake up method

// Word length
#define USART_WORD_LENGTH_8BITS			(0 << 12)	// 1 Start bit, 8 Data bits, n Stop bit
#define USART_WORD_LENGTH_9BITS			(1 << 12)	// 1 Start bit, 9 Data bits, n Stop bit

// USART enable
#define USART_USART_DISABLE				(0 << 13)	// USART presacler and output disabled
#define USART_USART_ENABLE				(1 << 13)	// USART enabled

// Oversampling mode
#define USART_OVERSAMPLING_16			(0 << 15)	// oversampling by 16
#define USART_OVERSAMPLING_8			(1 << 15)	// oversampling by 8


// CONTROL register 2
// LIN break detection length
#define USART_LBDL_10BITS				(0 << 5)	// 10-bit break detection
#define USART_LBDL_11BITS				(1 << 5)	// 11-bit break detection

// LIN break detection interrupt enable
#define USART_LBD_INTERRUPT_DISABLE		(0 << 6)	// LIN break detection interrupt disabled
#define USART_LBD_INTERRUPT_ENABLE		(1 << 6)	// LIN break detection interrupt enabled

// Last bit clock pulse
#define USART_LBCL_DISABLE				(0 << 8)	// Clock pulse of last data bit is not output to CK pin
#define USART_LBCL_ENABLE				(1 << 8)	// Clock pulse of last data bit is output to CK pin

// Clock phase
#define USART_CPHA_FIRST_TRANSITION		(0 << 9)	// First clock transition is first data capture edge
#define USART_CPHA_SECOND_TRANSITION	(1 << 9)	// Second clock transition is first data capture edge

// Clock polarity
#define USART_CPOL_STEADY_LOW			(0 << 10)	// Steady low value on CK pin outside transmission window
#define USART_CPOL_STEADY_HIGH			(1 << 10)	// Steady high value on CK pin outside transmission window

// Clock enable
#define USART_CLK_DISABLE				(0 << 11)	// CK pin is disabled
#define USART_CLK_ENABLE				(1 << 11)	// CK pin is enabled

// STOP bits
#define USART_STOP_BIT_1				(0 << 12)	// 1 Stop bits
#define USART_STOP_BIT_0.5				(1 << 12)	// 0.5 Stop bits
#define USART_STOP_BIT_2					(2 << 12)	// 2 Stop bits
#define USART_STOP_BIT_1.5				(3 << 12)	// 1.5 Stop bits

// LIN mode enable
#define USART_LIN_DISABLE				(0 << 14)	// Lin mode disabled
#define USART_LIN_ENABLE				(1 << 14)	// Lin mode enabled

// CONTROL register 3
// Error interrupt enable
#define USART_EIE_DISABLE				0			// Error interrupt disabled
#define USART_EIE_ENABLE				(1 << 0)	// Error interrupt enabled

// IrDA mode enable
#define USART_IRDA_DISABLE				(0 << 1)	// IrDA disabled
#define USART_IRDA_ENABLE				(1 << 1)	// IrDA enabled

// IrDA low-power mode
#define USART_IRDA_NORMAL				(0 << 2)	// Normal mode
#define USART_IRDA_LOW_POWER				(1 << 2)	// Low power mode

// Half-duplex selection
#define USART_HD_NOT_SELECTED			(0 << 3)	// Half duplex mode is not selected
#define USART_HD_SELECTED				(1 << 3)	// Half duplex mode is selected

// Smartcard NACK enable
#define USART_NACK_DISABLE				(0 << 4)	// NACK transmission is case of parity error is disabled
#define USART_NACK_ENABLE				(1 << 4)	// NACK	transmission during parity error is enabled

// Smartcard mode enable
#define USART_SC_DISALE					(0 << 5)	// Smartcard mode disabled
#define USART_SC_ENALE					(1 << 5)	// Smartcard mode enabled

// DMA enable receiver
#define USART_DMAR_DISABLE				(0 << 6)	// DMA mode is disabled for reception
#define USART_DMAR_ENABLE				(1 << 6)	// DMA mode is enabled for reception

// DMA enable transmitter
#define USART_DMAT_DISABLE				(0 << 7)	// DMA mode is disabled for transmission
#define USART_DMAT_ENABLE				(1 << 7)	// DMA mode is enabled for transmission

#define USART_RTS_DISABLE				(0 << 8)	// RTS hardware flow control disabled
#define USART_RTS_ENABLE					(1 << 8)	// RTS hardware flow control enabled

#define USART_CTS_DISABLE				(0 << 9)	// CTS hardware flow control disabled
#define USART_CTS_ENABLE					(1 << 9)	// CTS hardware flow control enabled

#define USART_CTSI_DISABLE				(0 << 10)	// CTS interrupt disabled
#define USART_CTSI_ENABLE				(1 << 10)	// CTS interrupt enabled

#define USART_ONEBIT_DISABLE				(0 << 11)	// Three sample bit method
#define USART_ONEBIT_ENABLE				(1 << 11)	// One sample bit method



void usartConfigure(USARTRegs *usart, long long mode, int baudrate, int peripheralFreq){
	usart->CR1 = (uint32_t)(mode & 0xffff);
	usart->CR2 = (uint32_t)((mode >>16 )& 0xffff);
	usart->CR3 = (uint32_t)(mode >> 32);


}
#endif /* __USART_H__ */
