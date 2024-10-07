/**
 * @file    motor.h
 * @author  Ahmed Hani & Youssef 
 * @brief    
 * @date    2024-10-07
 * @note    nan
 */


#ifndef MOTOR_MOTOR_H_
#define MOTOR_MOTOR_H_

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "stm32f4xx_hal.h"
#include <stm32f401xc.h>
#include <stm32f4xx_hal_tim.h>
#include "ecu_std.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/
#define MOTOR_MAX_SPEED (100)



/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/
/**
 * @brief this type represents the full interface with motor 
 * @param GpioxMotor array of two pointers each points to the port of each pin of the motor
 * @param GpioPinMotor array of two integers represents the pins selected for motor
 * @param SelectedTimer pointer to the selected time which generates the pwm for this motor
 * @param SelectedChannel the channel selected among the timer channels
 */
typedef struct
{
    GPIO_TypeDef  *GpioxMotor[2];
    uint16_t GpioPinMotor[2];
    TIM_HandleTypeDef *SelectedTimer;
    uint8_t SelectedChannel;
}motor_t;




/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief this function initialize the motor 
 * 
 * @param p_Motor object of motor 
 * @return ecu_status_t status of the operation
 */
ecu_status_t motor_init(motor_t *p_Motor);

/**
  * @brief This function moves the motor forward with specific speed
  * 
  * @param p_Motor object of motor 
  * @param p_Speed speed of motor
  * @return ecu_status_t status of the operation
 */
ecu_status_t motor_move_forward(motor_t *p_Motor , float_t p_Speed);

/**
  * @brief This function moves the motor backward with specific speed
  * 
  * @param p_Motor object of motor 
  * @param p_Speed speed of motor
  * @return ecu_status_t status of the operation
 */
ecu_status_t motor_move_backward(motor_t *p_Motor , float_t p_Speed);

/**
  * @brief This function stops the motor
  * 
  * @param p_Motor object of motor
  * @return ecu_status_t status of the operation
 */
ecu_status_t motor_stop(motor_t *p_Motor);

/**
  *
  * @brief This function change the speed of motor
  * 
  * @param p_Motor pin of motor
  * @param p_Speed speed of motor
  * @return ecu_status_t status of the operation
 */
ecu_status_t motor_change_speed(motor_t *p_Motor , float_t p_Speed);


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/


#endif /* MOTOR_MOTOR_H_ */
