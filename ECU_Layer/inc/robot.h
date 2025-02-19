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
#include "encoder.h"
#include "../../lib/inc/PID.h"
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
 * @param Encoder the encoder attached to the motor moving the wheel
 */
typedef struct
{
	motor_t Motor;
	float_t Speed;
	direction_t Direction;
	encoder_t Encoder;
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
	PID_Controller PID_FL;
	PID_Controller PID_FR;
	PID_Controller PID_RL;
	PID_Controller PID_RR;
  float_t Vx;
  float_t Vy;
  float_t Wz;
}robot_t;

/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
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
ecu_status_t robot_manual_move(robot_t *p_Robot , float_t p_Vx , float_t p_Vy, float_t p_Wz);

/**
  *
  * @brief This function move the robot depending on angle and speed
  *
  * @param p_Robot pointer to robot object
  * @param p_Angle angle of robot of motor
  * @param p_Speed speed of motor
  * @return ecu_status_t status of the operation
 */
ecu_status_t robot_move(robot_t *p_Robot , float_t p_Angle , float_t p_Speed);

/**
  *
  * @brief This function move the robot in circular path with specific radius and specific angle
  *
  * @param p_Robot pointer to robot object
  * @param p_Radius radius of the path
  * @param p_AngularSpeed the angular speed of the robot around this path in RPM
  * @return ecu_status_t status of the operation
 */
ecu_status_t robot_rotate(robot_t *p_Robot , float_t p_Radius , float_t p_AngularSpeed);

/**
  *
  * @brief This function stops the robot
  *
  * @param p_Robot pointer to robot object
  * @return ecu_status_t status of the operation
 */
ecu_status_t robot_stop(robot_t *p_Robot);

/**
 * @brief initialize each wheel (motor) of the robot
 * 
 * @param p_Robot pointer to the robot object needed to be initialized
 * @param p_TimeStep value which the speed will be modifed every time (in ms)
 * @return ecu_status_t status of the operation
 */
ecu_status_t robot_init(robot_t *p_Robot, float_t p_TimeStep);

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
 * @param p_TimeStep this functions should be called with time step - time is in (ms)
 * @return ecu_status_t status of the operation
 */
ecu_status_t robot_PID(robot_t *p_Robot, float_t p_TimeStep);


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              *
*                       |                                                                                              *
***********************************************************************************************************************/


#endif /* ROBOT_ROBOT_H_ */
