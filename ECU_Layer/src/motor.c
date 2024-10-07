/**
 * @file    motor.c
 * @author  Ahmed Hani & Youssef 
 * @brief    
 * @date    2024-10-07
 * @note    nan
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../inc/morot.h"
#include "../inc/ecu.h"


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
float_t MaxClibratedSpeed = DEFUALT_SPEED;          // defualt speed before calibration





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
 * @brief this function initialize the motor 
 * 
 * @param p_Motor object of motor 
 * @return ecu_status_t status of the operation
 */
ecu_status_t motor_init(motor_t *p_Motor)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_Motor)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        /* GPIO initializtion is DONE by CubeMX */

        /* start generating pwm with zero duty cycle */
        __HAL_TIM_SetCompare(p_Motor->SelectedTimer, p_Motor->SelectedChannel, ZERO);
        HAL_TIM_PWM_Start(p_Motor->SelectedTimer, p_Motor->SelectedChannel);
    }
    return l_EcuStatus;
}

/**
  * @brief This function moves the motor forward with specific speed
  * @param p_Motor object of motor 
  * @param p_Speed speed of motor
  * @return ecu_status_t status of the operation
 */
ecu_status_t motor_move_forward(motor_t *p_Motor , float_t p_Speed)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_Motor)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        HAL_GPIO_WritePin(p_Motor->GpioxMotor[0] , p_Motor->GpioPinMotor[0] , GPIO_PIN_SET);
        HAL_GPIO_WritePin(p_Motor->GpioxMotor[1] , p_Motor->GpioPinMotor[1] , GPIO_PIN_RESET);
    }
    return l_EcuStatus;
}

/**
  * @brief This function moves the motor backward with specific speed
  * @param p_Motor object of motor 
  * @param p_Speed speed of motor
  * @return ecu_status_t status of the operation
 */
ecu_status_t motor_move_backward(motor_t *p_Motor , float_t p_Speed)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_Motor)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        HAL_GPIO_WritePin(p_Motor->GpioxMotor[0] , p_Motor->GpioPinMotor[0] , GPIO_PIN_RESET);
        HAL_GPIO_WritePin(p_Motor->GpioxMotor[1] , p_Motor->GpioPinMotor[1] , GPIO_PIN_SET);
    }
    return l_EcuStatus;
}

/**
  * @brief This function stops the motor
  * @param p_Motor object of motor
  * @return ecu_status_t status of the operation
 */
ecu_status_t motor_stop(motor_t *p_Motor)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_Motor)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        HAL_GPIO_WritePin(p_Motor->GpioxMotor[0] , p_Motor->GpioPinMotor[0] , GPIO_PIN_RESET);
        HAL_GPIO_WritePin(p_Motor->GpioxMotor[1] , p_Motor->GpioPinMotor[1] , GPIO_PIN_RESET);
    }
    return l_EcuStatus;
}

/**
  *
  * @brief This function change the speed of motor
  * @param p_Motor pin of motor
  * @param p_Speed speed of motor
  * @return ecu_status_t status of the operation
 */
ecu_status_t motor_change_speed(motor_t *p_Motor , float_t p_Speed)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_Motor)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        // get the value of duty cycle in percentage
        float_t l_PwmDutyCycle = (float_t)((float_t)p_Speed / (float_t)MaxClibratedSpeed);
        // get the value of CCRx Register
        uint32_t l_PwmCCR = (uint32_t)(l_PwmDutyCycle * TIMER_AUTO_RELOAD_VAL);
        // change the output duty cycle of the timer
        __HAL_TIM_SetCompare(p_Motor->SelectedTimer, p_Motor->SelectedChannel, l_PwmCCR);
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
