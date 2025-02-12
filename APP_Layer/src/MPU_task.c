/**
 * @file    MPU_task.c
 * @author  Ahmed Hani
 * @brief   contains task of MPU 
 * @date    2025-02-08
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "APP.h"
#include "MPU_task.h"



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
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/






/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
 * @brief initialize the task of MPU
 * @param p_UsedMPU pointer to the used MPU
 * @return app_status_t status of operation
 */
app_status_t MPU_task_init(mpu_t *p_UsedMPU)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_UsedMPU)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        PID_Init(&p_UsedMPU->PID, 
                 MPU_PID_Kp, 
                 MPU_PID_Ki, 
                 MPU_PID_Kd, 
                 MPU_PID_N, 
                 MPU_PID_TIME_STEP, 
                 MPU_PID_MIN_OUT, 
                 MPU_PID_MAX_OUT);
        ecu_status_t l_EcuStatus = MPU6050_Init(p_UsedMPU->UsedI2C, p_UsedMPU->mpu);
        if (l_EcuStatus != ECU_OK)
        {
            l_AppStatus = APP_ERROR;
        }
    }
    return l_AppStatus;
}

/**
 * @brief  task of MPU which update the values (specially YAW)
 * @param p_UsedMPU pointer to the used MPU
 * @return app_status_t status of operation
 */
app_status_t MPU_update_task(mpu_t *p_UsedMPU)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_UsedMPU)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        ecu_status_t l_EcuStatus = MPU6050_Read_All(p_UsedMPU->UsedI2C, p_UsedMPU->mpu);
        if (l_EcuStatus != ECU_OK)
        {
            l_AppStatus = APP_ERROR;
        }
        else
        {
            p_UsedMPU->YAW = p_UsedMPU->mpu->Yaw;
        }
    }
    return l_AppStatus;
}

/**
 * @brief PID controller using MPU
 * @param p_UsedMPU pointer to the used MPU
 * @param p_Wz Return the value of PID Controller output
 * @param p_SetPoint set point of the angle i want
 * @return app_status_t status of operation
 */
app_status_t MPU_PID_task(mpu_t *p_UsedMPU, float_t *p_Wz, float_t p_SetPoint)
{
    app_status_t l_AppStatus = APP_OK;
    if ((NULL == p_UsedMPU) || (NULL == p_Wz))
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t temp = 0;
        temp = PID_Compute(&p_UsedMPU->PID, p_SetPoint, p_UsedMPU->YAW);
        temp = temp * 100.0f;
        temp = (temp > (floor(temp)+0.5f)) ? ceil(temp) : floor(temp);
        temp = temp / 100.0f;
        *p_Wz = temp;
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