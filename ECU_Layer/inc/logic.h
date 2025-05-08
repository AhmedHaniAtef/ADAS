/**
 * @file    logic.h
 * @author  youssef Benyamine
 * @brief   this file contains the data type and definitions of logic (like buzzer and leds) interfaces
 * @date    2024-03-12
 */

#ifndef LOGIC_H_
#define LOGIC_H_

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
 * @brief this data type for specifiy a logic device to an attatched pin
 * 
 * @param logic_Port pointer to the gpio port used
 * @param logic_pin the pin used by this device
 */
typedef struct
{	
	GPIO_TypeDef *logic_Port;	
	uint16_t logic_pin;
}logic_t;




/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief this function writes high to the pin attatched to the device
 * 
 * @param p_Logic pointer to the device
 * @return ecu_status_t status of operation
 */
ecu_status_t logic_set(logic_t *p_Logic);

/**
 * @brief this function writes low to the pin attatched to the device
 * 
 * @param p_Logic pointer to the device
 * @return ecu_status_t status of operation
 */
ecu_status_t logic_reset(logic_t *p_Logic);

/**
 * @brief this function toggles the pin attatched to the device
 * 
 * @param p_Logic pointer to the device
 * @return ecu_status_t status of operation
 */
ecu_status_t logic_toogle(logic_t *p_Logic);



/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif // LOGIC_H_
 
