/**
 * @file    motor.h
 * @brief   Implementation of ultrasonic sensor handling functions for STM32F4.
 * @date    2024-10-21
 */

#include "ultrasonic.h"

/* Global arrays and variables */
UltrasonicSensor* sensors[8];
float* distance[8];
float first_val[8] = { INITIAL, INITIAL, INITIAL, INITIAL, INITIAL, INITIAL, INITIAL, INITIAL };
float second_val[8];
uint16_t channel_select[4] = { TIM_IT_CC1, TIM_IT_CC2, TIM_IT_CC3, TIM_IT_CC4 };
uint16_t channel_select__[4] = { TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4 };
uint8_t sensors_to_get_distance = 1;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

/**
 * @brief   Delays execution by a specified number of microseconds.
 * @param   us Microseconds to delay.
 */
void delay_us(TIM_HandleTypeDef *htim,uint16_t us) {
    uint32_t temp = __HAL_TIM_GET_COUNTER(htim);
    temp += us;
    while (__HAL_TIM_GET_COUNTER(htim) < temp);
}

/**
 * @brief   Initializes multiple ultrasonic sensors.
 * @param   num_sensors Number of sensors to initialize.
 * @param   ...         Variable arguments, each of type UltrasonicSensor*.
 */
void Ultrasonic_Init(int num_sensors, ...) {
    va_list args;
    va_start(args, num_sensors);

    for (int i = 0; i < num_sensors; i++) {
        UltrasonicSensor* sensor = va_arg(args, UltrasonicSensor*);
        HAL_TIM_IC_Start_IT(sensor->htim, channel_select__[sensor->Channel]);
        distance[i] = sensor->distance;
    }

    va_end(args);
}

/**
 * @brief   Reads distance from multiple ultrasonic sensors.
 * @param   num_sensors Number of sensors to read from.
 * @param   ...         Variable arguments, each of type UltrasonicSensor*.
 */
void Ultrasonic_ReadDistance(int num_sensors, ...) {
    va_list args;
    va_start(args, num_sensors);
    sensors_to_get_distance = num_sensors;

    /* Store all sensor pointers */
    for (int i = 0; i < num_sensors; i++) {
        sensors[i] = va_arg(args, UltrasonicSensor*);
    }
    va_end(args);

    /* Set TRIG pins high for all sensors */
    for (int i = 0; i < num_sensors; i++) {
        HAL_GPIO_WritePin(sensors[i]->TRIG_PORT, sensors[i]->TRIG_PIN, GPIO_PIN_SET);
    }

    delay_us(sensors[0]->htim,10);

    /* Set TRIG pins low for all sensors */
    for (int i = 0; i < num_sensors; i++) {
        HAL_GPIO_WritePin(sensors[i]->TRIG_PORT, sensors[i]->TRIG_PIN, GPIO_PIN_RESET);
    }
}

/**
 * @brief   Callback function for input capture event to calculate distance.
 * @param   htim Pointer to the timer handle where the input capture occurred.
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    static uint8_t expression_1 = 0;
    static uint8_t expression_2 = 0;
    static uint8_t expression_3 = 0;
    static uint8_t expression_4 = 0;

    for (uint8_t counter = 0; counter < sensors_to_get_distance; counter++) {
        if ((sensors[counter]->htim) == htim) {
            expression_1 = (((sensors[counter]->Channel) == channel_1) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1));
            expression_2 = (((sensors[counter]->Channel) == channel_2) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2));
            expression_3 = (((sensors[counter]->Channel) == channel_3) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3));
            expression_4 = (((sensors[counter]->Channel) == channel_4) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4));

            if (expression_1 || expression_2 || expression_3 || expression_4) {
                if (INITIAL == first_val[counter]) {
                    first_val[counter]   = (float)HAL_TIM_ReadCapturedValue(htim, channel_select__[sensors[counter]->Channel]);
                } else {
                    second_val[counter]  = (float)HAL_TIM_ReadCapturedValue(htim, channel_select__[sensors[counter]->Channel]);
                    *(distance[counter]) = ((second_val[counter] - first_val[counter]) * 0.017);
                    first_val[counter]   = INITIAL;
                }
            }
        }
    }
}
