/**
 * @file    mpu.c
 * @brief   Source file for MPU6050
 * @authors Eslam Sameh & Youssef Benyamine
 * @date    2024-11-25
 */
#include "mpu.h"

/**
 * @brief  Initializes MPU6050 and I2C peripheral
 * @param  *p_MPU6050: Pointer to empty @ref MPU6050_t structure
 * @param  p_AccelerometerSensitivity: Set accelerometer sensitivity. This parameter can be a value of @ref MPU6050_accelerometer_t enumeration
 * @param  p_GyroscopeSensitivity: Set gyroscope sensitivity. This parameter can be a value of @ref MPU6050_gyroscope_t enumeration
 * @retval Initialization status:
 *            - MPU6050_result_t: Everything OK
 *            - Other member: in other cases
 */
MPU6050_result_t MPU6050_init(MPU6050_t *p_MPU6050, MPU6050_accelerometer_t p_AccelerometerSensitivity, MPU6050_gyroscope_t p_GyroscopeSensitivity)
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	uint8_t l_Data[2];
	uint8_t l_DataReturn = 0;
	if (NULL == p_MPU6050)
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		uint8_t l_WhoAmI = (uint8_t)MPU6050_WHO_AM_I;

		/* Check if device is connected */
		if (HAL_I2C_IsDeviceReady(p_MPU6050->I2Cx, p_MPU6050->Address, 2, 5) != HAL_OK)
		{
			return MPU6050_RESULT_ERROR;
		}

		/* Send address */

		if (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, p_MPU6050->Address, &l_WhoAmI, 1, 1000) != HAL_OK)
		{
			return MPU6050_RESULT_ERROR;
		}

		/* Receive multiple byte */
		if (HAL_I2C_Master_Receive(p_MPU6050->I2Cx, p_MPU6050->Address, &l_DataReturn, 1, 1000) != HAL_OK)
		{
			return MPU6050_RESULT_ERROR;
		}

		/* Checking */
		while (l_DataReturn != MPU6050_I_AM)
		{
			/* Return error */
			return MPU6050_RESULT_ERROR;
		}
		//------------------

		/* Wakeup MPU6050 */
		//------------------
		/* Format array to send */
		l_Data[0] = MPU6050_PWR_MGMT_1;
		l_Data[1] = 0x00;
		/* Try to transmit via I2C */
		if (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), (uint8_t *)l_Data, 2, 1000) != HAL_OK)
		{
			return MPU6050_RESULT_ERROR;
		}

		//-----------------
		/* Check who am I */
		//------------------

		/* Set sample rate to 8kHz */
		MPU6050_set_data_rate(p_MPU6050, (uint8_t)MPU6050_DATARATE_8KHZ);

		/* Config accelerometer */
		MPU6050_set_accelerometer(p_MPU6050, p_AccelerometerSensitivity);

		/* Config Gyroscope */
		MPU6050_set_gyroscope(p_MPU6050, p_GyroscopeSensitivity);

		/* Enables interrupts */
		MPU6050_enable_interrupts(p_MPU6050);

		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}

/**
 * @brief  Sets gyroscope sensitivity
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure indicating MPU6050 device
 * @param  p_GyroscopeSensitivity: gyroscope sensitivity value. This parameter can be a value of @ref MPU6050_gyroscope_t enumeration
 * @retval Member of @ref MPU6050_result_t enumeration
 */
MPU6050_result_t MPU6050_set_gyroscope(MPU6050_t *p_MPU6050, MPU6050_gyroscope_t p_GyroscopeSensitivity)
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	uint8_t l_Register = (uint8_t)MPU6050_GYRO_CONFIG;
	uint8_t l_DataReturn = 0;
	if (NULL == p_MPU6050)
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		/* Config gyroscope */
		while (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_Register, 1, 1000) != HAL_OK)
		{
			return MPU6050_RESULT_ERROR;
		}

		while (HAL_I2C_Master_Receive(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_DataReturn, 1, 1000) != HAL_OK)
		{
			return MPU6050_RESULT_ERROR;
		}

		l_DataReturn = (l_DataReturn & 0xE7) | (uint8_t)p_GyroscopeSensitivity << 3; // 0XE7 to no change the l_DataReturn and reset bit 3 and 4

		while (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_DataReturn, 1, 1000) != HAL_OK)
		{
			return MPU6050_RESULT_ERROR;
		}

		switch (p_GyroscopeSensitivity)
		{
			case (MPU6050_GYROSCOPE_250S):
				p_MPU6050->GyroscopeMult = (float)1 / MPU6050_GYRO_SENS_250;
				break;
			case (MPU6050_GYROSCOPE_500S):
				p_MPU6050->GyroscopeMult = (float)1 / MPU6050_GYRO_SENS_500;
				break;
			case (MPU6050_GYROSCOPE_1000S):
				p_MPU6050->GyroscopeMult = (float)1 / MPU6050_GYRO_SENS_1000;
				break;
			case (MPU6050_GYROSCOPE_2000S):
				p_MPU6050->GyroscopeMult = (float)1 / MPU6050_GYRO_SENS_2000;
				break;
			default:
				break;
		}

		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}

