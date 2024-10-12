/**
 * @file    robot.h
 * @author  Ahmed Hani & Youssef
 * @brief
 * @date    2024-10-11
 * @note    N
 */


#ifndef ROBOT_ROBOT_H_
#define ROBOT_ROBOT_H_

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "motor.h"
#include "../ecu_std.h"

/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/



/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/
/**
 * @brief this enum represents the type of motor direction
 */
typedef enum
{
	FORWARD = 0,
	BACWARD = 1,
	STOPED  = 2,
}direction_t;

/**
 * @brief this struct used to describe the motor, its speed and direction of rotation
 * @param Motor motor object
 * @param Speed of the wheel
 * @param Direction direction of rotation
 */
typedef struct
{
	motor_t Motor;
	float_t Speed;
	direction_t Direction;
}wheel_t;

/**
 * @brief this struct for descibing the robot as it contains 4 wheels
 * @param FL front left wheel object
 * @param FR front right wheel object
 * @param RL rear left wheel object
 * @param RR rear right wheel object
 */
typedef struct
{
	wheel_t FL;
	wheel_t FR;
	wheel_t RL;
	wheel_t RR;
}robot_t;

/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
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
ecu_status_t robot_move(robot_t *p_Robot , uint16_t p_Angle , float_t p_Speed);

/**
 * @brief initialize each wheel (motor) of the robot
 * 
 * @param p_Robot pointer to the robot object needed to be initialized
 * @return ecu_status_t status of the operation
 */
ecu_status_t robot_init(robot_t *p_Robot);

/**
 * @brief this function calibrates the motors of the robot by making it move forward - backward - left - right then
          find the PID gain constants
 * 
 * @param p_Robot pointer to the robot object needed to be calibrated
 * @return ecu_status_t status of the operation
 */
ecu_status_t robot_calibrate(robot_t *p_Robot);

/**
 * @brief main function of controlling the speed of robot and performing controll over PID
 * 
 * @param p_Robot pointer to the robot object needed to be controlled
 * @return ecu_status_t status of the operation
 */
ecu_status_t robot_PID(robot_t *p_Robot);


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              *
*                       |                                                                                              *
***********************************************************************************************************************/


#endif /* ROBOT_ROBOT_H_ */
