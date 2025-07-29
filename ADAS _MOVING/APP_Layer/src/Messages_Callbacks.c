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

/* Message for update encoder position callback */
static void msg_update_encoder_clb(uCAN_MSG *p_Message);

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
    .CallBack =msg_robot_rotate_clb,
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

/* Message for Updating the position of the encoder */
can_msg_t msg_update_encoder =
{
    .ID = 0x106,
    .CallBack = msg_update_encoder_clb,
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

/**
 * @brief responsible for adding messages to the list of expected messages to receive via CAN bus
 * @return app_status_t status of operation
 */
app_status_t messages_init(void)
{
    app_status_t l_AppStatus = APP_OK;
    l_AppStatus |= CAN_add_msg_rx(&Main_CAN, &msg_robot_strafe);
    l_AppStatus |= CAN_add_msg_rx(&Main_CAN, &msg_robot_rotate);
    l_AppStatus |= CAN_add_msg_rx(&Main_CAN, &msg_robot_stop);
    l_AppStatus |= CAN_add_msg_rx(&Main_CAN, &msg_robot_Wz);
    return l_AppStatus;
}




/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/

/* Message for robot strafe callback */
static void msg_robot_strafe_clb(uCAN_MSG *p_Message)
{
    two_float_conv temp = {0};
    memcpy(&temp.data, p_Message->frame.data, 8);
    float speed = temp.value[0];
    float direction = temp.value[1];
    robot_move(&ADAS_ROBOT, direction, speed);
}

/* Message for robot rotate callback */
static void msg_robot_rotate_clb(uCAN_MSG *p_Message)
{
	two_float_conv temp = {0};
    memcpy(&temp.data, p_Message->frame.data, 8);
    float speed = temp.value[0];
    float radius = temp.value[1];
    robot_rotate(&ADAS_ROBOT, radius, speed);
}

/* Message for robot stop callback */
static void msg_robot_stop_clb(uCAN_MSG *p_Message)
{
    uint8_t flag = p_Message->frame.data[0];
    if (0xFF == flag)
    {
        robot_stop(&ADAS_ROBOT);
    }
}

/* Message for robot updating the value of omega z callback */
static void msg_robot_Wz_clb(uCAN_MSG *p_Message)
{
	one_float_conv temp = {0};
    memcpy(&temp.data, p_Message->frame.data, 4);
    float l_new_Wz = temp.value;
    robot_manual_move(&ADAS_ROBOT, ADAS_ROBOT.Vx, ADAS_ROBOT.Vy, l_new_Wz);
}

/* Message for update encoder position callback */
static void msg_update_encoder_clb(uCAN_MSG *p_Message)
{
    p_Message->frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
    p_Message->frame.dlc = 8;

    two_float_conv temp;

    temp.value[0] = ADAS_ROBOT.FL.Encoder.Position;
    temp.value[1] = ADAS_ROBOT.FR.Encoder.Position;

    memcpy(p_Message->frame.data, temp.data, 8);
}


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
