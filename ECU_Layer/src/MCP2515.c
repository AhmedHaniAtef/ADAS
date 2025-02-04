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

#include "../inc/MCP2515.h"
#include "../inc/ecu.h"

/* SPI related variables */
#define SPI_TIMEOUT HAL_MAX_DELAY
#define MCP2515_CS_HIGH() HAL_GPIO_WritePin(CAN_MCP2515_CS_PORT, CAN_MCP2515_CS_PIN, GPIO_PIN_SET)
#define MCP2515_CS_LOW() HAL_GPIO_WritePin(CAN_MCP2515_CS_PORT, CAN_MCP2515_CS_PIN, GPIO_PIN_RESET)

/* Prototypes */
static ecu_status_t SPI_Tx(SPI_HandleTypeDef *p_UsedSPI, uint8_t data);
static ecu_status_t SPI_TxBuffer(SPI_HandleTypeDef *p_UsedSPI, uint8_t *buffer, uint8_t length);
static ecu_status_t SPI_Rx(SPI_HandleTypeDef *p_UsedSPI, uint8_t *p_Buffer);
static ecu_status_t SPI_RxBuffer(SPI_HandleTypeDef *p_UsedSPI, uint8_t *buffer, uint8_t length);

/* initialize MCP2515 */
ecu_status_t MCP2515_Initialize(SPI_HandleTypeDef *p_UsedSPI)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        MCP2515_CS_HIGH();

        uint8_t loop = 10;

        do
        {
            /* check SPI Ready */
            if (HAL_SPI_GetState(p_UsedSPI) == HAL_SPI_STATE_READY)
            {
                l_EcuStatus = ECU_OK;
                break;
            }
            else
            {
                loop--;
                l_EcuStatus = ECU_ERROR;
            }
        } while (loop > 0);
    }
    return l_EcuStatus;
}

/* change mode as configuration mode */
ecu_status_t MCP2515_SetConfigMode(SPI_HandleTypeDef *p_UsedSPI)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        /* configure CANCTRL Register */
        l_EcuStatus |= MCP2515_WriteByte(p_UsedSPI, MCP2515_CANCTRL, 0x80);

        uint8_t loop = 10;

        do
        {
            /* confirm mode configuration */
            uint8_t temp = 0;
            l_EcuStatus |= MCP2515_ReadByte(p_UsedSPI, MCP2515_CANSTAT, &temp);
            if ((temp & 0xE0) == 0x80)
            {
                l_EcuStatus = ECU_OK;
            }
            else
            {
                l_EcuStatus = ECU_ERROR;
            }

            loop--;
        } while (loop > 0);
    }
    return l_EcuStatus;    
}

/* change mode as normal mode */
ecu_status_t MCP2515_SetNormalMode(SPI_HandleTypeDef *p_UsedSPI)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        /* configure CANCTRL Register */
        l_EcuStatus |= MCP2515_WriteByte(p_UsedSPI, MCP2515_CANCTRL, 0x00);

        uint8_t loop = 10;

        do
        {
            /* confirm mode configuration */
            uint8_t temp = 0;
            l_EcuStatus |= MCP2515_ReadByte(p_UsedSPI, MCP2515_CANSTAT, &temp);
            if ((temp & 0xE0) == 0x00)
            {
                l_EcuStatus = ECU_OK;
            }
            else
            {
                l_EcuStatus = ECU_ERROR;
            }

            loop--;
        } while (loop > 0);
    }
    return l_EcuStatus;    
}

/* Entering sleep mode */
ecu_status_t MCP2515_SetSleepMode(SPI_HandleTypeDef *p_UsedSPI)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        /* configure CANCTRL Register */
        l_EcuStatus |= MCP2515_WriteByte(p_UsedSPI, MCP2515_CANCTRL, 0x20);

        uint8_t loop = 10;

        do
        {
            /* confirm mode configuration */
            uint8_t temp = 0;
            l_EcuStatus |= MCP2515_ReadByte(p_UsedSPI, MCP2515_CANSTAT, &temp);
            if ((temp & 0xE0) == 0x20)
            {
                l_EcuStatus = ECU_OK;
            }
            else
            {
                l_EcuStatus = ECU_ERROR;
            }

            loop--;
        } while (loop > 0);
    }
    return l_EcuStatus;    
}

