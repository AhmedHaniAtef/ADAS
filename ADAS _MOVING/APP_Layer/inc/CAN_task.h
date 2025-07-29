/**
 * @file    CAN_task.h
 * @author  Ahmed Hani
 * @brief   contains the CAN bus task 
 * @date    02-03-2025
 */

#ifndef CAN_TASK_H_
#define CAN_TASK_H_

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../../ECU_Layer/inc/ecu.h"
#include "../APP_std.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/

/**
 * @brief this data type to create messages which can be received from mcu and contains its callback function or could 
 *        be transsmited with callback to update data
 * 
 * @param ID the ID of the message received
 * @param CallBack pointer to the call back function to update the message frame in case of transsmiting and callback to 
 *                 process the message in case of receiving
 * @param Message message could be send or received
 */
typedef struct 
{
    uCAN_MSG Message;
    uint32_t ID;
    void (*CallBack)(uCAN_MSG *);
}can_msg_t;


typedef struct 
{
    Can_t *UsedCAN;
    List *ExpectedMSG;
}CAN_bus_t;






/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief used to initialize the can bus
 * 
 * @param p_CanBus pointer to the bus of CAN
 * @return app_status_t status of the operation 
 */
app_status_t CAN_task_init (CAN_bus_t *p_CanBus);

/**
 * @brief add a message which could be received through CAN bus
 * 
 * @param p_CanBus pointer to the CAN bus which could receive the message
 * @param p_Message pointer to the message could be received
 * @return app_status_t status of the operation 
 */
app_status_t CAN_add_msg_rx (CAN_bus_t *p_CanBus, can_msg_t *p_Message);

/**
 * @brief this is the task that handles receiving from the bus 
 * 
 * @param p_CanBus pointer to the CAN bus
 * @return app_status_t status of the operation 
 * @note this task should be blocked waiting for interrupt to unblock it
 */
app_status_t CAN_rx_task (CAN_bus_t *p_CanBus);

/**
 * @brief used to send a message to the bus
 * 
 * @param p_CanBus pointer to the CAN bus
 * @param p_Message pointer to the transsmitted message
 * @return app_status_t status of the operation 
 * @note this function calls callback to update the message frame
 */
app_status_t CAN_send_message (CAN_bus_t *p_CanBus, can_msg_t *p_Message);

/**
 * @brief clears the interrupt to make INT pin high
 * 
 * @param p_CanBus pointer to the CAN bus
 */
void CAN_CLEAR_INT(CAN_bus_t *p_CanBus);



#endif //CAN_TASK_H_



/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/