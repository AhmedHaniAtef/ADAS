/**
 * @file    mpu.h
 * @brief   Enhanced MPU6050 sensor handling on STM32F4 microcontrollers.
 *          Provides initialization, reading acceleration, angular velocity, temperature, and calculating angles using Kalman filtering.
 * @authors Eslam Sameh & Youssef Benyamain
 * @date    2024-12-10
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "mpu.h"

/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/
// I2C and MPU6050 instance pointers
static I2C_HandleTypeDef *I2C_INT;
static MPU6050_t *MPU_INT;
float  old_temp;
double old_roll;
double old_pitch;
double old_yaw;
/* Raw Accelerometer Data */
static int16_t Accel_X_RAW;  /**< Raw X-axis accelerometer data. */
static int16_t Accel_Y_RAW;  /**< Raw Y-axis accelerometer data. */
/* Raw Gyroscope Data */
static int16_t Gyro_Z_RAW;  /**< Raw Z-axis gyroscope data. */
// Timer for Kalman filter
uint32_t timer;

/***********************************************************************************************************************
*                                                  FUNCTION Implementation                                                *
***********************************************************************************************************************/
ecu_status_t MPU6050_Init(I2C_HandleTypeDef *I2Cx,MPU6050_t *DataStruct)
{
	I2C_INT=I2Cx;
	MPU_INT=DataStruct;
    uint8_t check;
    uint8_t Data;
    ecu_status_t l_EcuStatus=ECU_OK;

    // Check device ID WHO_AM_I
    HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR_REG, WHO_AM_I_REG, 1, &check, 1, 1000);
    if ((check == 0x68)&&(NULL!=I2Cx)&&(NULL!=DataStruct)) // 0x68 will be returned by the sensor if everything goes well
    {
        // Power management register 0x6B: Wake the sensor up
        Data = 0;
        HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR_REG, PWR_MGMT_1_REG, 1, &Data, 1, 100);

        // Set DATA RATE to 31.25Hz(8000/(1+255)) by writing to SMPLRT_DIV register
        Data = 0xFF;
        HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR_REG, SMPLRT_DIV_REG, 1, &Data, 1, 100);
        // Set accelerometer configuration in ACCEL_CONFIG Register (+-2g)
        Data = 0x00;
        HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR_REG, ACCEL_CONFIG_REG, 1, &Data, 1, 100);
        // Set gyroscope configuration in GYRO_CONFIG Register (-+250 degree per s)
        Data = 0x00;
        HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR_REG, GYRO_CONFIG_REG, 1, &Data, 1, 100);
        Data = 0x90; // Clear interrupt for any register read (active high interrupt)
        HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR_REG,INT_CFG_REG, 1, &Data, 1, 100);
        Data = 0x01; // Enable Data Ready Interrupt (INT_ENABLE register: 0x38)
        HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR_REG,INT_ENABLE_REG,1, &Data, 1, 100);

    }
    else
    {
    	l_EcuStatus=ECU_ERROR;
    }

    return l_EcuStatus;
}


ecu_status_t MPU6050_Read_Accel(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct)
{
	ecu_status_t l_EcuStatus = ECU_OK;
	if ((NULL != I2Cx) && (NULL != DataStruct))
	{
		uint8_t Rec_Data[4];
		// Read 6 BYTES of data starting from ACCEL_XOUT_H register
		HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR_REG, ACCEL_XOUT_H_REG, 1, Rec_Data, 4, 100);
		Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
		Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
		/*** convert the RAW values into acceleration in 'g'
			 we have to divide according to the Full scale value set in FS_SEL
			 I have configured FS_SEL = 0. So I am dividing by 16384.0
			 for more details check ACCEL_CONFIG Register           ***/
		DataStruct->Ax = Accel_X_RAW / 16384.0;
		DataStruct->Ay = Accel_Y_RAW / 16384.0;
	}
	else
	{
    	l_EcuStatus=ECU_ERROR;
	}
    return l_EcuStatus;

}

ecu_status_t MPU6050_Read_Gyro(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct)
{
	ecu_status_t l_EcuStatus=ECU_OK;
	if ((NULL!=I2Cx)&&(NULL!=DataStruct))
	{

		uint8_t Rec_Data[2];
		// Read 6 BYTES of data starting from GYRO_XOUT_H register
		HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR_REG, GYRO_ZOUT_H_REG, 1, Rec_Data, 2, 100);
	    Gyro_Z_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);

		    /*** convert the RAW values into dps (�/s)
	         we have to divide according to the Full scale value set in FS_SEL
	         I have configured FS_SEL = 0. So I am dividing by 131.0
		         for more details check GYRO_CONFIG Register              ****/
	    DataStruct->Gz = Gyro_Z_RAW / 131.0;
	}
	else
	{
		l_EcuStatus=ECU_ERROR;
	}
	return l_EcuStatus;
}
ecu_status_t MPU6050_Read_Temp(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct)
{
	ecu_status_t l_EcuStatus=ECU_OK;
	if ((NULL!=I2Cx)&&(NULL!=DataStruct))
	{
    uint8_t Rec_Data[2];
    int16_t temp;
    float new_temp;
    // Read 2 BYTES of data starting from TEMP_OUT_H_REG register
    HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR_REG, TEMP_OUT_H_REG, 1, Rec_Data, 2, 100);
    temp = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    new_temp= (float)((int16_t)temp / (float)340.0 + (float)36.53);
    old_temp=(fabs(new_temp-old_temp)>TEMPERATURE_THRESHOLD)?new_temp:old_temp;
    DataStruct->Temperature=((uint16_t)(old_temp*100))/100.0;
	}
	else
	{
		l_EcuStatus=ECU_ERROR;
	}
	return l_EcuStatus;
}

