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

#include "../inc/CANSPI.h"
#include "../inc/MCP2515.h"
#include "../inc/ecu.h"

/** Local Function Prototypes */
static ecu_status_t convertReg2ExtendedCANid(uint8_t tempRXBn_EIDH, uint8_t tempRXBn_EIDL, uint8_t tempRXBn_SIDH, 
                                             uint8_t tempRXBn_SIDL, uint32_t *p_ConvID);
static ecu_status_t convertReg2StandardCANid(uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL, uint32_t *p_ConvID);
static ecu_status_t convertCANid2Reg(uint32_t tempPassedInID, uint8_t canIdType, id_reg_t *passedIdReg);

/** Local Variables */
ctrl_status_t ctrlStatus;
ctrl_error_status_t errorStatus;
id_reg_t idReg;

/** CAN SPI APIs */

/* Entering Sleep Mode */
ecu_status_t CANSPI_Sleep(Can_t *p_CAN)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_CAN)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        /* Clear CAN bus wakeup interrupt */
        l_EcuStatus |= MCP2515_BitModify(p_CAN->UsedSPI, MCP2515_CANINTF, 0x40, 0x00);

        /* Enable CAN bus activity wakeup */
        l_EcuStatus |= MCP2515_BitModify(p_CAN->UsedSPI, MCP2515_CANINTE, 0x40, 0x40);

        l_EcuStatus |= MCP2515_SetSleepMode(p_CAN->UsedSPI);
    }
    return l_EcuStatus;
}