/**
 * @brief  Sets accelerometer sensitivity
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure indicating MPU6050 device
 * @param  p_AccelerometerSensitivity: Gyro sensitivity value. This parameter can be a value of @ref MPU6050_accelerometer_t enumeration
 * @retval Member of @ref MPU6050_result_t enumeration
 */
MPU6050_result_t MPU6050_set_accelerometer(MPU6050_t *p_MPU6050, MPU6050_accelerometer_t p_AccelerometerSensitivity)
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	uint8_t l_Register = (uint8_t)MPU6050_ACCEL_CONFIG;
	uint8_t l_DataReturn = 0;
	if (NULL == p_MPU6050)
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		/* Config gyroscope */
		while (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_Register, 1, 1000) != HAL_OK)
		{
			return MPU6050_RESULT_ERROR;
		}

		while (HAL_I2C_Master_Receive(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_DataReturn, 1, 1000) != HAL_OK)
		{
			return MPU6050_RESULT_ERROR;
		}

		l_DataReturn = (l_DataReturn & 0xE7) | (uint8_t)p_AccelerometerSensitivity << 3; // 0XE7 to no change the l_DataReturn and reset bit 3 and 4
		while (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_DataReturn, 1, 1000) != HAL_OK)
		{
			return MPU6050_RESULT_ERROR;
		}

		switch (p_AccelerometerSensitivity)
		{
			case (MPU6050_ACCELEROMETER_2G):
				p_MPU6050->AccelerometerMult = (float)1 / MPU6050_ACCE_SENS_2;
				break;
			case (MPU6050_ACCELEROMETER_4G):
				p_MPU6050->AccelerometerMult = (float)1 / MPU6050_ACCE_SENS_4;
				break;
			case (MPU6050_ACCELEROMETER_8G):
				p_MPU6050->AccelerometerMult = (float)1 / MPU6050_ACCE_SENS_8;
				break;
			case (MPU6050_ACCELEROMETER_16G):
				p_MPU6050->AccelerometerMult = (float)1 / MPU6050_ACCE_SENS_16;
				break;
			default:
				break;
		}
		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}

/**
 * @brief  Sets output data rate
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure indicating MPU6050 device
 * @param  p_DataRate: Data rate value. An 8-bit value for prescaler value
 * @retval Member of @ref MPU6050_result_t enumeration
 */
MPU6050_result_t MPU6050_set_data_rate(MPU6050_t *p_MPU6050, uint8_t p_DataRate)
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	uint8_t l_Data[2] = {MPU6050_SMPLRT_DIV, p_DataRate};
	if (NULL == p_MPU6050)
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		/* Set data sample rate */
		while (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), (uint8_t *)l_Data, 2, 1000) != HAL_OK)
		{
			return MPU6050_RESULT_ERROR;
		}

		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}

/**
 * @brief  Enables interrupts
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure indicating MPU6050 device
 * @retval Member of @ref MPU6050_result_t enumeration
 */
MPU6050_result_t MPU6050_enable_interrupts(MPU6050_t *p_MPU6050)
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	uint8_t l_DataReturn = 0;
	uint8_t l_Register[2] = {MPU6050_INT_ENABLE,0x21}; /* set the interrupts bits */
	uint8_t l_MPU_Register_Interrupt = MPU6050_INT_PIN_CFG;
	if (NULL == p_MPU6050)
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		/* Enable interrupts for data ready and motion detect */
		while(HAL_I2C_Master_Transmit(p_MPU6050->I2Cx , (uint16_t)(p_MPU6050->Address), l_Register, 2, 1000) != HAL_OK);
		
		/* Clear IRQ flag on any read operation */
		while(HAL_I2C_Master_Transmit(p_MPU6050->I2Cx , (uint16_t)(p_MPU6050->Address), &l_MPU_Register_Interrupt, 1, 1000) != HAL_OK);

		while(HAL_I2C_Master_Receive(p_MPU6050->I2Cx , (uint16_t)(p_MPU6050->Address), &l_DataReturn, 14, 1000) != HAL_OK);
		l_DataReturn |= 0x10;
		l_Register[0] = MPU6050_INT_PIN_CFG;
		l_Register[1] = l_DataReturn;
		while(HAL_I2C_Master_Transmit(p_MPU6050->I2Cx , (uint16_t)(p_MPU6050->Address), l_Register, 2, 1000) != HAL_OK);
		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}

