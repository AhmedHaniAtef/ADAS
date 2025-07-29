/**
 * @file    ALC_feature.h
 * @author  Ahmed Hani
 * @brief   contains the functions which handle the Auto Lane Change
 * @date    2025-05-06
*/

#ifndef ALC_FEATURE_H_
#define ALC_FEATURE_H_
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
 * @brief descripes the messages from the camera
 * 
 */
typedef enum 
{
   LEFT_ONLY = 1,
   RIGHT_ONLY ,
   LEFT_AND_RIGHT,
   LANE_ERROR,
   DRIFT_LEFT,
   DRIFT_RIGHT,
   CENTERALIZED,
   LEFT_CENTERALIZED,
   RIGHT_CENTERALIZED,
}Lane_Received_t;

/**
 * @brief directions 
 * 
 */
typedef enum
{
    D_LEFT = 1,
    D_RIGHT,
    CENTER,
    DIRECTION_ERROR,
}Lane_Direction_t;

/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief handles the received form rasbperry pi 5 through CAN bus
 * 
 * @param p_AvailableLane represents the available lanes which car can change to
 * @param p_CurrentPosition represents the position of the can in the lane
 * @return app_status_t status of operation
 */
app_status_t ALC_handle(Lane_Received_t p_AvailableLane, Lane_Received_t p_CurrentPosition);

/**
 * @brief this function will be called when the user wants to change his lane
 * 
 * @param p_WantedLane direction of the lane the user wants
 * @return app_status_t 
 */
app_status_t ALC_Change_Lane(Lane_Direction_t p_WantedLane);

/**
 * @brief the task of the Auto Lane Change
 * 
 * @return app_status_t 
 */
app_status_t ALC_Task(void);

/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif //ALC_FEATURE_H_
 
