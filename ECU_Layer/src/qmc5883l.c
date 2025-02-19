/**
 * @file    qmc5883l.c
 * @author  Ahmed Hani
 * @brief   contains all interfaces of compass module (QMC5883L)
 * @date    2025-02-013
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../inc/qmc5883l.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/

#define RAW_TO_GAUSS                    (1.0f / 3000.0f)



/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                               STATIC FUNCTION DEFINITION                                             *
***********************************************************************************************************************/





/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/

static int16_t Calibrations[3][2] = {{-2398, 278}, 
                                     {-1186, 1587}, 
                                     {-1   , 3838}};

static float_t Calibrations_offset[3];
static float_t Calibrations_scale[3];

/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/

/**
 * @brief helper function to read registers from the compass
 * 
 * @param p_Compass pointer to the compass
 * @param p_Address address of the register
 * @param p_Data pointer to the buffer where the read data will be stored
 * @param p_Lenght number of registers will be read
 * @return ecu_status_t status of operation
 */
static ecu_status_t QMC5883L_read_reg(QMC5883L_t *p_Compass, uint8_t p_Address, uint8_t *p_Data, uint8_t p_Lenght);

/**
 * @brief helper function to write data in one register
 * 
 * @param p_Compass pointer to the register
 * @param p_Address address of the register
 * @param p_Data data will be wrote to it
 * @return ecu_status_t status of register
 */
static ecu_status_t QMC5883L_write_reg(QMC5883L_t *p_Compass, uint8_t p_Address, uint8_t p_Data);




/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
 * @brief init the compass according to the configurations wanted by user
 * 
 * @param p_compass pointer to the compass and inculding the configurations 
 * @return ecu_status_t status of operation
 */
ecu_status_t QMC5883L_init(QMC5883L_t *p_Compass)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if ((p_Compass == NULL) || (p_Compass->UsedI2C == NULL))
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        uint8_t l_ChipID = 0;
        l_EcuStatus |= QMC5883L_read_reg(p_Compass, QMC5883L_REG_CHIP_ID, &l_ChipID, 1);
        if ((l_EcuStatus == ECU_OK) && (l_ChipID == QMC5883L_ID_VAL))
        {
            l_EcuStatus |= QMC5883L_write_reg(p_Compass, QMC5883L_REG_RESET, 0);
            uint8_t l_ControlReg1 = 0;
            l_ControlReg1 = (p_Compass->OutputDataRate) | 
                            (p_Compass->Mode)           | 
                            (p_Compass->FullScale)      | 
                            (p_Compass->OverSampleRatio);
            l_EcuStatus |= QMC5883L_write_reg(p_Compass, QMC5883L_REG_CTRL1, l_ControlReg1);

            uint8_t l_ControlReg2 = 0;
            l_ControlReg2 = p_Compass->DRDY;
            l_EcuStatus |= QMC5883L_write_reg(p_Compass, QMC5883L_REG_CTRL2, l_ControlReg2);

            Calibrations_offset[0] = (float_t)(Calibrations[0][0] + Calibrations[0][1]) / 2.0f;
            Calibrations_offset[1] = (float_t)(Calibrations[1][0] + Calibrations[1][1]) / 2.0f;
            Calibrations_offset[2] = (float_t)(Calibrations[2][0] + Calibrations[2][1]) / 2.0f;
        
            float_t x_avg_delta = (float_t)(Calibrations[0][1] - Calibrations[0][0]) / 2.0f;
            float_t y_avg_delta = (float_t)(Calibrations[1][1] - Calibrations[1][0]) / 2.0f;
            float_t z_avg_delta = (float_t)(Calibrations[2][1] - Calibrations[2][0]) / 2.0f;
        
            float_t avg_delta = (x_avg_delta + y_avg_delta + z_avg_delta) / 3.0f;
        
            Calibrations_scale[0] = avg_delta / x_avg_delta;
            Calibrations_scale[1] = avg_delta / y_avg_delta;
            Calibrations_scale[2] = avg_delta / z_avg_delta;
        }
    }
    return l_EcuStatus;
}

/**
 * @brief update the x, y and z magnetic values in the data type without any processing
 * 
 * @param p_compass pointer to the compass
 * @return ecu_status_t status of operation
 */
