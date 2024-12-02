/**
 * @file    mcp2515.h
 * @author  Ahmed Hani & Mohamed Kamal & Mohamed Saad
 * @brief   contains interfaces to communicate through external MCP2515 CAN module through SPI
 * @date    2024-11-29 
 * @note    
 */

#ifndef MCP2515_H
#define MCP2515_H

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "stm32f4xx_hal.h"
#include <stm32f401xc.h>
#include "ecu_std.h"
#include "../../lib/inc/list.h"
#include "../../lib/inc/queue.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/

#define CAN_STANDARD_FRAME  (0)
#define CAN_REMOTE_FRAME    (1)



/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/

/**
 * @brief it converts from 16-bit to 8-bit high and 8-bit low with any method of access
 * 
 */
typedef union
{
    struct ByteOrder
    {
        uint8_t High;
        uint8_t Low;
    }ByteOrder;
    uint16_t Full;
}Reg_16bit_t;

/**
 * @brief contains all values can be stored in masks and filters registers
 * @param RXM0 mask of buffer 0 
 * @param RXF0 to RXM1 filters of buffer 0
 * @param RXM1 mask of buffer 1
 * @param RXF2 to  RXM5 filters of buffer 1
 */
typedef struct 
{
    Reg_16bit_t RXM0;
    Reg_16bit_t RXM1;
    Reg_16bit_t RXF0;
    Reg_16bit_t RXF1;
    Reg_16bit_t RXF2;
    Reg_16bit_t RXF3;
    Reg_16bit_t RXF4;
    Reg_16bit_t RXF5;
}can_filters_t;

/**
 * @brief this data type represents the methods to identify the CAN and how to communicate with it
 * @param SPI pointer to the used SPI module to communicate
 * @param RxQueue pointer to the queue which will store the received messages until it served
 * @param Filters pointer to can_filters_t data type used when filter is needed in the can moudle controller
 * @param EXTI_Line pointer to object related external interrupt to -INT- pin in mcp2515 module contains 
 *                  line and callback @defgroup EXTI_Line  @file stm32f4xx_hal_exti.h
 * @param ChipSelectPort pointer to the port of the chip select pin
 * @param ChipSelectPin the pin selected for chip select @defgroup GPIO_pins_define
 * @param UserCallback pointer to callback function defined by the user of this data type to give him apility 
 *                     to check for the message received
 */
typedef struct 
{
    SPI_HandleTypeDef* SPI;
    Queue *RxQueue;
    GPIO_TypeDef *ChipSelectPort;
    can_filters_t *Filters;
    EXTI_HandleTypeDef *EXTI_Line;
    void (*UserCallback)(void);
    uint16_t ChipSelectPin;
}can_t;

/**
 * @brief data type represents the message frame (ID - DLC - Data)
 * @param Data pointer to the data of the message
 * @param ID the id of the message
 * @param DataLength stores the data length of the message (0 -> 8 bytes - more than 8 sends 8 bytes only)
 * @param Priority the priority of deciding which message should be transmitted from the module
 * @param RxDataRemote presents the received message is remote or standard
 * @param RESERVED 2 bits reserved for future
 * @note the priority of messages in the module is not same as in the controller
 */
typedef struct 
{
    void *Data;
    uint16_t ID;
    uint8_t DataLength :4;
    uint8_t Priority :2;
    uint8_t RxDataRemote :1;
    uint8_t RESEVED :1;
}can_message_t;

/**
 * @brief descripes the command as instructions and parameters
 * @param CommandParameters pointer to the parameters bytes
 * @param CommandParametersSize number of parameters
 * @param Instruction the main instruction sent to the module 
 */
typedef struct
{
    uint8_t* CommandParamerters;
    uint8_t CommandParamertersSize;
    uint8_t Instruction;
}can_command_t;




/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief initialize the can module 
 * @param p_Can pointer to the CAN module needed to be intialized
 * @return ecu_status_t status of the operation
 */
ecu_status_t can_init(can_t* p_Can);

/**
 * @brief this message receive a message using CAN protocol (returns the first message in the queue if available)
 * @param p_Can pointer to the CAN module used to receive this message
 * @param p_ReturnedMessage pointer to the message buffer want to receive to it
 * @return ecu_status_t status of the operation
 */
ecu_status_t can_message_receive_from_queue(can_t* p_Can, can_message_t **p_ReturnedMessage);

/** 
 * @brief this interface assign transmission buffers Parameters to send it throw send Command
 * @param p_Can pointer to the CAN module used to transmit this message
 * @param p_Message pointer to the message buffer want to be sent
 * @return ecu_status_t status of the operation
 */
ecu_status_t can_message_transmit_using_command(can_t* p_Can,can_message_t *p_TrasmitedMessage);



/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif // MCP2515_H