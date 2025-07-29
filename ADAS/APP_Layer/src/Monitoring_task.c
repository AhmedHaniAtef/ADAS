/**
 * @file    Monitoring_task.c
 * @author  Ahmed Hani
 * @brief   contains task of Monitoring 
 * @date    2025-02-08
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "APP.h"
#include "Monitoring_task.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                               STATIC FUNCTION DEFINITION                                             *
***********************************************************************************************************************/
static void update(void);




/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/






/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/
static uint32_t temp_monitor_buffer[25];
static uint32_t sync_bytes = 0xF0F0F0F0;
static uint32_t CRC_Value = 0;






/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
 * @brief initialize the monitor update task 
 * @param p_AllMonitoredValues pointer to the all monitored values
 * @return app_status_t 
 */
app_status_t monitor_task_init(monitor_values_t *p_AllMonitoredValues)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_AllMonitoredValues)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        p_AllMonitoredValues->M_sync->Size          = 4;
        p_AllMonitoredValues->M_crc->Size           = 4;
        p_AllMonitoredValues->all->Size             = 40;
        p_AllMonitoredValues->M_mpu->Size           = 4;
        p_AllMonitoredValues->M_ul0->Size           = 4;
        p_AllMonitoredValues->M_ul45->Size          = 4;
        p_AllMonitoredValues->M_ul90->Size          = 4;
        p_AllMonitoredValues->M_ul135->Size         = 4;
        p_AllMonitoredValues->M_ul180->Size         = 4;
        p_AllMonitoredValues->M_ul225->Size         = 4;
        p_AllMonitoredValues->M_ul270->Size         = 4;
        p_AllMonitoredValues->M_ul315->Size         = 4;
        p_AllMonitoredValues->M_Wz->Size            = 4;
        p_AllMonitoredValues->M_mpu_kp->Size        = 4;
        p_AllMonitoredValues->M_mpu_ki->Size        = 4;
        p_AllMonitoredValues->M_mpu_kd->Size        = 4;
        p_AllMonitoredValues->M_mpu_n->Size         = 4;
        p_AllMonitoredValues->M_mpu_sp->Size        = 4;
        p_AllMonitoredValues->M_Kf_Yaw->Size        = 4;
        p_AllMonitoredValues->M_Kf_qBias->Size      = 4;
        p_AllMonitoredValues->M_Kf_qAngle->Size     = 4;
        p_AllMonitoredValues->M_Kf_rMeasure->Size   = 4;
        p_AllMonitoredValues->M_vx_kp->Size         = 4;       
        p_AllMonitoredValues->M_vx_ki->Size         = 4;       
        p_AllMonitoredValues->M_vx_kd->Size         = 4;           
        p_AllMonitoredValues->M_vx_n->Size          = 4;
        p_AllMonitoredValues->M_wanted_speed->Size  = 4;           
        p_AllMonitoredValues->M_max_speed->Size     = 4;   
        p_AllMonitoredValues->Emergency_Parking->Size = 4;
        p_AllMonitoredValues->Traffic_Sign->Size    = 4;
        p_AllMonitoredValues->Traffic_Speed->Size   = 4;      
        


        p_AllMonitoredValues->M_sync->Data.OriginalData             = &sync_bytes;
        p_AllMonitoredValues->M_crc->Data.OriginalData              = &CRC_Value;
        p_AllMonitoredValues->all->Data.OriginalData                = &temp_monitor_buffer;
        p_AllMonitoredValues->M_mpu->Data.OriginalData              = &Main_MPU.YAW;
        p_AllMonitoredValues->M_ul0->Data.OriginalData              = Main_Ultrasonics.UL_0->Distance;
        p_AllMonitoredValues->M_ul45->Data.OriginalData             = Main_Ultrasonics.UL_45->Distance;
        p_AllMonitoredValues->M_ul90->Data.OriginalData             = Main_Ultrasonics.UL_90->Distance;
        p_AllMonitoredValues->M_ul135->Data.OriginalData            = Main_Ultrasonics.UL_135->Distance;
        p_AllMonitoredValues->M_ul180->Data.OriginalData            = Main_Ultrasonics.UL_180->Distance;
        p_AllMonitoredValues->M_ul225->Data.OriginalData            = Main_Ultrasonics.UL_225->Distance;
        p_AllMonitoredValues->M_ul270->Data.OriginalData            = Main_Ultrasonics.UL_270->Distance;
        p_AllMonitoredValues->M_ul315->Data.OriginalData            = Main_Ultrasonics.UL_315->Distance;
        p_AllMonitoredValues->M_Wz->Data.OriginalData               = &Omega_z;
        p_AllMonitoredValues->M_mpu_kp->Data.OriginalData           = &Main_Orientation.PID.Kp;
        p_AllMonitoredValues->M_mpu_ki->Data.OriginalData           = &Main_Orientation.PID.Ki;
        p_AllMonitoredValues->M_mpu_kd->Data.OriginalData           = &Main_Orientation.PID.Kd;
        p_AllMonitoredValues->M_mpu_n->Data.OriginalData            = &Main_Orientation.PID.N;
        p_AllMonitoredValues->M_mpu_sp->Data.OriginalData           = &Car_Wanted_Angle;
        p_AllMonitoredValues->M_Kf_Yaw->Data.OriginalData           = &Main_Orientation.FilteredYAW;
        p_AllMonitoredValues->M_Kf_qBias->Data.OriginalData         = &Main_Orientation.Kf_YAW.Q_angle;
        p_AllMonitoredValues->M_Kf_qAngle->Data.OriginalData        = &Main_Orientation.Kf_YAW.Q_bias;
        p_AllMonitoredValues->M_Kf_rMeasure->Data.OriginalData      = &Main_Orientation.Kf_YAW.R_measure;
        p_AllMonitoredValues->M_vx_kp->Data.OriginalData            = &Main_Orientation.PID.Kp;
        p_AllMonitoredValues->M_vx_ki->Data.OriginalData            = &Main_Orientation.PID.Ki;
        p_AllMonitoredValues->M_vx_kd->Data.OriginalData            = &Main_Orientation.PID.Kd;
        p_AllMonitoredValues->M_vx_n->Data.OriginalData             = &Main_Orientation.PID.N;
        p_AllMonitoredValues->M_wanted_speed->Data.OriginalData     = &Car_Wanted_Speed;
        p_AllMonitoredValues->M_max_speed->Data.OriginalData        = &Car_Max_Forced_Speed;
        p_AllMonitoredValues->Emergency_Parking->Data.OriginalData  = &Emergency_Parking_Flag;
        p_AllMonitoredValues->Traffic_Sign->Data.OriginalData       = &Traffic_Sign;
        p_AllMonitoredValues->Traffic_Speed->Data.OriginalData      = &Traffic_Speed;

        p_AllMonitoredValues->all->MonitorUpdateData_CALLBACK = update;
    }
    return l_AppStatus;
}

