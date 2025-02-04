/**
 * @file    monitoring.h
 * @author  Ahmed Hani
 * @brief   this file contains the data type and definitions of monitoring through UART
 * @date    2025-01-29
 */

#ifndef MONITORING_H_
#define MONITORING_H_

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
 * @brief this data type used to convert data from a struct which passed as pointer to array of bytes to make it easy to
 *        send through USART
 * @param OriginalData pointer to the struct wanted to be sent
 * @param SendData pointer to the first byte of this struct
 */
typedef union 
{
    void *OriginalData;
    uint8_t *SendData;
}monitoring_data_t;

/**
 * @brief this struct contains the union used to send data and the size of data in bytes
 * @param Data union used to send data
 * @param Size size of the data in BYTES
 * @param MonitorUpdateData_CALLBACK pointer to user call back used to notify the user to updata the values of the data
 * @param UsedUART the UART used to send data
 */
typedef struct 
{
    UART_HandleTypeDef *UsedUART;
    void (*MonitorUpdateData_CALLBACK)(void);
    monitoring_data_t Data;
    uint8_t Size;
}monitoring_t;







/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief used to send the data
 * 
 * @param p_MonitorOBJ object to the data and size of the data
 * @return ecu_status_t 
 */
ecu_status_t monitoring_send_data(monitoring_t *p_MonitorOBJ);




/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif // MONITORING_H_
