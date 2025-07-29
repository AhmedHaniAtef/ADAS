/**
 * @file    ecu.h
 * @author  Ahmed Hani
 * @brief   contains all configuation of the ecu layer
 * @date    2024-10-07
 * @note    nan
 */

#ifndef ECU_H_
#define ECU_H_

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../../Core/Inc/main.h"
#include "motor.h"
#include "robot.h"
#include "encoder.h"
#include "CANSPI.h"
#include "MCP2515.h"
#include "monitoring.h"
#include "ultrasonic.h"
#include "mpu.h"
#include "Control.h"
#include "qmc5883l.h"
#include "logic.h"


/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/

#define LAST_ADD_USED_FLASH     (0x0803FFF8U)
#define ADD_LAST_MAX_CAL_SPEED  (LAST_ADD_USED_FLASH)
#define ADD_Kp_VALUE            (LAST_ADD_USED_FLASH - 4)
#define ADD_Ki_VALUE            (LAST_ADD_USED_FLASH - 8)
#define ADD_Kd_VALUE            (LAST_ADD_USED_FLASH - 12)
#define ADD_N_VALUE             (LAST_ADD_USED_FLASH - 16)

#define TIMER_AUTO_RELOAD_VAL   (4199)
#define ROBOT_LENGHT_X          (0.0925)
#define ROBOT_LENGHT_Y          (0.085)
#define RADIUS_WHEEL            (0.03)

#define DEFUALT_Kp_VALUE        (0.85f)
#define DEFUALT_Ki_VALUE        (9.5f)
#define DEFUALT_Kd_VALUE        (0.07f)
#define DEFUALT_N_VALUE         (0.8f)

#define DEFUALT_MIN_PID_OUT     (0.0f)
#define DEFAULT_MAX_PID_OUT     (255.0f)

#define DEFUALT_MOTOR_MAX_SPEED (195.0f)

#define ROBOT_CALIBRATE_UART    (0)
#define ROBOT_CALIBRATE_SPI     (1)
#define ROBOT_CALIBRATE_MANUAL  (2)
#define ROBOT_CALIBRATE_I2C     (3)

#define ROBOT_CALIBRATE_TYPE (ROBOT_CALIBRATE_MANUAL)

#define ENCODER_MAX_COUNTER_VAL (44000)
#define ENCODER_PRESCALER       (4)
#define ENCODER_PULSES_NUMBER   (11)
#define ENCODER_GEAR_RATIO      (40)

#define ENCODER_READ_FILTER_CONST   (0.5f)

#define CAN_MCP2515_CS_PORT     (CAN_CS_GPIO_Port)
#define CAN_MCP2515_CS_PIN      (CAN_CS_Pin)
#define CAN_RX_INTERRUP_ENABLE  (0x03)
#define CAN_RX_INTERRUP_DISABLE (0x00)
#define CAN_RX_INTERRUPT  (CAN_RX_INTERRUP_ENABLE)

/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   EXTERN OBJECTS                                                     *
***********************************************************************************************************************/

extern CRC_HandleTypeDef hcrc;

/* MPU Objects extern */
extern I2C_HandleTypeDef hi2c2;
extern MPU6050_t MPUPin;

/* CAN Objects */
extern Can_t CAN;

/* Monitoring Objects */
/* needs to be initialized (size - callback - data to be sent)*/
extern monitoring_t all;
extern monitoring_t mpu_monitor;
extern monitoring_t ul0;
extern monitoring_t ul45;
extern monitoring_t ul90;
extern monitoring_t ul135;
extern monitoring_t ul180;
extern monitoring_t ul225;
extern monitoring_t ul270;
extern monitoring_t ul315;
extern monitoring_t Wz;
extern monitoring_t mpu_kp;
extern monitoring_t mpu_ki;
extern monitoring_t mpu_kd;
extern monitoring_t mpu_n;
extern monitoring_t mpu_sp;
extern monitoring_t sync;
extern monitoring_t Kf_outYaw;
extern monitoring_t q_bias;
extern monitoring_t q_angle;
extern monitoring_t r_measure;
extern monitoring_t CRC_frame;
extern monitoring_t vx_kp;
extern monitoring_t vx_ki;
extern monitoring_t vx_kd;
extern monitoring_t vx_n;
extern monitoring_t wanted_speed;
extern monitoring_t max_speed;
extern monitoring_t emergency_parking;
extern monitoring_t traffic_sign;
extern monitoring_t traffic_speed;

/* Controller Objects */
extern control_t Controller;

/* Ultrasonic Objects */
extern UltrasonicSensor sensor_0;
extern UltrasonicSensor sensor_45;
extern UltrasonicSensor sensor_90;
extern UltrasonicSensor sensor_135;
extern UltrasonicSensor sensor_180;
extern UltrasonicSensor sensor_225;
extern UltrasonicSensor sensor_270;
extern UltrasonicSensor sensor_315;

extern float_t sensor_0_dis;
extern float_t sensor_45_dis;
extern float_t sensor_90_dis;
extern float_t sensor_135_dis;
extern float_t sensor_180_dis;
extern float_t sensor_225_dis;
extern float_t sensor_270_dis;
extern float_t sensor_315_dis;

/* Logic Objects */
extern logic_t buzzer;
extern logic_t led_left;
extern logic_t led_right;

/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/




/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/


#endif // ECU_H_
