/**
 * @file    Control_task.h
 * @author  Ahmed Hani
 * @brief   contains task of MPU 
 * @date    2025-02-08
 */

#ifndef CONTROL_TASK_H_
#define CONTROL_TASK_H_

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

typedef struct 
{
    control_t *UsedController;
    void (* UP_button_clb)(void *);
    void (* DOWN_button_clb)(void *);
    void (* LEFT_button_clb)(void *);
    void (* RIGHT_button_clb)(void *);
    void (* SELECT_button_clb)(void *);
    void (* START_button_clb)(void *);
    void (* LEFT_STICK_button_clb)(void *);
    void (* RITHT_STICK_button_clb)(void *);
    float_t *CompassAnglePTR;
}controller_t;

typedef enum
{
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
    SELECT,
    START,
    LEFT_STICK,
    RITHT_STICK,
}pressed_button_t;


/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief adds a callback associated to the button pressed
 * @param p_UsedController pointer to the controller
 * @param p_Button which buttor @ref pressed_button_t
 * @param p_Callback pointer to the callback needed to be associated to the button
 * @return app_status_t 
 */
app_status_t controller_add_callback(controller_t *p_UsedController, 
                                     pressed_button_t p_Button, 
                                     void (* p_Callback)(void *));

/**
 * @brief this function request to receive commands from controller
 * @param p_UsedController pointer to the controller
 * @return app_status_t status of operation
 */
app_status_t controller_wait_receive(controller_t *p_UsedController);

/**
 * @brief this function take an action depending on the command received
 * @param p_UsedController pointer to the controller
 * @return app_status_t status of operation
 */
app_status_t controller_task(controller_t *p_UsedController);

/**
 * @brief get the control of orientation from the car while in manual driving
 * 
 * @param p_UsedController pointer to the controller 
 * @param get_yaw_control_yaw pointer to the callback function which get the control of the yaw angle
 * @return app_status_t status of operation
 */
app_status_t controller_get_yaw_control_init(controller_t *p_UsedController, void (get_yaw_control_yaw)(void));

/**
 * @brief initalize the callback pointer which gives the control of orientation from the car while in manual driving
 * 
 * @param p_UsedController pointer to the controller 
 * @param get_yaw_control_yaw pointer to the callback function which give the control of the yaw angle
 * @return app_status_t status of operation
 */
app_status_t controller_give_yaw_control_init(controller_t *p_UsedController, void (* give_yaw_control_yaw)(void));

/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif //CONTROL_TASK_H_
