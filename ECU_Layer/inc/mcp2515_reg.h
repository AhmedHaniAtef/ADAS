/**
 * @file    mcp2515_reg.h
 * @author  Chat GPT
 * @brief   contains register map of the mcp2515 module
 * @date    2024-11-29
 */

#ifndef MCP2515_REG_H
#define MCP2515_REG_H

/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/

// Define MCP2515 SPI Commands
#define CAN_RESET       0xC0
#define CAN_READ        0x03
#define CAN_READ_RX     0x90
#define CAN_WRITE       0x02
#define CAN_WRITE_TX    0x40
#define CAN_RTS         0x80
#define CAN_READ_STATUS 0xA0
#define CAN_RX_STATUS   0xB0
#define CAN_BIT_MODIFY  0x05

// Define Specific Read RX and write TX
#define CAN_READ_RXB0SIDH   (0x90 | 0b00000000)
#define CAN_READ_RXB0D0     (0x90 | 0b00000010)
#define CAN_READ_RXB1SIDH   (0x90 | 0b00000100)
#define CAN_READ_RXB1D0     (0x90 | 0b00000110)

// Define MCP2515 Register Addresses
// Receive Filter 0

#define RXF0SIDH        0x00  
#define RXF0SIDL        0x01
#define RXF0EID8        0x02
#define RXF0EID0        0x03

// Receive Filter 1

#define RXF1SIDH        0x04  
#define RXF1SIDL        0x05
#define RXF1EID8        0x06
#define RXF1EID0        0x07

// Receive Filter 2

#define RXF2SIDH        0x08  
#define RXF2SIDL        0x09
#define RXF2EID8        0x0A
#define RXF2EID0        0x0B

// Receive Filter 3

#define RXF3SIDH        0x10  
#define RXF3SIDL        0x11
#define RXF3EID8        0x12
#define RXF3EID0        0x13

// Receive Filter 4

#define RXF4SIDH        0x14  
#define RXF4SIDL        0x15
#define RXF4EID8        0x16
#define RXF4EID0        0x17

// Receive Filter 5

#define RXF5SIDH        0x18  
#define RXF5SIDL        0x19
#define RXF5EID8        0x1A
#define RXF5EID0        0x1B

// Masks
// Receive Mask 0

#define RXM0SIDH        0x20  
#define RXM0SIDL        0x21
#define RXM0EID8        0x22
#define RXM0EID0        0x23

// Receive Mask 1

#define RXM1SIDH        0x24  
#define RXM1SIDL        0x25
#define RXM1EID8        0x26
#define RXM1EID0        0x27


// Transmit Buffers
// Transmit Buffer 0 Control

#define TXB0CTRL        0x30  
#define TXB0SIDH        0x31
#define TXB0SIDL        0x32
#define TXB0EID8        0x33
#define TXB0EID0        0x34
#define TXB0DLC         0x35
#define TXB0D0          0x36
#define TXB0D1          0x37
#define TXB0D2          0x38
#define TXB0D3          0x39
#define TXB0D4          0x3A
#define TXB0D5          0x3B
#define TXB0D6          0x3C
#define TXB0D7          0x3D

// Transmit Buffer 1 Control

#define TXB1CTRL        0x40  
#define TXB1SIDH        0x41
#define TXB1SIDL        0x42
#define TXB1EID8        0x43
#define TXB1EID0        0x44
#define TXB1DLC         0x45
#define TXB1D0          0x46
#define TXB1D1          0x47
#define TXB1D2          0x48
#define TXB1D3          0x49
#define TXB1D4          0x4A
#define TXB1D5          0x4B
#define TXB1D6          0x4C
#define TXB1D7          0x4D

// Transmit Buffer 2 Control

#define TXB2CTRL        0x50  
#define TXB2SIDH        0x51
#define TXB2SIDL        0x52
#define TXB2EID8        0x53
#define TXB2EID0        0x54
#define TXB2DLC         0x55
#define TXB2D0          0x56
#define TXB2D1          0x57
#define TXB2D2          0x58
#define TXB2D3          0x59
#define TXB2D4          0x5A
#define TXB2D5          0x5B
#define TXB2D6          0x5C
#define TXB2D7          0x5D