/**
 * @brief  Disables interrupts
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure indicating MPU6050 device
 * @retval Member of @ref MPU6050_result_t enumeration
 */
MPU6050_result_t MPU6050_disable_interrupts(MPU6050_t *p_MPU6050)
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	uint8_t l_Register[2] = {MPU6050_INT_ENABLE, 0x00};
	if (NULL == p_MPU6050)
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		/* Disable interrupts */
		while (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), l_Register, 2, 1000) != HAL_OK);

		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}

/**
 * @brief  Reads and clears interrupts
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure indicating MPU6050 device
 * @param  *p_InterruptsStruct: Pointer to @ref MPU6050_interrupt_t structure to store status in
 * @retval Member of @ref MPU6050_result_t enumeration
 */
MPU6050_result_t MPU6050_read_interrupts(MPU6050_t *p_MPU6050, MPU6050_interrupt_status_t *p_InterruptsStruct)
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	uint8_t l_DataReturn = 0;
	uint8_t l_Register = MPU6050_INT_ENABLE;
	p_InterruptsStruct->MPU6050_interrupt_status = 0;
	if ((NULL == p_MPU6050) || (NULL == p_InterruptsStruct))
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		while (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_Register, 1, 1000) != HAL_OK);

		while (HAL_I2C_Master_Receive(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_DataReturn, 14, 1000) != HAL_OK);

		/* Fill value */
		p_InterruptsStruct->MPU6050_interrupt_status = l_DataReturn;

		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}

/**
 * @brief  Reads accelerometer data from sensor
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure to store data to
 * @retval Member of @ref MPU6050_result_t:
 */
MPU6050_result_t MPU6050_read_accelerometer(MPU6050_t *p_MPU6050)
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	uint8_t l_Data[6];
	uint8_t l_Register = MPU6050_ACCEL_XOUT_H;
	if (NULL == p_MPU6050)
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		/* Read temperature */
		while (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_Register, 1, 1000) != HAL_OK);

		while (HAL_I2C_Master_Receive(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), l_Data, 6, 1000) != HAL_OK);

		/* Format */
		p_MPU6050->AccelerometerX = (int16_t)(l_Data[0] << 8 | l_Data[1]);
		p_MPU6050->AccelerometerY = (int16_t)(l_Data[2] << 8 | l_Data[3]);
		p_MPU6050->AccelerometerZ = (int16_t)(l_Data[4] << 8 | l_Data[5]);

		// Convert raw values to acceleration in 'g'
		p_MPU6050->AccelerometerX *= p_MPU6050->AccelerometerMult;
		p_MPU6050->AccelerometerY *= p_MPU6050->AccelerometerMult;
		p_MPU6050->AccelerometerZ *= p_MPU6050->AccelerometerMult;

		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}

/**
 * @brief  Reads gyroscope data from sensor
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure to store data to
 * @retval Member of @ref MPU6050_result_t:
 */
MPU6050_result_t MPU6050_read_gyroscope(MPU6050_t *p_MPU6050)
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	uint8_t l_Data[6];
	uint8_t l_Register = MPU6050_GYRO_XOUT_H;
	if (NULL == p_MPU6050)
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		/* Read temperature */
		while (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_Register, 1, 1000) != HAL_OK);

		while (HAL_I2C_Master_Receive(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), l_Data, 6, 1000) != HAL_OK);

		/* Format */
		p_MPU6050->GyroscopeX = (int16_t)(l_Data[0] << 8 | l_Data[1]);
		p_MPU6050->GyroscopeY = (int16_t)(l_Data[2] << 8 | l_Data[3]);
		p_MPU6050->GyroscopeZ = (int16_t)(l_Data[4] << 8 | l_Data[5]);

		// Convert raw values to angular velocity in °/s
		p_MPU6050->GyroscopeX *= p_MPU6050->GyroscopeMult;
		p_MPU6050->GyroscopeY *= p_MPU6050->GyroscopeMult;
		p_MPU6050->GyroscopeZ *= p_MPU6050->GyroscopeMult;

		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}

/**
 * @brief  Reads temperature data from sensor
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure to store data to
 * @retval Member of @ref MPU6050_result_t:
 */
MPU6050_result_t MPU6050_read_temperature(MPU6050_t *p_MPU6050)
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	uint8_t l_Data[2];
	uint8_t l_Register = MPU6050_TEMP_OUT_H;
	uint8_t l_DataReturn = 0;
	if (NULL == p_MPU6050)
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		/* Read temperature */
		while (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_Register, 1, 1000) != HAL_OK);

		while (HAL_I2C_Master_Receive(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), l_Data, 2, 1000) != HAL_OK);

		/* Format temperature */
		l_DataReturn = (l_Data[0] << 8 | l_Data[1]);
		p_MPU6050->Temperature = (float)((int16_t)l_DataReturn / (float)340.0 + (float)36.53);

		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}

