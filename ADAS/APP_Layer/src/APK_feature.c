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

float_t FR_Encoder_pos_initial = 0.0f;
float_t FL_Encoder_pos_initial = 0.0f;
float_t FR_Encoder_pos_wanted = 0.0f;
float_t FL_Encoder_pos_wanted = 0.0f;
float_t FR_Encoder_pos_wanted_back_ward = 0.0f;
float_t FL_Encoder_pos_wanted_back_ward = 0.0f;
float_t FR_Encoder_pos_bci = 0.0f;
float_t FL_Encoder_pos_bci = 0.0f;
static float_t Angle_Wanted = 0 ;
static uint8_t Min_Distance_Flag = 0 ; // 0 -> not complete search   1-> complete search
static uint8_t Parking_Flag = 0 ;
static uint8_t Reset_Flag = 0 ;
static uint8_t stopbeforerotate = 0 ;
static uint8_t stopafterrotate = 0 ;
static uint8_t RotateFlag = 0 ;

float_t Car_Angle_initial = 0.0f;

uint8_t bmlor = 0 ; // befor move left or right flag

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
            p_APK->Mode_Parking = MODE_PARKING_SEARCH;
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
            p_APK->Mode_Parking = MODE_PARKING_SEARCH;
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
        Car_Wanted_Speed = 0.15f;
        Car_Wanted_direction = 0.0f;  
        if(Reset_Flag == 1)
        {
            p_APK->Mode_Parking = MODE_PARKING_SEARCH;
            FR_Encoder_pos_initial = 0.0f;
            FL_Encoder_pos_initial = 0.0f;
            FR_Encoder_pos_wanted = 0.0f;
            FL_Encoder_pos_wanted = 0.0f;
            FR_Encoder_pos_wanted_back_ward = 0.0f;
            FL_Encoder_pos_wanted_back_ward = 0.0f;
            FR_Encoder_pos_bci = 0.0f;
            FL_Encoder_pos_bci = 0.0f;

            Angle_Wanted = 0 ;
            Min_Distance_Flag = 0 ;
            Parking_Flag = 0 ;
            Reset_Flag = 0 ;
            stopbeforerotate = 0 ;
            stopafterrotate = 0 ;
            RotateFlag = 0 ;

            Car_Angle_initial = 0.0f;
            bmlor = 0 ; 
        }
         
        if(Parking_Flag == 0)  
            l_AppStatus |= APK_Check_Axis(p_APK);
        if(p_APK->Axis == AUTOPARKING_ROTATE_AXIS)
        {
            p_APK->Mode_Parking = MODE_PARKING_SEARCH;
            if(Min_Distance_Flag == 0)
            {
                FR_Encoder_pos_wanted = FR_Encoder_pos_initial + (MIN_DISTANCE_PARKING_Y_AXIS);
                FL_Encoder_pos_wanted = FL_Encoder_pos_initial + (MIN_DISTANCE_PARKING_Y_AXIS);
                FL_Encoder_pos_bci = FL_Encoder_pos;
                FR_Encoder_pos_bci = FR_Encoder_pos;
            }       
            if((FL_Encoder_pos > FL_Encoder_pos_wanted) || (FR_Encoder_pos > FR_Encoder_pos_wanted) || (Min_Distance_Flag == 1))
            {
                Min_Distance_Flag = 1 ;
                p_APK->Mode_Parking = MODE_PARKING_UNPARK;
                
                Car_Wanted_direction = 180.0f;
                FR_Encoder_pos_wanted_back_ward = FR_Encoder_pos_bci - (HALF_MIN_DISTANCE_PARKING_Y_AXIS);
                FL_Encoder_pos_wanted_back_ward = FL_Encoder_pos_bci - (HALF_MIN_DISTANCE_PARKING_Y_AXIS);               
                if((FL_Encoder_pos < FL_Encoder_pos_wanted_back_ward) || (FR_Encoder_pos < FR_Encoder_pos_wanted_back_ward) || (bmlor == 1))
                {
                    bmlor = 1 ;
                    Parking_Flag = 1;
                    if(stopbeforerotate == 0)
                    {
                        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_stop);
                        stopbeforerotate = 1;
                        HAL_Delay(1000);
                    }
                    /* rotate 90 degree clockwise or anti-clockwise */
                    if(p_APK->Side == LEFT_PARKING_SIDE)
                    {                       
                        Car_Wanted_Angle = Car_Angle_initial + DEFAULT_PARKING_ROTATE_ANGLE_LEFT;
                        Car_Wanted_Speed=0;
                        RotateFlag = 1;
                        //if((Main_Orientation.FilteredYAW) < Car_Wanted_Angle)
                        if(value_in_range((Main_Orientation.FilteredYAW) , Car_Wanted_Angle-2 , Car_Wanted_Angle+2) || (stopafterrotate == 1))
                        {       
                            stopafterrotate = 1;
                            RotateFlag = 0;                     
                            Car_Wanted_Speed = 0.15f;
                            Car_Wanted_direction = 180.0f;
                            if(*(Main_Ultrasonics.UL_180->Distance) < MIN_DISTANCE_CAR_Y_AXIS)
                            {
                                /* stop */
                                Car_Wanted_Speed = 0.0f;
                                Car_Wanted_direction = 0.0f;
                                Reset_Flag = 1;
                                p_APK->Mode_Parking = MODE_PARKING_PARK;
                                l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_stop);  
                            }
                            else
                            {
                                /* continue */
                            }
                        }
                    }
                        
                    else if(p_APK->Side == RIGHT_PARKING_SIDE)
                    {
                        Car_Wanted_Angle = Car_Angle_initial + DEFAULT_PARKING_ROTATE_ANGLE_RIGHT; 
                        Car_Wanted_Speed=0;  
                        RotateFlag = 1;
                        //if((Main_Orientation.FilteredYAW) > Car_Wanted_Angle)
                        if(value_in_range((Main_Orientation.FilteredYAW) , Car_Wanted_Angle-2 , Car_Wanted_Angle+2) || (stopafterrotate == 1))
                        {       
                            stopafterrotate = 1;
                            RotateFlag = 0;
                            
                            Car_Wanted_direction = 180.0f;
                            Car_Wanted_Speed = 0.15f;
                            if(*(Main_Ultrasonics.UL_180->Distance) < MIN_DISTANCE_CAR_Y_AXIS)
                            {
                                /* stop */
                               
                                Car_Wanted_Speed = 0.0f;
                                Car_Wanted_direction = 0.0f;
                                Reset_Flag = 1;
                                p_APK->Mode_Parking = MODE_PARKING_PARK;
                                l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_stop);  
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
        }
        else if(p_APK->Axis == AUTOPARKING_SAME_AXIS)
        {
            p_APK->Mode_Parking = MODE_PARKING_UNPARK;
            if(Min_Distance_Flag == 0)
            {
                FR_Encoder_pos_wanted = FR_Encoder_pos_initial + (MIN_DISTANCE_PARKING_X_AXIS);
                FL_Encoder_pos_wanted = FL_Encoder_pos_initial + (MIN_DISTANCE_PARKING_X_AXIS);
                FL_Encoder_pos_bci = FL_Encoder_pos;
                FR_Encoder_pos_bci = FR_Encoder_pos;
            }
            if((FR_Encoder_pos > FR_Encoder_pos_wanted) || (FL_Encoder_pos > FL_Encoder_pos_wanted) || (Min_Distance_Flag == 1))
            {
                Min_Distance_Flag = 1;

                Car_Wanted_direction = 180.0f; 
                FR_Encoder_pos_wanted_back_ward = FR_Encoder_pos_bci - (HALF_MIN_DISTANCE_PARKING_X_AXIS);
                FL_Encoder_pos_wanted_back_ward = FL_Encoder_pos_bci - (HALF_MIN_DISTANCE_PARKING_X_AXIS);
                if((FR_Encoder_pos < FR_Encoder_pos_wanted_back_ward) || (FL_Encoder_pos < FL_Encoder_pos_wanted_back_ward) || (bmlor == 1))
                {
                    bmlor = 1 ;
                    Parking_Flag = 1;
                    /* move in the same axis left or right */
                    if(p_APK->Side == LEFT_PARKING_SIDE)
                    {
                        Car_Wanted_direction = 90.0f;
                        if(*(Main_Ultrasonics.UL_270->Distance) < MIN_DISTANCE_CAR_X_AXIS)
                        {
                            /* stop */
                            Car_Wanted_Speed = 0.0f;
                            Car_Wanted_direction = 0.0f;
                            Reset_Flag = 1;
                            p_APK->Mode_Parking = MODE_PARKING_PARK;
                        }
                        else
                        {
                            /* continue */
                        }
                    }
                        
                    else if(p_APK->Side == RIGHT_PARKING_SIDE)
                    {
                        Car_Wanted_direction = 270.0f;
                        if(*(Main_Ultrasonics.UL_90->Distance) < MIN_DISTANCE_CAR_X_AXIS)
                        {
                            /* stop */
                            Car_Wanted_Speed = 0.0f;
                            Car_Wanted_direction = 0.0f;
                            Reset_Flag = 1;
                            p_APK->Mode_Parking = MODE_PARKING_PARK;   
                                                  
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
                p_APK->Mode_Parking = MODE_PARKING_SEARCH;
                /* continue */
            }
        }
        else // 
        {
            p_APK->Mode_Parking    = MODE_PARKING_SEARCH;
            FR_Encoder_pos_initial = FR_Encoder_pos;
            FL_Encoder_pos_initial = FL_Encoder_pos;
            Angle_Wanted = 0 ;
            Car_Angle_initial = (Main_Orientation.FilteredYAW);
            l_AppStatus = APP_ERROR;
        }        
    }
    if(RotateFlag == 0)
        l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
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


