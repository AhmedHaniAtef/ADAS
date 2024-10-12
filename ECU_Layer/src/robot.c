/**
 * @file    robot.c
 * @author  Ahmed Hani & Youssef
 * @brief	this driver provides interfaces to move the robot and controll its move
 * @date    2024-10-11
 * @note	see noted at end of file
 */


/* Driver deals with robot with this shape and directions*/
/*
this is for robot strafe and rotation
									     X+VE
										  |    speed
										  |   /
										  |0 /
										 _|^/
                      					| |/|
						 Y-ve ----------|-+-|---------- Y+ve
										|_|_|
										  |
										  |
										  |
										  |
										 X-VE

										 origin: center of the robot Z-axis
										 angel 0: between X+ve and vectore of speed
										 so if angle == 0 then it will move forward
										 and if angle != 0 will measured Vx and Vy

*/




/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../inc/ecu.h"
#include "../inc/robot.h"


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





/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/
static float_t Kp = DEFUALT_Kp_VALUE;
static float_t Ki = DEFUALT_Ki_VALUE;
static float_t Kd = DEFUALT_Kd_VALUE;



/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
  *
  * @brief This function move the robot depending on angle and speed
  *
  * @param p_Robot pointer to robot object
  * @param p_Angle angle of robot of motor
  * @param p_Speed speed of motor
  * @return ecu_status_t status of the operation
 */
ecu_status_t robot_move(robot_t *p_Robot , uint16_t p_Angle , float_t p_Speed)
{
	ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_Robot)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
    	float_t l_AngularVelocityFrontLeft  = 0;   // Wfl
    	float_t l_AngularVelocityFrontRight = 0;   // Wfr
    	float_t l_AngularVelocityRearLeft   = 0;   // Wrl
    	float_t l_AngularVelocityRearRight  = 0;   // Wrr
    	float_t l_LinearVelocityXAxis       = 0;   // Vx
    	float_t l_LinearVelocityYAxis       = 0;   // Vy
    	float_t l_AngularVelocityRobotBase  = 0;   // Wz

		// convert velocity and angle to x and y velocity
    	l_LinearVelocityXAxis = p_Speed * cos(p_Angle);
    	l_LinearVelocityYAxis = p_Speed * sin(p_Angle);

		// calculate the speed of each wheel
    	l_AngularVelocityFrontLeft  = ((1.0 / RADIUS_WHEEL) * (l_LinearVelocityXAxis - l_LinearVelocityYAxis -
    								  ((ROBOT_LENGHT_X + ROBOT_LENGHT_Y) * l_AngularVelocityRobotBase)));
    	l_AngularVelocityFrontRight = ((1.0 / RADIUS_WHEEL) * (l_LinearVelocityXAxis + l_LinearVelocityYAxis +
    								  ((ROBOT_LENGHT_X + ROBOT_LENGHT_Y) * l_AngularVelocityRobotBase)));
    	l_AngularVelocityRearLeft   = ((1.0 / RADIUS_WHEEL) * (l_LinearVelocityXAxis + l_LinearVelocityYAxis -
    								  ((ROBOT_LENGHT_X + ROBOT_LENGHT_Y) * l_AngularVelocityRobotBase)));
    	l_AngularVelocityRearRight  = ((1.0 / RADIUS_WHEEL) * (l_LinearVelocityXAxis - l_LinearVelocityYAxis +
    								  ((ROBOT_LENGHT_X + ROBOT_LENGHT_Y) * l_AngularVelocityRobotBase)));

		// store the speed of each wheel
    	p_Robot->FL.Speed = fabs(RPS_TO_RPM(l_AngularVelocityFrontLeft ));
    	p_Robot->FR.Speed = fabs(RPS_TO_RPM(l_AngularVelocityFrontRight));
    	p_Robot->RL.Speed = fabs(RPS_TO_RPM(l_AngularVelocityRearLeft  ));
    	p_Robot->RR.Speed = fabs(RPS_TO_RPM(l_AngularVelocityRearRight ));

		// determine the direction of Front Left motor rotation then store and move it 
    	if(l_AngularVelocityFrontLeft < 0)
    	{
    		p_Robot->FL.Direction = BACWARD;
    		l_EcuStatus = motor_move_backward(&p_Robot->FL.Motor , p_Robot->FL.Speed);
    	}
    	else
    	{
    		p_Robot->FL.Direction = FORWARD;
    		l_EcuStatus = motor_move_forward(&p_Robot->FL.Motor , p_Robot->FL.Speed);
    	}
		// determine the direction of Front Right motor rotation then store and move it 
    	if(l_AngularVelocityFrontRight < 0)
    	{
    	    p_Robot->FR.Direction = BACWARD;
    	    l_EcuStatus = motor_move_backward(&p_Robot->FR.Motor , p_Robot->FR.Speed);
    	}
    	else
    	{
    	    p_Robot->FR.Direction = FORWARD;
    	    l_EcuStatus = motor_move_forward(&p_Robot->FR.Motor , p_Robot->FR.Speed);
    	}
		// determine the direction of Rear Left motor rotation then store and move it 
    	if(l_AngularVelocityRearLeft < 0)
    	{
    	    p_Robot->RL.Direction = BACWARD;
    	    l_EcuStatus = motor_move_backward(&p_Robot->RL.Motor , p_Robot->RL.Speed);
    	}
    	else
    	{
    	    p_Robot->RL.Direction = FORWARD;
    	    l_EcuStatus = motor_move_forward(&p_Robot->RL.Motor , p_Robot->RL.Speed);
    	}
		// determine the direction of Rear Right motor rotation then store and move it 
    	if(l_AngularVelocityRearRight < 0)
    	{
    	    p_Robot->RR.Direction = BACWARD;
    	    l_EcuStatus = motor_move_backward(&p_Robot->RR.Motor , p_Robot->RR.Speed);
    	}
    	else
    	{
    	    p_Robot->RR.Direction = FORWARD;
    	    l_EcuStatus = motor_move_forward(&p_Robot->RR.Motor , p_Robot->RR.Speed);
    	}
    }
	return l_EcuStatus;
}

