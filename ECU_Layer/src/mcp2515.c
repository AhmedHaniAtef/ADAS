/**
 * @file    mcp2515.c
 * @author  Ahmed Hani & Mohamed Kamal & Mohamed Saad
 * @brief   contains interfaces to communicate through external MCP2515 CAN module through SPI
 * @date    2024-11-29
 * @note
 */

/***********************************************************************************************************************
 *                                                      INCLUDES                                                        *
 ***********************************************************************************************************************/
#include "../inc/mcp2515_reg.h"
#include "../inc/mcp2515.h"
#include "../inc/ecu.h"

/***********************************************************************************************************************
 *                                                    MACRO DEFINES                                                     *
 ***********************************************************************************************************************/
#define CAN_RX_BUFFER_READY (1)
#define CAN_RX_BUFFER_UNREADY (0)

#define CAN_RXB0_READY (1)
#define CAN_RXB1_READY (2)
#define CAN_RXB0_RXB1_READY (3)

#define CAN_READ_BIT(INPUT, BIT) ((INPUT >> BIT) && 1)

/***********************************************************************************************************************
 *                                                   MACRO FUNCTIONS                                                    *
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 *                                               STATIC FUNCTION DEFINITION                                             *
 ***********************************************************************************************************************/

/**
 * @brief this function used to send full command to the CAN module through SPI
 * @param p_Can pointer to the object of the CAN module used
 * @param p_Command pointer to the command needed to be sent
 * @return ecu_status_t status of the operation
 */
static ecu_status_t can_command_send(can_t *p_Can, can_command_t *p_Command);

/**
 * @brief this function used to send full command to the CAN module through SPI then read bytes data from it
 * @param p_Can pointer to the object of the CAN module used
 * @param p_Command pointer to the command needed to be sent
 * @param p_data pointer to the buffer used to store the received data
 * @param p_Data_Length number of bytes of data expected to read
 * @return ecu_status_t status of the operation
 */
static ecu_status_t can_command_read(can_t *p_Can, can_command_t *p_Command, void *p_data, uint8_t p_Data_Length);

/**
 * @brief callback associated to the external interrupt that represents that the Rx buffer in mcp2515 has new data
 *
 */
void Rx_Buffer_Ready_Callback(void);

/**
 * @brief this function used to receive the messages ready in the RX0 , RX1 buffer in the CAN module
 *        and put them in the queue
 * @param p_Can pointer to the CAN object that descipe which should receive from
 * @return ecu_status_t status of the operation
 */
static ecu_status_t can_receive_from_buffer(can_t *p_Can);

/**
 * @brief parse the buffer into new message
 * @param p_Buffer pointer to the buffer contains data
 * @param p_Message pointer to the new message want to parse data into
 * @return can_message_t* return the same pointer of the new message
 */
static can_message_t *can_parser(uint8_t *p_Buffer, can_message_t *p_Message);

/***********************************************************************************************************************
 *                                                     GLOBAL OBJECTS                                                   *
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 *                                                     STATIC OBJECTS                                                   *
 ***********************************************************************************************************************/

static uint32_t CanRxBufferReadyFlag = CAN_RX_BUFFER_UNREADY;
static void (*CanUserCallback)(void) = NULL;

/***********************************************************************************************************************
 *                                                      DATA TYPES                                                      *
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 *                                                  FUNCTION DECLARATION                                                *
 ***********************************************************************************************************************/

/**
 * @brief initialize the can module
 * @param p_Can pointer to the CAN module needed to be intialized
 * @return ecu_status_t status of the operation
 */