// Receive Buffers// Receive Buffer 0 Control

#define RXB0CTRL        0x60  
#define RXB0SIDH        0x61
#define RXB0SIDL        0x62
#define RXB0EID8        0x63
#define RXB0EID0        0x64
#define RXB0DLC         0x65
#define RXB0D0          0x66
#define RXB0D1          0x67
#define RXB0D2          0x68
#define RXB0D3          0x69
#define RXB0D4          0x6A
#define RXB0D5          0x6B
#define RXB0D6          0x6C
#define RXB0D7          0x6D

// Receive Buffer 1 Control

#define RXB1CTRL        0x70  
#define RXB1SIDH        0x71
#define RXB1SIDL        0x72
#define RXB1EID8        0x73
#define RXB1EID0        0x74
#define RXB1DLC         0x75
#define RXB1D0          0x76
#define RXB1D1          0x77
#define RXB1D2          0x78
#define RXB1D3          0x79
#define RXB1D4          0x7A
#define RXB1D5          0x7B
#define RXB1D6          0x7C
#define RXB1D7          0x7D

// Bit Timing Registers

#define CNF3            0x28
#define CNF2            0x29
#define CNF1            0x2A

// Interrupt Registers

#define CANINTE         0x2B
#define CANINTF         0x2C

// Error Registers

#define EFLG            0x2D

// Miscellaneous

#define CANSTAT         0x0E
#define CANCTRL         0x0F

// CANINTE - CAN Interrupt Enable Register (Address: 0x2B)

#define CANINTE_RX0IE   (1 << 0)  // Enable RX Buffer 0 Full Interrupt
#define CANINTE_RX1IE   (1 << 1)  // Enable RX Buffer 1 Full Interrupt
#define CANINTE_TX0IE   (1 << 2)  // Enable TX Buffer 0 Empty Interrupt
#define CANINTE_TX1IE   (1 << 3)  // Enable TX Buffer 1 Empty Interrupt
#define CANINTE_TX2IE   (1 << 4)  // Enable TX Buffer 2 Empty Interrupt
#define CANINTE_ERRIE   (1 << 5)  // Enable Error Interrupt
#define CANINTE_WAKIE   (1 << 6)  // Enable Wake-Up Interrupt
#define CANINTE_MERRE   (1 << 7)  // Enable Message Error Interrupt
#define CANINTE_MASK    0xFF      // Mask for all bits

// CANINTF - CAN Interrupt Flag Register (Address: 0x2C)

#define CANINTF_RX0IF   (1 << 0)  // RX Buffer 0 Full Interrupt Flag
#define CANINTF_RX1IF   (1 << 1)  // RX Buffer 1 Full Interrupt Flag
#define CANINTF_TX0IF   (1 << 2)  // TX Buffer 0 Empty Interrupt Flag
#define CANINTF_TX1IF   (1 << 3)  // TX Buffer 1 Empty Interrupt Flag
#define CANINTF_TX2IF   (1 << 4)  // TX Buffer 2 Empty Interrupt Flag
#define CANINTF_ERRIF   (1 << 5)  // Error Interrupt Flag
#define CANINTF_WAKIF   (1 << 6)  // Wake-Up Interrupt Flag
#define CANINTF_MERRF   (1 << 7)  // Message Error Flag
#define CANINTF_MASK    0xFF      // Mask for all bits

// EFLG - Error Flag Register (Address: 0x2D)

