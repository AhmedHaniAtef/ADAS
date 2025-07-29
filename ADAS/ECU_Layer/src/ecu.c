/**
 * @file    ecu.C
 * @author  Ahmed Hani
 * @brief   contains all configuation of the ecu layer
 * @date    2024-10-07 
 * @note    nan
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../inc/ecu.h"
#include "../../Core/Inc/gpio.h"
#include "../../Core/Inc/tim.h"
#include "../../Core/Inc/main.h"
#include "../../core/Inc/spi.h"
#include "../../core/Inc/usart.h"
#include "../../Core/Inc/i2c.h"



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

/* MPU Objects*/
MPU6050_t MPUPin =
{
	.GPIOx = MPU_RESET_GPIO_Port,
	.GPIO_Pin = MPU_RESET_Pin,
};

/* CAN Objects */
Can_t CAN = {
	.Speed = MCP_8MHz_1000kBPS,
	.UsedSPI = &hspi1,
};

/* Monitoring Objects */
/* needs to be initialized (size - callback - data to be sent)*/

monitoring_t mpu_monitor = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t all =
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t ul0 = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t ul45 = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t ul90 = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t ul135 = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t ul180 = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t ul225 = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t ul270 = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t ul315 = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t Wz = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t mpu_kp = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t mpu_ki = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t mpu_kd = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t mpu_n = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t mpu_sp =
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t sync =
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t CRC_frame = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t Kf_outYaw = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t q_bias = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t q_angle = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t r_measure = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t vx_kp = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t vx_ki = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t vx_kd = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t vx_n = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t wanted_speed = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t max_speed = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t emergency_parking = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t traffic_sign = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

monitoring_t traffic_speed = 
{
    .Size = 0,
    .UsedUART = &huart2,
    .MonitorUpdateData_CALLBACK = NULL,
    .Data.OriginalData = NULL,
};

/* Controller Objects */
control_t Controller =
{
    .UsedUART = &huart1,
    .Data = {0},
};

/* Ultrasonic Objects */

float_t sensor_0_dis   = 0.0f;
float_t sensor_45_dis  = 0.0f;
float_t sensor_90_dis  = 0.0f;
float_t sensor_135_dis = 0.0f;
float_t sensor_180_dis = 0.0f;
float_t sensor_225_dis = 0.0f;
float_t sensor_270_dis = 0.0f;
float_t sensor_315_dis = 0.0f;

UltrasonicSensor sensor_0 =
{
    .Channel = TIM_CHANNEL_1,
    .Distance = &sensor_0_dis,
    .htim = &htim3,
    .TRIG_PIN = UL_TRIGGER_X_Pin,
    .TRIG_PORT = UL_TRIGGER_X_GPIO_Port,
};

UltrasonicSensor sensor_45 =
{
    .Channel = TIM_CHANNEL_2,
    .Distance = &sensor_45_dis,
    .htim = &htim3,
    .TRIG_PIN = UL_TRIGGER_U_Pin,
    .TRIG_PORT =UL_TRIGGER_U_GPIO_Port,
};

UltrasonicSensor sensor_90 =
{
    .Channel = TIM_CHANNEL_3,
    .Distance = &sensor_90_dis,
    .htim = &htim3,
    .TRIG_PIN = UL_TRIGGER_Y_Pin,
    .TRIG_PORT = UL_TRIGGER_Y_GPIO_Port,
};

UltrasonicSensor sensor_135 =
{
    .Channel = TIM_CHANNEL_4,
    .Distance = &sensor_135_dis,
    .htim = &htim3,
    .TRIG_PIN = UL_TRIGGER_V_Pin,
    .TRIG_PORT = UL_TRIGGER_V_GPIO_Port,
};

UltrasonicSensor sensor_180 =
{
    .Channel = TIM_CHANNEL_1,
    .Distance = &sensor_180_dis,
    .htim = &htim4,
    .TRIG_PIN = UL_TRIGGER_X_Pin,
    .TRIG_PORT = UL_TRIGGER_X_GPIO_Port,
};

UltrasonicSensor sensor_225 =
{
    .Channel = TIM_CHANNEL_2,
    .Distance = &sensor_225_dis,
    .htim = &htim4,
    .TRIG_PIN = UL_TRIGGER_U_Pin,
    .TRIG_PORT =UL_TRIGGER_U_GPIO_Port,
};

UltrasonicSensor sensor_270 =
{
    .Channel = TIM_CHANNEL_3,
    .Distance = &sensor_270_dis,
    .htim = &htim4,
    .TRIG_PIN = UL_TRIGGER_Y_Pin,
    .TRIG_PORT = UL_TRIGGER_Y_GPIO_Port,
};

UltrasonicSensor sensor_315 =
{
    .Channel = TIM_CHANNEL_4,
    .Distance = &sensor_315_dis,
    .htim = &htim4,
    .TRIG_PIN = UL_TRIGGER_V_Pin,
    .TRIG_PORT = UL_TRIGGER_V_GPIO_Port,
};


logic_t buzzer =
{
    .logic_Port = BUZZER_GPIO_Port,
    .logic_pin = BUZZER_Pin,
};

logic_t led_left =
{
    .logic_Port = LED_LEFT_GPIO_Port,
    .logic_pin = LED_LEFT_Pin,
};

logic_t led_right =
{
    .logic_Port = LED_RIGHT_GPIO_Port,
    .logic_pin = LED_RIGHT_Pin,
};

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