ecu_status_t can_init(can_t *p_Can)
{
    HAL_StatusTypeDef l_hal_status = HAL_OK;
    ecu_status_t l_EcuStatus = ECU_OK;
    can_command_t l_init_commands;
    uint8_t param[11];
    if (NULL == p_Can)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        // 1- send RESET Command (CAN_RESET 0xC0 - no more parameters)
        l_init_commands.Instruction = CAN_RESET;
        l_init_commands.CommandParamertersSize = 0;
        l_init_commands.CommandParamerters = NULL;
        l_EcuStatus |= can_command_send(p_Can, &l_init_commands);

        // 2- check for filters (configure it if availabel)
        if (p_Can->Filters != NULL)
        {
            // fill RXF0SIDH, RXF0SIDL, RXF1SIDH, RXF1SIDl, RXF2SIDH, RXF2SIDl with filters
            // in the can module and extended filters with 0
            l_init_commands.Instruction = CAN_WRITE;
            l_init_commands.CommandParamertersSize = 11;
            param[0] = RXF0SIDH;
            param[1] = p_Can->Filters->RXF0.ByteOrder.High;
            param[2] = p_Can->Filters->RXF0.ByteOrder.Low;
            param[3] = 0;
            param[4] = 0;
            param[5] = p_Can->Filters->RXF1.ByteOrder.High;
            param[6] = p_Can->Filters->RXF1.ByteOrder.Low;
            param[7] = 0;
            param[8] = 0;
            param[9] = p_Can->Filters->RXF2.ByteOrder.High;
            param[10] = p_Can->Filters->RXF2.ByteOrder.Low;
            l_init_commands.CommandParamerters = param;
            l_EcuStatus |= can_command_send(p_Can, &l_init_commands);

            // fill RXF3SIDH, RXF3SIDL, RXF4SIDH, RXF4SIDl, RXF5SIDH, RXF5SIDl with filters
            // in the can module and extended filters with 0
            l_init_commands.Instruction = CAN_WRITE;
            l_init_commands.CommandParamertersSize = 11;
            param[0] = RXF3SIDH;
            param[1] = p_Can->Filters->RXF3.ByteOrder.High;
            param[2] = p_Can->Filters->RXF3.ByteOrder.Low;
            param[3] = 0;
            param[4] = 0;
            param[5] = p_Can->Filters->RXF4.ByteOrder.High;
            param[6] = p_Can->Filters->RXF4.ByteOrder.Low;
            param[7] = 0;
            param[8] = 0;
            param[9] = p_Can->Filters->RXF5.ByteOrder.High;
            param[10] = p_Can->Filters->RXF5.ByteOrder.Low;
            l_init_commands.CommandParamerters = param;
            l_EcuStatus |= can_command_send(p_Can, &l_init_commands);

            // fill RXM0SIDH, RXM0SIDL, RXM1SIDH, RXM1SIDL with masks
            // in the can module and extended masks with 0
            l_init_commands.Instruction = CAN_WRITE;
            l_init_commands.CommandParamertersSize = 7;
            param[0] = RXM0SIDH;
            param[1] = p_Can->Filters->RXM0.ByteOrder.High;
            param[2] = p_Can->Filters->RXM0.ByteOrder.Low;
            param[3] = 0;
            param[4] = 0;
            param[5] = p_Can->Filters->RXM1.ByteOrder.High;
            param[6] = p_Can->Filters->RXM1.ByteOrder.Low;
            l_init_commands.CommandParamerters = param;
            l_EcuStatus |= can_command_send(p_Can, &l_init_commands);
        }

        // 3- Enable RX interrupt
        l_init_commands.Instruction = CAN_BIT_MODIFY;
        l_init_commands.CommandParamertersSize = 3;
        param[0] = CANCTRL;
        param[1] = (CANINTE_RX0IE | CANINTE_RX1IE);
        param[2] = (CANINTE_RX0IE | CANINTE_RX1IE);
        l_init_commands.CommandParamerters = param;
        l_EcuStatus |= can_command_send(p_Can, &l_init_commands);

        // 4- Switch to Norma mode
        l_init_commands.Instruction = CAN_BIT_MODIFY;
        l_init_commands.CommandParamertersSize = 3;
        param[0] = CANINTE;
        param[1] = CANCTRL_MODE_MASK,
        param[2] = 0x00;
        l_init_commands.CommandParamerters = param;
        l_EcuStatus |= can_command_send(p_Can, &l_init_commands);

        // 5- queue initialization
        p_Can->RxQueue = queue_create();

        // 6- external callback interrupt change
        p_Can->EXTI_Line = (EXTI_HandleTypeDef *)malloc(sizeof(EXTI_HandleTypeDef));
        l_hal_status = HAL_EXTI_RegisterCallback(p_Can->EXTI_Line, HAL_EXTI_COMMON_CB_ID, Rx_Buffer_Ready_Callback);
        if (HAL_ERROR == l_hal_status)
            l_EcuStatus = ECU_ERROR;
        // 7- store the user callback to call it in the rx callback
        CanUserCallback = p_Can->UserCallback;
    }
    return l_EcuStatus;
}