/**
 * @brief initialize each wheel (motor) of the robot
 * 
 * @param p_Robot pointer to the robot object needed to be initialized
 * @return ecu_status_t status of the operation
 */
ecu_status_t robot_init(robot_t *p_Robot)
{
	ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_Robot)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
		/* initialize wheels of the robot (front left) */
		l_EcuStatus |= motor_init(&p_Robot->FL.Motor);
		p_Robot->FL.Speed = 0.0;
		p_Robot->FL.Direction = STOPED;
		/* initialize wheels of the robot (front right) */
		l_EcuStatus |= motor_init(&p_Robot->FR.Motor);
		p_Robot->FR.Speed = 0.0;
		p_Robot->FR.Direction = STOPED;
		/* initialize wheels of the robot (rear left) */
		l_EcuStatus |= motor_init(&p_Robot->RL.Motor);
		p_Robot->RL.Speed = 0.0;
		p_Robot->RL.Direction = STOPED;
		/* initialize wheels of the robot (rear right) */
		l_EcuStatus |= motor_init(&p_Robot->RR.Motor);
		p_Robot->RR.Speed = 0.0;
		p_Robot->RR.Direction = STOPED;

		/* restore the PID constants from flash */
		Kp = *((volatile float_t *)(ADD_Kp_VALUE));
		Ki = *((volatile float_t *)(ADD_Ki_VALUE));
		Kd = *((volatile float_t *)(ADD_Kd_VALUE));
	}
	return l_EcuStatus;
}

/**
 * @brief this function calibrates the motors of the robot by making it move forward - backward - left - right then
          find the PID gain constants (all of this could be done manually by editing ecu.h if you need)
 * 
 * @param p_Robot pointer to the robot object needed to be calibrated
 * @return ecu_status_t status of the operation
 */
ecu_status_t robot_calibrate(robot_t *p_Robot)
{
	ecu_status_t l_EcuStatus = ECU_OK;
	HAL_StatusTypeDef l_HalStatus = HAL_OK;
	__attribute__((unused)) float_t l_MaxSpeed = 0;
    if (NULL == p_Robot)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
		/* perform wheels and PID calibration*/
#if   (ROBOT_CALIBRATE_TYPE == ROBOT_CALIBRATE_UART)
		/* NOT IMPLEMENTED YET */

#elif (ROBOT_CALIBRATE_TYPE == ROBOT_CALIBRATE_SPI)
		/* NOT IMPLEMENTED YET */
	
#elif (ROBOT_CALIBRATE_TYPE == ROBOT_CALIBRATE_MANUAL)
		/* perform motor calibration*/
		float_t l_MaxSpeed = (float_t)(DEFUALT_MOTOR_MAX_SPEED);
		l_EcuStatus = motor_calibrate(&l_MaxSpeed);
		/* perfom PID calibtation*/
		Kp = DEFUALT_Kp_VALUE;
		Ki = DEFUALT_Ki_VALUE;
		Kd = DEFUALT_Kd_VALUE;

#elif (ROBOT_CALIBRATE_TYPE == ROBOT_CALIBRATE_I2C)
		/* NOT IMPLEMENTED YET */

#endif

		/* unlock the flash memory */
		l_HalStatus = HAL_FLASH_Unlock();
		if (HAL_OK != l_HalStatus)
		{
			l_EcuStatus = ECU_ERROR;
		}
		else
		{   
			/* store the Kp in flash */
			l_HalStatus |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ADD_LAST_MAX_CAL_SPEED, Kp);
			/* store the Ki in flash */
			l_HalStatus |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ADD_LAST_MAX_CAL_SPEED, Ki);
			/* store the Kd in flash */
			l_HalStatus |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ADD_LAST_MAX_CAL_SPEED, Kd);

			/* Lock the Flash memory */
			do
			{
				l_HalStatus |= HAL_FLASH_Lock();
			} while (HAL_OK != l_HalStatus);

			/* check for any error happened */
			if (HAL_OK != l_HalStatus)
				l_EcuStatus = ECU_ERROR;
		}
	}
	return l_EcuStatus;
}

/**
 * @brief main function of controlling the speed of robot and performing controll over PID
 * 
 * @param p_Robot pointer to the robot object needed to be controlled
 * @return ecu_status_t status of the operation
 */
ecu_status_t robot_PID(robot_t *p_Robot)
{
	ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_Robot)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
		
	}
	return l_EcuStatus;
}
/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/




/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
* Ahmed Hani Atef       |* robot_calibrate is  not completed untill now uart-spi-i2c calibration is not done yet 	   *
*                       |* robot_PID not implemented yet 															   *
***********************************************************************************************************************/
