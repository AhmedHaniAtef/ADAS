/**
 * @file    mpu.h
 * @brief   Header file for MPU6050
 * @authors Eslam Sameh & Youssef Benyamine
 * @date    2024-11-25
 */

#ifndef SRC_MPU_H_
#define SRC_MPU_H_


/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "stm32f4xx_hal.h"
#include <stm32f401xc.h>      // MCU-specific header for STM32F401
#include "ecu_std.h"
#include "stm32f4xx_hal_i2c.h"

/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/
/* Default I2C clock */
#define MPU6050_I2C_CLOCK              400000            /*!< Default I2C clock speed */

/* Data rates */
#define MPU6050_DATARATE_8KHZ       0   /*!< Sample rate set to 8 kHz */
#define MPU6050_DATARATE_4KHZ       1   /*!< Sample rate set to 4 kHz */
#define MPU6050_DATARATE_2KHZ       3   /*!< Sample rate set to 2 kHz */
#define MPU6050_DATARATE_1KHZ       7   /*!< Sample rate set to 1 kHz */
#define MPU6050_DATARATE_500HZ      15  /*!< Sample rate set to 500 Hz */
#define MPU6050_DATARATE_250HZ      31  /*!< Sample rate set to 250 Hz */
#define MPU6050_DATARATE_125HZ      63  /*!< Sample rate set to 125 Hz */
#define MPU6050_DATARATE_100HZ      79  /*!< Sample rate set to 100 Hz */

/* Default I2C address */
#define MPU6050_I2C_ADDR			0xD0

/* Who I am register value */
#define MPU6050_I_AM				0x68

/* MPU6050 registers */
#define MPU6050_AUX_VDDIO			0x01
#define MPU6050_SMPLRT_DIV			0x19
#define MPU6050_CONFIG				0x1A
#define MPU6050_GYRO_CONFIG			0x1B
#define MPU6050_ACCEL_CONFIG		0x1C
#define MPU6050_MOTION_THRESH		0x1F
#define MPU6050_INT_PIN_CFG			0x37
#define MPU6050_INT_ENABLE			0x38
#define MPU6050_INT_STATUS			0x3A
#define MPU6050_ACCEL_XOUT_H		0x3B
#define MPU6050_ACCEL_XOUT_L		0x3C
#define MPU6050_ACCEL_YOUT_H		0x3D
#define MPU6050_ACCEL_YOUT_L		0x3E
#define MPU6050_ACCEL_ZOUT_H		0x3F
#define MPU6050_ACCEL_ZOUT_L		0x40
#define MPU6050_TEMP_OUT_H			0x41
#define MPU6050_TEMP_OUT_L			0x42
#define MPU6050_GYRO_XOUT_H			0x43
#define MPU6050_GYRO_XOUT_L			0x44
#define MPU6050_GYRO_YOUT_H			0x45
#define MPU6050_GYRO_YOUT_L			0x46
#define MPU6050_GYRO_ZOUT_H			0x47
#define MPU6050_GYRO_ZOUT_L			0x48
#define MPU6050_MOT_DETECT_STATUS	0x61
#define MPU6050_SIGNAL_PATH_RESET	0x68
#define MPU6050_MOT_DETECT_CTRL		0x69
#define MPU6050_USER_CTRL			0x6A
#define MPU6050_PWR_MGMT_1			0x6B
#define MPU6050_PWR_MGMT_2			0x6C
#define MPU6050_FIFO_COUNTH			0x72
#define MPU6050_FIFO_COUNTL			0x73
#define MPU6050_FIFO_R_W			0x74
#define MPU6050_WHO_AM_I			0x75

/* Gyroscope sensitivities in degrees/s */
#define MPU6050_GYRO_SENS_250		((float) 131)
#define MPU6050_GYRO_SENS_500		((float) 65.5)
#define MPU6050_GYRO_SENS_1000		((float) 32.8)
#define MPU6050_GYRO_SENS_2000		((float) 16.4)

/* Accelerometer sensitivities in g/s */
#define MPU6050_ACCE_SENS_2			((float) 16384)
#define MPU6050_ACCE_SENS_4			((float) 8192)
#define MPU6050_ACCE_SENS_8			((float) 4096)
#define MPU6050_ACCE_SENS_16		((float) 2048)


