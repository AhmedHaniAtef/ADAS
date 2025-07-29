/**
 * @file    ACC_feature.h
 * @author  Ahmed Hani
 * @brief   contains the functions which handle the Adaptive Cruise Control feature
 * @date    2025-02-23
 */

#ifndef ACC_FEATURE_H_
#define ACC_FEATURE_H_
/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../../ECU_Layer/inc/ecu.h"
#include "../APP_std.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/

/**
 * @brief data type handles the data used by acc feature
 * @param Front_UL pointer to the value of reading of Ultrasonnic on front
 * @param Right_UL pointer to the value of reading of Ultrasonnic on 45
 * @param Left_UL pointer to the value of reading of Ultrasonnic on 315
 * @param Front_Relative_speed value of realative spped of obstacle on Front
 * @param Right_Relative_speed value of realative spped of obstacle on Right
 * @param Left_Relative_speed value of realative spped of obstacle on Left
 */
typedef struct 
{
   PID_Controller PID_Vx;
   PID_Controller PID_Vy;
   float_t *Front_UL;
   float_t *Right_UL;
   float_t *Left_UL;
   float_t Front_Relative_speed;
   float_t Right_Relative_speed;
   float_t Left_Relative_speed;
   float_t dt;
   float_t PID_Vx_Output;
}ACC_t;

/**
 * @brief data type to identify the max speed request by
 * @param ACC Adaptive Cruise Control
 * @param BSD Blind Spot Detection
 * @param TSR Traffic Sign Recognition
 * @param ALC Auto Lane Change
 * @param APK Auto Parking
 */
typedef enum
{
   ACC = 0,
   BSD,
   TSR,
   ALC,
   APK,
}Max_Speed_ID_t;

/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/
/**
 * @brief this task is periodically called to detect the obstacles and the realative speeds
 * 
 * @param p_ACC pointer to the object used
 * @return app_status_t status of operation
 */
app_status_t ACC_relative_task(ACC_t *p_ACC);

/**
 * @brief initialize the task of ACC
 * 
 * @param p_ACC pointer to the object used
 * @return app_status_t status of operation
 */
app_status_t ACC_task_init(ACC_t *p_ACC);

/**
 * @brief this function should be called when a feature want to limit the speed of the car
 * 
 * @param p_MaxSpeed the limited max speed 
 * @param p_ID the id of request
 * @return the maximum speed should the car move with
 */
float_t Max_speed_responsible(float_t p_MaxSpeed, Max_Speed_ID_t p_ID);


/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif //ACC_FEATURE_H_
