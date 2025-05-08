/**
 * @file    BSD_feature.c
 * @author  Youssef Benyamine
 * @brief   contains the functions which handle the Blind Spot Detection
 * @date    2025-04-22
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "APP.h"
#include "BSD_feature.h"
#include "ACC_feature.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                               STATIC FUNCTION DEFINITION                                             *
***********************************************************************************************************************/

static inline ecu_status_t right_led_turn_on(BSD_t *p_bls);
static inline ecu_status_t right_led_turn_off(BSD_t *p_bls);
static inline ecu_status_t left_led_turn_on(BSD_t *p_bls);
static inline ecu_status_t left_led_turn_off(BSD_t *p_bls);
static inline ecu_status_t buzzer_on(BSD_t *p_bls);
static inline ecu_status_t buzzer_off(BSD_t *p_bls);
static inline ecu_status_t buzzer_toggle(BSD_t *p_bls);




/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/






/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/

static void (*delay_call_back)(uint32_t) = NULL;
static void (*buzzer_resume)(void) = NULL;
static void (*buzzer_suspend)(void) = NULL;



/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

app_status_t BSD_init(BSD_t *p_bls , void (*p_delay)(uint32_t) , void (*p_suspend)(void) , void (*p_resume)(void))
{
	app_status_t l_AppStatus = APP_OK;
	if (NULL == p_bls)
	{
		l_AppStatus = APP_ERROR;
	}
	else
	{
		delay_call_back = p_delay;
		buzzer_resume = p_resume;
		buzzer_suspend = p_suspend;
	}
	return l_AppStatus;
}

app_status_t BSD_task(BSD_t *p_bls)
{
	app_status_t l_AppStatus = APP_OK;
	ecu_status_t l_EcuStatus = ECU_OK;
	if (NULL == p_bls)
	{
		l_AppStatus = APP_ERROR;
	}
	else
	{
		if  ((*Main_Ultrasonics.UL_90->Distance <= MIN_DISTANCE) || (*Main_Ultrasonics.UL_135->Distance <= MIN_DISTANCE)
		 || (*Main_Ultrasonics.UL_270->Distance <= MIN_DISTANCE) || (*Main_Ultrasonics.UL_225->Distance <= MIN_DISTANCE))
		{
            // right of car
			if((*Main_Ultrasonics.UL_90->Distance <= MIN_DISTANCE) || (*Main_Ultrasonics.UL_135->Distance <= MIN_DISTANCE))  
			{
			    l_EcuStatus = right_led_turn_on(p_bls);
		    }
			else
		    {
			    l_EcuStatus = right_led_turn_off(p_bls);
		    }
            // left of car
			if((*Main_Ultrasonics.UL_270->Distance <= MIN_DISTANCE) || (*Main_Ultrasonics.UL_225->Distance <= MIN_DISTANCE)) 
			{
			    l_EcuStatus = left_led_turn_on(p_bls);
	 	    }
			else
		    {
			    l_EcuStatus = left_led_turn_off(p_bls);
		    }

			buzzer_resume();
		}
		else
		{
			l_EcuStatus = right_led_turn_off(p_bls);
			l_EcuStatus = buzzer_off(p_bls);
			l_EcuStatus = left_led_turn_off(p_bls);
			buzzer_suspend();
		}

	}
	return l_AppStatus;
}

app_status_t BSD_buzzer_task(BSD_t *p_bls)
{
	app_status_t l_AppStatus = APP_OK;
	ecu_status_t l_EcuStatus = ECU_OK;
	if (NULL == p_bls)
	{
		l_AppStatus = APP_ERROR;
	}
	else
	{
		l_EcuStatus = buzzer_on(p_bls);
		delay_call_back(500);
		l_EcuStatus = buzzer_off(p_bls);
		delay_call_back(250);
	}
	return l_AppStatus;
}



/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/

static inline ecu_status_t right_led_turn_on(BSD_t *p_bls)
{
	return (logic_set(p_bls->RightLed));
}

static inline ecu_status_t right_led_turn_off(BSD_t *p_bls)
{
	return (logic_reset(p_bls->RightLed));
}

static inline ecu_status_t left_led_turn_on(BSD_t *p_bls)
{
	return (logic_set(p_bls->LeftLed));
}

static inline ecu_status_t left_led_turn_off(BSD_t *p_bls)
{
	return (logic_reset(p_bls->LeftLed));
}

static inline ecu_status_t buzzer_on(BSD_t *p_bls)
{
	return (logic_set(p_bls->Buzzer));
}

static inline ecu_status_t buzzer_off(BSD_t *p_bls)
{
	return (logic_reset(p_bls->Buzzer));
}

static inline ecu_status_t buzzer_toggle(BSD_t *p_bls)
{
	return (logic_toogle(p_bls->Buzzer));
}

/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
