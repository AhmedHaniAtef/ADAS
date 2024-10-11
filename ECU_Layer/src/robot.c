/**
 * @file    robot.c
 * @author  Ahmed Hani & Youssef
 * @brief
 * @date    2024-10-11
 * @note
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../inc/ecu.h"
#include "../inc/robot.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/
#define DEFUALT_SPEED (100.0)




/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                               STATIC FUNCTION DEFINITION                                             *
***********************************************************************************************************************/





/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/
/*
									     X+VE
										 |
                                Y-VE-----|----- Y+VE
										 |
										 X-VE
										 origin (511,511)
										 if angle == 0 then it will move forward
										 and if angle != 0 will measured CK
										 angle between x+VE and velocity direction

*/
/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/




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


    	l_LinearVelocityXAxis = p_Speed * cos(p_Angle);
    	l_LinearVelocityYAxis = p_Speed * sin(p_Angle);


    	l_AngularVelocityFrontLeft  = ((1.0 / RADIUS_WHEEL) * (l_LinearVelocityXAxis - l_LinearVelocityYAxis -
    									((ROBOT_LENGHT_X + ROBOT_LENGHT_Y) * l_AngularVelocityRobotBase)));
    	l_AngularVelocityFrontRight = ((1.0 / RADIUS_WHEEL) * (l_LinearVelocityXAxis + l_LinearVelocityYAxis +
    									((ROBOT_LENGHT_X + ROBOT_LENGHT_Y) * l_AngularVelocityRobotBase)));
    	l_AngularVelocityRearLeft   = ((1.0 / RADIUS_WHEEL) * (l_LinearVelocityXAxis + l_LinearVelocityYAxis -
    									((ROBOT_LENGHT_X + ROBOT_LENGHT_Y) * l_AngularVelocityRobotBase)));
    	l_AngularVelocityRearRight  = ((1.0 / RADIUS_WHEEL) * (l_LinearVelocityXAxis - l_LinearVelocityYAxis +
    									((ROBOT_LENGHT_X + ROBOT_LENGHT_Y) * l_AngularVelocityRobotBase)));

    	p_Robot->FL.Speed = fabs(RPS_TO_RPM(l_AngularVelocityFrontLeft));  //rad/s
    	p_Robot->FR.Speed = fabs(RPS_TO_RPM(l_AngularVelocityFrontRight));
    	p_Robot->RL.Speed = fabs(RPS_TO_RPM(l_AngularVelocityRearLeft));
    	p_Robot->RR.Speed = fabs(RPS_TO_RPM(l_AngularVelocityRearRight));

    	if(l_AngularVelocityFrontLeft < 0)
    	{
    		p_Robot->FL.direction = BACWARD;
    		l_EcuStatus = motor_move_backward(p_Robot->FL , p_Robot->FL.Speed);
    	}
    	else
    	{
    		p_Robot->FL.direction = FORWARD;
    		l_EcuStatus = motor_move_forward(p_Robot->FL , p_Robot->FL.Speed);
    	}
    	if(l_AngularVelocityFrontRight < 0)
    	{
    	    p_Robot->FR.direction = BACWARD;
    	    l_EcuStatus = motor_move_backward(p_Robot->FR , p_Robot->FR.Speed);
    	}
    	else
    	{
    	    p_Robot->FR.direction = FORWARD;
    	    l_EcuStatus = motor_move_forward(p_Robot->FR , p_Robot->FR.Speed);
    	}
    	if(l_AngularVelocityRearLeft < 0)
    	{
    	    p_Robot->RL.direction = BACWARD;
    	    l_EcuStatus = motor_move_backward(p_Robot->RL , p_Robot->RL.Speed);
    	}
    	else
    	{
    	    p_Robot->RL.direction = FORWARD;
    	    l_EcuStatus = motor_move_forward(p_Robot->RL , p_Robot->RL.Speed);
    	}
    	if(l_AngularVelocityRearRight < 0)
    	{
    	    p_Robot->RR.direction = BACWARD;
    	    l_EcuStatus = motor_move_backward(p_Robot->RR , p_Robot->RR.Speed);
    	}
    	else
    	{
    	    p_Robot->RR.direction = FORWARD;
    	    l_EcuStatus = motor_move_forward(p_Robot->RR , p_Robot->RR.Speed);
    	}
    }

	return l_EcuStatus;
}

/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/




/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              *
*                       |                                                                                              *
***********************************************************************************************************************/
