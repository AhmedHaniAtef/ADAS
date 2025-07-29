/**
 * @file    BSD_feature.h
 * @author  Youssef Benyamine
 * @brief   contains the functions which handle the Blind Spot Detection
 * @date    2025-04-22
 */

#ifndef BSD_FEATURE_H_
#define BSD_FEATURE_H_
/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../../ECU_Layer/inc/ecu.h"
#include "../APP_std.h"



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
 * @brief data type for Blind Spot Detection feature
 * 
 * @param Buzzer pointer to the Buzzer 
 * @param RightLed pointer to the Right led 
 * @param LeftLed pointer to the Left led 
 */
typedef struct
{
	logic_t *Buzzer;
	logic_t *RightLed;
	logic_t *LeftLed;
}BSD_t;



/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief this function initialize the task respoinsible for Blind Spot Detection
 * 
 * @param p_bls pointer to the object
 * @param p_delay pointer to the function which makes a OS delay
 * @param p_suspend pointer to the function which suspend the task
 * @param p_resume pointer to the function which resume the task
 * @return app_status_t status of operation
 */
app_status_t BSD_init(BSD_t *p_bls , void (*p_delay)(uint32_t) , void (*p_suspend)(void) , void (*p_resume)(void));

/**
 * @brief the main function of the blind spot detection task
 * 
 * @param p_bls pointer to the object
 * @return app_status_t status of operation
 */
app_status_t BSD_task(BSD_t *p_bls);

/**
 * @brief the function which make alert using buzzer
 * 
 * @param p_bls pointer to the object
 * @return app_status_t status of operation
 */
app_status_t BSD_buzzer_task(BSD_t *p_bls);


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif //BSD_FEATURE_H_
 