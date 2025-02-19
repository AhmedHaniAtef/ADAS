/**
 * @file    APP.c
 * @author  Youssef
 * @brief   contains all interfaces to controll the car through uart
 * @date    2025-02-07
 */

#ifndef CONTROL_H_
#define CONTORL_H_

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "stm32f4xx_hal.h"
#include <stm32f401xc.h>
#include "ecu_std.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/
#define LEFT_JOY_STICK      (0XAA)   // robot_move
#define RIGHT_JOY_STICK     (0XAB)   // robot_rotate
#define PUSH_BUTTON         (0XAC)
#define COMPASS_ANGLE		(0XAD)



/***********************************************************************************************************************
*                                                   Globlal Objects                                                    *
***********************************************************************************************************************/



/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/

typedef struct
{
	uint8_t Address;
	float_t Mag;
	float_t Ang;
}Joy_Stick_frame;

typedef union
{
	Joy_Stick_frame frame;
	uint8_t uart_data[9];
}Joy_Stick_Data;

typedef struct
{
	uint8_t Address;
	uint8_t up;
	uint8_t down;
	uint8_t left;
	uint8_t right;
	uint8_t select;
	uint8_t start;
	uint8_t left_stick;
	uint8_t right_stick;
}buttons_frame;

typedef union
{
	buttons_frame frame;
	uint8_t uart_data[9];

}Push_Button_Joy_Stick_Data;

typedef struct 
{
    UART_HandleTypeDef *UsedUART;
    uint8_t Data[9];
}
control_t;



/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief this function waits for reciving commands from controller
 * @param p_UsedUART 
 * @return ecu_status_t 
 */
ecu_status_t control_receive_data(control_t *p_UsedController);



/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
#endif // CONTROL_H_