/* MCP2515 SPI-Reset */
ecu_status_t MCP2515_Reset(SPI_HandleTypeDef *p_UsedSPI)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        MCP2515_CS_LOW();

        l_EcuStatus |= SPI_Tx(p_UsedSPI, MCP2515_RESET);

        MCP2515_CS_HIGH();
    }
    return l_EcuStatus;
}

/* read single byte */
ecu_status_t MCP2515_ReadByte (SPI_HandleTypeDef *p_UsedSPI,uint8_t address, uint8_t *buffer)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        uint8_t retVal;

        MCP2515_CS_LOW();

        l_EcuStatus |= SPI_Tx(p_UsedSPI, MCP2515_READ);
        l_EcuStatus |= SPI_Tx(p_UsedSPI, address);
        l_EcuStatus |= SPI_Rx(p_UsedSPI, &retVal);

        MCP2515_CS_HIGH();

        *buffer = retVal;
    }
    return l_EcuStatus;
}

/* read buffer */
ecu_status_t MCP2515_ReadRxSequence(SPI_HandleTypeDef *p_UsedSPI, uint8_t instruction, uint8_t *data, uint8_t length)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        MCP2515_CS_LOW();

        l_EcuStatus |= SPI_Tx(p_UsedSPI, instruction);
        l_EcuStatus |= SPI_RxBuffer(p_UsedSPI, data, length);

        MCP2515_CS_HIGH();
    }
    return l_EcuStatus;
}

/* write single byte */
ecu_status_t MCP2515_WriteByte(SPI_HandleTypeDef *p_UsedSPI, uint8_t address, uint8_t data)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        MCP2515_CS_LOW();

        l_EcuStatus |= SPI_Tx(p_UsedSPI, MCP2515_WRITE);
        l_EcuStatus |= SPI_Tx(p_UsedSPI, address);
        l_EcuStatus |= SPI_Tx(p_UsedSPI, data);

        MCP2515_CS_HIGH();
    }
    return l_EcuStatus;
}

/* write buffer */
ecu_status_t MCP2515_WriteByteSequence(SPI_HandleTypeDef *p_UsedSPI, 
                                       uint8_t startAddress, 
                                       uint8_t endAddress, 
                                       uint8_t *data)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        MCP2515_CS_LOW();

        l_EcuStatus |= SPI_Tx(p_UsedSPI, MCP2515_WRITE);
        l_EcuStatus |= SPI_Tx(p_UsedSPI, startAddress);
        l_EcuStatus |= SPI_TxBuffer(p_UsedSPI, data, (endAddress - startAddress + 1));

        MCP2515_CS_HIGH();
    }
    return l_EcuStatus;
}

/* write to TxBuffer */
ecu_status_t MCP2515_LoadTxSequence(SPI_HandleTypeDef *p_UsedSPI, 
                                    uint8_t instruction, 
                                    uint8_t *idReg, 
                                    uint8_t dlc, 
                                    uint8_t *data)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        MCP2515_CS_LOW();

        l_EcuStatus |= SPI_Tx(p_UsedSPI, instruction);
        l_EcuStatus |= SPI_TxBuffer(p_UsedSPI, idReg, 4);
        l_EcuStatus |= SPI_Tx(p_UsedSPI, dlc);
        l_EcuStatus |= SPI_TxBuffer(p_UsedSPI, data, dlc);

        MCP2515_CS_HIGH();

    }
    return l_EcuStatus;
}

/* write to TxBuffer(1 byte) */
ecu_status_t MCP2515_LoadTxBuffer(SPI_HandleTypeDef *p_UsedSPI, uint8_t instruction, uint8_t data)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        MCP2515_CS_LOW();

        l_EcuStatus |= SPI_Tx(p_UsedSPI, instruction);
        l_EcuStatus |= SPI_Tx(p_UsedSPI, data);

        MCP2515_CS_HIGH();
    }
    return l_EcuStatus;
}

