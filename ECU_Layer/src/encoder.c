/**
 * @file    encoder.c
 * @author  Ahmed Hani
 * @brief   this file contains the declaration of the encoder interfaces
 * @date    2024-12-08
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../inc/encoder.h"
#include "../inc/ecu.h"
#include <math.h>


/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                               STATIC FUNCTION DEFINITION                                             *
***********************************************************************************************************************/





/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/






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
 * @brief initialize the parameters of the encoder
 * @param p_Encoder pointer to the encoder wanted to be initialized
 * @return ecu_status_t status of operation
 */
ecu_status_t encoder_init(encoder_t *p_Encoder)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (p_Encoder == NULL)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        p_Encoder->Speed = 0.0f;
        p_Encoder->Position = 0.0f;
        p_Encoder->LastCount = 0.0f;
        __HAL_TIM_SET_COUNTER(p_Encoder->SelectedTimer, 22000);
        if (HAL_TIM_Encoder_Start(p_Encoder->SelectedTimer, TIM_CHANNEL_ALL) != HAL_OK)
        {
            l_EcuStatus = ECU_ERROR;
        }
    }
    return l_EcuStatus;
}

/**
 * @brief this function should be called periodically to update the speed and position
 * @param p_Encoder pointer to the encoder to be updated
 * @param p_PeriodTime the period time which this function would be called every (ms)
 * @return ecu_status_t status of operation
 */
ecu_status_t encoder_periodic_update(encoder_t *p_Encoder, float_t p_PeriodTime)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (p_Encoder == NULL)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        uint32_t l_CurrentCount = __HAL_TIM_GET_COUNTER(p_Encoder->SelectedTimer);
        __HAL_TIM_SET_COUNTER(p_Encoder->SelectedTimer, 22000);
        float_t l_MovedCycles = 0;
        float_t l_Revolutions = 0;

        l_MovedCycles = (float_t)((float_t)l_CurrentCount - 22000.0);

        l_Revolutions = (float_t)((float_t)l_MovedCycles / ((float_t)ENCODER_PULSES_NUMBER * (float_t)40.0));

        p_Encoder->Speed = (((l_Revolutions) * 60)  / (p_PeriodTime / 1000.0));

        if (p_Encoder->Speed < 0)
            p_Encoder->Speed *= -1.0;

        p_Encoder->Position += l_Revolutions * 2 *  M_PI * p_Encoder->WheelRadius;

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
