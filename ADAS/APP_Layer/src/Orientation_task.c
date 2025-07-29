/**
 * @file    Orientation_task.c
 * @author  Ahmed Hani
 * @brief   contains task of Orientaion control 
 * @date    2025-02-17
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "APP.h"
#include "Orientation_task.h"



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

uint8_t FlagInit = 0;
float_t YawInitial = 0.0f;
float_t previousAzimuth = 0.0f;



/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
  * @brief initialize the task of orientation
  * @param p_Orientaion pointer to the data descripes the current orientation
  * @return app_status_t status of operation
  */
app_status_t Orientation_task_init(orientation_t *p_Orientaion)
{
    app_status_t l_AppStatus = APP_OK;
    if ((NULL == p_Orientaion))
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        PID_Init(&p_Orientaion->PID, 
                 MPU_PID_Kp, 
                 MPU_PID_Ki, 
                 MPU_PID_Kd, 
                 MPU_PID_N, 
                 MPU_PID_TIME_STEP, 
                 MPU_PID_MIN_OUT, 
                 MPU_PID_MAX_OUT);
       
        Kalman_Init(&p_Orientaion->Kf_YAW);
    }
    return l_AppStatus;
}

/**
  * @brief calibrate the gyro bias of mpu
  * @param p_Orientaion pointer to the data descripes the current orientation
  * @return app_status_t status of operation
  * @note this task should be repeated number of times with specific time constant to perform calibration
  */
app_status_t Orientation_Gyro_calibration(orientation_t *p_Orientaion)
{
    app_status_t l_AppStatus = APP_OK;
    if ((NULL == p_Orientaion))
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        p_Orientaion->GyroBias += *(p_Orientaion->MpuWz);
    }
    return l_AppStatus;
}

/**
  * @brief PID controller using current orientaion of the car
  * @param p_Orientaion pointer to the data descripes the current orientation
  * @param p_Wz Return the value of PID Controller output
  * @param p_SetPoint set point of the angle i want
  * @return app_status_t status of operation
  */
app_status_t Orientation_PID_task(orientation_t *p_Orientaion, float_t *p_Wz, float_t p_SetPoint)
{
    app_status_t l_AppStatus = APP_OK;
    if ((NULL == p_Orientaion) || (NULL == p_Wz))
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t temp = 0;
        temp = PID_Compute(&p_Orientaion->PID, p_SetPoint, p_Orientaion->FilteredYAW);
        temp = temp *10.0f;
        temp = (temp > (floor(temp)+0.5f)) ? ceil(temp) : floor(temp);
        temp = temp / -10.0f;
        *p_Wz = temp;
    }
    return l_AppStatus;
}

/**
 * @brief perform kalman filter on the orientation angle
 * 
 * @param p_Orientaion pointer to the orientation
 * @return app_status_t status of operation
 */
app_status_t Orientation_Kf_task(orientation_t *p_Orientaion)
{
    app_status_t l_AppStatus = APP_OK;
    if ((NULL == p_Orientaion))
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t l_GyroZ = *p_Orientaion->MpuWz - p_Orientaion->GyroBias;
        float_t l_Filtered = Kalman_Update(&p_Orientaion->Kf_YAW, 
                                                  *p_Orientaion->CompassYAW, 
                                                  l_GyroZ, 
                                                  p_Orientaion->dt);

        p_Orientaion->FilteredYAW = (float_t)(trunc(l_Filtered));
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
