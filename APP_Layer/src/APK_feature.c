/**
 * @file    APK_feature.c
 * @author  Youssef Benyamine
 * @brief   contains the functions which handle the Auto Parking feature
 * @date    2025-05-08
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/

#include "APP.h"
#include "APK_feature.h"

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

float_t FR_Encoder_pos_initial = FR_Encoder_pos;
float_t FL_Encoder_pos_initial = FL_Encoder_pos;

float_t FR_Encoder_pos_wanted = 0.0f;
float_t FL_Encoder_pos_wanted = 0.0f;

float_t FR_Encoder_pos_wanted_back_ward = 0.0f;
float_t FL_Encoder_pos_wanted_back_ward = 0.0f;


float_t Car_Angle = 0.0f;

/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/



/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
 * @brief 
 * 
 * @return app_status_t 
 */
app_status_t APK_Check_Side(APK_t *p_APK)
{
    app_status_t l_AppStatus = APP_OK;
    if(p_APK == NULL)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t l_Distance_ul90  = *(Main_Ultrasonics.UL_90->Distance);
        float_t l_Distance_ul270 = *(Main_Ultrasonics.UL_270->Distance);

        if(l_Distance_ul90 > MIN_DISTANCE_PARKING_Y_AXIS)
        {
            p_APK->Side = RIGHT_PARKING_SIDE;
            l_AppStatus = APP_OK;
        }

        else if(l_Distance_ul270 > MIN_DISTANCE_PARKING_Y_AXIS)
        {
            p_APK->Side = LEFT_PARKING_SIDE;
            l_AppStatus = APP_OK;
        }
        else
        {
            p_APK->Side = NO_PARKING_SIDE;
            l_AppStatus = APP_ERROR;
        }
    }
    return l_AppStatus;
}

/**
 * @brief 
 * 
 * @return app_status_t 
 */
app_status_t APK_Check_Axis(APK_t *p_APK)
{
     app_status_t l_AppStatus = APP_OK;
    if(p_APK == NULL)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t l_Distance_ul90  = *(Main_Ultrasonics.UL_90->Distance);
        float_t l_Distance_ul270 = *(Main_Ultrasonics.UL_270->Distance);
        l_AppStatus |= APK_Check_Side(p_APK);
        if(p_APK->Side == RIGHT_PARKING_SIDE)
        {
            if(l_Distance_ul90 > MIN_DISTANCE_PARKING_X_AXIS)
                p_APK->Axis = AUTOPARKING_ROTATE_AXIS;
            else
                p_APK->Axis = AUTOPARKING_SAME_AXIS;
            l_AppStatus = APP_OK;
        }

        else if(p_APK->Side == LEFT_PARKING_SIDE)
        {
            if(l_Distance_ul270 > MIN_DISTANCE_PARKING_X_AXIS)
                p_APK->Axis = AUTOPARKING_ROTATE_AXIS;
            else
                p_APK->Axis = AUTOPARKING_SAME_AXIS;
            l_AppStatus = APP_OK;
        }
        else
        {
            p_APK->Axis = NO_PARKING_SIDE;
            l_AppStatus = APP_ERROR;
        }
    }
    return l_AppStatus;
}

/**
 * @brief 
 * 
 * @return app_status_t 
 */