/**
 * @brief  Reads accelerometer, gyroscope and temperature data from sensor
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure to store data to
 * @retval Member of @ref MPU6050_result_t:
 */
MPU6050_result_t MPU6050_read_all(MPU6050_t *p_MPU6050)
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	uint8_t l_Data[14];
	int16_t l_DataReturn;
	uint8_t l_Register = MPU6050_ACCEL_XOUT_H;
	if (NULL == p_MPU6050)
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		/* Read full raw data, 14bytes */
		while (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_Register, 1, 1000) != HAL_OK);

		while (HAL_I2C_Master_Receive(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), l_Data, 14, 1000) != HAL_OK);

		/* Format accelerometer data */
		p_MPU6050->AccelerometerX = (int16_t)(l_Data[0] << 8 | l_Data[1]);
		p_MPU6050->AccelerometerY = (int16_t)(l_Data[2] << 8 | l_Data[3]);
		p_MPU6050->AccelerometerZ = (int16_t)(l_Data[4] << 8 | l_Data[5]);

		// Convert raw values to acceleration in 'g'
		p_MPU6050->AccelerometerX *= p_MPU6050->AccelerometerMult;
		p_MPU6050->AccelerometerY *= p_MPU6050->AccelerometerMult;
		p_MPU6050->AccelerometerZ *= p_MPU6050->AccelerometerMult;

		/* Format temperature */
		l_DataReturn = (l_Data[6] << 8 | l_Data[7]);
		p_MPU6050->Temperature = (float)((float)((int16_t)l_DataReturn) / (float)340.0 + (float)36.53);

		/* Format gyroscope data */
		p_MPU6050->GyroscopeX = (int16_t)(l_Data[8]  << 8 | l_Data[9] );
		p_MPU6050->GyroscopeY = (int16_t)(l_Data[10] << 8 | l_Data[11]);
		p_MPU6050->GyroscopeZ = (int16_t)(l_Data[12] << 8 | l_Data[13]);

		// Convert raw values to angular velocity in °/s
		p_MPU6050->GyroscopeX *= p_MPU6050->GyroscopeMult;
		p_MPU6050->GyroscopeY *= p_MPU6050->GyroscopeMult;
		p_MPU6050->GyroscopeZ *= p_MPU6050->GyroscopeMult;

		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}

/**
 * @brief  Reads gyroscope of Z axis only data from sensor
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure to store data to
 * @retval Member of @ref MPU6050_result_t:
 */
MPU6050_result_t MPU6050_read_gyroscope_z(MPU6050_t *p_MPU6050)
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	uint8_t l_Data[2] = {0,0};
	uint8_t l_Register = MPU6050_GYRO_ZOUT_H;
	if (NULL == p_MPU6050)
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		/* Read temperature */
		while (HAL_I2C_Master_Transmit(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), &l_Register, 1, 1000) != HAL_OK);

		while (HAL_I2C_Master_Receive(p_MPU6050->I2Cx, (uint16_t)(p_MPU6050->Address), l_Data, 2 , 1000) != HAL_OK);

		/* Format */
		p_MPU6050->GyroscopeZ = (int16_t)(l_Data[0] << 8 | l_Data[1]);

		// Convert raw values to angular velocity in °/s
		p_MPU6050->GyroscopeZ *= p_MPU6050->GyroscopeMult ;

		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}


/**
 * @brief  Function to measure yaw (Z-axis angle)
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure to store data to
 * @retval Member of @ref MPU6050_result_t:
 */
MPU6050_result_t MPU6050_calculate_yaw(MPU6050_t *p_MPU6050) 
{
	MPU6050_result_t l_MPU6050Result = MPU6050_RESULT_OK;
	float_t l_GyroRateZ = 0;
	static uint32_t l_PreviousTime = 0;
	if (NULL == p_MPU6050)
	{
		l_MPU6050Result = MPU6050_RESULT_ERROR;
	}
	else
	{
		// Gyroscope rate of change in degrees/second
		l_GyroRateZ = p_MPU6050->GyroscopeZ * p_MPU6050->GyroscopeMult;

		uint32_t l_CurrentTime = HAL_GetTick(); // Get current time in ms
		float_t dt = (float_t)((l_CurrentTime - l_PreviousTime) / 1000.0); // Convert to seconds
		l_PreviousTime = l_CurrentTime;

		// Integrate gyroscope data to calculate yaw
    	(p_MPU6050->AngleYaw) += (l_GyroRateZ * dt);
		
		l_MPU6050Result = MPU6050_RESULT_OK;
	}
	return l_MPU6050Result;
}


