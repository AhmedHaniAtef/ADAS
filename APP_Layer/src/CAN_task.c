/**
 * @file    CAN_task.C
 * @author  Ahmed Hani
 * @brief   contains full requirements for can bus communication
 * @date    2525-02-03
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../inc/CAN_task.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                               STATIC FUNCTION DEFINITION                                             *
***********************************************************************************************************************/

/**
 * @brief call back to find the pointer to the message 
 * 
 * @param p_MsgExpected pointer to the messages which can expected to receive and handle
 * @param p_MsgReceived pointer to the received message
 * @return int 1 if yes 0 if no
 */
static int find_msg_with_id(void *p_MsgExpected, const void *p_MsgReceived);




/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/


/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/






/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
 * @brief used to initialize the can bus
 * 
 * @param p_CanBus pointer to the bus of CAN
 * @return app_status_t status of the operation 
 */
app_status_t CAN_task_init (CAN_bus_t *p_CanBus)
{
    app_status_t l_AppStatus = APP_OK;
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_CanBus)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        l_EcuStatus |= CANSPI_Initialize(p_CanBus->UsedCAN);
        p_CanBus->ExpectedMSG = list_create();
        if ((l_EcuStatus != ECU_OK) || (NULL == p_CanBus->ExpectedMSG))
        {
            l_AppStatus = APP_ERROR;
        }
    }
    return l_AppStatus;
}

/**
 * @brief add a message which could be received through CAN bus
 * 
 * @param p_CanBus pointer to the CAN bus which could receive the message
 * @param p_Message pointer to the message could be received
 * @return app_status_t status of the operation 
 */
app_status_t CAN_add_msg_rx (CAN_bus_t *p_CanBus, can_msg_t *p_Message)
{
    app_status_t l_AppStatus = APP_OK;
    if ((NULL == p_CanBus) || (NULL == p_Message))
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        list_append(p_CanBus->ExpectedMSG, p_Message);
    }
    return l_AppStatus;
}

/**
 * @brief this is the task that handles receiving from the bus 
 * 
 * @param p_CanBus pointer to the CAN bus
 * @return app_status_t status of the operation 
 * @note this task should be blocked waiting for interrupt to unblock it
 */
app_status_t CAN_rx_task (CAN_bus_t *p_CanBus)
{
    app_status_t l_AppStatus = APP_OK;
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_CanBus)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        uCAN_MSG l_TempMessage;
        can_msg_t *l_RxMessage = NULL;
        l_EcuStatus |= CANSPI_Receive(p_CanBus->UsedCAN, &l_TempMessage);
        if (l_EcuStatus == ECU_OK)
        {
            l_RxMessage = (can_msg_t *)list_find(p_CanBus->ExpectedMSG, &(l_TempMessage), find_msg_with_id);
            if (l_RxMessage != NULL)
            {
                memcpy(&l_RxMessage->Message, &l_TempMessage, sizeof(uCAN_MSG));
                l_RxMessage->CallBack(&l_RxMessage->Message);
            }
        }
    }
    return l_AppStatus;
}

/**
 * @brief used to send a message to the bus
 * 
 * @param p_CanBus pointer to the CAN bus
 * @param p_Message pointer to the transsmitted message
 * @return app_status_t status of the operation 
 * @note this function calls callback to update the message frame
 */
app_status_t CAN_send_message (CAN_bus_t *p_CanBus, can_msg_t *p_Message)
{
    app_status_t l_AppStatus = APP_OK;
    ecu_status_t l_EcuStatus = ECU_OK;
    if ((NULL == p_CanBus) || (NULL == p_Message))
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        p_Message->CallBack(&p_Message->Message);
        p_Message->Message.frame.id = p_Message->ID;
        l_EcuStatus = CANSPI_isBussOff(p_CanBus->UsedCAN);
        if (l_EcuStatus == ECU_OK)
        {
            l_EcuStatus |= CANSPI_Transmit(p_CanBus->UsedCAN, &p_Message->Message);
            if ((l_EcuStatus != ECU_OK))
            {
                l_AppStatus = APP_ERROR;
                if (l_EcuStatus == ECU_ERROR)
                {
                	l_EcuStatus = CANSPI_Initialize(p_CanBus->UsedCAN);
                	l_AppStatus = APP_ERROR;
                }
            }
        }
        else
        {
            l_EcuStatus = CANSPI_isTxErrorPassive(p_CanBus->UsedCAN);
            if (l_EcuStatus == ECU_ERROR)
            {
                l_EcuStatus = CANSPI_Initialize(p_CanBus->UsedCAN);
                l_AppStatus = APP_ERROR;
            }
        }
    }
    return l_AppStatus;
}

/**
 * @brief clears the interrupt to make INT pin high
 * 
 * @param p_CanBus pointer to the CAN bus
 */
void CAN_CLEAR_INT(CAN_bus_t *p_CanBus)
{
    MCP2515_WriteByte(p_CanBus->UsedCAN->UsedSPI, MCP2515_CANINTF, 0x00);
}


/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/

/**
 * @brief call back to find the pointer to the message 
 * 
 * @param p_MsgExpected pointer to the messages which can expected to receive and handle
 * @param p_MsgReceived pointer to the received message
 * @return int 1 if yes 0 if no
 */
static int find_msg_with_id(void *p_MsgExpected, const void *p_MsgReceived)
{
    int l_RetVal = 0;
    if (((can_msg_t *)p_MsgExpected)->ID == ((uCAN_MSG *)p_MsgReceived)->frame.id)
    {
        l_RetVal = 1;
    }
    return l_RetVal;
}


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
