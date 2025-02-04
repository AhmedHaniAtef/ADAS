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

robot_t ADAS_ROBOT =
{
	.FL.Encoder.SelectedTimer = &htim1,
	.FR.Encoder.SelectedTimer = &htim2,
	.RL.Encoder.SelectedTimer = &htim3,
	.RR.Encoder.SelectedTimer = &htim5,

	.FL.Encoder.WheelRadius = 3.0f,
	.FR.Encoder.WheelRadius = 3.0f,
	.RL.Encoder.WheelRadius = 3.0f,
	.RR.Encoder.WheelRadius = 3.0f,

	.FL.Motor.GpioxMotor[0]   = FL_1_GPIO_Port,
	.FL.Motor.GpioxMotor[1]   = FL_2_GPIO_Port,
	.FL.Motor.GpioPinMotor[0] = FL_1_Pin,
	.FL.Motor.GpioPinMotor[1] = FL_2_Pin,
	.FL.Motor.SelectedTimer = &htim4,
	.FL.Motor.SelectedChannel = TIM_CHANNEL_3,

	.FR.Motor.GpioxMotor[0]   = FR_1_GPIO_Port,
	.FR.Motor.GpioxMotor[1]   = FR_2_GPIO_Port,
	.FR.Motor.GpioPinMotor[0] = FR_1_Pin,
	.FR.Motor.GpioPinMotor[1] = FR_2_Pin,
	.FR.Motor.SelectedTimer = &htim4,
	.FR.Motor.SelectedChannel = TIM_CHANNEL_4,

	.RL.Motor.GpioxMotor[0]   = RL_1_GPIO_Port,
	.RL.Motor.GpioxMotor[1]   = RL_2_GPIO_Port,
	.RL.Motor.GpioPinMotor[0] = RL_1_Pin,
	.RL.Motor.GpioPinMotor[1] = RL_2_Pin,
	.RL.Motor.SelectedTimer = &htim9,
	.RL.Motor.SelectedChannel = TIM_CHANNEL_1,

	.RR.Motor.GpioxMotor[0]   = RR_1_GPIO_Port,
	.RR.Motor.GpioxMotor[1]   = RR_2_GPIO_Port,
	.RR.Motor.GpioPinMotor[0] = RR_1_Pin,
	.RR.Motor.GpioPinMotor[1] = RR_2_Pin,
	.RR.Motor.SelectedTimer = &htim9,
	.RR.Motor.SelectedChannel = TIM_CHANNEL_2,
};


motor_t zeft =
{
	 .GpioxMotor[0]   = FL_1_GPIO_Port,
	 .GpioxMotor[0]   = FL_1_GPIO_Port,
	 .GpioxMotor[1]   = FL_2_GPIO_Port,
	 .GpioPinMotor[0] = FL_1_Pin,
	 .GpioPinMotor[1] = FL_2_Pin,
	 .SelectedTimer = &htim4,
	 .SelectedChannel = TIM_CHANNEL_1,
};

encoder_t encoder_test =
{
	.SelectedTimer = &htim1,
	.WheelRadius = 3,
};

PID_Controller PID;

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
