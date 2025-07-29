/**
 * @file    TSR_feature.c
 * @author  Ahmed Hani
 * @brief   contains the functions which handle the Traffic Sign Recognition feature
 * @date    2025-04-16
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "APP.h"
#include "TSR_feature.h"
#include "ACC_feature.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                               STATIC FUNCTION DEFINITION                                             *
***********************************************************************************************************************/



/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/






/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/

static float_t LastCarAngle = 0.0f;

/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
 * @brief this function will be called when a sign or speed detected
 * 
 * @param p_Speed detected speed if exists
 * @param p_Sign detected sign if exists
 * @return app_status_t status of operation
 */
app_status_t TSR_handle(traffic_sign_t p_Speed, traffic_sign_t p_Sign)
{
    app_status_t l_AppStatus = APP_OK;
    
    switch (p_Speed)
    {
    case SPEED_30:
        Car_Max_Forced_Speed = Max_speed_responsible(SPEED_30_PERCENTAGE * DEFUALT_ROBOT_MAX_SPEED, TSR);
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        break;
    
    case SPEED_40:
        Car_Max_Forced_Speed = Max_speed_responsible(SPEED_40_PERCENTAGE * DEFUALT_ROBOT_MAX_SPEED, TSR);
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        break;
    
    case SPEED_50:
        Car_Max_Forced_Speed = Max_speed_responsible(SPEED_50_PERCENTAGE * DEFUALT_ROBOT_MAX_SPEED, TSR);
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        break;
    
    case SPEED_60:
        Car_Max_Forced_Speed = Max_speed_responsible(SPEED_60_PERCENTAGE * DEFUALT_ROBOT_MAX_SPEED, TSR);
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        break;
    
    case SPEED_70:
        Car_Max_Forced_Speed = Max_speed_responsible(SPEED_70_PERCENTAGE * DEFUALT_ROBOT_MAX_SPEED, TSR);
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        break;
    
    case SPEED_80:
        Car_Max_Forced_Speed = Max_speed_responsible(SPEED_80_PERCENTAGE * DEFUALT_ROBOT_MAX_SPEED, TSR);
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        break;
    
    case SPEED_90:
        Car_Max_Forced_Speed = Max_speed_responsible(SPEED_90_PERCENTAGE * DEFUALT_ROBOT_MAX_SPEED, TSR);
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        break;
    
    case SPEED_100:
        Car_Max_Forced_Speed = Max_speed_responsible(SPEED_100_PERCENTAGE * DEFUALT_ROBOT_MAX_SPEED, TSR);
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        break;
    
    case SPEED_120:
        Car_Max_Forced_Speed = Max_speed_responsible(SPEED_120_PERCENTAGE * DEFUALT_ROBOT_MAX_SPEED, TSR);
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        break;

    default:
        break;
    }
    
    switch (p_Sign)
    {
    case DIRECTION:
        /* Reserved Until Thinking of the behavior */
        break;
    
    case STOP:
        /* Reserved Until Thinking of the behavior */
        break;
    
    case RED:
        /* Reserved Until Thinking of the behavior */
        break;
    
    case GREEN:
        /* Reserved Until Thinking of the behavior */
        break;
    
    default:
        break;
    }
    return l_AppStatus;
}

/**
 * @brief this function updates the max speed of the car (depending on the last max limited speed deteced)
 * 
 * @return app_status_t status of operation
 */
app_status_t TSR_task(void)
{
    app_status_t l_AppStatus = APP_OK;

    float_t l_TempAngle = 0.0f;
    float_t l_AngleDifference = 0.0f;
    
    if (Car_Wanted_Angle > 180)
        l_TempAngle = Car_Wanted_Angle - 360.0f;
    
    l_AngleDifference = LastCarAngle - l_TempAngle;

    if (!(value_in_range(l_AngleDifference, 0, 10) || value_in_range(l_AngleDifference, -10, 0)))
    {
        Car_Max_Forced_Speed = Max_speed_responsible(DEFUALT_ROBOT_MAX_SPEED, TSR);

        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
    }

    LastCarAngle = Car_Wanted_Angle;
}


/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/



/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
