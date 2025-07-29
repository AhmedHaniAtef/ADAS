/**
 * @file    MPU_task.h
 * @author  Ahmed Hani
 * @brief   contains task of MPU 
 * @date    2025-02-08
 */

#ifndef MPU_TASK_H_
#define MPU_TASK_H_
/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../../ECU_Layer/inc/ecu.h"
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
    I2C_HandleTypeDef *UsedI2C;
    MPU6050_t *mpu;
    float_t YAW;
}mpu_t;


/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief initialize the task of MPU
 * @param p_UsedMPU pointer to the used MPU
 * @return app_status_t status of operation
 */
app_status_t MPU_task_init(mpu_t *p_UsedMPU);

/**
 * @brief  task of MPU which update the values (specially YAW)
 * @param p_UsedMPU pointer to the used MPU
 * @return app_status_t status of operation
 */
app_status_t MPU_update_task(mpu_t *p_UsedMPU);

/**
 * @brief PID controller using MPU
 * @param p_UsedMPU pointer to the used MPU
 * @param p_Wz Return the value of PID Controller output
 * @param p_SetPoint set point of the angle i want
 * @return app_status_t status of operation
 */
app_status_t MPU_PID_task(mpu_t *p_UsedMPU, float_t *p_Wz, float_t p_SetPoint);


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif //MPU_TASK_H_
