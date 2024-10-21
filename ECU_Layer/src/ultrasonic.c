/**
 * @file    ultrasonic.c
 * @author  Eslam Sameh & Mohammed Saad
 * @brief    
 * @date    2024-10-21
 * @note
 */
#include "../inc/ultrasonic.h"
void Ultrasonic_Init(UltrasonicSensor *sensor)
{
		// Configure TRIG pin as output
	    GPIO_InitTypeDef GPIO_InitStruct = {0};
	    GPIO_InitStruct.Pin = sensor->TRIG_PIN;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    HAL_GPIO_Init(sensor->TRIG_PORT, &GPIO_InitStruct);

	    // Configure ECHO pin as input
	    GPIO_InitStruct.Pin = sensor->ECHO_PIN;
	    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    HAL_GPIO_Init(sensor->ECHO_PORT, &GPIO_InitStruct);



}
float Ultrasonic_ReadDistance(UltrasonicSensor *sensor)
{
		uint32_t startTick, endTick, timeElapsed;
	    float distance;

	    // Send a 10us pulse to the TRIG pin
	    HAL_GPIO_WritePin(sensor->TRIG_PORT, sensor->TRIG_PIN, GPIO_PIN_SET);
	    delay_us(10);
	    HAL_GPIO_WritePin(sensor->TRIG_PORT, sensor->TRIG_PIN, GPIO_PIN_RESET);

	    // Wait for the ECHO pin to go HIGH (start of echo)
	    while (HAL_GPIO_ReadPin(sensor->ECHO_PORT, sensor->ECHO_PIN) == GPIO_PIN_RESET);

	    // Record the start time
	    startTick = __HAL_TIM_GET_COUNTER(&htim2);  // Assuming you are still using TIM2 for timing

	    // Wait for the ECHO pin to go LOW (end of echo)
	    while (HAL_GPIO_ReadPin(sensor->ECHO_PORT, sensor->ECHO_PIN) == GPIO_PIN_SET);

	    // Record the end time
	    endTick = __HAL_TIM_GET_COUNTER(&htim2);

	    // Calculate the time elapsed in microseconds
	    timeElapsed = endTick - startTick;

	    // Calculate the distance in cm (speed of sound is 34300 cm/s)
	    distance = (timeElapsed * 0.0343) / 2;  // Divide by 2 to account for round-trip

	    return distance;
}
void TIM2_Delay_us(uint32_t us) {
    __HAL_TIM_SET_COUNTER(&htim2, 0);  // Reset the timer counter
    while (__HAL_TIM_GET_COUNTER(&htim2) < us);  // Wait for the counter to reach the target us value
}

