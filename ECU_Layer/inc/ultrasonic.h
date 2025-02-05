/**
 * @file    ultrasonic.h
 * @brief   Header file for ultrasonic sensor handling on STM32F4 microcontrollers.
 *          Provides initialization and distance reading functions for multiple ultrasonic sensors.
 * @authors Eslam Sameh & Mohammed Saad
 * @date    2024-10-21
 */

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "stm32f4xx_hal.h"
#include <stdarg.h>           // For handling variadic arguments
#include <stm32f401xc.h>      // MCU-specific header for STM32F401
#include "ecu_std.h"

/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/
#define INITIAL 0.5f  /**< Initial distance value or threshold setting */
#define TOTOAL_NUMBER_OF_ULTRASONIC 8
/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/

/* Add any relevant macro functions here if required */

/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/

/**
 * @struct UltrasonicSensor
 * @brief Structure defining properties for each ultrasonic sensor.
 */
typedef struct {
    GPIO_TypeDef *TRIG_PORT;     /**< GPIO port for TRIG pin */
    uint16_t TRIG_PIN;           /**< GPIO pin for TRIG signal */
    TIM_HandleTypeDef *htim;     /**< Timer handle for ECHO capture */
    uint8_t Channel;           /**< Timer channel for input capture @defgroup TIM_Channel ex: TIM_CHANNEL_1 */
    float *Distance;             /**< Pointer to store calculated distance */
} UltrasonicSensor;

/***********************************************************************************************************************
*                                                  FUNCTION PROTOTYPES                                                 *
***********************************************************************************************************************/
/**
 * @brief   Initializes multiple ultrasonic Sensors.
 * @param   p_NumSensors Number of Sensors to initialize.
 * @param   ...         Variable arguments, each of type UltrasonicSensor*.
 * @example EcuStatus |= Ultrasonic_Init(4,&sensor_1,&sensor_2,&sensor_3,&sensor_4);
 * @return  ecu_status_t status of operation
 */
ecu_status_t Ultrasonic_Init(int num_sensors, ...);

/**
 * @brief   Reads Distance from multiple ultrasonic Sensors.
 * @param   p_NumSensors Number of Sensors to read from.
 * @param	index : is the index to of first ultrasonic to read in Ultrasonic_Init() argument  ||
 * @param   ...         Variable arguments, each of type UltrasonicSensor*.                    \/
 * @example EcuStatus |= Ultrasonic_Init(4,&sensor_1,&sensor_2,&sensor_3,&sensor_4);
 * 						 Ultrasonic_ReadDistance(4,1,&sensor_1,&sensor_2,&sensor_3,&sensor_4);
 * 					or   Ultrasonic_ReadDistance(3,2,&sensor_2,&sensor_3,&sensor_4);
 * 					or   Ultrasonic_ReadDistance(2,3,&sensor_3,&sensor_4);
 * 					or   Ultrasonic_ReadDistance(1,4,&sensor_4);
 * @return ecu_status_t status of the operation
 */
ecu_status_t Ultrasonic_ReadDistance(int num_sensors,int index, ...);

/**
 * @brief   Delays the execution by specified microseconds.
 * @param   us Number of microseconds to delay.
 */
void delay_us(TIM_HandleTypeDef *htim,uint16_t us);

/**
 * @brief   Callback for input capture interrupt and overflow (USER_DEFINED) Processes ECHO pulse width.
 * @param   htim Timer handle where input capture interrupt and overflow occurred.
 */
void USER_TIM_IC_CALLBACK(TIM_HandleTypeDef *htim);
void USER_TIM_OVERFLOW_CALLBACK(TIM_HandleTypeDef *htim);



/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif /* ULTRASONIC_H */
