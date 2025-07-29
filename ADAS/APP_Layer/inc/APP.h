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
#include "BSD_feature.h"
#include "ALC_feature.h"
#include "APK_feature.h"


/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/

#define MPU_PID_Kp          (-0.018f)
#define MPU_PID_Kd          (0.5f)
#define MPU_PID_Ki          (0.0f)
#define MPU_PID_N           (0.0f)
// #define MPU_PID_Kp          (-0.02f)
// #define MPU_PID_Kd          (-100.0f)
// #define MPU_PID_Ki          (-5e-8f)
// #define MPU_PID_N           (10000.0f)
//#define MPU_PID_Kp          (-0.025f)
//#define MPU_PID_Kd          (0.03f)
//#define MPU_PID_Ki          (0.0f)
//#define MPU_PID_N           (0.0f)
#define MPU_PID_TIME_STEP   (0.01f)
#define MPU_PID_MIN_OUT     (-10.0f)
#define MPU_PID_MAX_OUT     (10.0f)


#define ROBOT_STOP_FLAG                 (0xFF)
#define DEFUALT_ROBOT_MAX_RADIUS        (0.5f)
#define DEFUALT_ROBOT_MAX_SPEED         (0.45f)
#define DEFUALT_ROBOT_MAX_ANGULAR_SPEED (15.0f)

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

#define SPEED_40_PERCENTAGE      (30.0f / 100.0f)
#define SPEED_30_PERCENTAGE      (20.0f / 100.0f)
#define SPEED_50_PERCENTAGE      (40.0f / 100.0f)
#define SPEED_60_PERCENTAGE      (50.0f / 100.0f)
#define SPEED_70_PERCENTAGE      (60.0f / 100.0f)
#define SPEED_80_PERCENTAGE      (70.0f / 100.0f)
#define SPEED_90_PERCENTAGE      (80.0f / 100.0f)
#define SPEED_100_PERCENTAGE     (90.0f / 100.0f)
#define SPEED_120_PERCENTAGE     (100.0f / 100.0f)

#define MIN_DISTANCE    (5.0f)

#define MIN_ANGLE_LANE_CHANGE (5.0f)
#define MAX_ANGLE_LANE_CHANGE (45.0f)

#define MIN_DISTANCE_PARKING_X_AXIS (50.0f)   // in cm
#define MIN_DISTANCE_PARKING_Y_AXIS (30.0f)   // in cm

#define HALF_MIN_DISTANCE_PARKING_X_AXIS (25.f)   // in cm
#define HALF_MIN_DISTANCE_PARKING_Y_AXIS (15.f)   // in cm

#define AUTOPARKING_SAME_AXIS    (0.0f)    // in same axis
#define AUTOPARKING_ROTATE_AXIS  (1.0f)    // in rotate axis

#define PARKING_ROTATE_CLOCKWISE     (0.0f)    // +90
#define PARKING_ROTATE_ANTI_CLOCKWISE (1.0f)    // -90

#define RIGHT_PARKING_SIDE       (0.0f)    // in right side
#define LEFT_PARKING_SIDE        (1.0f)    // in left side
#define NO_PARKING_SIDE          (2.0F)    // no space to parking

#define MODE_PARKING_SEARCH      (0.0f)    // search for parking
#define MODE_PARKING_UNPARK      (1.0f)    // unpark from the parking
#define MODE_PARKING_PARK        (2.0f)    // park in the parking

#define DEFAULT_PARKING_ROTATE_ANGLE_LEFT  (-90.0f)
#define DEFAULT_PARKING_ROTATE_ANGLE_RIGHT (90.0f)

#define MIN_DISTANCE_CAR_X_AXIS (10.0f)   // in cm
#define MIN_DISTANCE_CAR_Y_AXIS (10.0f)   // in cm
/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   EXTERN OBJECTS                                                     *
***********************************************************************************************************************/
extern int test;
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

/* Auto Parking objects */
extern float_t FL_Encoder_pos;
extern float_t FR_Encoder_pos;
extern APK_t APK_Object;

/* Blind Spot Detection Objects*/
extern BSD_t BSD_Object;

/* Driver Drowsines System Object */
extern uint32_t Emergency_Parking_Flag;

/* Traffic Sign Detection Objects */
extern uint32_t Traffic_Speed;
extern uint32_t Traffic_Sign;


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