/**
 * @brief monitor update task 
 * @param p_AllMonitoredValues pointer to the all monitored values
 * @return app_status_t 
 */
app_status_t monitor_update_task(monitor_values_t *p_AllMonitoredValues)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_AllMonitoredValues)
    {
        l_AppStatus = APP_ERROR;
    }   
    else
    {
        memcpy(&temp_monitor_buffer[0],  p_AllMonitoredValues->M_mpu->Data.SendData,    4);
        memcpy(&temp_monitor_buffer[1],  p_AllMonitoredValues->M_Kf_Yaw->Data.SendData, 4);
        memcpy(&temp_monitor_buffer[2],  p_AllMonitoredValues->M_mpu_sp->Data.SendData, 4);
        memcpy(&temp_monitor_buffer[3],  p_AllMonitoredValues->M_ul0->Data.SendData,    4);
        memcpy(&temp_monitor_buffer[4],  p_AllMonitoredValues->M_ul45->Data.SendData,   4);
        memcpy(&temp_monitor_buffer[5],  p_AllMonitoredValues->M_ul90->Data.SendData,   4);
        memcpy(&temp_monitor_buffer[6],  p_AllMonitoredValues->M_ul135->Data.SendData,  4);
        memcpy(&temp_monitor_buffer[7],  p_AllMonitoredValues->M_ul180->Data.SendData,  4);
        memcpy(&temp_monitor_buffer[8],  p_AllMonitoredValues->M_ul225->Data.SendData,  4);
        memcpy(&temp_monitor_buffer[9],  p_AllMonitoredValues->M_ul270->Data.SendData,  4);
        memcpy(&temp_monitor_buffer[10], p_AllMonitoredValues->M_ul315->Data.SendData,  4);
        memcpy(&temp_monitor_buffer[11], p_AllMonitoredValues->M_wanted_speed->Data.SendData,   4);        
        memcpy(&temp_monitor_buffer[12], p_AllMonitoredValues->M_max_speed->Data.SendData,   4);    
        memcpy(&temp_monitor_buffer[13], p_AllMonitoredValues->Emergency_Parking->Data.SendData, 4);    
        memcpy(&temp_monitor_buffer[14], p_AllMonitoredValues->Traffic_Sign->Data.SendData, 4);
        memcpy(&temp_monitor_buffer[15], p_AllMonitoredValues->Traffic_Speed->Data.SendData, 4);

        /* Reset CRC Unit */
        __HAL_CRC_DR_RESET(&hcrc);

        /* Calculate local CRC */
        for (uint8_t counter = 0; counter < 1; counter++)
        {

            CRC_Value = HAL_CRC_Accumulate(&hcrc, &temp_monitor_buffer[counter], 16);
        }
        memset(temp_monitor_buffer, 0, 100);


        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_sync);

        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_mpu);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_Kf_Yaw);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_mpu_sp);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_ul0);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_ul45);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_ul90);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_ul135);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_ul180);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_ul225);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_ul270);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_ul315);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_wanted_speed);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_max_speed);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->Emergency_Parking);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->Traffic_Sign);
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->Traffic_Speed);
        
        l_AppStatus |= monitoring_send_data(p_AllMonitoredValues->M_crc);
        


    }
    return l_AppStatus;
}



/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/

static void update(void)
{
    one_float_conv temp;
    temp.value = Main_MPU.YAW;
    memcpy(&temp_monitor_buffer[0], temp.data, 4);
    temp.value = sensor_0_dis;
    memcpy(&temp_monitor_buffer[1], temp.data, 4);
    temp.value = sensor_45_dis;
    memcpy(&temp_monitor_buffer[2], temp.data, 4);
    temp.value = sensor_90_dis;
    memcpy(&temp_monitor_buffer[3], temp.data, 4);
    temp.value = sensor_135_dis;
    memcpy(&temp_monitor_buffer[4], temp.data, 4);
    temp.value = sensor_180_dis;
    memcpy(&temp_monitor_buffer[5], temp.data, 4);
    temp.value = sensor_225_dis;
    memcpy(&temp_monitor_buffer[6], temp.data, 4);
    temp.value = sensor_270_dis;
    memcpy(&temp_monitor_buffer[7], temp.data, 4);
    temp.value = sensor_315_dis;
    memcpy(&temp_monitor_buffer[9], temp.data, 4);
    temp.value = Omega_z;
    memcpy(&temp_monitor_buffer[10], temp.data, 4);
}



/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