/**
 * @brief this message receive a message using CAN protocol (returns the first message in the queue if available)
 * @param p_Can pointer to the CAN module used to receive this message
 * @param p_ReturnedMessage pointer to the message buffer want to receive to it
 * @return ecu_status_t status of the operation
 */
ecu_status_t can_message_receive_from_queue(can_t *p_Can, can_message_t **p_ReturnedMessage)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if ((NULL == p_Can) || (NULL == p_ReturnedMessage))
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        // 1- receive messages in the buffers if availabale
        if (CAN_RX_BUFFER_READY == CanRxBufferReadyFlag)
        {
            l_EcuStatus |= can_receive_from_buffer(p_Can);
            CanRxBufferReadyFlag = CAN_RX_BUFFER_UNREADY;
        }

        // 2- dequeue first message in the order in the queue
        if (queue_size(p_Can->RxQueue))
        {
            *p_ReturnedMessage = (can_message_t *)queue_dequeue(p_Can->RxQueue);
        }
    }
    return l_EcuStatus;
}

/**
 * @brief this interface assign transmission buffers Parameters to send it throw send Command
 * @param p_Can pointer to the CAN module used to transmit this message
 * @param p_Message pointer to the message buffer want to be sent
 * @return ecu_status_t status of the operation
 */
ecu_status_t can_message_transmit_using_command(can_t *p_Can, can_message_t *p_TrasmitedMessage)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (!(p_Can && p_TrasmitedMessage))
        l_EcuStatus = ECU_ERROR;
    else
    {
        void *data_ptr = p_TrasmitedMessage->Data;
        uint8_t tx_reg[3] = {TXB0CTRL, TXB1CTRL, TXB2CTRL};
        uint8_t tx_result = 0;
        can_command_t l_init_commands;
        uint8_t param[13] = {0};
        param[0] = (uint8_t)((p_TrasmitedMessage->ID) >> 3);
        param[1] = (uint8_t)(p_TrasmitedMessage->ID << 5);

        param[2] = 0;
        param[3] = 0;
        param[4] = p_TrasmitedMessage->DataLength;

        param[5] = (*((uint8_t *)data_ptr++));
        param[6] = (*((uint8_t *)data_ptr++));
        param[7] = (*((uint8_t *)data_ptr++));
        param[8] = (*((uint8_t *)data_ptr++));
        param[9] = (*((uint8_t *)data_ptr++));
        param[10] = (*((uint8_t *)data_ptr++));
        param[11] = (*((uint8_t *)data_ptr++));
        param[12] = (*((uint8_t *)data_ptr));
        l_init_commands.Instruction = CAN_READ;
        l_init_commands.CommandParamertersSize = 1;
        l_init_commands.CommandParamerters = (tx_reg[0]);
        l_EcuStatus |= can_command_read(p_Can, &l_init_commands, &tx_result, 1);
        if (!(CAN_READ_BIT(tx_result, 3)))
        {
            uint8_t prameters[3] = {(tx_reg[0]), TXBCTRL_PRIORITY_MASK, (p_TrasmitedMessage->Priority)};
            l_init_commands.Instruction = CAN_BIT_MODIFY;
            l_init_commands.CommandParamertersSize = 3;
            l_init_commands.CommandParamerters = prameters;
            l_EcuStatus |= can_command_send(p_Can, &l_init_commands);
            l_init_commands.Instruction = TX_Buffer_0_Start_at_TXB0SIDH;
            l_init_commands.CommandParamertersSize = 13;
            l_init_commands.CommandParamerters = param;
            l_EcuStatus |= can_command_send(p_Can, &l_init_commands);
            l_init_commands.Instruction = (CAN_RTS | 1);
            l_init_commands.CommandParamertersSize = 0;
            l_init_commands.CommandParamerters = NULL;
            l_EcuStatus |= can_command_send(p_Can, &l_init_commands);
        }
        else
        {
            l_init_commands.Instruction = CAN_READ;
            l_init_commands.CommandParamertersSize = 1;
            l_init_commands.CommandParamerters = (tx_reg[1]);
            l_EcuStatus |= can_command_read(p_Can, &l_init_commands, &tx_result, 1);
            if (!(CAN_READ_BIT(tx_result, 3)))
            {
                uint8_t prameters[3] = {(tx_reg[1]), TXBCTRL_PRIORITY_MASK, (p_TrasmitedMessage->Priority)};
                l_init_commands.Instruction = CAN_BIT_MODIFY;
                l_init_commands.CommandParamertersSize = 3;
                l_init_commands.CommandParamerters = prameters;
                l_EcuStatus |= can_command_send(p_Can, &l_init_commands);
                l_init_commands.Instruction = TX_Buffer_1_Start_at_TXB1SIDH;
                l_init_commands.CommandParamertersSize = 13;
                l_init_commands.CommandParamerters = param;
                l_EcuStatus |= can_command_send(p_Can, &l_init_commands);
                l_init_commands.Instruction = (CAN_RTS | 2);
                l_init_commands.CommandParamertersSize = 0;
                l_init_commands.CommandParamerters = NULL;
                l_EcuStatus |= can_command_send(p_Can, &l_init_commands);
            }
            else
            {
                l_init_commands.Instruction = CAN_READ;
                l_init_commands.CommandParamertersSize = 1;
                l_init_commands.CommandParamerters = (tx_reg[2]);
                l_EcuStatus |= can_command_read(p_Can, &l_init_commands, &tx_result, 1);
                if (!(CAN_READ_BIT(tx_result, 3)))
                {
                    uint8_t parameters[3] = {(tx_reg[2]), TXBCTRL_PRIORITY_MASK, (p_TrasmitedMessage->Priority)};
                    l_init_commands.Instruction = CAN_BIT_MODIFY;
                    l_init_commands.CommandParamertersSize = 3;
                    l_init_commands.CommandParamerters = parameters;
                    l_EcuStatus |= can_command_send(p_Can, &l_init_commands);
                    l_init_commands.Instruction = TX_Buffer_2_Start_at_TXB2SIDH;
                    l_init_commands.CommandParamertersSize = 13;
                    l_init_commands.CommandParamerters = param;
                    l_EcuStatus |= can_command_send(p_Can, &l_init_commands);
                    l_init_commands.Instruction = (CAN_RTS | 4);
                    l_init_commands.CommandParamertersSize = 0;
                    l_init_commands.CommandParamerters = NULL;
                    l_EcuStatus |= can_command_send(p_Can, &l_init_commands);
                }
                else
                {
                    l_EcuStatus = ECU_ERROR;
                }
            }
        }
    }
    return l_EcuStatus;
}

