/**
 * @file    Ultrasonic_task.h
 * @author  Ahmed Hani
 * @brief   contains task of Ultrasonics 
 * @date    2025-02-08
 */

#ifndef ULTRASONIC_TASK_H_
#define ULTRASONIC_TASK_H_
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
    UltrasonicSensor *UL_0;
    UltrasonicSensor *UL_45;
    UltrasonicSensor *UL_90;
    UltrasonicSensor *UL_135;
    UltrasonicSensor *UL_180;
    UltrasonicSensor *UL_225;
    UltrasonicSensor *UL_270;
    UltrasonicSensor *UL_315;
}ultrasonics_t;




/***********************************************************************************************************************
*                                                  FUNCTION DEFINITION                                                 *
***********************************************************************************************************************/

/**
 * @brief this function initialize the task of reading from all ultrasonics
 * @param p_AllUltrasonics pointer to the Ultrasonics of the car
 * @return app_status_t status of operation
 */
app_status_t ultrasonics_task_init(ultrasonics_t *p_AllUltrasonics);

/**
 * @brief this function requests for update the value of 0-180 ultrasonics
 * @param p_AllUltrasonics pointer to the Ultrasonics of the car
 * @return app_status_t status of operation
 */
app_status_t ultrasonics_update_task_X(ultrasonics_t *p_AllUltrasonics);

/**
 * @brief this function requests for update the value of 45-135-225-315 ultrasonics
 * @param p_AllUltrasonics pointer to the Ultrasonics of the car
 * @return app_status_t status of operation
 */
app_status_t ultrasonics_update_task_U(ultrasonics_t *p_AllUltrasonics);

/**
 * @brief this function requests for update the value of 90-270 ultrasonics
 * @param p_AllUltrasonics pointer to the Ultrasonics of the car
 * @return app_status_t status of operation
 */
app_status_t ultrasonics_update_task_Y(ultrasonics_t *p_AllUltrasonics);

/**
 * @brief this function requests for update the value of 45-135-225-315 ultrasonics
 * @param p_AllUltrasonics pointer to the Ultrasonics of the car
 * @return app_status_t status of operation
 */
app_status_t ultrasonics_update_task_V(ultrasonics_t *p_AllUltrasonics);

/**
 * @brief update the value of the reading of ultrasonics 0-180 depending on the outlier detection method
 * 
 * @param p_AllUltrasonics pointer to all ultrasonics
 * @return app_status_t status of operations
 */
app_status_t ultrasonics_outlier_detect_X(ultrasonics_t *p_AllUltrasonics);

/**
 * @brief update the value of the reading of ultrasonics 45-225 depending on the outlier detection method
 * 
 * @param p_AllUltrasonics pointer to all ultrasonics
 * @return app_status_t status of operations
 */
app_status_t ultrasonics_outlier_detect_U(ultrasonics_t *p_AllUltrasonics);

/**
 * @brief update the value of the reading of ultrasonics 90-270 depending on the outlier detection method
 * 
 * @param p_AllUltrasonics pointer to all ultrasonics
 * @return app_status_t status of operations
 */
app_status_t ultrasonics_outlier_detect_Y(ultrasonics_t *p_AllUltrasonics);

/**
 * @brief update the value of the reading of ultrasonics 135-315 depending on the outlier detection method
 * 
 * @param p_AllUltrasonics pointer to all ultrasonics
 * @return app_status_t status of operations
 */
app_status_t ultrasonics_outlier_detect_V(ultrasonics_t *p_AllUltrasonics);



/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/

#endif //ULTRASONIC_TASK_H_
