/**
 * @file    qmc5883l.h
 * @author  Ahmed Hani
 * @brief   contains all interfaces of compass module (QMC5883L)
 * @date    2025-02-013
 */

#ifndef QMC5883L_H_
#define QMC5883L_H_


/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "stm32f4xx_hal.h"
#include <stm32f401xc.h>
#include <stm32f4xx_hal_tim.h>
#include "ecu_std.h"




/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/

#define QMC5883L_I2C_ADDR              (0x0D)
#define QMC5883L_I2C_TIMEOUT           (HAL_MAX_DELAY)

/* Register addresses */
#define QMC5883L_REG_X_LSB             (0x00)
#define QMC5883L_REG_STATUS            (0x06)
#define QMC5883L_REG_CTRL1             (0x09)
#define QMC5883L_REG_CTRL2             (0x0A)
#define QMC5883L_REG_RESET             (0x0B)
#define QMC5883L_REG_CHIP_ID           (0x0D)

/* Configuration values */
#define QMC5883L_ID_VAL                (0xFF)



/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/

/**
 * @brief determine the Mode Control of QMC5883L
 * 
 */
typedef enum
{
    STANDBY = 0b00,
    CONTINUOUS = 0b01,
}QMC5883L_Mode_t;

/**
 * @brief determine the Output Data Rate of QMC5883L in Hz
 * 
 */
typedef enum 
{
    ODR_10 = 0b0000,
    ODR_50 = 0b0100,
    ODR_100 = 0b1000, 
    ODR_200 = 0b1100,
}QMC5883L_ODR_t;

/**
 * @brief determine the Full Scale of QMC5883L in G
 * 
 */
typedef enum 
{
    RNG_2G = 0b000000,
    RNG_8G = 0b010000,
}QMC5883L_RNG_t;

/**
 * @brief determine the Over Sample Ratio in QMC5883L
 * 
 */
typedef enum
{
    OSR_512 = 0b00000000,
    OSR_256 = 0b01000000, 
    OSR_128 = 0b10000000, 
    OSR_64  = 0b11000000,
}QMC5883L_OSR_t;

/**
 * @brief determine the state of interrupt on DRDY pin
 * 
 */
typedef enum
{
    DRDY_INT_ENABLE = 0,
    DRDY_INT_DISABLE,
}QMC5883L_DRDY_t;

/**
 * @brief datatype which descripes the compass QMC5883L
 * @param UsedI2C pointer to the used i2c to interface with the compass
 * @param DeclinationAngle declination angle of the palce of the compass in (RADIAN)
 * @param OutputDataRate output data rate @ref QMC5883L_ODR_t
 * @param Mode mode of operation @ref QMC5883L_Mode_t
 * @param FullScale Full Scale of compass @ref QMC5883L_RNG_t
 * @param OverSampleRatio Over Sample Ratio @ref QMC5883L_OSR_t
 * @param DRDY DRDY pin interrupt @ref QMC5883L_DRDY_t
 * @param x value of magnetic field in x direction (will be updated each raw read)    
 * @param y value of magnetic field in y direction (will be updated each raw read)
 * @param z value of magnetic field in z direction (will be updated each raw read)
 */
typedef struct 
{
    I2C_HandleTypeDef *UsedI2C;
    float_t DeclinationAngle;
    QMC5883L_ODR_t OutputDataRate;
    QMC5883L_Mode_t Mode;
    QMC5883L_RNG_t FullScale;
    QMC5883L_OSR_t OverSampleRatio;
    QMC5883L_DRDY_t DRDY;
    float_t x;
    float_t y;
    float_t z;
}QMC5883L_t;



/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief init the compass according to the configurations wanted by user
 * 
 * @param p_compass pointer to the compass and inculding the configurations 
 * @return ecu_status_t status of operation
 */
ecu_status_t QMC5883L_init(QMC5883L_t *p_Compass);

/**
 * @brief update the x, y and z magnetic values in the data type without any processing
 * 
 * @param p_compass pointer to the compass
 * @return ecu_status_t status of operation
 */
ecu_status_t QMC5883L_read_raw(QMC5883L_t *p_Compass);

/**
 * @brief update the angle of the compass
 * 
 * @param p_compass pointer to the compass
 * @return ecu_status_t status of operation
 */
ecu_status_t QMC5883L_read_angle(QMC5883L_t *p_Compass, float_t *p_Angle);

ecu_status_t read_control_register(QMC5883L_t *p_Compass);


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif // QMC5883L_H_