ecu_status_t MPU6050_Read_All(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct)
{
	ecu_status_t l_EcuStatus=ECU_OK;
	if ((NULL!=I2Cx)&&(NULL!=DataStruct))
	{
	float new_temp;
    uint8_t Rec_Data_Acc[4];
    uint8_t Rec_Data_Temp[2];
    uint8_t Rec_Data_Gyro[2];
    int16_t temp;
    // Read 14 BYTES of data starting from ACCEL_XOUT_H register
    HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR_REG, ACCEL_XOUT_H_REG, 1, Rec_Data_Acc, 4, 100);
    Accel_X_RAW = (int16_t)(Rec_Data_Acc[0] << 8 | Rec_Data_Acc[1]);
    Accel_Y_RAW = (int16_t)(Rec_Data_Acc[2] << 8 | Rec_Data_Acc[3]);
    HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR_REG, TEMP_OUT_H_REG, 1, Rec_Data_Temp, 2, 100);
    temp = (int16_t)(Rec_Data_Temp[0] << 8 | Rec_Data_Temp[1]);
    HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR_REG, GYRO_ZOUT_H_REG, 1, Rec_Data_Gyro, 2, 100);
    Gyro_Z_RAW = (int16_t)(Rec_Data_Gyro[0] << 8 | Rec_Data_Gyro[1]);

    DataStruct->Ax = Accel_X_RAW / 16384.0 + 0.06;
    DataStruct->Ay = Accel_Y_RAW / 16384.0 + 0.02;

    new_temp= (float)((int16_t)temp / (float)340.0 + (float)36.53);
    old_temp=(fabs(new_temp-old_temp)>TEMPERATURE_THRESHOLD)?new_temp:old_temp;
    DataStruct->Temperature=((uint16_t)(old_temp*100))/100.0;

    DataStruct->Gz = Gyro_Z_RAW / 131.0;
    // Kalman angle solve
    double dt = (double)(HAL_GetTick() - timer) / 1000.0;
    timer = HAL_GetTick();
    // Yaw Calculation with Threshold (0.8 degrees per trial)
    if (fabs(DataStruct->Gz * dt) > YAW_THRESHOLD) // Only integrate if increase in yaw (DataStruct->Gz * dt) is larger than yaw_threshold
    	old_yaw += DataStruct->Gz * dt;
    DataStruct->Yaw=(__int16_t)(old_yaw);
    }
	else
	{
		l_EcuStatus=ECU_ERROR;
	}
	return l_EcuStatus;
}



double Kalman_getAngle(Kalman_t *Kalman, double newAngle, double newRate, double dt)
{
    double rate = newRate - Kalman->bias;
    Kalman->angle += dt * rate;

    Kalman->P[0][0] += dt * (dt * Kalman->P[1][1] - Kalman->P[0][1] - Kalman->P[1][0] + Kalman->Q_angle);
    Kalman->P[0][1] -= dt * Kalman->P[1][1];
    Kalman->P[1][0] -= dt * Kalman->P[1][1];
    Kalman->P[1][1] += Kalman->Q_bias * dt;

    double S = Kalman->P[0][0] + Kalman->R_measure;
    double K[2];
    K[0] = Kalman->P[0][0] / S;
    K[1] = Kalman->P[1][0] / S;

    double y = newAngle - Kalman->angle;
    Kalman->angle += K[0] * y;
    Kalman->bias += K[1] * y;

    double P00_temp = Kalman->P[0][0];
    double P01_temp = Kalman->P[0][1];

    Kalman->P[0][0] -= K[0] * P00_temp;
    Kalman->P[0][1] -= K[0] * P01_temp;
    Kalman->P[1][0] -= K[1] * P00_temp;
    Kalman->P[1][1] -= K[1] * P01_temp;

    return Kalman->angle;
};

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_4) // Check if the interrupt is from Port B Pin 4
    {
        MPU6050_Read_All(I2C_INT,MPU_INT);
    }
}
/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              *
*                       |                                                                                              *
***********************************************************************************************************************/
