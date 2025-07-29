/**
 * @file    motor.h
 * @authors Eslam Sameh & Mohammed Saad
 * @brief   Implementation of ultrasonic sensor handling functions for STM32F4.
 * @date    2024-10-21
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "ultrasonic.h"

/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/


/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/

/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/

static UltrasonicSensor* Sensors[TOTOAL_NUMBER_OF_ULTRASONIC];
static float* Distance[TOTOAL_NUMBER_OF_ULTRASONIC];
static float FirstVal[TOTOAL_NUMBER_OF_ULTRASONIC] = { INITIAL, INITIAL, INITIAL, INITIAL, INITIAL, INITIAL, INITIAL, INITIAL };
float OverFlow[TOTOAL_NUMBER_OF_ULTRASONIC] = { INITIAL, INITIAL, INITIAL, INITIAL, INITIAL, INITIAL, INITIAL, INITIAL };
static float SecondVal[TOTOAL_NUMBER_OF_ULTRASONIC];
static uint8_t SensorsToGetDistance = 1;  // At least one sensor to get its distance.

/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
 * @brief   Introduces a delay of the specified duration in microseconds.
 * @param   p_Htim Pointer to the timer used for the delay.
 * @param   p_US   Duration of the delay in microseconds.
 *
 * @note    This function has been modified to handle timer overflow(to safely deal with 16 bit timers).
 *          However, it does not account for multiple overflows.
 *          Ensure that p_US is at least 10 µs, and the timer period is not less than 10 µs.
 */
void delay_us(TIM_HandleTypeDef *p_Htim, uint16_t p_US) {
    uint32_t l_Temp = __HAL_TIM_GET_COUNTER(p_Htim);
    l_Temp += p_US;
    if(l_Temp < p_Htim->Init.Period)
    	while (__HAL_TIM_GET_COUNTER(p_Htim) < l_Temp);
    else
    {
    	while (__HAL_TIM_GET_COUNTER(p_Htim) != p_Htim->Init.Period );
    	while (__HAL_TIM_GET_COUNTER(p_Htim)< l_Temp-p_Htim->Init.Period);
    }
}

/**
 * @brief   Initializes multiple ultrasonic Sensors.
 * @param   p_NumSensors Number of Sensors to initialize.
 * @param   ...         Variable arguments, each of type UltrasonicSensor*.
 * @example EcuStatus |= Ultrasonic_Init(4,&sensor_1,&sensor_2,&sensor_3,&sensor_4);
 * @return  ecu_status_t status of operation
 */
ecu_status_t Ultrasonic_Init(int p_NumSensors, ...) {
    ecu_status_t l_EcuStatus = ECU_OK;
    va_list l_Args;
    va_start(l_Args, p_NumSensors);

    for (int i = 0; i < p_NumSensors; i++) {
        UltrasonicSensor* sensor = va_arg(l_Args, UltrasonicSensor*);
        if (NULL == sensor)
        {
            l_EcuStatus = ECU_ERROR;
            break;
        }
        HAL_TIM_RegisterCallback(sensor->htim,HAL_TIM_IC_CAPTURE_CB_ID,USER_TIM_IC_CALLBACK);
        HAL_TIM_RegisterCallback(sensor->htim,HAL_TIM_PERIOD_ELAPSED_CB_ID,USER_TIM_OVERFLOW_CALLBACK);
        HAL_TIM_IC_Start_IT(sensor->htim, sensor->Channel);
        HAL_TIM_Base_Start_IT(sensor->htim);
        Distance[i] = sensor->Distance;
    }

    va_end(l_Args);
    return l_EcuStatus;
}

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
ecu_status_t Ultrasonic_ReadDistance(int p_NumSensors,int index, ...) {
    ecu_status_t l_EcuStatus = ECU_OK;
    va_list l_Args;
    va_start(l_Args, p_NumSensors);
    SensorsToGetDistance = p_NumSensors;
    int i=index-1;
    /* Store all sensor pointers */
    for (i;i <(index-1+p_NumSensors);i++)
    {
        Sensors[i] = va_arg(l_Args, UltrasonicSensor*);
        if (NULL == Sensors[i])
        {
            l_EcuStatus = ECU_ERROR;
            break;
        }
    }
    va_end(l_Args);

    if (l_EcuStatus != ECU_ERROR)
    {
        /* Set TRIG pins high for all Sensors */
        for (i=index-1;i < (index-1+p_NumSensors); i++) {
            HAL_GPIO_WritePin(Sensors[i]->TRIG_PORT, Sensors[i]->TRIG_PIN, GPIO_PIN_SET);
        }

        delay_us(Sensors[index-1]->htim,15);

        /* Set TRIG pins low for all Sensors */
        for (i=index-1;i < (index-1+p_NumSensors); i++) {
            HAL_GPIO_WritePin(Sensors[i]->TRIG_PORT, Sensors[i]->TRIG_PIN, GPIO_PIN_RESET);
        }
    }
    return l_EcuStatus;
}

