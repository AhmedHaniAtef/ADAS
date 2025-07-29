/**
 * @file    ACC_feature.c
 * @author  Ahmed Hani
 * @brief   contains the functions which handle the Adaptive Cruise Control feature
 * @date    2025-02-23
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "APP.h"
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

/**
 * @brief this task perfom the suitable action depending on the situation of the car
 * 
 * @param p_ACC pointer to the object used
 * @return app_status_t status of operation
 */
static app_status_t ACC_action_task_x(ACC_t *p_ACC, float_t *p_RetVelocity, float_t p_DisSetPoint);

/**
 * @brief this task perfom the suitable action depending on the situation of the car
 * 
 * @param p_ACC pointer to the object used
 * @return app_status_t status of operation
 */
static app_status_t ACC_action_task_y_neg(ACC_t *p_ACC, float_t *p_RetVelocity);

/**
 * @brief this task perfom the suitable action depending on the situation of the car
 * 
 * @param p_ACC pointer to the object used
 * @return app_status_t status of operation
 */
static app_status_t ACC_action_task_y_pos(ACC_t *p_ACC, float_t *p_RetVelocity);


/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/






/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/


static float_t FeatureMaxSpeed[5] = {DEFUALT_ROBOT_MAX_SPEED,
                                     DEFUALT_ROBOT_MAX_SPEED,
                                     DEFUALT_ROBOT_MAX_SPEED,
                                     DEFUALT_ROBOT_MAX_SPEED,
                                     DEFUALT_ROBOT_MAX_SPEED};

/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
 * @brief this task is periodically called to detect the obstacles and the realative speeds
 * 
 * @param p_ACC pointer to the object used
 * @return app_status_t status of operation
 */
app_status_t ACC_relative_task(ACC_t *p_ACC)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_ACC)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t l_Vx = 0.0f;

        float_t l_SetPointDistance = 50.0f * min(Car_Wanted_Speed, Car_Max_Forced_Speed) + PID_MIN_SP;

        l_AppStatus |= ACC_action_task_x(p_ACC, &l_Vx, l_SetPointDistance);

        if ((value_in_range(Car_Wanted_direction, 0, 45) || value_in_range(Car_Wanted_direction, 315, 360))
            && (Car_Wanted_Speed != 0))
        {
            Car_Max_Forced_Speed = Max_speed_responsible(Car_Max_Forced_Speed - l_Vx, ACC);
            l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
        }
        else
        {
            Car_Max_Forced_Speed = DEFUALT_ROBOT_MAX_SPEED;
        }
    }
    return l_AppStatus;
}

/**
 * @brief initialize the task of ACC
 * 
 * @param p_ACC pointer to the object used
 * @return app_status_t status of operation
 */
app_status_t ACC_task_init(ACC_t *p_ACC)
{
    app_status_t l_AppStatus = APP_OK;
    if ((NULL == p_ACC))
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        PID_Init(&p_ACC->PID_Vx, Vx_Kp, Vx_Ki, Vx_Kd, Vx_N, p_ACC->dt, Vx_MIN_OUT, Vx_MAX_OUT);
        PID_Init(&p_ACC->PID_Vy, Vy_Kp, Vy_Ki, Vy_Kd, Vy_N, p_ACC->dt, Vy_MIN_OUT, Vy_MAX_OUT);
    }
    return l_AppStatus;
}

/**
 * @brief this function should be called when a feature want to limit the speed of the car
 * 
 * @param p_MaxSpeed the limited max speed 
 * @param p_ID the id of request
 * @return the maximum speed should the car move with
 */
float_t Max_speed_responsible(float_t p_MaxSpeed, Max_Speed_ID_t p_ID)
{
    float_t l_MaxSpeed = DEFUALT_ROBOT_MAX_SPEED;
    if (p_MaxSpeed < 0)
    	FeatureMaxSpeed[p_ID] = 0.0f;
    else if (p_MaxSpeed > DEFUALT_ROBOT_MAX_SPEED)
    	FeatureMaxSpeed[p_ID] = DEFUALT_ROBOT_MAX_SPEED;
    else
    	FeatureMaxSpeed[p_ID] = p_MaxSpeed;

    for (uint8_t i = 0; i < 5; i++)
    {
        if (l_MaxSpeed >= FeatureMaxSpeed[i])
            l_MaxSpeed = FeatureMaxSpeed[i];
    }
    return l_MaxSpeed;
}


/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/

/**
 * @brief this task perfom the suitable action depending on the situation of the car
 * 
 * @param p_ACC pointer to the object used
 * @return app_status_t status of operation
 */
static app_status_t ACC_action_task_x(ACC_t *p_ACC, float_t *p_RetVelocity, float_t p_DisSetPoint)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_ACC)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t l_Vx = PID_Compute(&p_ACC->PID_Vx, p_DisSetPoint/100, *p_ACC->Front_UL/100);
        // p_ACC->PID_Vx_Output = l_Vx * 0.1;
        // *p_RetVelocity = l_Vx * 0.1;
        p_ACC->PID_Vx_Output = l_Vx;
        *p_RetVelocity = l_Vx;
    }
    return l_AppStatus;
}

/**
 * @brief this task perfom the suitable action depending on the situation of the car
 * 
 * @param p_ACC pointer to the object used
 * @return app_status_t status of operation
 */
static app_status_t ACC_action_task_y_neg(ACC_t *p_ACC, float_t *p_RetVelocity)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_ACC)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t l_HorizontalDIs = -1.0f * (*p_ACC->Left_UL) * cos(45);
        float_t l_Vy = PID_Compute(&p_ACC->PID_Vy, PID_MIN_SP,  l_HorizontalDIs);
        *p_RetVelocity = l_Vy * 0.1;
    }
    return l_AppStatus;
}

/**
 * @brief this task perfom the suitable action depending on the situation of the car
 * 
 * @param p_ACC pointer to the object used
 * @return app_status_t status of operation
 */
static app_status_t ACC_action_task_y_pos(ACC_t *p_ACC, float_t *p_RetVelocity)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_ACC)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t l_HorizontalDIs = (*p_ACC->Left_UL) * cos(45);
        float_t l_Vy = PID_Compute(&p_ACC->PID_Vy, PID_MIN_SP, l_HorizontalDIs);
        *p_RetVelocity = l_Vy  * 0.1;
    }
    return l_AppStatus;
}



/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