ecu_status_t QMC5883L_read_raw(QMC5883L_t *p_Compass)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if ((p_Compass == NULL) || (p_Compass->UsedI2C == NULL))
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        uint8_t temp = 0;
        while ((p_Compass->DRDY == DRDY_INT_DISABLE) && ((temp & 0x01) == 0))
        {
            l_EcuStatus |= QMC5883L_read_reg(p_Compass, QMC5883L_REG_STATUS, &temp, 1);
        };
        uint8_t data[6] = {0};
        l_EcuStatus |= QMC5883L_read_reg(p_Compass, QMC5883L_REG_X_LSB, data, 6);
        if (l_EcuStatus == ECU_OK)
        {
            int16_t temp_x = (int16_t)(((int16_t)data[1] << 8) | (int16_t)(data[0]));  /* X */
            int16_t temp_y = (int16_t)(((int16_t)data[3] << 8) | (int16_t)(data[2]));  /* Y */
            int16_t temp_z = (int16_t)(((int16_t)data[5] << 8) | (int16_t)(data[4]));  /* Z */
            
            p_Compass->x = ((float_t)temp_x - Calibrations_offset[0]) * Calibrations_scale[0];
            p_Compass->y = ((float_t)temp_y - Calibrations_offset[1]) * Calibrations_scale[1];
            p_Compass->z = ((float_t)temp_z - Calibrations_offset[2]) * Calibrations_scale[2];
            // p_Compass->x = ((float_t)temp_x);
            // p_Compass->y = ((float_t)temp_y);
            // p_Compass->z = ((float_t)temp_z);
        }
    }
    return l_EcuStatus;
}

/**
 * @brief update the angle of the compass
 * 
 * @param p_compass pointer to the compass
 * @return ecu_status_t status of operation
 */
ecu_status_t QMC5883L_read_angle(QMC5883L_t *p_Compass, float_t *p_Angle)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    if ((p_Compass == NULL) || (p_Compass->UsedI2C == NULL))
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        l_EcuStatus |= QMC5883L_read_raw(p_Compass);
        if (l_EcuStatus == ECU_OK)
        {
            float_t x_gauss = ((float_t)p_Compass->x * RAW_TO_GAUSS);
            float_t y_gauss = ((float_t)p_Compass->y * RAW_TO_GAUSS);
            float_t z_gauss = ((float_t)p_Compass->z * RAW_TO_GAUSS);
            
            float_t l_TempAngle = 0;
            l_TempAngle = atan2(y_gauss, x_gauss) * (180.0f / M_PI);
            if (l_TempAngle < 0) {
                l_TempAngle += 360.0f;
            }
            *p_Angle = l_TempAngle;
        }
    }
    return l_EcuStatus;
}



/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/

/**
 * @brief helper function to read registers from the compass
 * 
 * @param p_Compass pointer to the compass
 * @param p_Address address of the register
 * @param p_Data pointer to the buffer where the read data will be stored
 * @param p_Lenght number of registers will be read
 * @return ecu_status_t status of operation
 */
static ecu_status_t QMC5883L_read_reg(QMC5883L_t *p_Compass, uint8_t p_Address, uint8_t *p_Data, uint8_t p_Lenght)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    HAL_StatusTypeDef l_HalStatus = HAL_OK;
    if ((p_Compass == NULL) || (p_Compass->UsedI2C == NULL) || (p_Data == NULL))
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        l_HalStatus |= HAL_I2C_Mem_Read(p_Compass->UsedI2C, 
                                        QMC5883L_I2C_ADDR << 1, 
                                        p_Address, 
                                        I2C_MEMADD_SIZE_8BIT, 
                                        p_Data, 
                                        p_Lenght, 
                                        QMC5883L_I2C_TIMEOUT);
        if (l_HalStatus != HAL_OK)
        {
            l_EcuStatus = ECU_ERROR;
        }
    }
    return l_EcuStatus;
}

/**
 * @brief helper function to write data in one register
 * 
 * @param p_Compass pointer to the register
 * @param p_Address address of the register
 * @param p_Data data will be wrote to it
 * @return ecu_status_t status of register
 */
static ecu_status_t QMC5883L_write_reg(QMC5883L_t *p_Compass, uint8_t p_Address, uint8_t p_Data)
{
    ecu_status_t l_EcuStatus = ECU_OK;
    HAL_StatusTypeDef l_HalStatus = HAL_OK;
    if ((p_Compass == NULL) || (p_Compass->UsedI2C == NULL))
    {
        l_EcuStatus = ECU_ERROR;
    }
    else
    {
        l_HalStatus |= HAL_I2C_Mem_Write(p_Compass->UsedI2C, 
                                        QMC5883L_I2C_ADDR << 1, 
                                        p_Address, 
                                        I2C_MEMADD_SIZE_8BIT, 
                                        &p_Data, 
                                        1, 
                                        QMC5883L_I2C_TIMEOUT);
        if (l_HalStatus != HAL_OK)
        {
            l_EcuStatus = ECU_ERROR;
        }
    }
    return l_EcuStatus;
}

ecu_status_t read_control_register(QMC5883L_t *p_Compass)
{
	ecu_status_t l_EcuStatus = ECU_OK;
	HAL_StatusTypeDef l_HalStatus = HAL_OK;
	if (p_Compass == NULL)
	{
	    l_EcuStatus = ECU_ERROR;
	}
    else
    {
        uint8_t cont_reg_1;
        uint8_t cont_reg_2; 
        l_EcuStatus = QMC5883L_read_reg(p_Compass , QMC5883L_REG_CTRL1 , &cont_reg_1 , 1);
        l_EcuStatus = QMC5883L_read_reg(p_Compass , QMC5883L_REG_CTRL2 , &cont_reg_2 , 1);
    }
	
	return l_EcuStatus;
}


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