app_status_t APK_Task(APK_t *p_APK)
{
    app_status_t l_AppStatus = APP_OK;
    if(p_APK == NULL)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        Car_Wanted_Speed = 0.1f;
        Car_Wanted_direction = 0.0f;       
        l_AppStatus |= APK_Check_Axis(p_APK);
        if(p_APK->Axis == AUTOPARKING_ROTATE_AXIS)
        {
            p_APK->Mode_Parking = MODE_PARKING_UNPARK;
            FR_Encoder_pos_wanted = FR_Encoder_pos_initial + (MIN_DISTANCE_PARKING_Y_AXIS);
            FL_Encoder_pos_wanted = FL_Encoder_pos_initial + (MIN_DISTANCE_PARKING_Y_AXIS);
            if(value_in_range(FR_Encoder_pos, FR_Encoder_pos_wanted, MIN_DISTANCE_PARKING_Y_AXIS) || value_in_range(FL_Encoder_pos, FL_Encoder_pos_wanted, MIN_DISTANCE_PARKING_Y_AXIS))
            {
                Car_Wanted_Angle = 0.0f;
                Car_Wanted_direction = 180.0f;
                FR_Encoder_pos_wanted_back_ward = FR_Encoder_pos - (HALF_MIN_DISTANCE_PARKING_Y_AXIS);
                FL_Encoder_pos_wanted_back_ward = FL_Encoder_pos - (HALF_MIN_DISTANCE_PARKING_Y_AXIS);
                if(value_in_range(FR_Encoder_pos, FR_Encoder_pos_wanted_back_ward, HALF_MIN_DISTANCE_PARKING_Y_AXIS) || value_in_range(FL_Encoder_pos, FL_Encoder_pos_wanted_back_ward, HALF_MIN_DISTANCE_PARKING_Y_AXIS))
                {
                    /* rotate 90 degree clockwise or anti-clockwise */
                    if(p_APK->Side == LEFT_PARKING_SIDE)
                    {                       
                        Car_Wanted_Angle = DEFAULT_PARKING_ROTATE_ANGLE_LEFT;
                        Car_Angle = *(Main_Orientation.FilteredYAW) + DEFAULT_PARKING_ROTATE_ANGLE_LEFT;
                        if(Main_Orientation.FilteredYAW > Car_Angle)
                        {
                            Car_Wanted_Angle = 0.0f;
                            Car_Wanted_direction = 180.0f;
                            if(*(Main_Ultrasonics.UL_0->Distance) < MIN_DISTANCE_CAR_Y_AXIS)
                            {
                                p_APK->Mode_Parking = MODE_PARKING_PARK;
                                
                                /* stop */
                                Car_Wanted_Speed = 0.0f;
                                Car_Wanted_direction = 0.0f;
                            }
                            else
                            {
                                /* continue */
                            }

                        }
                    }
                        
                    else if(p_APK->Side == RIGHT_PARKING_SIDE)
                    {
                        Car_Wanted_Angle = DEFAULT_PARKING_ROTATE_ANGLE_RIGHT;
                        Car_Angle = *(Main_Orientation.FilteredYAW) + DEFAULT_PARKING_ROTATE_ANGLE_RIGHT;
                        if(Main_Orientation.FilteredYAW < Car_Angle)
                        {
                            Car_Wanted_Angle = 0.0f;
                            Car_Wanted_direction = 180.0f;
                            if(*(Main_Ultrasonics.UL_0->Distance) < MIN_DISTANCE_CAR_Y_AXIS)
                            {
                                p_APK->Mode_Parking = MODE_PARKING_PARK;

                                /* stop */
                                Car_Wanted_Speed = 0.0f;
                                Car_Wanted_direction = 0.0f;
                            }
                            else
                            {
                                /* continue */
                            }
                        }
                    }
                }
            }
            else
            {
                /* continue */
            }
        }
        else if(p_APK->Axis == AUTOPARKING_SAME_AXIS)
        {
            p_APK->Mode_Parking = MODE_PARKING_UNPARK;
            FR_Encoder_pos_wanted = FR_Encoder_pos_initial + (MIN_DISTANCE_PARKING_X_AXIS);
            FL_Encoder_pos_wanted = FL_Encoder_pos_initial + (MIN_DISTANCE_PARKING_X_AXIS);
            if(value_in_range(FR_Encoder_pos, FR_Encoder_pos_wanted, MIN_DISTANCE_PARKING_X_AXIS) || value_in_range(FL_Encoder_pos, FL_Encoder_pos_wanted, MIN_DISTANCE_PARKING_X_AXIS))
            {
                Car_Wanted_Angle = 0.0f;
                Car_Wanted_direction = 180.0f; 
                FR_Encoder_pos_wanted_back_ward = FR_Encoder_pos - (HALF_MIN_DISTANCE_PARKING_X_AXIS);
                FL_Encoder_pos_wanted_back_ward = FL_Encoder_pos - (HALF_MIN_DISTANCE_PARKING_X_AXIS);
                if(value_in_range(FR_Encoder_pos, FR_Encoder_pos_wanted_back_ward, HALF_MIN_DISTANCE_PARKING_X_AXIS) || value_in_range(FL_Encoder_pos, FL_Encoder_pos_wanted_back_ward, HALF_MIN_DISTANCE_PARKING_X_AXIS))
                {
                    /* move in the same axis left or right */
                    if(p_APK->Side == LEFT_PARKING_SIDE)
                    {
                        Car_Wanted_direction = 270.0f;
                        if(*(Main_Ultrasonics.UL_270->Distance) < MIN_DISTANCE_CAR_X_AXIS)
                        {
                            p_APK->Mode_Parking = MODE_PARKING_PARK;
                            /* stop */
                            Car_Wanted_Speed = 0.0f;
                            Car_Wanted_direction = 0.0f;
                        }
                        else
                        {
                            /* continue */
                        }
                    }
                        
                    else if(p_APK->Side == RIGHT_PARKING_SIDE)
                    {
                        Car_Wanted_direction = 90.0f;
                        if(*(Main_Ultrasonics.UL_90->Distance) < MIN_DISTANCE_CAR_X_AXIS)
                        {
                            p_APK->Mode_Parking = MODE_PARKING_PARK;
                            /* stop */
                            Car_Wanted_Speed = 0.0f;
                            Car_Wanted_direction = 0.0f;
                        }
                        else
                        {
                            /* continue */
                        }
                    }
                } 
            }
            else
            {
                /* continue */
            }
        }
        else // 
        {
            p_APK->Mode_Parking    = MODE_PARKING_SEARCH;
            FR_Encoder_pos_initial = FR_Encoder_pos;
            FL_Encoder_pos_initial = FL_Encoder_pos;
            l_AppStatus = APP_ERROR;
        }
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
    }
    return l_AppStatus;
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