/* Initialize CAN */
ecu_status_t CANSPI_Initialize(Can_t *p_CAN)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_CAN)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        RXF0 RXF0reg;
        RXF1 RXF1reg;
        RXF2 RXF2reg;
        RXF3 RXF3reg;
        RXF4 RXF4reg;
        RXF5 RXF5reg;
        RXM0 RXM0reg;
        RXM1 RXM1reg;

        /* Intialize Rx Mask values */
        RXM0reg.RXM0SIDH = 0x00;
        RXM0reg.RXM0SIDL = 0x00;
        RXM0reg.RXM0EID8 = 0x00;
        RXM0reg.RXM0EID0 = 0x00;

        RXM1reg.RXM1SIDH = 0x00;
        RXM1reg.RXM1SIDL = 0x00;
        RXM1reg.RXM1EID8 = 0x00;
        RXM1reg.RXM1EID0 = 0x00;

        /* Intialize Rx Filter values */
        RXF0reg.RXF0SIDH = 0x00;
        RXF0reg.RXF0SIDL = 0x00; // Starndard Filter
        RXF0reg.RXF0EID8 = 0x00;
        RXF0reg.RXF0EID0 = 0x00;

        RXF1reg.RXF1SIDH = 0x00;
        RXF1reg.RXF1SIDL = 0x08; // Exntended Filter
        RXF1reg.RXF1EID8 = 0x00;
        RXF1reg.RXF1EID0 = 0x00;

        RXF2reg.RXF2SIDH = 0x00;
        RXF2reg.RXF2SIDL = 0x00;
        RXF2reg.RXF2EID8 = 0x00;
        RXF2reg.RXF2EID0 = 0x00;

        RXF3reg.RXF3SIDH = 0x00;
        RXF3reg.RXF3SIDL = 0x00;
        RXF3reg.RXF3EID8 = 0x00;
        RXF3reg.RXF3EID0 = 0x00;

        RXF4reg.RXF4SIDH = 0x00;
        RXF4reg.RXF4SIDL = 0x00;
        RXF4reg.RXF4EID8 = 0x00;
        RXF4reg.RXF4EID0 = 0x00;

        RXF5reg.RXF5SIDH = 0x00;
        RXF5reg.RXF5SIDL = 0x08;
        RXF5reg.RXF5EID8 = 0x00;
        RXF5reg.RXF5EID0 = 0x00;

        /* Intialize MCP2515, check SPI */
        l_EcuStatus |= MCP2515_Initialize(p_CAN->UsedSPI);
        
        if (l_EcuStatus == ECU_OK)
        {
            l_EcuStatus |= MCP2515_Reset(p_CAN->UsedSPI);

            /* Change mode as configuration mode */
            l_EcuStatus |= MCP2515_SetConfigMode(p_CAN->UsedSPI);
            if (l_EcuStatus == ECU_OK)
            {
                /* enable interrupt on receiving */
                l_EcuStatus |= MCP2515_WriteByte(p_CAN->UsedSPI, MCP2515_CANINTE, CAN_RX_INTERRUPT);

                /* Configure filter & mask */
                l_EcuStatus |= MCP2515_WriteByteSequence(p_CAN->UsedSPI, MCP2515_RXM0SIDH, MCP2515_RXM0EID0, 
                                                                                           &(RXM0reg.RXM0SIDH));
                l_EcuStatus |= MCP2515_WriteByteSequence(p_CAN->UsedSPI, MCP2515_RXM1SIDH, MCP2515_RXM1EID0, 
                                                                                           &(RXM1reg.RXM1SIDH));
                l_EcuStatus |= MCP2515_WriteByteSequence(p_CAN->UsedSPI, MCP2515_RXF0SIDH, MCP2515_RXF0EID0, 
                                                                                           &(RXF0reg.RXF0SIDH));
                l_EcuStatus |= MCP2515_WriteByteSequence(p_CAN->UsedSPI, MCP2515_RXF1SIDH, MCP2515_RXF1EID0, 
                                                                                           &(RXF1reg.RXF1SIDH));
                l_EcuStatus |= MCP2515_WriteByteSequence(p_CAN->UsedSPI, MCP2515_RXF2SIDH, MCP2515_RXF2EID0, 
                                                                                           &(RXF2reg.RXF2SIDH));
                l_EcuStatus |= MCP2515_WriteByteSequence(p_CAN->UsedSPI, MCP2515_RXF3SIDH, MCP2515_RXF3EID0, 
                                                                                           &(RXF3reg.RXF3SIDH));
                l_EcuStatus |= MCP2515_WriteByteSequence(p_CAN->UsedSPI, MCP2515_RXF4SIDH, MCP2515_RXF4EID0, 
                                                                                           &(RXF4reg.RXF4SIDH));
                l_EcuStatus |= MCP2515_WriteByteSequence(p_CAN->UsedSPI, MCP2515_RXF5SIDH, MCP2515_RXF5EID0, 
                                                                                           &(RXF5reg.RXF5SIDH));

                /* Accept All (Standard + Extended) */
                // Enable BUKT, Accept Filter 0
                l_EcuStatus |= MCP2515_WriteByte(p_CAN->UsedSPI, MCP2515_RXB0CTRL, 0x04); 
                // Accept Filter 1
                l_EcuStatus |= MCP2515_WriteByte(p_CAN->UsedSPI, MCP2515_RXB1CTRL, 0x01); 
                
                
                uint8_t l_CNF_val[3] = {0, 0, 0};

                switch (p_CAN->Speed)
                {
                case MCP_8MHz_1000kBPS:
                    l_CNF_val[0] = 0x00;
                    l_CNF_val[1] = 0xC0;
                    l_CNF_val[2] = 0x80;
                    break;

                case MCP_8MHz_500kBPS:
                    l_CNF_val[0] = 0x00;
                    l_CNF_val[1] = 0xD1;
                    l_CNF_val[2] = 0x81;
                    break;

                case MCP_8MHz_250kBPS:
                    l_CNF_val[0] = 0x80;
                    l_CNF_val[1] = 0xE5;
                    l_CNF_val[2] = 0x83;
                    break;

                case MCP_8MHz_200kBPS:
                    l_CNF_val[0] = 0x80;
                    l_CNF_val[1] = 0xF6;
                    l_CNF_val[2] = 0x84;
                    break;

                case MCP_8MHz_125kBPS:
                    l_CNF_val[0] = 0x81;
                    l_CNF_val[1] = 0xE5;
                    l_CNF_val[2] = 0x83;
                    break;

                case MCP_8MHz_100kBPS:
                    l_CNF_val[0] = 0x81;
                    l_CNF_val[1] = 0xF6;
                    l_CNF_val[2] = 0x84;
                    break;
                
                default:
                    l_EcuStatus = ECU_ERROR;
                    break;
                }

                l_EcuStatus |= MCP2515_WriteByte(p_CAN->UsedSPI, MCP2515_CNF1, l_CNF_val[0]);
                l_EcuStatus |= MCP2515_WriteByte(p_CAN->UsedSPI, MCP2515_CNF2, l_CNF_val[1]);
                l_EcuStatus |= MCP2515_WriteByte(p_CAN->UsedSPI, MCP2515_CNF3, l_CNF_val[2]);

                l_EcuStatus |= MCP2515_SetNormalMode(p_CAN->UsedSPI);
            }
        }
    }
    return l_EcuStatus;
}

