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
#include <stdarg.h>          // For handling variadic arguments
#include <stm32f401xc.h>      // MCU-specific header for STM32F401

/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/
#define INITIAL 0.5f  /**< Initial distance value or threshold setting */

/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/

/* Add any relevant macro functions here if required */

/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/

/**
 * @enum channel_t
 * @brief Enumeration for available timer channels.
 */
typedef enum {
	channel_1,    /**< Timer Channel 1 */
	channel_2,    /**< Timer Channel 2 */
	channel_3,    /**< Timer Channel 3 */
	channel_4     /**< Timer Channel 4 */
} channel_t;

/**
 * @struct UltrasonicSensor
 * @brief Structure defining properties for each ultrasonic sensor.
 */
typedef struct {
    GPIO_TypeDef *TRIG_PORT;     /**< GPIO port for TRIG pin */
    uint16_t TRIG_PIN;           /**< GPIO pin for TRIG signal */
    TIM_HandleTypeDef *htim;     /**< Timer handle for ECHO capture */
    channel_t Channel;           /**< Timer channel for input capture */
    float *distance;             /**< Pointer to store calculated distance */
} UltrasonicSensor;

/***********************************************************************************************************************
*                                                  FUNCTION PROTOTYPES                                                 *
***********************************************************************************************************************/

/**
 * @brief   Initializes multiple ultrasonic sensors.
 * @param   num_sensors Number of sensors to initialize.
 * @param   ...         Variable arguments of type UltrasonicSensor* for each sensor.
 */
void Ultrasonic_Init(int num_sensors, ...);

/**
 * @brief   Reads distance from multiple ultrasonic sensors.
 * @param   num_sensors Number of sensors to read from.
 * @param   ...         Variable arguments of type UltrasonicSensor* for each sensor.
 */
void Ultrasonic_ReadDistance(int num_sensors, ...);

/**
 * @brief   Delays the execution by specified microseconds.
 * @param   us Number of microseconds to delay.
 */
void delay_us(uint16_t us);

/**
 * @brief   Callback for input capture interrupt. Processes ECHO pulse width.
 * @param   htim Timer handle where input capture interrupt occurred.
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

#endif /* ULTRASONIC_H */
