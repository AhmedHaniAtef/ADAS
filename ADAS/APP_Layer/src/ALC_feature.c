/**
 * @file    ALC_feature.c
 * @author  Ahmed Hani
 * @brief   contains the functions which handle the Auto Lane Change
 * @date    2025-05-06
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "APP.h"
#include "ALC_feature.h"



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
 * @brief make the car move in the direction specified based on the speed of the car
 * 
 * @param p_Direction direction to move
 * @return app_status_t status of operation
 */
static app_status_t LaneChange (Lane_Direction_t p_Direction);



/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/






/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/

static Lane_Received_t AvailableLanes = 0;
static Lane_Received_t CurrentPosition = 0;

static Lane_Received_t TempPosition = 0;

static Lane_Direction_t WantedLaneToChange = 0;
static uint8_t FlagtoChange = 0;

static float_t LocalCarSpeed = 0.0f;
static float_t LocalCarDirection = 0.0f;

/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
 * @brief handles the received form rasbperry pi 5 through CAN bus
 * 
 * @param p_AvailableLane represents the available lanes which car can change to
 * @param p_CurrentPosition represents the position of the can in the lane
 * @return app_status_t status of operation
 */
app_status_t ALC_handle(Lane_Received_t p_AvailableLane, Lane_Received_t p_CurrentPosition)
{
    app_status_t l_AppStatus = APP_OK;
    CurrentPosition = p_CurrentPosition;
    switch (p_CurrentPosition)
    {
    case DRIFT_LEFT:
        AvailableLanes = LANE_ERROR;
        break;

    case DRIFT_RIGHT:
        AvailableLanes = LANE_ERROR;
        break;

    case LEFT_CENTERALIZED:
        AvailableLanes = p_AvailableLane;
        break;

    case RIGHT_CENTERALIZED:
        AvailableLanes = p_AvailableLane;
        break;

    case CENTERALIZED:
        AvailableLanes = p_AvailableLane;
        break;
    
    default:
        break;
    }

}

/**
 * @brief the task of the Auto Lane Change
 * 
 * @return app_status_t 
 */
app_status_t ALC_Task(void)
{
    app_status_t l_AppStatus = APP_OK;

    if (!FlagtoChange)
    {
        TempPosition = CENTERALIZED;
        if (CurrentPosition == DRIFT_LEFT)
        {
            /* go right */
            l_AppStatus |= LaneChange(D_RIGHT);
            FlagtoChange = D_RIGHT;
        }
        else if (CurrentPosition == DRIFT_RIGHT)
        {
            /* go left */
            l_AppStatus |= LaneChange(D_LEFT);
            FlagtoChange = D_LEFT;
        }
        else if (WantedLaneToChange == D_LEFT)
        {
            /* go left */
            l_AppStatus |= LaneChange(D_LEFT);
            TempPosition = DRIFT_RIGHT;
            FlagtoChange = D_LEFT;
            
        }
        else if (WantedLaneToChange == D_RIGHT)
        {
            /* go right */
            l_AppStatus |= LaneChange(D_RIGHT);
            TempPosition = DRIFT_LEFT;
            FlagtoChange = D_RIGHT;
        }
    }
    else
    {
        if (CurrentPosition == TempPosition)
        {
            /* stay at this lane */
            l_AppStatus |= LaneChange(CENTER);
            WantedLaneToChange = CENTER;
            FlagtoChange = 0;
            TempPosition = CENTERALIZED;
        }
        else
        {
            l_AppStatus |= LaneChange(FlagtoChange);
        }
    }
}

/**
 * @brief this function will be called when the user wants to change his lane
 * 
 * @param p_WantedLane direction of the lane the user wants
 * @return app_status_t 
 */
app_status_t ALC_Change_Lane(Lane_Direction_t p_WantedLane)
{
    app_status_t l_AppStatus = APP_OK;
    if (((AvailableLanes == LEFT_ONLY) || (AvailableLanes == LEFT_AND_RIGHT)) && (p_WantedLane == D_LEFT))
    {
        WantedLaneToChange = D_LEFT;
    }
    else if (((AvailableLanes == RIGHT_ONLY) || (AvailableLanes == LEFT_AND_RIGHT)) && (p_WantedLane == D_RIGHT))
    {
        WantedLaneToChange = D_RIGHT;
    }
    else
    {
        WantedLaneToChange = DIRECTION_ERROR;
        l_AppStatus = APP_ERROR;
    }
    return l_AppStatus;
}

/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/

/**
 * @brief make the car move in the direction specified based on the speed of the car
 * 
 * @param p_Direction direction to move
 * @return app_status_t status of operation
 */
static app_status_t LaneChange (Lane_Direction_t p_Direction)
{
    app_status_t l_AppStatus = APP_OK;
    float_t l_DirectionOfChange = 0.0f;
    switch (p_Direction)
    {
    case D_LEFT:
        l_DirectionOfChange = (Car_Wanted_Speed * 
                            (MIN_ANGLE_LANE_CHANGE - MAX_ANGLE_LANE_CHANGE) / 
                            DEFUALT_ROBOT_MAX_SPEED ) + MAX_ANGLE_LANE_CHANGE;
        
        Car_Wanted_direction = l_DirectionOfChange;
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        break;

    case D_RIGHT:
        l_DirectionOfChange = (Car_Wanted_Speed * 
                               (MIN_ANGLE_LANE_CHANGE - MAX_ANGLE_LANE_CHANGE) / 
                               DEFUALT_ROBOT_MAX_SPEED ) + MAX_ANGLE_LANE_CHANGE;
        
        Car_Wanted_direction = 360 - l_DirectionOfChange;
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        break;

    case CENTER:
        Car_Wanted_direction = 0;
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        break;
    
    default:
        l_AppStatus = APP_ERROR;
        break;
    }
}




/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
