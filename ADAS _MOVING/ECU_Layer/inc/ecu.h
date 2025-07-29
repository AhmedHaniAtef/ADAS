/**
 * @file    ecu.h
 * @author  Ahmed Hani
 * @brief   contains all configuation of the ecu layer
 * @date    2024-10-07
 * @note    nan
 */

#ifndef ECU_H_
#define ECU_H_

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../../Core/Inc/main.h"
#include "motor.h"
#include "robot.h"
#include "encoder.h"
#include "CANSPI.h"
#include "MCP2515.h"
#include "monitoring.h"


/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/

#define LAST_ADD_USED_FLASH     (0x0803FFF8U)
#define ADD_LAST_MAX_CAL_SPEED  (LAST_ADD_USED_FLASH)
#define ADD_Kp_VALUE            (LAST_ADD_USED_FLASH - 4)
#define ADD_Ki_VALUE            (LAST_ADD_USED_FLASH - 8)
#define ADD_Kd_VALUE            (LAST_ADD_USED_FLASH - 12)
#define ADD_N_VALUE             (LAST_ADD_USED_FLASH - 16)

#define TIMER_AUTO_RELOAD_VAL   (4199)
#define ROBOT_LENGHT_X          (0.0925)
#define ROBOT_LENGHT_Y          (0.085)
#define RADIUS_WHEEL            (0.03)

#define DEFUALT_Kp_VALUE        (0.85f)
#define DEFUALT_Ki_VALUE        (9.5f)
#define DEFUALT_Kd_VALUE        (0.07f)
#define DEFUALT_N_VALUE         (0.8f)

#define DEFUALT_MIN_PID_OUT     (0.0f)
#define DEFAULT_MAX_PID_OUT     (255.0f)

#define DEFUALT_MOTOR_MAX_SPEED (180.0f)

#define ROBOT_CALIBRATE_UART    (0)
#define ROBOT_CALIBRATE_SPI     (1)
#define ROBOT_CALIBRATE_MANUAL  (2)
#define ROBOT_CALIBRATE_I2C     (3)

#define ROBOT_CALIBRATE_TYPE (ROBOT_CALIBRATE_MANUAL)

#define ENCODER_MAX_COUNTER_VAL (44000)
#define ENCODER_PRESCALER       (4)
#define ENCODER_PULSES_NUMBER   (11)
#define ENCODER_GEAR_RATIO      (40)

#define ENCODER_READ_FILTER_CONST   (0.5f)

#define CAN_MCP2515_CS_PORT     (CAN_CS_GPIO_Port)
#define CAN_MCP2515_CS_PIN      (CAN_CS_Pin)
#define CAN_RX_INTERRUP_ENABLE  (0x03)
#define CAN_RX_INTERRUP_DISABLE (0x00)
#define CAN_RX_INTERRUPT  (CAN_RX_INTERRUP_ENABLE)

/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   EXTERN OBJECTS                                                     *
***********************************************************************************************************************/

/* robot object extern */
extern robot_t ADAS_ROBOT;

/* CAN object extern */
extern Can_t CAN;

/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/




/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/


#endif // ECU_H_
