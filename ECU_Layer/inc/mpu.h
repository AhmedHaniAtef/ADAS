/**
 * @file    mpu.h
 * @brief   Header file for MPU6050 sensor handling on STM32F4 microcontrollers.
 *          Provides initialization, reading acceleration, angular velocity, and calculating angles.
 * @authors Eslam Sameh & Youssef Benyamain
 * @date    2024-12-10
 */

#ifndef MPU_6050_H
#define MPU_6050_H

/***********************************************************************************************************************
 *                                                      INCLUDES                                                       *
 ***********************************************************************************************************************/
#include "stm32f4xx_hal.h"
#include <math.h>
#include "ecu_std.h"

/***********************************************************************************************************************
 *                                                    MACRO DEFINES                                                    *
 ***********************************************************************************************************************/
#define RAD_TO_DEG            57.295779513082320876798154814105  /**< Radian to Degree conversion factor. */
#define WHO_AM_I_REG          0x75 /**< WHO_AM_I register address (Device ID). */
#define PWR_MGMT_1_REG        0x6B /**< Power Management register address. */
#define SMPLRT_DIV_REG        0x19 /**< Sample Rate Divider register address. */
#define ACCEL_CONFIG_REG      0x1C /**< Accelerometer Configuration register address. */
#define ACCEL_XOUT_H_REG      0x3B /**< Accelerometer X-axis high byte output register. */
#define TEMP_OUT_H_REG        0x41 /**< Temperature high byte output register. */
#define GYRO_CONFIG_REG       0x1B /**< Gyroscope Configuration register address. */
#define GYRO_XOUT_H_REG       0x43 /**< Gyroscope X-axis high byte output register. */
#define MPU6050_ADDR_REG      0xD0 /**< Default I2C address for MPU6050 (0x68 shifted left by 1). */
#define INT_CFG_REG           0x37 /**< interrupt Configuration register address. */
#define INT_ENABLE_REG		  0x38 /**< interrupt Enable register address. */
#define ROLL_PITCH_THRESHOLD  0.2  /**< Sensitivity threshold of roll and pitch in degree. */
#define YAW_THRESHOLD         0.025/**< Sensitivity threshold of yaw in degree. */
#define TEMPERATURE_THRESHOLD 0.3  /**< Sensitivity threshold of temp in celsius degree. */
#define OFFSET_IN_PITCH       3.7  /**< OFFSET in pitch angel value (should be substracted */
/***********************************************************************************************************************
 *                                                      DATA TYPES                                                     *
 ***********************************************************************************************************************/
/**
 * @struct MPU6050_t
 * @brief Structure holding MPU6050 raw and processed sensor data.
 */
typedef struct
{


	float Temperature;    /**< Temperature data in degrees Celsius. */
    /* Processed Accelerometer Data */
    double Ax;  /**< Acceleration along X-axis in g (gravity). */
    double Ay;  /**< Acceleration along Y-axis in g (gravity). */
    double Az;  /**< Acceleration along Z-axis in g (gravity). */
    /* Processed Gyroscope Data */
    double Gx;  /**< Angular velocity around X-axis in degrees/second. */
    double Gy;  /**< Angular velocity around Y-axis in degrees/second. */
    double Gz;  /**< Angular velocity around Z-axis in degrees/second. */
    /* Calculated Angles */
    double Roll;	/**< Roll angle calculated from Kalman filter. */
    double Pitch;	/**< Pitch angle calculated from Kalman filter. */
    double Yaw;     /**< Yaw angle calculated from gyroscope data. */
} MPU6050_t;

/**
 * @struct Kalman_t
 * @brief Structure holding Kalman filter parameters.
 */
typedef struct
{
    double Q_angle;   /**< Process noise variance for the accelerometer. */
    double Q_bias;    /**< Process noise variance for the gyro bias. */
    double R_measure; /**< Measurement noise variance. */

    double angle;     /**< Current estimated angle. */
    double bias;      /**< Current gyro bias. */
    double P[2][2];   /**< Error covariance matrix. */
} Kalman_t;

/***********************************************************************************************************************
 *                                                  FUNCTION PROTOTYPES                                                *
 ***********************************************************************************************************************/
/**
 * @brief  Initializes the MPU6050 sensor.
 * @param  I2Cx: Pointer to the I2C handle structure (I2C peripheral).
 * @param  DataStruct: Pointer to MPU6050_t structure to store sensor data.
 * @return ECU_OK or ECU_ERROR.
 */
ecu_status_t MPU6050_Init(I2C_HandleTypeDef *I2Cx, MPU6050_t *mpu);

/**
 * @brief  Reads raw and processed acceleration data from MPU6050.
 * @param  I2Cx: Pointer to the I2C handle structure (I2C peripheral).
 * @param  DataStruct: Pointer to MPU6050_t structure to store acceleration data.
 * @return ECU_OK or ECU_ERROR.
 */
ecu_status_t MPU6050_Read_Accel(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

/**
 * @brief  Reads raw and processed gyroscope data from MPU6050.
 * @param  I2Cx: Pointer to the I2C handle structure (I2C peripheral).
 * @param  DataStruct: Pointer to MPU6050_t structure to store gyroscope data.
 * @return ECU_OK or ECU_ERROR.
 */
ecu_status_t MPU6050_Read_Gyro(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

/**
 * @brief  Reads the temperature data from MPU6050.
 * @param  I2Cx: Pointer to the I2C handle structure (I2C peripheral).
 * @param  DataStruct: Pointer to MPU6050_t structure to store temperature data.
 * @return ECU_OK or ECU_ERROR.
 */
ecu_status_t MPU6050_Read_Temp(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

/**
 * @brief  Reads all sensor data (acceleration, gyroscope, temperature) from MPU6050 and calculates angles.
 * @param  I2Cx: Pointer to the I2C handle structure (I2C peripheral).
 * @param  DataStruct: Pointer to MPU6050_t structure to store all sensor data.
 * @return ECU_OK or ECU_ERROR.
 */
ecu_status_t MPU6050_Read_All(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

/**
 * @brief  Computes the angle using Kalman filter.
 * @param  Kalman: Pointer to Kalman_t structure containing filter parameters.
 * @param  newAngle: The new angle input (e.g., from accelerometer).
 * @param  newRate: The new rate input (e.g., from gyroscope).
 * @param  dt: Time difference between measurements in seconds.
 * @return The estimated angle.
 */
double Kalman_getAngle(Kalman_t *Kalman, double newAngle, double newRate, double dt);

/***********************************************************************************************************************
 * AUTHOR                |   note                                                                                         *
 ***********************************************************************************************************************
 * Eslam Sameh          |                                                                                            *
 * Youssef Benyamain    |                                                                                            *
 ***********************************************************************************************************************/

#endif /* MPU_6050_H */
