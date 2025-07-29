/**
 * @file    Monitoring_task.h
 * @author  Ahmed Hani
 * @brief   contains task of Monitoring 
 * @date    2025-02-08
 */

#ifndef MONITORING_TASK_H_
#define MONITORING_TASK_H_
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
    monitoring_t *M_crc;
    monitoring_t *M_sync;
    monitoring_t *all;
    monitoring_t *M_mpu;
    monitoring_t *M_ul0;
    monitoring_t *M_ul45;
    monitoring_t *M_ul90;
    monitoring_t *M_ul135;
    monitoring_t *M_ul180;
    monitoring_t *M_ul225;
    monitoring_t *M_ul270;
    monitoring_t *M_ul315;
    monitoring_t *M_Wz;
    monitoring_t *M_mpu_kp;
    monitoring_t *M_mpu_ki;
    monitoring_t *M_mpu_kd;
    monitoring_t *M_mpu_n;
    monitoring_t *M_mpu_sp;
    monitoring_t *M_Kf_Yaw;
    monitoring_t *M_Kf_qBias;
    monitoring_t *M_Kf_qAngle;
    monitoring_t *M_Kf_rMeasure;
    monitoring_t *M_vx_kp;
    monitoring_t *M_vx_ki;    
    monitoring_t *M_vx_kd;
    monitoring_t *M_vx_n; 
    monitoring_t *M_wanted_speed;
    monitoring_t *M_max_speed;   
    monitoring_t *Emergency_Parking;
    monitoring_t *Traffic_Sign;
    monitoring_t *Traffic_Speed;
}monitor_values_t;




/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief initialize the monitor update task 
 * @param p_AllMonitoredValues pointer to the all monitored values
 * @return app_status_t 
 */
app_status_t monitor_task_init(monitor_values_t *p_AllMonitoredValues);

/**
 * @brief monitor update task 
 * @param p_AllMonitoredValues pointer to the all monitored values
 * @return app_status_t 
 */
app_status_t monitor_update_task(monitor_values_t *p_AllMonitoredValues);




/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif //MONITORING_TASK_H_
