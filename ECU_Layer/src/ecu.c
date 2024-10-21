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

robot_t Robot =
{
	.FL.Motor.SelectedChannel = TIM_CHANNEL_1,
	.FL.Motor.SelectedTimer = &htim4,
	.FL.Motor.GpioxMotor[0] = Motor1_IN1_GPIO_Port,
	.FL.Motor.GpioxMotor[1] = Motor1_IN2_GPIO_Port,
	.FL.Motor.GpioPinMotor[0] = Motor1_IN1_Pin,
	.FL.Motor.GpioPinMotor[1] = Motor1_IN2_Pin,

	.FR.Motor.SelectedChannel = TIM_CHANNEL_1,
	.FR.Motor.SelectedTimer = &htim3,
	.FR.Motor.GpioxMotor[0] = Motor2_IN1_GPIO_Port,
	.FR.Motor.GpioxMotor[1] = Motor2_IN2_GPIO_Port,
	.FR.Motor.GpioPinMotor[0] = Motor2_IN1_Pin,
	.FR.Motor.GpioPinMotor[1] = Motor2_IN2_Pin,

	.RL.Motor.SelectedChannel = TIM_CHANNEL_3,
	.RL.Motor.SelectedTimer = &htim4,
	.RL.Motor.GpioxMotor[0] = Motor3_IN1_GPIO_Port,
	.RL.Motor.GpioxMotor[1] = Motor3_IN2_GPIO_Port,
	.RL.Motor.GpioPinMotor[0] = Motor3_IN1_Pin,
	.RL.Motor.GpioPinMotor[1] = Motor3_IN2_Pin,

	.RR.Motor.SelectedChannel = TIM_CHANNEL_4,
	.RR.Motor.SelectedTimer = &htim4,
	.RR.Motor.GpioxMotor[0] = Motor4_IN1_GPIO_Port,
	.RR.Motor.GpioxMotor[1] = Motor4_IN2_GPIO_Port,
	.RR.Motor.GpioPinMotor[0] = Motor4_IN1_Pin,
	.RR.Motor.GpioPinMotor[1] = Motor4_IN2_Pin,
};

motor_t MotorFrontLeft =
{
	.SelectedChannel = TIM_CHANNEL_1,
	.SelectedTimer = &htim4,
	.GpioxMotor[0] = Motor1_IN1_GPIO_Port,
	.GpioxMotor[1] = Motor1_IN2_GPIO_Port,
	.GpioPinMotor[0] = Motor1_IN1_Pin,
	.GpioPinMotor[1] = Motor1_IN2_Pin,
};

motor_t MotorFrontRight =
{
	.SelectedChannel = TIM_CHANNEL_2,
	.SelectedTimer = &htim4,
	.GpioxMotor[0] = Motor2_IN1_GPIO_Port,
	.GpioxMotor[1] = Motor2_IN2_GPIO_Port,
	.GpioPinMotor[0] = Motor2_IN1_Pin,
	.GpioPinMotor[1] = Motor2_IN2_Pin,
};

motor_t MotorRearLeft =
{
	.SelectedChannel = TIM_CHANNEL_3,
	.SelectedTimer = &htim4,
	.GpioxMotor[0] = Motor3_IN1_GPIO_Port,
	.GpioxMotor[1] = Motor3_IN2_GPIO_Port,
	.GpioPinMotor[0] = Motor3_IN1_Pin,
	.GpioPinMotor[1] = Motor3_IN2_Pin,
};

motor_t MotorRearRight =
{
	.SelectedChannel = TIM_CHANNEL_4,
	.SelectedTimer = &htim4,
	.GpioxMotor[0] = Motor4_IN1_GPIO_Port,
	.GpioxMotor[1] = Motor4_IN2_GPIO_Port,
	.GpioPinMotor[0] = Motor4_IN1_Pin,
	.GpioPinMotor[1] = Motor4_IN2_Pin,
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