/* Transmit CAN message */
ecu_status_t CANSPI_Transmit(Can_t *p_CAN, uCAN_MSG *tempCanMsg)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if ((NULL == p_CAN) || (NULL == tempCanMsg))
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        idReg.tempSIDH = 0;
        idReg.tempSIDL = 0;
        idReg.tempEID8 = 0;
        idReg.tempEID0 = 0;

        l_EcuStatus |= MCP2515_ReadStatus(p_CAN->UsedSPI, &ctrlStatus.ctrl_status);

        /* Finding empty buffer */
        if (ctrlStatus.TXB0REQ != 1)
        {
            /* convert CAN ID for register */
            l_EcuStatus |= convertCANid2Reg(tempCanMsg->frame.id, tempCanMsg->frame.idType, &idReg);

            /* Load data to Tx Buffer */
            l_EcuStatus |= MCP2515_LoadTxSequence(p_CAN->UsedSPI, MCP2515_LOAD_TXB0SIDH, &(idReg.tempSIDH), 
                                                  tempCanMsg->frame.dlc, &(tempCanMsg->frame.data[0]));

            /* Request to transmit */
            l_EcuStatus |= MCP2515_RequestToSend(p_CAN->UsedSPI, MCP2515_RTS_TX0);
        }
        else if (ctrlStatus.TXB1REQ != 1)
        {
            l_EcuStatus |= convertCANid2Reg(tempCanMsg->frame.id, tempCanMsg->frame.idType, &idReg);

            l_EcuStatus |= MCP2515_LoadTxSequence(p_CAN->UsedSPI, MCP2515_LOAD_TXB1SIDH, &(idReg.tempSIDH), 
                                                  tempCanMsg->frame.dlc, &(tempCanMsg->frame.data[0]));
            l_EcuStatus |= MCP2515_RequestToSend(p_CAN->UsedSPI, MCP2515_RTS_TX1);
        }
        else if (ctrlStatus.TXB2REQ != 1)
        {
            l_EcuStatus |= convertCANid2Reg(tempCanMsg->frame.id, tempCanMsg->frame.idType, &idReg);

            l_EcuStatus |= MCP2515_LoadTxSequence(p_CAN->UsedSPI, MCP2515_LOAD_TXB2SIDH, &(idReg.tempSIDH), 
                                                  tempCanMsg->frame.dlc, &(tempCanMsg->frame.data[0]));
            l_EcuStatus |= MCP2515_RequestToSend(p_CAN->UsedSPI, MCP2515_RTS_TX2);
        }
        else
        {
            l_EcuStatus = ECU_ERROR;
        }
    }
    return l_EcuStatus;
}