/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/

/* Add any relevant macro functions here if required */

/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/

/**
 * @brief  MPU6050 result enumeration
 */
typedef enum
{
	MPU6050_RESULT_OK = 0x00,
	MPU6050_RESULT_ERROR,
	MPU6050_RESULT_DEVICE_NOT_CONNECTED,
	MPU6050_RESULT_DEVICE_INVALID
}MPU6050_result_t;

/**
 * @brief  Parameters for accelerometer range
 */
typedef enum
{
	MPU6050_ACCELEROMETER_2G = 0x00, /*!< Range is +- 2G */
	MPU6050_ACCELEROMETER_4G, /*!< Range is +- 4G */
	MPU6050_ACCELEROMETER_8G, /*!< Range is +- 8G */
	MPU6050_ACCELEROMETER_16G  /*!< Range is +- 16G */
}MPU6050_accelerometer_t;

/**
 * @brief  Parameters for gyroscope range
 */
typedef enum
{
	MPU6050_GYROSCOPE_250S = 0x00,  /*!< Range is +- 250 degrees/s */
	MPU6050_GYROSCOPE_500S,         /*!< Range is +- 500 degrees/s */
	MPU6050_GYROSCOPE_1000S,        /*!< Range is +- 1000 degrees/s */
	MPU6050_GYROSCOPE_2000S         /*!< Range is +- 2000 degrees/s */
}MPU6050_gyroscope_t;

/**
 * @brief  Main MPU6050 structure
 */
typedef struct
{
	/* Private */
	uint8_t Address;               /*!< I2C address of device. */
	I2C_HandleTypeDef *I2Cx;
	float   GyroscopeMult;         /*!< Gyroscope corrector from raw data to "degrees/s". Only for private use */
	float   AccelerometerMult;     /*!< Accelerometer corrector from raw data to "g". Only for private use */

	/* Public */
	int16_t AccelerometerX;        /*!< Accelerometer value X axis */
	int16_t AccelerometerY;        /*!< Accelerometer value Y axis */
	int16_t AccelerometerZ;        /*!< Accelerometer value Z axis */
	int16_t GyroscopeX;            /*!< Gyroscope value X axis */
	int16_t GyroscopeY;            /*!< Gyroscope value Y axis */
	int16_t GyroscopeZ;            /*!< Gyroscope value Z axis */
	float   Temperature;           /*!< Temperature in degrees */
	float_t AngleYaw; 			   /*!< Angle of z axis */

}MPU6050_t;

/**
 * @brief  Interrupts union and structure
 */
typedef union
{
	struct
	{
		uint8_t DataReady:1;       /*!< Data ready interrupt */
		uint8_t Reserved2:2;       /*!< Reserved bits */
		uint8_t Master:1;          /*!< Master interrupt. Not enabled with library */
		uint8_t FifoOverflow:1;    /*!< FIFO overflow interrupt. Not enabled with library */
		uint8_t reserved1:1;       /*!< Reserved bit */
		uint8_t MotionDetection:1; /*!< Motion detected interrupt */
		uint8_t reserved0:1;       /*!< Reserved bit */
	} F;
	uint8_t MPU6050_interrupt_status;
}MPU6050_interrupt_status_t;


/***********************************************************************************************************************
*                                                  FUNCTION PROTOTYPES                                                 *
***********************************************************************************************************************/

/**
 * @brief  Initializes MPU6050 and I2C peripheral
 * @param  *p_MPU6050: Pointer to empty @ref MPU6050_t structure
 * @param  p_AccelerometerSensitivity: Set accelerometer sensitivity. This parameter can be a value of @ref MPU6050_accelerometer_t enumeration
 * @param  p_GyroscopeSensitivity: Set gyroscope sensitivity. This parameter can be a value of @ref MPU6050_gyroscope_t enumeration
 * @retval Initialization status:
 *            - MPU6050_result_t: Everything OK
 *            - Other member: in other cases
 */
