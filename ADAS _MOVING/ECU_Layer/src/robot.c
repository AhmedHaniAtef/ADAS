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
						 Y-VE ----------|-+-|---------- Y+VE
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
#include <ecu_std.h>


/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/
#define PID_ROBOT_MAX_OUT	(255.0f)
#define PID_ROBOT_MIN_OUT	(0.0f)



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
static float_t N  = DEFUALT_N_VALUE;


/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
  *
  * @brief This function move the robot manually using Vx, Vy and Wz
  * 
  * @param p_Robot pointer to robot object
  * @param p_Vx speed in x direction (m/s)
  * @param p_Vy speed in y direction (m/s)
  * @param p_Wz angular speed (radian per second)
  * @return ecu_status_t status of the operation
 */
ecu_status_t robot_manual_move(robot_t *p_Robot , float_t p_Vx , float_t p_Vy, float_t p_Wz)
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

		l_LinearVelocityXAxis      = p_Vx;
		l_LinearVelocityYAxis      = p_Vy;
		l_AngularVelocityRobotBase = p_Wz;

		p_Robot->Vx = l_LinearVelocityXAxis;
		p_Robot->Vy = l_LinearVelocityYAxis;
		p_Robot->Wz = l_AngularVelocityRobotBase;

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
  *
  * @brief This function move the robot depending on angle and speed
  *
  * @param p_Robot pointer to robot object
  * @param p_Angle angle of robot of motor
  * @param p_Speed speed of motor
  * @return ecu_status_t status of the operation
 */
ecu_status_t robot_move(robot_t *p_Robot , float_t p_Angle , float_t p_Speed)
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
    	float_t l_AngularVelocityRobotBase  = p_Robot->Wz;   // Wz

    	p_Speed = roundf(100 * p_Speed) / 100.0;
		// convert velocity and angle to x and y velocity
    	l_LinearVelocityXAxis = p_Speed * cos(DEGREE_TO_RADIAN(p_Angle));
    	l_LinearVelocityYAxis = p_Speed * sin(DEGREE_TO_RADIAN(p_Angle));

		p_Robot->Vx = l_LinearVelocityXAxis;
		p_Robot->Vy = l_LinearVelocityYAxis;
		p_Robot->Wz = l_AngularVelocityRobotBase;

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
  *
  * @brief This function move the robot in circular path with specific radius and specific angle
  *
  * @param p_Robot pointer to robot object
  * @param p_Radius radius of the path
  * @param p_AngularSpeed the angular speed of the robot around this path in RPM
  * @return ecu_status_t status of the operation
 */
ecu_status_t robot_rotate(robot_t *p_Robot , float_t p_Radius , float_t p_AngularSpeed)
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

		p_AngularSpeed = RPM_TO_RPS(p_AngularSpeed);
    	p_AngularSpeed = roundf(100 * p_AngularSpeed) / 100.0;
		// convert velocity and angle to x and y velocity
    	l_LinearVelocityXAxis = p_AngularSpeed * p_Radius;
    	l_LinearVelocityYAxis = 0;
		l_AngularVelocityRobotBase = (-1.0) * p_AngularSpeed;

		p_Robot->Vx = l_LinearVelocityXAxis;
		p_Robot->Vy = l_LinearVelocityYAxis;
		p_Robot->Wz = l_AngularVelocityRobotBase;

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
  *
  * @brief This function stops the robot
  *
  * @param p_Robot pointer to robot object
  * @return ecu_status_t status of the operation
 */