/***********************************************************************************************************************
 *                                               STATIC FUNCTION DECLARATION                                            *
 ***********************************************************************************************************************/

/**
 * @brief this function used to send full command to the CAN module through SPI
 * @param p_Can pointer to the object of the CAN module used
 * @param p_Command pointer to the command needed to be sent
 * @return ecu_status_t status of the operation
 */
static ecu_status_t can_command_send(can_t *p_Can, can_command_t *p_Command)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    // Validate the input pointers
    if ((p_Can == NULL) || (p_Command == NULL) || (p_Command->CommandParamerters == NULL) || (p_Can->SPI == NULL))
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        // Pull CS (PA4) low to start the SPI transaction
        HAL_GPIO_WritePin(p_Can->ChipSelectPort, p_Can->ChipSelectPin, GPIO_PIN_RESET);

        // Send the instruction byte
        if (HAL_SPI_Transmit(p_Can->SPI, &(p_Command->Instruction), 1, HAL_MAX_DELAY) != HAL_OK)
        {
            l_EcuStatus = ECU_ERROR;
        }
        else
        {
            // Send the command parameters if any
            if (p_Command->CommandParamertersSize > 0)
            {
                if (HAL_SPI_Transmit(p_Can->SPI,
                                     p_Command->CommandParamerters,
                                     p_Command->CommandParamertersSize,
                                     HAL_MAX_DELAY) != HAL_OK)
                {
                    l_EcuStatus = ECU_ERROR;
                }
            }
            else
            { /*NOTHING*/
            }
        }

        // Pull CS (PA4) high to end the SPI transaction
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    }
    return l_EcuStatus;
}

/**
 * @brief this function used to send full command to the CAN module through SPI then read bytes data from it
 * @param p_Can pointer to the object of the CAN module used
 * @param p_Command pointer to the command needed to be sent
 * @param p_data pointer to the buffer used to store the received data
 * @param p_Data_Length number of bytes of data expected to read
 * @return ecu_status_t status of the operation
 */
