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
float_t old_roll;
float_t old_pitch;
float_t old_yaw;
/* Raw Accelerometer Data */
static int16_t Accel_X_RAW;  /**< Raw X-axis accelerometer data. */
static int16_t Accel_Y_RAW;  /**< Raw Y-axis accelerometer data. */
static int16_t Accel_Z_RAW;  /**< Raw Z-axis accelerometer data. */
/* Raw Gyroscope Data */
static int16_t Gyro_X_RAW;  /**< Raw X-axis gyroscope data. */
static int16_t Gyro_Y_RAW;  /**< Raw Y-axis gyroscope data. */
static int16_t Gyro_Z_RAW;  /**< Raw Z-axis gyroscope data. */
float_t KalmanAngleX;  /**< Roll angle calculated using Kalman Filter. */
float_t KalmanAngleY;  /**< Pitch angle calculated using Kalman Filter. */
// Timer for Kalman filter
uint32_t timer;

// Kalman filter structures for X and Y angles
Kalman_t KalmanX = {.Q_angle = 0.001f, .Q_bias = 0.003f, .R_measure = 0.03f};
Kalman_t KalmanY = {.Q_angle = 0.001f, .Q_bias = 0.003f, .R_measure = 0.03f};

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
    HAL_GPIO_WritePin(DataStruct->GPIOx , DataStruct->GPIO_Pin , GPIO_PIN_RESET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(DataStruct->GPIOx , DataStruct->GPIO_Pin , GPIO_PIN_SET);
    HAL_Delay(500);
    // Check device ID WHO_AM_I
    HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR_REG, WHO_AM_I_REG, 1, &check, 1, 100);
    if ((check == 0x68)&&(NULL!=I2Cx)&&(NULL!=DataStruct)) // 0x68 will be returned by the sensor if everything goes well
    {
        // Power management register 0x6B: Wake the sensor up
        Data = 0;
        HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR_REG, PWR_MGMT_1_REG, 1, &Data, 1, 100);

        // Set DATA RATE to 31.25Hz(8000/(1+255)) by writing to SMPLRT_DIV register
        Data = 0x07;
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
	ecu_status_t l_EcuStatus=ECU_OK;
	if ((NULL!=I2Cx)&&(NULL!=DataStruct)){
		uint8_t Rec_Data[6];
		// Read 6 BYTES of data starting from ACCEL_XOUT_H register
		HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR_REG, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, 100);
		Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
		Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
		Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
		/*** convert the RAW values into acceleration in 'g'
			 we have to divide according to the Full scale value set in FS_SEL
			 I have configured FS_SEL = 0. So I am dividing by 16384.0
			 for more details check ACCEL_CONFIG Register           ***/
		DataStruct->Ax = Accel_X_RAW / 16384.0;
		DataStruct->Ay = Accel_Y_RAW / 16384.0;
		DataStruct->Az = Accel_Z_RAW / 16384.0;
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
	if ((NULL!=I2Cx)&&(NULL!=DataStruct)){

		uint8_t Rec_Data[6];
		// Read 6 BYTES of data starting from GYRO_XOUT_H register
		HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR_REG, GYRO_XOUT_H_REG, 1, Rec_Data, 6, 100);
	    Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
	    Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
	    Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
		    /*** convert the RAW values into dps (�/s)
	         we have to divide according to the Full scale value set in FS_SEL
	         I have configured FS_SEL = 0. So I am dividing by 131.0
		         for more details check GYRO_CONFIG Register              ****/
	    DataStruct->Gx = Gyro_X_RAW / 131.0;
	    DataStruct->Gy = Gyro_Y_RAW / 131.0;
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
	if ((NULL!=I2Cx)&&(NULL!=DataStruct)){
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
	if ((NULL!=I2Cx)&&(NULL!=DataStruct)){
	float new_temp;
    uint8_t Rec_Data[14];
    int16_t temp;
    // Read 14 BYTES of data starting from ACCEL_XOUT_H register
    HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR_REG, ACCEL_XOUT_H_REG, 1, Rec_Data, 14, 100);
    Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
    Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
    temp = (int16_t)(Rec_Data[6] << 8 | Rec_Data[7]);
    Gyro_X_RAW = (int16_t)(Rec_Data[8] << 8 | Rec_Data[9]);
    Gyro_Y_RAW = (int16_t)(Rec_Data[10] << 8 | Rec_Data[11]);
    Gyro_Z_RAW = (int16_t)(Rec_Data[12] << 8 | Rec_Data[13]);
    DataStruct->Ax = Accel_X_RAW / 16384.0 + 0.06;
    DataStruct->Ay = Accel_Y_RAW / 16384.0 + 0.02;
    DataStruct->Az = Accel_Z_RAW / 16384.0 + 0.014;
    new_temp= (float)((int16_t)temp / (float)340.0 + (float)36.53);
    old_temp=(fabs(new_temp-old_temp)>TEMPERATURE_THRESHOLD)?new_temp:old_temp;
    DataStruct->Temperature=((uint16_t)(old_temp*100))/100.0;
    DataStruct->Gx = Gyro_X_RAW / 131.0;
    DataStruct->Gy = Gyro_Y_RAW / 131.0;
    DataStruct->Gz = Gyro_Z_RAW / 131.0;
    // Kalman angle solve
    float_t dt = (float_t)(HAL_GetTick() - timer) / 1000.0;
    timer = HAL_GetTick();
    // Roll Calculation
    float_t roll_sqrt = sqrt(
    Accel_X_RAW * Accel_X_RAW + Accel_Z_RAW * Accel_Z_RAW);
    float_t roll;
    if (roll_sqrt != 0.0)
    {
        roll = atan(Accel_Y_RAW / roll_sqrt) * RAD_TO_DEG;
    }
    else
    {
        roll = 0.0;
    }
    // Pitch Calculation
    float_t pitch = atan2(-Accel_X_RAW, Accel_Z_RAW) * RAD_TO_DEG;
    if ((pitch < -90 && KalmanAngleY > 90) || (pitch > 90 && KalmanAngleY < -90))
    {
        KalmanY.angle = pitch;
        KalmanAngleY =pitch-OFFSET_IN_PITCH;
        old_pitch = (fabs(KalmanAngleY-old_pitch) > ROLL_PITCH_THRESHOLD) ? KalmanAngleY:old_pitch;
        DataStruct->Pitch=(__int16_t)(old_pitch);
    }
    else
    {
        KalmanAngleY =Kalman_getAngle(&KalmanY, pitch, DataStruct->Gy, dt)-OFFSET_IN_PITCH;
        old_pitch = (fabs(KalmanAngleY-old_pitch) > ROLL_PITCH_THRESHOLD) ? KalmanAngleY:old_pitch;
        DataStruct->Pitch=(__int16_t)(old_pitch);
    }
    if (fabs(KalmanAngleY) > 90){
        DataStruct->Gx = -DataStruct->Gx;
    }
    	KalmanAngleX = Kalman_getAngle(&KalmanX, roll, DataStruct->Gx, dt);
    	old_roll = (fabs(KalmanAngleX-old_roll) > ROLL_PITCH_THRESHOLD) ? KalmanAngleX:old_roll;
    	DataStruct->Roll=(__int16_t)(old_roll);


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



float_t Kalman_getAngle(Kalman_t *Kalman, float_t newAngle, float_t newRate, float_t dt)
{
    float_t rate = newRate - Kalman->bias;
    Kalman->angle += dt * rate;

    Kalman->P[0][0] += dt * (dt * Kalman->P[1][1] - Kalman->P[0][1] - Kalman->P[1][0] + Kalman->Q_angle);
    Kalman->P[0][1] -= dt * Kalman->P[1][1];
    Kalman->P[1][0] -= dt * Kalman->P[1][1];
    Kalman->P[1][1] += Kalman->Q_bias * dt;

    float_t S = Kalman->P[0][0] + Kalman->R_measure;
    float_t K[2];
    K[0] = Kalman->P[0][0] / S;
    K[1] = Kalman->P[1][0] / S;

    float_t y = newAngle - Kalman->angle;
    Kalman->angle += K[0] * y;
    Kalman->bias += K[1] * y;

    float_t P00_temp = Kalman->P[0][0];
    float_t P01_temp = Kalman->P[0][1];

    Kalman->P[0][0] -= K[0] * P00_temp;
    Kalman->P[0][1] -= K[0] * P01_temp;
    Kalman->P[1][0] -= K[1] * P00_temp;
    Kalman->P[1][1] -= K[1] * P01_temp;

    return Kalman->angle;
};

/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              *
*                       |                                                                                              *
***********************************************************************************************************************/
