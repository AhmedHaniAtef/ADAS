/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#ifndef __CAN_SPI_H
#define __CAN_SPI_H

#include "stm32f4xx_hal.h"
#include <stm32f401xc.h>
#include <stdbool.h>
#include "../ecu_std.h"

typedef enum
{
    MCP_8MHz_1000kBPS = 0,
    MCP_8MHz_500kBPS,
    MCP_8MHz_250kBPS,
    MCP_8MHz_200kBPS,
    MCP_8MHz_125kBPS,
    MCP_8MHz_100kBPS,
}can_speed_t;

typedef struct
{
    SPI_HandleTypeDef *UsedSPI;
    can_speed_t Speed;
}Can_t;

typedef union
{
    struct
    {
        uint8_t idType;
        uint32_t id;
        uint8_t dlc;
        uint8_t data[8];
    } frame;
    uint8_t array[14];
} uCAN_MSG;

#define dSTANDARD_CAN_MSG_ID_2_0B 1
#define dEXTENDED_CAN_MSG_ID_2_0B 2

ecu_status_t CANSPI_Initialize(Can_t *p_CAN);
ecu_status_t CANSPI_Sleep(Can_t *p_CAN);
ecu_status_t CANSPI_Transmit(Can_t *p_CAN, uCAN_MSG *tempCanMsg);
ecu_status_t CANSPI_Receive(Can_t *p_CAN, uCAN_MSG *tempCanMsg);
ecu_status_t CANSPI_messagesInBuffer(Can_t *p_CAN, uint8_t *count);
ecu_status_t CANSPI_isBussOff(Can_t *p_CAN);
ecu_status_t CANSPI_isRxErrorPassive(Can_t *p_CAN);
ecu_status_t CANSPI_isTxErrorPassive(Can_t *p_CAN);

#endif /* __CAN_SPI_H */