ecu_status_t robot_stop(robot_t *p_Robot)
{
	ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_Robot)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
		// store the speed of each wheel
    	p_Robot->FL.Speed = 0.0f;
    	p_Robot->FR.Speed = 0.0f;
    	p_Robot->RL.Speed = 0.0f;
    	p_Robot->RR.Speed = 0.0f;

		p_Robot->Vx = 0.0f;
		p_Robot->Vy = 0.0f;
		p_Robot->Wz = 0.0f;

		// determine the direction of Front Left motor rotation then store and move it
		p_Robot->FL.Direction = STOPED;
    	l_EcuStatus = motor_stop(&p_Robot->FL.Motor);
		p_Robot->FR.Direction = STOPED;
    	l_EcuStatus = motor_stop(&p_Robot->FR.Motor);
		p_Robot->RL.Direction = STOPED;
    	l_EcuStatus = motor_stop(&p_Robot->RL.Motor);
		p_Robot->RR.Direction = STOPED;
    	l_EcuStatus = motor_stop(&p_Robot->RR.Motor);
		p_Robot->FL.Encoder.Speed = 0.0f;
		p_Robot->FR.Encoder.Speed = 0.0f;
		p_Robot->RL.Encoder.Speed = 0.0f;
		p_Robot->RR.Encoder.Speed = 0.0f;
	}
	return l_EcuStatus;
}

/**
 * @brief initialize each wheel (motor) of the robot
 * 
 * @param p_Robot pointer to the robot object needed to be initialized
 * @param p_TimeStep value which the speed will be modifed every time (in ms)
 * @return ecu_status_t status of the operation
 */
ecu_status_t robot_init(robot_t *p_Robot, float_t p_TimeStep)
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
		l_EcuStatus |= encoder_init(&p_Robot->FL.Encoder);
		p_Robot->FL.Speed = 0.0;
		p_Robot->FL.Direction = STOPED;
		/* initialize wheels of the robot (front right) */
		l_EcuStatus |= motor_init(&p_Robot->FR.Motor);
		l_EcuStatus |= encoder_init(&p_Robot->FR.Encoder);
		p_Robot->FR.Speed = 0.0;
		p_Robot->FR.Direction = STOPED;
		/* initialize wheels of the robot (rear left) */
		l_EcuStatus |= motor_init(&p_Robot->RL.Motor);
		l_EcuStatus |= encoder_init(&p_Robot->RL.Encoder);
		p_Robot->RL.Speed = 0.0;
		p_Robot->RL.Direction = STOPED;
		/* initialize wheels of the robot (rear right) */
		l_EcuStatus |= motor_init(&p_Robot->RR.Motor);
		l_EcuStatus |= encoder_init(&p_Robot->RR.Encoder);
		p_Robot->RR.Speed = 0.0;
		p_Robot->RR.Direction = STOPED;

		/* restore the PID constants from flash */
		Kp = DEFUALT_Kp_VALUE;
		Ki = DEFUALT_Ki_VALUE;
		Kd = DEFUALT_Kd_VALUE;
		N  = DEFUALT_N_VALUE;

		// initialize the controller
		PID_Init(&p_Robot->PID_FL, Kp, Ki, Kd, N, p_TimeStep / 1000.0 , PID_ROBOT_MIN_OUT, PID_ROBOT_MAX_OUT);
		PID_Init(&p_Robot->PID_FR, Kp, Ki, Kd, N, p_TimeStep / 1000.0 , PID_ROBOT_MIN_OUT, PID_ROBOT_MAX_OUT);
		PID_Init(&p_Robot->PID_RL, Kp, Ki, Kd, N, p_TimeStep / 1000.0 , PID_ROBOT_MIN_OUT, PID_ROBOT_MAX_OUT);
		PID_Init(&p_Robot->PID_RR, Kp, Ki, Kd, N, p_TimeStep / 1000.0 , PID_ROBOT_MIN_OUT, PID_ROBOT_MAX_OUT);
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
		N  = DEFUALT_N_VALUE;

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
			l_HalStatus |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ADD_Kp_VALUE, Kp);
			/* store the Ki in flash */
			l_HalStatus |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ADD_Ki_VALUE, Ki);
			/* store the Kd in flash */
			l_HalStatus |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ADD_Kd_VALUE, Kd);
			/* store the N in flash */
			l_HalStatus |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ADD_N_VALUE, N);

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
 * @param p_TimeStep this functions should be called with time step - time is in (ms)
 * @return ecu_status_t status of the operation
 */