/**
 * @brief   Callback function for input capture event to calculate Distance.
 * @param   htim Pointer to the timer handle where the input capture occurred.
 */
void USER_TIM_IC_CALLBACK(TIM_HandleTypeDef *htim) {
    static uint8_t l_Expression_1 = 0;
    static uint8_t l_Expression_2 = 0;
    static uint8_t l_Expression_3 = 0;
    static uint8_t l_Expression_4 = 0;

    for (uint8_t l_Counter = 0; l_Counter < TOTOAL_NUMBER_OF_ULTRASONIC; l_Counter++) {
        if ((Sensors[l_Counter]->htim) == htim) {
            l_Expression_1 = (((Sensors[l_Counter]->Channel) == TIM_CHANNEL_1) && 
                                   (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1));
            l_Expression_2 = (((Sensors[l_Counter]->Channel) == TIM_CHANNEL_2) && 
                                   (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2));
            l_Expression_3 = (((Sensors[l_Counter]->Channel) == TIM_CHANNEL_3) && 
                                   (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3));
            l_Expression_4 = (((Sensors[l_Counter]->Channel) == TIM_CHANNEL_4) && 
                                   (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4));

            if (l_Expression_1 || l_Expression_2 || l_Expression_3 || l_Expression_4) {
                if (INITIAL == FirstVal[l_Counter]) {
                    OverFlow[l_Counter]    = 0;
                    FirstVal[l_Counter]    = (float)HAL_TIM_ReadCapturedValue(htim, Sensors[l_Counter]->Channel);
                } else {
                    SecondVal[l_Counter]   = (float)HAL_TIM_ReadCapturedValue(htim, Sensors[l_Counter]->Channel);
                    *(Distance[l_Counter]) = (OverFlow[l_Counter]*((Sensors[l_Counter]->htim)->Init.Period)+(SecondVal[l_Counter] - FirstVal[l_Counter]))*0.017;
                    FirstVal[l_Counter]    = INITIAL;
                    OverFlow[l_Counter]    = INITIAL;
                }
            }
        }
    }
}
/**
 * @brief   Callback function for calculate timer overflows.
 * @param   htim Pointer to the timer handle where the overflow occurred.
 */
void USER_TIM_OVERFLOW_CALLBACK(TIM_HandleTypeDef *htim) {
		static uint8_t l_Expression_1 = 0;
	    static uint8_t l_Expression_2 = 0;
	    static uint8_t l_Expression_3 = 0;
	    static uint8_t l_Expression_4 = 0;
	for (uint8_t l_Counter = 0; l_Counter < TOTOAL_NUMBER_OF_ULTRASONIC; l_Counter++) {
		l_Expression_1=((Sensors[l_Counter]->htim->Instance==TIM2)&&(htim->Instance == TIM2));
		l_Expression_2=((Sensors[l_Counter]->htim->Instance==TIM3)&&(htim->Instance == TIM3));
		l_Expression_3=((Sensors[l_Counter]->htim->Instance==TIM4)&&(htim->Instance == TIM4));
		l_Expression_4=((Sensors[l_Counter]->htim->Instance==TIM5)&&(htim->Instance == TIM5));
			if(l_Expression_1||l_Expression_2||l_Expression_3||l_Expression_4){
				if ((OverFlow[l_Counter]!=INITIAL)){
					OverFlow[l_Counter]++;
				}
			}
	    }

}


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