/* request to send */
ecu_status_t MCP2515_RequestToSend(SPI_HandleTypeDef *p_UsedSPI, uint8_t instruction)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        MCP2515_CS_LOW();
 
        l_EcuStatus |= SPI_Tx(p_UsedSPI, instruction);

        MCP2515_CS_HIGH();
    }
    return l_EcuStatus;
}

/* read status */
ecu_status_t MCP2515_ReadStatus(SPI_HandleTypeDef *p_UsedSPI, uint8_t *p_Buffer)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        uint8_t retVal;

        MCP2515_CS_LOW();

        l_EcuStatus |= SPI_Tx(p_UsedSPI, MCP2515_READ_STATUS);
        l_EcuStatus |= SPI_Rx(p_UsedSPI, &retVal);

        MCP2515_CS_HIGH();

        *p_Buffer = retVal;
    }
    return l_EcuStatus;
}

/* read RX STATUS register */
ecu_status_t MCP2515_GetRxStatus(SPI_HandleTypeDef *p_UsedSPI, uint8_t *p_Buffer)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        uint8_t retVal;

        MCP2515_CS_LOW();

        l_EcuStatus |= SPI_Tx(p_UsedSPI, MCP2515_RX_STATUS);
        l_EcuStatus |= SPI_Rx(p_UsedSPI, &retVal);

        MCP2515_CS_HIGH();

        *p_Buffer = retVal;
    }
    return l_EcuStatus;
}

/* Use when changing register value */
ecu_status_t MCP2515_BitModify(SPI_HandleTypeDef *p_UsedSPI, uint8_t address, uint8_t mask, uint8_t data)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        MCP2515_CS_LOW();

        l_EcuStatus |= SPI_Tx(p_UsedSPI, MCP2515_BIT_MOD);
        l_EcuStatus |= SPI_Tx(p_UsedSPI, address);
        l_EcuStatus |= SPI_Tx(p_UsedSPI, mask);
        l_EcuStatus |= SPI_Tx(p_UsedSPI, data);

        MCP2515_CS_HIGH();
    }
    return l_EcuStatus;
}

/* SPI Tx wrapper function  */
static ecu_status_t SPI_Tx(SPI_HandleTypeDef *p_UsedSPI, uint8_t data)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        if (HAL_SPI_Transmit(p_UsedSPI, &data, 1, SPI_TIMEOUT) != HAL_OK)
        {
            l_EcuStatus = ECU_ERROR;
        }
    }
    return l_EcuStatus;
}

/* SPI Tx wrapper function */
static ecu_status_t SPI_TxBuffer(SPI_HandleTypeDef *p_UsedSPI, uint8_t *buffer, uint8_t length)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        if (HAL_SPI_Transmit(p_UsedSPI, buffer, length, SPI_TIMEOUT) != HAL_OK)
        {
            l_EcuStatus = ECU_ERROR;
        }
    }
    return l_EcuStatus;
}

/* SPI Rx wrapper function */
static ecu_status_t SPI_Rx(SPI_HandleTypeDef *p_UsedSPI, uint8_t *p_Buffer)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        uint8_t retVal = 0;
        if (HAL_SPI_Receive(p_UsedSPI, &retVal, 1, SPI_TIMEOUT) != HAL_OK)
        {
            l_EcuStatus = ECU_ERROR;
        }
        else
        {
            *p_Buffer = retVal;
        }
    }
    return l_EcuStatus;
}

/* SPI Rx wrapper function */
static ecu_status_t SPI_RxBuffer(SPI_HandleTypeDef *p_UsedSPI, uint8_t *buffer, uint8_t length)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_UsedSPI)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        if (HAL_SPI_Receive(p_UsedSPI, buffer, length, SPI_TIMEOUT) != HAL_OK)
        {
            l_EcuStatus = ECU_ERROR;
        }
    }
    return l_EcuStatus;
}
