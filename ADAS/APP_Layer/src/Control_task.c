/**
 * @file    Control_task.c
 * @author  Ahmed Hani
 * @brief   contains task of Controller 
 * @date    2025-02-08
 */

/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "../inc/APP.h"
#include "Control_task.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                               STATIC FUNCTION DEFINITION                                             *
***********************************************************************************************************************/





/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/






/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/

static void (*manual_control_yaw) (void) = NULL;
static void (*auto_control_yaw)   (void) = NULL;




/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
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
                                     void (* p_Callback) (void *))
{
    app_status_t l_AppStatus = APP_OK;
    if ((NULL == p_UsedController) || (NULL == p_Callback))
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        switch (p_Button)
        {
        case UP:
            p_UsedController->UP_button_clb = p_Callback;
            break;

        case DOWN:
            p_UsedController->DOWN_button_clb = p_Callback;
            break;
            
        case LEFT:
            p_UsedController->LEFT_button_clb = p_Callback;
            break;

        case RIGHT:
            p_UsedController->RIGHT_button_clb = p_Callback;
            break;

        case SELECT:
            p_UsedController->SELECT_button_clb = p_Callback;
            break;

        case START:
            p_UsedController->START_button_clb = p_Callback;
            break;

        case LEFT_STICK:
            p_UsedController->LEFT_STICK_button_clb = p_Callback;
            break;

        case RITHT_STICK:
            p_UsedController->RITHT_STICK_button_clb = p_Callback;
            break;
        
        default:
            l_AppStatus = APP_ERROR;
            break;
        }
    }
    return l_AppStatus;
}

/**
 * @brief this function request to receive commands from controller
 * @param p_UsedController pointer to the controller
 * @return app_status_t status of operation
 */
app_status_t controller_wait_receive(controller_t *p_UsedController)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_UsedController)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        ecu_status_t l_EcuStatus= control_receive_data(p_UsedController->UsedController);
        if (l_EcuStatus != ECU_OK)
        {
            l_AppStatus = APP_ERROR;
        }
    }
    return l_AppStatus;
}

/**
 * @brief this function take an action depending on the command received
 * @param p_UsedController pointer to the controller
 * @return app_status_t status of operation
 */
app_status_t controller_task(controller_t *p_UsedController)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_UsedController)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        Push_Button_Joy_Stick_Data l_PushButton;
        two_float_conv temp;
        one_float_conv temp_angle;
        switch (p_UsedController->UsedController->Data[0])
		{
			case (LEFT_JOY_STICK):
                memcpy(temp.data, &p_UsedController->UsedController->Data[1], 8);
				Car_Wanted_Speed = ((temp.value[0]) * DEFUALT_ROBOT_MAX_SPEED) / 100.0;
                Car_Wanted_direction = temp.value[1];
                /* send CAN Message of strafe */
                l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_strafe);
				break;

			case(RIGHT_JOY_STICK):
                memcpy(temp.data, &p_UsedController->UsedController->Data[1], 8);
                Car_Wanted_Angular_Speed = ((temp.value[0]) * DEFUALT_ROBOT_MAX_ANGULAR_SPEED) / 100.0;
                Car_Wanted_Rotate_Radius = ((temp.value[1]) * DEFUALT_ROBOT_MAX_RADIUS) / 100.0;
                Car_Wanted_Angle = Main_Orientation.FilteredYAW;
                /* send CAN Message of rotate */
                l_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_rotate);
                if ((Car_Wanted_Angular_Speed == 0) && (Car_Wanted_Rotate_Radius == 0))
                {
                    auto_control_yaw();
                }
                else
                {
                    manual_control_yaw();
                }
				break;

			case(PUSH_BUTTON):
                memcpy(l_PushButton.uart_data, p_UsedController->UsedController->Data, 9);
				if ((l_PushButton.frame.up == 0x01) && (NULL != p_UsedController->UP_button_clb))
                {
                    p_UsedController->UP_button_clb(NULL);
                }

                if ((l_PushButton.frame.down == 0x01) && (NULL != p_UsedController->DOWN_button_clb))
                {
                    p_UsedController->DOWN_button_clb(NULL);
                }

                if ((l_PushButton.frame.left == 0x01) && (NULL != p_UsedController->LEFT_button_clb))
                {
                    p_UsedController->LEFT_button_clb(NULL);
                }

                if ((l_PushButton.frame.right == 0x01) && (NULL != p_UsedController->RIGHT_button_clb))
                {
                    p_UsedController->RIGHT_button_clb(NULL);
                }

                if ((l_PushButton.frame.select == 0x01) && (NULL != p_UsedController->SELECT_button_clb))
                {
                    p_UsedController->SELECT_button_clb(NULL);
                }

                if ((l_PushButton.frame.start == 0x01) && (NULL != p_UsedController->START_button_clb))
                {
                    p_UsedController->START_button_clb(NULL);
                }

                if ((l_PushButton.frame.left_stick == 0x01) && (NULL != p_UsedController->LEFT_STICK_button_clb))
                {
                    p_UsedController->LEFT_STICK_button_clb(NULL);
                }

                if ((l_PushButton.frame.right_stick == 0x01) && (NULL != p_UsedController->RITHT_STICK_button_clb))
                {
                    p_UsedController->RITHT_STICK_button_clb(NULL);
                }
				break;
            
            case (COMPASS_ANGLE):
                memcpy(temp_angle.data, &p_UsedController->UsedController->Data[1], 4);
                *p_UsedController->CompassAnglePTR = temp_angle.value; 
                break;

			default:
				l_AppStatus = APP_ERROR;
			break;
		}
    }
    return l_AppStatus;
}

/**
 * @brief initalize the callback pointer which gets the control of orientation from the car while in manual driving
 * 
 * @param p_UsedController pointer to the controller 
 * @param get_yaw_control_yaw pointer to the callback function which get the control of the yaw angle
 * @return app_status_t status of operation
 */
app_status_t controller_get_yaw_control_init(controller_t *p_UsedController, void (* get_yaw_control_yaw)(void))
{
    app_status_t l_AppStatus = APP_OK;
    if ((NULL == p_UsedController) || (NULL == get_yaw_control_yaw))
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        manual_control_yaw = get_yaw_control_yaw;
    }
    return l_AppStatus;
}


/**
 * @brief initalize the callback pointer which gives the control of orientation from the car while in manual driving
 * 
 * @param p_UsedController pointer to the controller 
 * @param get_yaw_control_yaw pointer to the callback function which give the control of the yaw angle
 * @return app_status_t status of operation
 */
app_status_t controller_give_yaw_control_init(controller_t *p_UsedController, void (* give_yaw_control_yaw)(void))
{
    app_status_t l_AppStatus = APP_OK;
    if ((NULL == p_UsedController) || (NULL == give_yaw_control_yaw))
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        auto_control_yaw = give_yaw_control_yaw;
    }
    return l_AppStatus;
}


/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/




/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
