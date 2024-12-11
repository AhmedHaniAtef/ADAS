/**
 * @file    encoder.h
 * @author  Ahmed Hani
 * @brief   this file contains the data type and definitions of encoder interfaces
 * @date    2024-12-08
 */

#ifndef ENCODER_H_
#define ENCODER_H_

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "stm32f4xx_hal.h"
#include <stm32f401xc.h>
#include "ecu_std.h"



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
 * @brief data type for encoder which stores the counter used, speed and posiotion
 * @param SelectedTimer pointer to the timer used for this encoder
 * @param Speed stores the speed of the motor attached to that encoder (RPM)
 * @param Position used to store the current position of the motor (how much does it move from first position) (cm)
 * @param WheelRadius used in position calculations and represents the radius of the wheel in (cm)
 * @param LastCount stores the last counter value
 */
typedef struct 
{
    TIM_HandleTypeDef *SelectedTimer;
    float_t Speed;
    float_t Position;
    float_t WheelRadius;
    uint32_t LastCount;
}encoder_t;




/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief initialize the parameters of the encoder
 * @param p_Encoder pointer to the encoder wanted to be initialized
 * @return ecu_status_t status of operation
 */
ecu_status_t encoder_init(encoder_t *p_Encoder);

/**
 * @brief this function should be called periodically to update the speed and position
 * @param p_Encoder pointer to the encoder to be updated
 * @param p_PeriodTime the period time which this function would be called every  (ms)
 * @return ecu_status_t status of operation
 */
ecu_status_t encoder_periodic_update(encoder_t *p_Encoder, float_t p_PeriodTime);



/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif // ENCODER_H_