/* Receive CAN message */
ecu_status_t CANSPI_Receive(Can_t *p_CAN, uCAN_MSG *tempCanMsg)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if ((NULL == p_CAN) || (NULL == tempCanMsg))
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        rx_reg_t rxReg;
        ctrl_rx_status_t rxStatus;

        l_EcuStatus |= MCP2515_GetRxStatus(p_CAN->UsedSPI, &rxStatus.ctrl_rx_status);

        /* Check receive buffer */
        if (rxStatus.rxBuffer != 0)
        {
            /* finding buffer which has a message */
            if ((rxStatus.rxBuffer == MSG_IN_RXB0) | (rxStatus.rxBuffer == MSG_IN_BOTH_BUFFERS))
            {
                l_EcuStatus |= MCP2515_ReadRxSequence(p_CAN->UsedSPI, MCP2515_READ_RXB0SIDH, rxReg.rx_reg_array, 
                                                      sizeof(rxReg.rx_reg_array));
                #if CAN_RX_INTERRUPT == CAN_RX_INTERRUP_ENABLE
                l_EcuStatus |= MCP2515_BitModify(p_CAN->UsedSPI, MCP2515_CANINTF, 0xFF, 0x00);
                #endif
            }
            else if (rxStatus.rxBuffer == MSG_IN_RXB1)
            {
                l_EcuStatus |= MCP2515_ReadRxSequence(p_CAN->UsedSPI, MCP2515_READ_RXB1SIDH, rxReg.rx_reg_array, 
                                                      sizeof(rxReg.rx_reg_array));
                #if CAN_RX_INTERRUPT == CAN_RX_INTERRUP_ENABLE
                l_EcuStatus |= MCP2515_BitModify(p_CAN->UsedSPI, MCP2515_CANINTF, 0xFF, 0x00);
                #endif
            }

            /* if the message is extended CAN type */
            if (rxStatus.msgType == dEXTENDED_CAN_MSG_ID_2_0B)
            {
                tempCanMsg->frame.idType = (uint8_t)dEXTENDED_CAN_MSG_ID_2_0B;
                l_EcuStatus |= convertReg2ExtendedCANid(rxReg.RXBnEID8, rxReg.RXBnEID0, rxReg.RXBnSIDH, 
                                                        rxReg.RXBnSIDL, &tempCanMsg->frame.id);
            }
            else
            {
                /* Standard type */
                tempCanMsg->frame.idType = (uint8_t)dSTANDARD_CAN_MSG_ID_2_0B;
                l_EcuStatus |= convertReg2StandardCANid(rxReg.RXBnSIDH, rxReg.RXBnSIDL, &tempCanMsg->frame.id);
            }

            tempCanMsg->frame.dlc = rxReg.RXBnDLC;
            tempCanMsg->frame.data[0] = rxReg.RXBnD0;
            tempCanMsg->frame.data[1] = rxReg.RXBnD1;
            tempCanMsg->frame.data[2] = rxReg.RXBnD2;
            tempCanMsg->frame.data[3] = rxReg.RXBnD3;
            tempCanMsg->frame.data[4] = rxReg.RXBnD4;
            tempCanMsg->frame.data[5] = rxReg.RXBnD5;
            tempCanMsg->frame.data[6] = rxReg.RXBnD6;
            tempCanMsg->frame.data[7] = rxReg.RXBnD7;
        }
        else
        {
            l_EcuStatus = ECU_ERROR;
        }
    }
    return l_EcuStatus;
}

/* check message buffer and return count */
ecu_status_t CANSPI_messagesInBuffer(Can_t *p_CAN, uint8_t *count)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if ((NULL == p_CAN) || (NULL == count))
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        uint8_t messageCount = 0;

        l_EcuStatus |= MCP2515_ReadStatus(p_CAN->UsedSPI, &ctrlStatus.ctrl_status);

        if (ctrlStatus.RX0IF != 0)
        {
            messageCount++;
        }

        if (ctrlStatus.RX1IF != 0)
        {
            messageCount++;
        }

        *count = messageCount;
    }
    return l_EcuStatus;
}

/* check BUS off */
ecu_status_t CANSPI_isBussOff(Can_t *p_CAN)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_CAN)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        l_EcuStatus |= MCP2515_ReadByte(p_CAN->UsedSPI, MCP2515_EFLG, &errorStatus.error_flag_reg);

        if (errorStatus.TXBO == 1)
        {
            l_EcuStatus = ECU_ERROR;
        }
    }
    return l_EcuStatus;
}