ecu_status_t robot_PID(robot_t *p_Robot, float_t p_TimeStep)
{
	ecu_status_t l_EcuStatus = ECU_OK;
	static float_t l_FilterdSpeed_FL = 0;
	static float_t l_FilterdSpeed_FR = 0;
	static float_t l_FilterdSpeed_RL = 0;
	static float_t l_FilterdSpeed_RR = 0;
	static float_t l_PID_Out_FL = 0;
	static float_t l_PID_Out_FR = 0;
	static float_t l_PID_Out_RL = 0;
	static float_t l_PID_Out_RR = 0;
    if (NULL == p_Robot)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
		// Update the speed come from each encoder
		l_EcuStatus |= encoder_periodic_update(&p_Robot->FL.Encoder, p_TimeStep);
		l_EcuStatus |= encoder_periodic_update(&p_Robot->FR.Encoder, p_TimeStep);
		l_EcuStatus |= encoder_periodic_update(&p_Robot->RL.Encoder, p_TimeStep);
		l_EcuStatus |= encoder_periodic_update(&p_Robot->RR.Encoder, p_TimeStep);

		// Filter the final speed using low pass filter
		l_FilterdSpeed_FL = ENCODER_READ_FILTER_CONST * p_Robot->FL.Encoder.Speed + 
							(1 - ENCODER_READ_FILTER_CONST) * l_FilterdSpeed_FL;
		l_FilterdSpeed_FR = ENCODER_READ_FILTER_CONST * p_Robot->FR.Encoder.Speed + 
							(1 - ENCODER_READ_FILTER_CONST) * l_FilterdSpeed_FR;
		l_FilterdSpeed_RL = ENCODER_READ_FILTER_CONST * p_Robot->RL.Encoder.Speed + 
							(1 - ENCODER_READ_FILTER_CONST) * l_FilterdSpeed_RL;
		l_FilterdSpeed_RR = ENCODER_READ_FILTER_CONST * p_Robot->RR.Encoder.Speed + 
							(1 - ENCODER_READ_FILTER_CONST) * l_FilterdSpeed_RR;
		
		// PID output 
		l_PID_Out_FL = PID_Compute(&p_Robot->PID_FL, p_Robot->FL.Speed, l_FilterdSpeed_FL);
		l_PID_Out_FR = PID_Compute(&p_Robot->PID_FR, p_Robot->FR.Speed, l_FilterdSpeed_FR);
		l_PID_Out_RL = PID_Compute(&p_Robot->PID_RL, p_Robot->RL.Speed, l_FilterdSpeed_RL);
		l_PID_Out_RR = PID_Compute(&p_Robot->PID_RR, p_Robot->RR.Speed, l_FilterdSpeed_RR);

		// Map PID output from (0 - 255) to be (0 - MaxClibratedSpeed of motors)
		l_PID_Out_FL *= (MaxClibratedSpeed / PID_ROBOT_MAX_OUT);
		l_PID_Out_FR *= (MaxClibratedSpeed / PID_ROBOT_MAX_OUT);
		l_PID_Out_RL *= (MaxClibratedSpeed / PID_ROBOT_MAX_OUT);
		l_PID_Out_RR *= (MaxClibratedSpeed / PID_ROBOT_MAX_OUT);

		// change the speed of the motor according to the PID output
		l_EcuStatus |= motor_change_speed(&p_Robot->FL.Motor, l_PID_Out_FL);
		l_EcuStatus |= motor_change_speed(&p_Robot->FR.Motor, l_PID_Out_FR);
		l_EcuStatus |= motor_change_speed(&p_Robot->RL.Motor, l_PID_Out_RL);
		l_EcuStatus |= motor_change_speed(&p_Robot->RR.Motor, l_PID_Out_RR);

	}
	return l_EcuStatus;
}

/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/




/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
* Ahmed Hani Atef       |* 																						 	   *
*                       |*  																						   *
***********************************************************************************************************************/