MPU6050_result_t MPU6050_init(MPU6050_t *p_MPU6050 , MPU6050_accelerometer_t p_AccelerometerSensitivity , MPU6050_gyroscope_t p_GyroscopeSensitivity);

/**
 * @brief  Sets gyroscope sensitivity
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure indicating MPU6050 device
 * @param  p_GyroscopeSensitivity: gyroscope sensitivity value. This parameter can be a value of @ref MPU6050_gyroscope_t enumeration
 * @retval Member of @ref MPU6050_result_t enumeration
 */
MPU6050_result_t MPU6050_set_gyroscope(MPU6050_t *p_MPU6050, MPU6050_gyroscope_t p_GyroscopeSensitivity);

/**
 * @brief  Sets accelerometer sensitivity
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure indicating MPU6050 device
 * @param  p_AccelerometerSensitivity: Gyro sensitivity value. This parameter can be a value of @ref MPU6050_accelerometer_t enumeration
 * @retval Member of @ref MPU6050_result_t enumeration
 */
MPU6050_result_t MPU6050_set_accelerometer(MPU6050_t *p_MPU6050, MPU6050_accelerometer_t p_AccelerometerSensitivity);

/**
 * @brief  Sets output data rate
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure indicating MPU6050 device
 * @param  p_DataRate: Data rate value. An 8-bit value for prescaler value
 * @retval Member of @ref MPU6050_result_t enumeration
 */
MPU6050_result_t MPU6050_set_data_rate(MPU6050_t *p_MPU6050 , uint8_t p_DataRate);

/**
 * @brief  Enables interrupts
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure indicating MPU6050 device
 * @retval Member of @ref MPU6050_result_t enumeration
 */
MPU6050_result_t MPU6050_enable_interrupts(MPU6050_t *p_MPU6050);

/**
 * @brief  Disables interrupts
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure indicating MPU6050 device
 * @retval Member of @ref MPU6050_result_t enumeration
 */
MPU6050_result_t MPU6050_disable_interrupts(MPU6050_t *p_MPU6050);

/**
 * @brief  Reads and clears interrupts
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure indicating MPU6050 device
 * @param  *p_InterruptsStruct: Pointer to @ref MPU6050_interrupt_t structure to store status in
 * @retval Member of @ref MPU6050_result_t enumeration
 */
MPU6050_result_t MPU6050_read_interrupts(MPU6050_t *p_MPU6050, MPU6050_interrupt_status_t *p_InterruptsStruct);

/**
 * @brief  Reads accelerometer data from sensor
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure to store data to
 * @retval Member of @ref MPU6050_result_t:
 */
MPU6050_result_t MPU6050_read_accelerometer(MPU6050_t *p_MPU6050);

/**
 * @brief  Reads gyroscope data from sensor
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure to store data to
 * @retval Member of @ref MPU6050_result_t:
 */
MPU6050_result_t MPU6050_read_gyroscope(MPU6050_t *p_MPU6050);

/**
 * @brief  Reads temperature data from sensor
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure to store data to
 * @retval Member of @ref MPU6050_result_t:
 */
MPU6050_result_t MPU6050_read_temperature(MPU6050_t *p_MPU6050);

/**
 * @brief  Reads accelerometer, gyroscope and temperature data from sensor
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure to store data to
 * @retval Member of @ref MPU6050_result_t:
 */
MPU6050_result_t MPU6050_read_all(MPU6050_t *p_MPU6050);

/**
 * @brief  Reads gyroscope of Z axis only data from sensor
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure to store data to
 * @retval Member of @ref MPU6050_result_t:
 */
MPU6050_result_t MPU6050_read_gyroscope_z(MPU6050_t *p_MPU6050);

/**
 * @brief  Function to measure yaw (Z-axis angle)
 * @param  *p_MPU6050: Pointer to @ref MPU6050_t structure to store data to
 * @retval Member of @ref MPU6050_result_t:
 */
MPU6050_result_t MPU6050_calculate_yaw(MPU6050_t *p_MPU6050);


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              *
*                       |                                                                                              *
***********************************************************************************************************************/


#endif /* SRC_MPU_H_ */