/* check Rx Passive Error */
ecu_status_t CANSPI_isRxErrorPassive(Can_t *p_CAN)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_CAN)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        l_EcuStatus |= MCP2515_ReadByte(p_CAN->UsedSPI, MCP2515_EFLG ,&errorStatus.error_flag_reg );

        if (errorStatus.RXEP == 1)
        {
            l_EcuStatus = ECU_ERROR;
        }
    }
    return l_EcuStatus;
}

/* check Tx Passive Error */
ecu_status_t CANSPI_isTxErrorPassive(Can_t *p_CAN)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if (NULL == p_CAN)
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        l_EcuStatus |= MCP2515_ReadByte(p_CAN->UsedSPI, MCP2515_EFLG, &errorStatus.error_flag_reg);

        if (errorStatus.TXEP == 1)
        {
            l_EcuStatus = ECU_ERROR;
        }
    }
    return l_EcuStatus;
}

/* convert register value to extended CAN ID */
static ecu_status_t convertReg2ExtendedCANid(uint8_t tempRXBn_EIDH, uint8_t tempRXBn_EIDL, uint8_t tempRXBn_SIDH, 
                                             uint8_t tempRXBn_SIDL, uint32_t *p_ConvID)
{
    uint32_t returnValue = 0;
    uint32_t ConvertedID = 0;
    uint8_t CAN_standardLo_ID_lo2bits;
    uint8_t CAN_standardLo_ID_hi3bits;

    CAN_standardLo_ID_lo2bits = (tempRXBn_SIDL & 0x03);
    CAN_standardLo_ID_hi3bits = (tempRXBn_SIDL >> 5);
    ConvertedID = (tempRXBn_SIDH << 3);
    ConvertedID = ConvertedID + CAN_standardLo_ID_hi3bits;
    ConvertedID = (ConvertedID << 2);
    ConvertedID = ConvertedID + CAN_standardLo_ID_lo2bits;
    ConvertedID = (ConvertedID << 8);
    ConvertedID = ConvertedID + tempRXBn_EIDH;
    ConvertedID = (ConvertedID << 8);
    ConvertedID = ConvertedID + tempRXBn_EIDL;
    returnValue = ConvertedID;
    return (returnValue);
}

/* convert register value to standard CAN ID */
static ecu_status_t convertReg2StandardCANid(uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL, uint32_t *p_ConvID)
{
	ecu_status_t l_EcuStatus = ECU_OK;
    uint32_t ConvertedID;

    ConvertedID = (tempRXBn_SIDH << 3);
    ConvertedID = ConvertedID + (tempRXBn_SIDL >> 5);
    *p_ConvID = ConvertedID;

    return (l_EcuStatus);
}

/* convert CAN ID to register value */
static ecu_status_t convertCANid2Reg(uint32_t tempPassedInID, uint8_t canIdType, id_reg_t *passedIdReg)
{
	ecu_status_t l_EcuStatus = ECU_OK;
    uint8_t wipSIDL = 0;

    if (canIdType == dEXTENDED_CAN_MSG_ID_2_0B)
    {
        // EID0
        passedIdReg->tempEID0 = 0xFF & tempPassedInID;
        tempPassedInID = tempPassedInID >> 8;

        // EID8
        passedIdReg->tempEID8 = 0xFF & tempPassedInID;
        tempPassedInID = tempPassedInID >> 8;

        // SIDL
        wipSIDL = 0x03 & tempPassedInID;
        tempPassedInID = tempPassedInID << 3;
        wipSIDL = (0xE0 & tempPassedInID) + wipSIDL;
        wipSIDL = wipSIDL + 0x08;
        passedIdReg->tempSIDL = 0xEB & wipSIDL;

        // SIDH
        tempPassedInID = tempPassedInID >> 8;
        passedIdReg->tempSIDH = 0xFF & tempPassedInID;
    }
    else
    {
        passedIdReg->tempEID8 = 0;
        passedIdReg->tempEID0 = 0;
        tempPassedInID = tempPassedInID << 5;
        passedIdReg->tempSIDL = 0xFF & tempPassedInID;
        tempPassedInID = tempPassedInID >> 8;
        passedIdReg->tempSIDH = 0xFF & tempPassedInID;
    }
    return (l_EcuStatus);
}
