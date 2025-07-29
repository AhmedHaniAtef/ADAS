/**
 * @file    Orientation_task.h
 * @author  Ahmed Hani
 * @brief   contains task of Orientaion control 
 * @date    2025-02-17
 */

 #ifndef ORIENTATION_TASK_H_
 #define ORIENTATION_TASK_H_
 /***********************************************************************************************************************
 *                                                      INCLUDES                                                        *
 ***********************************************************************************************************************/
 #include "../../ECU_Layer/inc/ecu.h"
 #include "../../lib/inc/KalmanFilter.h"
 #include "../APP_std.h"
 
 
 
 /***********************************************************************************************************************
 *                                                    MACRO DEFINES                                                     *
 ***********************************************************************************************************************/
 
 
 
 
 /***********************************************************************************************************************
 *                                                   MACRO FUNCTIONS                                                    *
 ***********************************************************************************************************************/
 
 
 
 
 /***********************************************************************************************************************
 *                                                      DATA TYPES                                                      *
 ***********************************************************************************************************************/
 
 typedef struct 
 {
    KalmanFilter Kf_YAW;
    PID_Controller PID;
    float_t *MpuWz;
    float_t *CompassYAW;
    float_t FilteredYAW;
    float_t GyroBias;
    float_t dt;
 }orientation_t;
 

 
 
 /***********************************************************************************************************************
 *                                                  FUNCTION DEFINITION                                                 *
 ***********************************************************************************************************************/

/**
  * @brief initialize the task of orientation
  * @param p_Orientaion pointer to the data descripes the current orientation
  * @return app_status_t status of operation
  */
 app_status_t Orientation_task_init(orientation_t *p_Orientaion);

/**
  * @brief calibrate the gyro bias of mpu
  * @param p_Orientaion pointer to the data descripes the current orientation
  * @return app_status_t status of operation
  * @note this task should be repeated number of times with specific time constant to perform calibration
  */
 app_status_t Orientation_Gyro_calibration(orientation_t *p_Orientaion);

/**
 * @brief perform kalman filter on the orientation angle
 * 
 * @param p_Orientaion pointer to the orientation
 * @return app_status_t status of operation
 */
app_status_t Orientation_Kf_task(orientation_t *p_Orientaion);

 /**
  * @brief PID controller using current orientaion of the car
  * @param p_Orientaion pointer to the data descripes the current orientation
  * @param p_Wz Return the value of PID Controller output
  * @param p_SetPoint set point of the angle i want
  * @return app_status_t status of operation
  */
app_status_t Orientation_PID_task(orientation_t *p_Orientaion, float_t *p_Wz, float_t p_SetPoint);
 
 
 /***********************************************************************************************************************
 * AUTHOR                |* NOTE                                                                                        *
 ************************************************************************************************************************
 *                       |                                                                                              * 
 *                       |                                                                                              * 
 ***********************************************************************************************************************/
 
 #endif //ORIENTATION_TASK_H_
 