static ecu_status_t can_command_read(can_t *p_Can, can_command_t *p_Command, void *p_data, uint8_t p_Data_Length)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    // Validate the input pointers
    if ((p_Can == NULL) || (p_Command == NULL) || (p_Command->CommandParamerters == NULL) || (p_Can->SPI == NULL))
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        // Pull CS (PA4) low to start the SPI transaction
        HAL_GPIO_WritePin(p_Can->ChipSelectPort, p_Can->ChipSelectPin, GPIO_PIN_RESET);

        // Send the instruction byte
        if (HAL_SPI_Transmit(p_Can->SPI, &(p_Command->Instruction), 1, HAL_MAX_DELAY) != HAL_OK)
        {
            l_EcuStatus = ECU_ERROR;
        }
        else
        {
            // Send the command parameters if any
            if (p_Command->CommandParamertersSize > 0)
            {
                if (HAL_SPI_Transmit(p_Can->SPI,
                                     p_Command->CommandParamerters,
                                     p_Command->CommandParamertersSize,
                                     HAL_MAX_DELAY) != HAL_OK)
                {
                    l_EcuStatus = ECU_ERROR;
                }
            }
            else
            { /*NOTHING*/
            }

            // Receive the data from spi
            if (HAL_SPI_Receive(p_Can->SPI,
                                p_data,
                                p_Data_Length,
                                HAL_MAX_DELAY) != HAL_OK)
            {
                l_EcuStatus = ECU_ERROR;
            }
            else
            { /*NOTHING*/
            }
        }

        // Pull CS (PA4) high to end the SPI transaction
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    }
    return l_EcuStatus;
}

/**
 * @brief callback associated to the external interrupt that represents that the Rx buffer in mcp2515 has new data
 *
 */
void Rx_Buffer_Ready_Callback(void)
{
    CanRxBufferReadyFlag = CAN_RX_BUFFER_READY;
    if (CanUserCallback)
        CanUserCallback();
}

/**
 * @brief this function used to receive the messages ready in the RX0 , RX1 buffer in the CAN module
 *        and put them in the queue
 * @param p_Can pointer to the CAN object that descipe which should receive from
 * @return ecu_status_t status of the operation
 */
static ecu_status_t can_receive_from_buffer(can_t *p_Can)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    uint8_t l_temp_buffer[14];
    can_command_t l_rx_command;
    can_message_t *l_NewMessage = NULL;
    if (NULL == p_Can)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        // 1- check for which buffer is ready (rx status command)
        l_rx_command.Instruction = CAN_RX_STATUS;
        l_rx_command.CommandParamertersSize = 0;
        l_rx_command.CommandParamerters = NULL;
        l_EcuStatus |= can_command_read(p_Can, &l_rx_command, l_temp_buffer, 2);
        // check if first and second bytes are same to make sure of validity of status
        if (l_temp_buffer[0] == l_temp_buffer[1])
        {
            uint8_t l_StatusByte = l_temp_buffer[0];
            uint8_t l_ReadyBuffers = ((l_StatusByte >> 6) & (0b00000011));

            // 2- read the buffer
            switch (l_ReadyBuffers)
            {
            case CAN_RXB0_READY:
                l_rx_command.Instruction = CAN_READ_RXB0SIDH;
                l_rx_command.CommandParamertersSize = 0;
                l_rx_command.CommandParamerters = NULL;
                l_EcuStatus |= can_command_read(p_Can, &l_rx_command, l_temp_buffer, 13);
                l_NewMessage = (can_message_t *)calloc(1, sizeof(can_message_t));

                // 3- parse the data into a message
                l_NewMessage = can_parser(l_temp_buffer, l_NewMessage);

                // 4- enqueue the message in the queue of this can object
                queue_enqueue(p_Can->RxQueue, l_NewMessage);

                break;

            case CAN_RXB1_READY:
                l_rx_command.Instruction = CAN_READ_RXB1SIDH;
                l_rx_command.CommandParamertersSize = 0;
                l_rx_command.CommandParamerters = NULL;
                l_EcuStatus |= can_command_read(p_Can, &l_rx_command, l_temp_buffer, 13);
                l_NewMessage = (can_message_t *)calloc(1, sizeof(can_message_t));

                // 3- parse the data into a message
                l_NewMessage = can_parser(l_temp_buffer, l_NewMessage);

                // 4- enqueue the message in the queue of this can object
                queue_enqueue(p_Can->RxQueue, l_NewMessage);

                break;

            case CAN_RXB0_RXB1_READY:
                l_rx_command.Instruction = CAN_READ_RXB0SIDH;
                l_rx_command.CommandParamertersSize = 0;
                l_rx_command.CommandParamerters = NULL;
                l_EcuStatus |= can_command_read(p_Can, &l_rx_command, l_temp_buffer, 13);
                l_NewMessage = (can_message_t *)calloc(1, sizeof(can_message_t));

                // 3- parse the data into a message
                l_NewMessage = can_parser(l_temp_buffer, l_NewMessage);

                // 4- enqueue the message in the queue of this can object
                queue_enqueue(p_Can->RxQueue, l_NewMessage);

                l_rx_command.Instruction = CAN_READ_RXB1SIDH;
                l_rx_command.CommandParamertersSize = 0;
                l_rx_command.CommandParamerters = NULL;
                l_EcuStatus |= can_command_read(p_Can, &l_rx_command, l_temp_buffer, 13);
                l_NewMessage = (can_message_t *)calloc(1, sizeof(can_message_t));

                // 3- parse the data into a message
                l_NewMessage = can_parser(l_temp_buffer, l_NewMessage);

                // 4- enqueue the message in the queue of this can object
                queue_enqueue(p_Can->RxQueue, l_NewMessage);
                break;

            default:
                break;
            }

            // 3- parse the data into a message

            // 4- enqueue the message in the queue of this can object
        }
        else
        {
            l_EcuStatus = ECU_ERROR;
        }
    }
    return l_EcuStatus;
}

