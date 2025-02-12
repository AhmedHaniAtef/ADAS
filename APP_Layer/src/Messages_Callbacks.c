/**
 * @file    Messages_Callbacks.c
 * @author  Ahmed Hani
 * @brief   contains all messages can be transmitted or received on CAN bus
 * @date    2025-02-07
 * @note    nan
 */
/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../inc/Messages_Callbacks.h"
#include "../inc/APP.h"




/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/



/***********************************************************************************************************************
*                                               STATIC FUNCTION DEFINATIONS                                            *
***********************************************************************************************************************/

/* Message for robot strafe callback */
static void msg_robot_strafe_clb(uCAN_MSG *p_Message);

/* Message for robot rotate callback */
static void msg_robot_rotate_clb(uCAN_MSG *p_Message);

/* Message for robot stop callback */
static void msg_robot_stop_clb(uCAN_MSG *p_Message);

/* Message for robot updating the value of omega z callback */
static void msg_robot_Wz_clb(uCAN_MSG *p_Message);



/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/

/* Message for robot strafe */
can_msg_t msg_robot_strafe =
{
    .ID = 0x100,
    .CallBack = msg_robot_strafe_clb,
};

/* Message for robot rotate */
can_msg_t msg_robot_rotate =
{
    .ID = 0x101,
    .CallBack = msg_robot_rotate_clb,
};

/* Message for robot stop */
can_msg_t msg_robot_stop =
{
    .ID = 0x102,
    .CallBack = msg_robot_stop_clb,
};

/* Message for robot updating the value of omega z */
can_msg_t msg_robot_Wz =
{
    .ID = 0x103,
    .CallBack = msg_robot_Wz_clb,
};



/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/






/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/





/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/

/* Message for robot strafe callback */
static void msg_robot_strafe_clb(uCAN_MSG *p_Message)
{
    p_Message->frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
    p_Message->frame.dlc = 8;
  
    two_float_conv temp;
    temp.value[0] = Car_Wanted_Speed;
    temp.value[1] = Car_Wanted_direction;
    memcpy(p_Message->frame.data, temp.data, 8);
}

/* Message for robot rotate callback */
static void msg_robot_rotate_clb(uCAN_MSG *p_Message)
{
    p_Message->frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
    p_Message->frame.dlc = 8;
  
    two_float_conv temp;
    temp.value[0] = Car_Wanted_Angular_Speed;
    temp.value[1] = Car_Wanted_Rotate_Radius;
    memcpy(p_Message->frame.data, temp.data, 8);
}

/* Message for robot stop callback */
static void msg_robot_stop_clb(uCAN_MSG *p_Message)
{
    p_Message->frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
    p_Message->frame.dlc = 1;
  
    uint8_t temp = ROBOT_STOP_FLAG;
    memcpy(p_Message->frame.data, &temp, 1);
}

/* Message for robot updating the value of omega z callback */
static void msg_robot_Wz_clb(uCAN_MSG *p_Message)
{
    p_Message->frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
    p_Message->frame.dlc = 4;
  
    one_float_conv temp;
    temp.value = Omega_z;
    memcpy(p_Message->frame.data, temp.data, 4);
}





/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
