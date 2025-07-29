/**
 * @file    APP.c
 * @author  Ahmed Hani
 * @brief   contains all configuation of the APP layer
 * @date    2025-02-07
 * @note    nan
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../inc/APP.h"



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
int test = 0;
/* speeds - angles - etc objects */
float_t Car_Wanted_Angle = 0.0f;
float_t Car_Wanted_Speed = 0.0f;
float_t Car_Max_Forced_Angle = 0.0f;
float_t Car_Max_Forced_Speed = 0.6f;
float_t Car_Wanted_direction = 0.0f;
float_t Car_Wanted_Angular_Speed = 0.0f;
float_t Car_Wanted_Rotate_Radius = 0.0f;
float_t CompasAngle = 0.0f;
float_t MpuGz = 0.0f;

/* Orientation PID */
float_t Omega_z = 0.0f;

/* CAN Bus Objects */
CAN_bus_t Main_CAN =
{
    .UsedCAN = &CAN,
};

/* Ultrasonic Objects */
ultrasonics_t Main_Ultrasonics =
{
    .UL_0 = &sensor_0,
    .UL_45 = &sensor_45,
    .UL_90 = &sensor_90,
    .UL_135 = &sensor_135,
    .UL_180 = &sensor_180,
    .UL_225 = &sensor_225,
    .UL_270 = &sensor_270,
    .UL_315 = &sensor_315,
};

/* MPU Objects */
mpu_t Main_MPU =
{
    .mpu = &MPUPin,
    .UsedI2C = &hi2c2,
    .YAW = 0,
};

/* Monitoring Objects */
monitor_values_t Main_Monitor_values =
{
    .M_crc          = &CRC_frame,
    .M_sync         = &sync,
    .all            = &all,
    .M_mpu          = &mpu_monitor,
    .M_ul0          = &ul0 ,
    .M_ul45         = &ul45 ,
    .M_ul90         = &ul90 ,
    .M_ul135        = &ul135,
    .M_ul180        = &ul180,
    .M_ul225        = &ul225,
    .M_ul270        = &ul270,
    .M_ul315        = &ul315,
    .M_Wz           = &Wz,
    .M_mpu_kp       = &mpu_kp,
    .M_mpu_ki       = &mpu_ki,
    .M_mpu_kd       = &mpu_kd,
    .M_mpu_n        = &mpu_n,
    .M_mpu_sp       = &mpu_sp,
    .M_Kf_Yaw       = &Kf_outYaw,
    .M_Kf_qBias     = &q_bias,
    .M_Kf_qAngle    = &q_angle,
    .M_Kf_rMeasure  = &r_measure,
    .M_vx_kp        = &vx_kp,
    .M_vx_ki        = &vx_ki,
    .M_vx_kd        = &vx_kd,
    .M_vx_n         = &vx_n,  
    .M_wanted_speed = &wanted_speed,
    .M_max_speed    = &max_speed,
    .Emergency_Parking = &emergency_parking,
    .Traffic_Sign   = &traffic_sign,
    .Traffic_Speed  = &traffic_speed,
};

/* Controller objects */
controller_t Main_Controller =
{
    .UsedController = &Controller,
    .UP_button_clb = NULL,
    .DOWN_button_clb = NULL,
    .LEFT_button_clb = NULL,
    .RIGHT_button_clb = NULL,
    .SELECT_button_clb = NULL,
    .START_button_clb = NULL,
    .LEFT_STICK_button_clb = NULL,
    .RITHT_STICK_button_clb = NULL,
    .CompassAnglePTR = &CompasAngle,
};

/* Orientation objects */
orientation_t Main_Orientation =
{
    .CompassYAW = &CompasAngle,
    .dt = 0.01f,
    .FilteredYAW = 0.0f,
    .GyroBias = 0.0f,
    .MpuWz = &MpuGz,
};

/* Adaptive cruise control */
ACC_t ACC_Object =
{
    .dt = 100,
    .Front_UL = &sensor_0_dis,
    .Left_UL = &sensor_315_dis,
    .Right_UL = &sensor_45_dis,
};

/* Auto Parking objects */
float_t FL_Encoder_pos = 0.0f;
float_t FR_Encoder_pos = 0.0f;
APK_t APK_Object;

/* Blind Spot Detection Objects*/
BSD_t BSD_Object = 
{
    .Buzzer = &buzzer,
    .LeftLed = &led_left,
    .RightLed = &led_right,
};

/* Driver Drowsines System Object */
uint32_t Emergency_Parking_Flag = 0;

/* Traffic Sign Detection Objects */
uint32_t Traffic_Speed = 0;
uint32_t Traffic_Sign = 0;

/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/






/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/





/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/




/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
