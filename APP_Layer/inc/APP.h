/**
 * @file    APP.h
 * @author  Ahmed Hani
 * @brief   contains all configuation of the APP layer
 * @date    2025-02-07
 * @note    nan
 */

#ifndef APP_H_
#define APP_H_

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../../ECU_Layer/inc/ecu.h"
#include "CAN_task.h"
#include "MPU_task.h"
#include "Ultrasonic_task.h"
#include "Monitoring_task.h"
#include "Messages_Callbacks.h"
#include "Control_task.h"
#include "Orientation_task.h"
#include "ACC_feature.h"
#include "TSR_feature.h"


/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/

#define MPU_PID_Kp          (-0.0207f)
#define MPU_PID_Kd          (1.4399999f)
#define MPU_PID_Ki          (0.0f)
#define MPU_PID_N           (1.0299999f)
// #define MPU_PID_Kp          (-0.02f)
// #define MPU_PID_Kd          (-100.0f)
// #define MPU_PID_Ki          (-5e-8f)
// #define MPU_PID_N           (10000.0f)
//#define MPU_PID_Kp          (-0.025f)
//#define MPU_PID_Kd          (0.03f)
//#define MPU_PID_Ki          (0.0f)
//#define MPU_PID_N           (0.0f)
#define MPU_PID_TIME_STEP   (100.0f)
#define MPU_PID_MIN_OUT     (-10.0f)
#define MPU_PID_MAX_OUT     (10.0f)


#define ROBOT_STOP_FLAG                 (0xFF)
#define DEFUALT_ROBOT_MAX_RADIUS        (0.5f)
#define DEFUALT_ROBOT_MAX_SPEED         (0.55f)
#define DEFUALT_ROBOT_MAX_ANGULAR_SPEED (3.0f)

#define OUTLIER_WINDOW_SIZE     (5)
#define OUTLIER_THRESHOLD       (400.0f)
#define ULTRASONIC_MAX_READING  (400.0f)

#define ACC_MIN_DISTANCE        (50.0f)
#define ACC_MARGIN              (3.0f)
#define Vx_Kp                   (1.5f)
#define Vx_Ki                   (0.0f)
#define Vx_Kd                   (21.0f)
#define Vx_N                    (22.8f)
#define Vx_MAX_OUT              (1.0f)
#define Vx_MIN_OUT              (-1.0f)
#define Vy_Kp                   (100.0f)
#define Vy_Ki                   (0.5f)
#define Vy_Kd                   (0.8f)
#define Vy_N                    (100.0f)
#define Vy_MAX_OUT              (0.6f)
#define Vy_MIN_OUT              (-0.6f)
#define PID_MIN_SP              (20.0f)

#define SPEED_30_PERCENTAGE      (20.0f)
#define SPEED_40_PERCENTAGE      (30.0f)
#define SPEED_50_PERCENTAGE      (40.0f)
#define SPEED_60_PERCENTAGE      (50.0f)
#define SPEED_70_PERCENTAGE      (60.0f)
#define SPEED_80_PERCENTAGE      (70.0f)
#define SPEED_90_PERCENTAGE      (80.0f)
#define SPEED_100_PERCENTAGE     (90.0f)
#define SPEED_120_PERCENTAGE     (100.0f)
/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   EXTERN OBJECTS                                                     *
***********************************************************************************************************************/

/* speeds - angles - etc objects */
extern float_t Car_Wanted_Angle;
extern float_t Car_Wanted_Speed;
extern float_t Car_Max_Forced_Angle;
extern float_t Car_Max_Forced_Speed;
extern float_t Car_Wanted_direction;
extern float_t Car_Wanted_Angular_Speed;
extern float_t Car_Wanted_Rotate_Radius;
extern float_t CompasAngle;
extern float_t MpuGz;


/* Orientation PID */
extern float_t Omega_z;

/* CAN Bus Objects */
extern CAN_bus_t Main_CAN;

/* Ultrasonic Objects */
extern ultrasonics_t Main_Ultrasonics;

/* MPU Objects */
extern mpu_t Main_MPU;

/* Monitoring Objects */
extern monitor_values_t Main_Monitor_values;

/* Controller objects */
extern controller_t Main_Controller;

/* Orientation objects */
extern orientation_t Main_Orientation;

/* Adaptive cruise control */
extern ACC_t ACC_Object;
/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/
typedef union 
{
    float value;
    uint8_t data[4];
}one_float_conv;

typedef union 
{
    float value[2];
    uint8_t data[8];
}two_float_conv;



/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/




/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/


#endif // APP_H_