#define EFLG_RX1OVR     (1 << 7)  // RX Buffer 1 Overflow
#define EFLG_RX0OVR     (1 << 6)  // RX Buffer 0 Overflow
#define EFLG_TXBO       (1 << 5)  // Transmit Bus-Off
#define EFLG_TXEP       (1 << 4)  // Transmit Error-Passive
#define EFLG_RXEP       (1 << 3)  // Receive Error-Passive
#define EFLG_TXWAR      (1 << 2)  // Transmit Error Warning
#define EFLG_RXWAR      (1 << 1)  // Receive Error Warning
#define EFLG_EWARN      (1 << 0)  // Error Warning
#define EFLG_MASK       0xFF      // Mask for all bits

// CANCTRL - CAN Control Register (Address: 0x0F)

#define CANCTRL_REQOP2  (1 << 7)  // Request Operation Mode Bit 2
#define CANCTRL_REQOP1  (1 << 6)  // Request Operation Mode Bit 1
#define CANCTRL_REQOP0  (1 << 5)  // Request Operation Mode Bit 0
#define CANCTRL_ABAT    (1 << 4)  // Abort All Pending Transmissions
#define CANCTRL_OSM     (1 << 3)  // One-Shot Mode
#define CANCTRL_CLKEN   (1 << 2)  // CLKOUT Pin Enable
#define CANCTRL_CLKPRE1 (1 << 1)  // CLKOUT Pin Prescaler Bit 1
#define CANCTRL_CLKPRE0 (1 << 0)  // CLKOUT Pin Prescaler Bit 0
#define CANCTRL_MODE_MASK   0xE0  // Mask for Operation Mode Bits
#define CANCTRL_CLOCK_MASK  0x07  // Mask for Clock Prescaler Bits

// TXBnCTRL - Transmit Buffer n Control Registers (Addresses: 0x30, 0x40, 0x50)

#define TXBCTRL_ABTF    (1 << 6)  // Message Aborted Flag
#define TXBCTRL_MLOA    (1 << 5)  // Message Lost Arbitration
#define TXBCTRL_TXERR   (1 << 4)  // Transmission Error Detected
#define TXBCTRL_TXREQ   (1 << 3)  // Transmit Request
#define TXBCTRL_TXP1    (1 << 1)  // Transmit Priority Bit 1
#define TXBCTRL_TXP0    (1 << 0)  // Transmit Priority Bit 0
#define TXBCTRL_PRIORITY_MASK (TXBCTRL_TXP1 | TXBCTRL_TXP0) // Mask for Priority Bits

// RXB0CTRL - Receive Buffer 0 Control Register (Address: 0x60)

#define RXB0CTRL_RXM1   (1 << 6)  // Receive Buffer Mode Bit 1
#define RXB0CTRL_RXM0   (1 << 5)  // Receive Buffer Mode Bit 0
#define RXB0CTRL_RXRTR  (1 << 3)  // Remote Transfer Request Received
#define RXB0CTRL_BUKT   (1 << 2)  // Rollover Enable
#define RXB0CTRL_FILHIT (1 << 0)  // Filter Hit Indicator

// RXB1CTRL - Receive Buffer 1 Control Register (Address: 0x70)

#define RXB1CTRL_RXM1       (1 << 6)  // Receive Buffer Mode Bit 1
#define RXB1CTRL_RXM0       (1 << 5)  // Receive Buffer Mode Bit 0
#define RXB1CTRL_RXRTR      (1 << 3)  // Remote Transfer Request Received
#define RXB1CTRL_FILHIT2    (1 << 2) // Filter Hit Indicator 2
#define RXB1CTRL_FILHIT1    (1 << 1) // Filter Hit Indicator 1
#define RXB1CTRL_FILHIT0    (1 << 0) // Filter Hit Indicator 0

// Load TX Instructions

#define TX_Buffer_0_Start_at_TXB0SIDH  0x40
#define TX_Buffer_0_Start_at_TXB0D0    0x41
#define TX_Buffer_1_Start_at_TXB1SIDH  0x42
#define TX_Buffer_1_Start_at_TXB1D0    0x43
#define TX_Buffer_2_Start_at_TXB2SIDH  0x44
#define TX_Buffer_2_Start_at_TXB2D0    0x45

#endif // MCP2515_REG_H
