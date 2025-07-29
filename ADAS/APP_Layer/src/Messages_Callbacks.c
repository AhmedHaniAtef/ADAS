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

/* Message for Traffic Sign Recognition Feature detects a Sign callback */
static void msg_sign_detected_clb(uCAN_MSG *p_Message);

/* Message for Auto Lane Change callback (Not Completed)*/
static void msg_lane_update_clb(uCAN_MSG *p_Message);

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

/* Message for Traffic Sign Recognition Feature detects a Sign */
can_msg_t msg_sign_detected =
{
    .ID = 0x104,
    .CallBack = msg_sign_detected_clb,
};

/* Message for Auto Lane Change*/
can_msg_t msg_lane_update =
{
    .ID = 0x105,
    .CallBack = msg_lane_update_clb,
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

static void (*YawSusCB)(void) = NULL;
static void (*YawResCB)(void) = NULL;



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
app_status_t messages_init(void (*p_SusCB)(void), void (*p_ResCB)(void))
{
    app_status_t l_AppStatus = APP_OK;
    l_AppStatus |= CAN_add_msg_rx(&Main_CAN, &msg_sign_detected);   
    l_AppStatus |= CAN_add_msg_rx(&Main_CAN, &msg_lane_update);   
    l_AppStatus |= CAN_add_msg_rx(&Main_CAN, &msg_update_encoder);   
    YawSusCB = p_SusCB,
    YawResCB = p_ResCB;
    return l_AppStatus;
}




/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/

/* Message for robot strafe callback */
static void msg_robot_strafe_clb(uCAN_MSG *p_Message)
{
    p_Message->frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
    p_Message->frame.dlc = 8;
  
    if(Car_Wanted_Speed != 0)
    {
        YawSusCB();
    }
    else
    {
        Car_Wanted_Angle = Main_Orientation.FilteredYAW;
        YawResCB();
    }

    two_float_conv temp;
    if (Car_Wanted_Speed > Car_Max_Forced_Speed)
    {
        temp.value[0] = Car_Max_Forced_Speed;
    }
    else
    {
        temp.value[0] = Car_Wanted_Speed;
    }
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

    Car_Wanted_Angle = Main_Orientation.FilteredYAW;
    YawResCB();
  
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

/* Message for Traffic Sign Recognition Feature detects a Sign callback */
static void msg_sign_detected_clb(uCAN_MSG *p_Message)
{
    uint8_t temp[2] = {0};
    memcpy(temp, p_Message->frame.data, 2);

    traffic_sign_t l_Speed = temp[0];
    traffic_sign_t l_Sign = temp[1];

    Traffic_Speed = l_Speed;
    Traffic_Sign = l_Sign;

    TSR_handle(l_Speed, l_Sign);
}

/* Message for Auto Lane Change callback (Not Completed)*/
static void msg_lane_update_clb(uCAN_MSG *p_Message)
{
    uint8_t temp[2] = {0};
    memcpy(temp, p_Message->frame.data, 2);

    Lane_Received_t l_AvailableLane = temp[0];
    Lane_Received_t l_CurrentPosition = temp[1];

    ALC_handle(l_AvailableLane, l_CurrentPosition);
}

/* Message for update encoder position callback */
static void msg_update_encoder_clb(uCAN_MSG *p_Message)
{
    two_float_conv temp = {0};
    memcpy(&temp.data, p_Message->frame.data, 8);
    float l_fl_encoder = temp.value[0];
    float l_fr_encoder = temp.value[1];
    
    FL_Encoder_pos = l_fl_encoder;
    FR_Encoder_pos = l_fr_encoder;
}


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/



