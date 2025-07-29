/**
 * @file    Parking.h
 * @brief   Interface for the Auto Parking system on STM32F4 microcontrollers.
 * @details Provides data structures and function declarations for an autonomous parking system using ultrasonic sensors and encoders.
 * 
 * @authors Eslam Sameh  
 *          Mohammed Saad  
 *          Mazen Elbialey
 * @date    2025-04-20
 */

 #ifndef AUTO_PARKING_H
 #define AUTO_PARKING_H
 
 /***********************************************************************************************************************
  *                                                      INCLUDES                                                       *
  ***********************************************************************************************************************/
 #include <stdio.h>
 /***********************************************************************************************************************
  *                                                   MACRO DEFINES                                                     *
  ***********************************************************************************************************************/
 #define L 30         // Length of the car in m
 #define W 25         // Width of the car in m
 #define VELOCITY 0.05  // Speed in m/s
 #define DISTANCE_STEP 0.05 // Distance step in cm
 /***********************************************************************************************************************
  *                                                  MACRO FUNCTIONS                                                    *
  ***********************************************************************************************************************/
 // Define any relevant macro functions here
 
 /***********************************************************************************************************************
  *                                                     DATA TYPES                                                      *
  ***********************************************************************************************************************/
 typedef enum
{
    ECU_OK = 0,
    ECU_ERROR = 1,
    ECU_RESERVED,
}ecu_status_t;
 /**
  * @struct  APK_t
  * @brief   Contains sensor and encoder data for the Auto Parking algorithm.
  * @note    All members are pointers to allow live data updates from sensors.
  */
 typedef struct
 {
     float dt;              /**< Time step in seconds */
     float *Front_ul;       /**< Pointer to front ultrasonic sensor reading */
     float *Right_ul;       /**< Pointer to right ultrasonic sensor reading */
     float *Left_ul;        /**< Pointer to left ultrasonic sensor reading */
     float *Rear_ul;        /**< Pointer to rear ultrasonic sensor reading */
     int   *FL_en;          /**< Pointer to front-left wheel encoder position */
     int   *FR_en;          /**< Pointer to front-right wheel encoder position */
     float *Car_Wanted_Speed; /**< Pointer to desired speed of the car */
     int *Car_Wanted_direction ; /**< Pointer to desired direction of the car (e.g., left or right) */
     int *Car_Wanted_Angle ; /**< Pointer to desired angle of the car */
     CAN_bus_t *Main_CAN; /**< Pointer to the main CAN bus structure */
     int *msg_robot_strafe; /**< Pointer to the message for robot strafe control */
 } APK_t;
 
 /***********************************************************************************************************************
  *                                                FUNCTION PROTOTYPES                                                  *
  ***********************************************************************************************************************/
 
/**
 * @brief   Executes the autonomous parking sequence.
 * @param   apk Pointer to a fully-initialized APK_t structure containing sensor and encoder data.
 * @return  ecu_status_t Status of the parking operation (e.g., success or failure).
 */
ecu_status_t Auto_Parking(const APK_t *apk);
 
 /***********************************************************************************************************************
  *                                                   AUTHOR NOTES                                                      *
  ***********************************************************************************************************************
  * Additional documentation, revision history, or implementation notes can be added here.
  ***********************************************************************************************************************/
 
 #endif /* AUTO_PARKING_H */
 