/**
 * @brief parse the buffer into new message
 * @param p_Buffer pointer to the buffer contains data
 * @param p_Message pointer to the new message want to parse data into
 * @return can_message_t* return the same pointer of the new message
 */
static can_message_t *can_parser(uint8_t *p_Buffer, can_message_t *p_Message)
{
    p_Message->ID = ((p_Buffer[0]) | (p_Buffer[1] >> 5));
    p_Message->Priority = 0;
    p_Message->RxDataRemote = (p_Buffer[1] & (0b00010000)) >> 4;
    p_Message->DataLength = p_Buffer[4] & (0b00001111);
    p_Message->Data = (uint8_t *)malloc(p_Message->DataLength);
    for (uint8_t counter = 0; counter < p_Message->DataLength; counter++)
    {
        ((uint8_t *)p_Message->Data)[counter] = p_Buffer[counter + 5];
    }
    return p_Message;
}

/***********************************************************************************************************************
 * AUTHOR                |* NOTE                                                                                        *
 ************************************************************************************************************************
 *                       |                                                                                              *
 *                       |                                                                                              *
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Explaination of How to Use This Driver                                                                               *
************************************************************************************************************************
---------------------------------------------- %% Initializing %% ------------------------------------------------------
1- First, you should create an object of (can_t) and initialize it with right data
2- Before begining of the main application you should call (can_init) to initialize this object and the target module
NOTE: the user (you) is responsibe for each element in the created object (can_t) except RxQueue the driver handles it
NOTE: if you want to use filters and masks you should create object of (can_filters_t) and initialize it at first then
      pass it to the object created from (can_t)

---------------------------------------------- %% Transmitting %% ------------------------------------------------------

---------------------------------------------- %% Receiving %% ---------------------------------------------------------
1- you can use (can_message_receive_from_queue) to receive a message from the module if the frame is ready in the buffer
NOTE: This function returns NULL in case of the queue is empty
NOTE: you should keep in mind that it would go first to receive the messages from the module then returns the
      first message in the queue for example:

Before calling it:
QUEUE:  MESSAGE 1 <- MESSAGE 2 <- MESSAGE 3
BUFFER: MESSAGE 4 IN RXB0 AND MESSAGE 5 IN RXB1

After calling it:
QUEUE:  MESSAGE 2 <- MESSAGE 3 <- MESSAGE 4 <- MESSAGE 5
BUFFER: EMPTY
RETURNED: MESSAGE 1
***********************************************************************************************************************/
