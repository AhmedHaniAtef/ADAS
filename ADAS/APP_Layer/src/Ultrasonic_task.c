/**
 * @file    Ultrasonic_task.c
 * @author  Ahmed Hani
 * @brief   contains task of Ultrasonics 
 * @date    2025-02-08
 */


/***********************************************************************************************************************
*                                                      INCLUDES                                                        *
***********************************************************************************************************************/
#include "APP.h"
#include "Ultrasonic_task.h"



/***********************************************************************************************************************
*                                                    MACRO DEFINES                                                     *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                   MACRO FUNCTIONS                                                    *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                               STATIC FUNCTION DEFINITION                                             *

***********************************************************************************************************************/

/**
 * @brief shifts the reading in the buffer
 * 
 * @param p_ReadingBuffer buffer will be shifted
 * @param p_NewReading the new value should be shiffted in the buffer
 */
static void shift_readings(float_t *p_ReadingBuffer, float_t p_NewReading);

/**
 * @brief detect the outlier depending on the window of the last reading values
 * 
 * @param p_ReadingBuffer the buffer he will take the action depending on 
 * @param p_NewReading the value want to be checked
 * @return float_t best reading decision
 * @note this function returns the new reading if no outlier detected and returns the median of 
 * last N (OUTLIER_WINDOW_SIZE) reading if detected
 */
static float_t outlier_detect(float_t *p_ReadingBuffer, float_t p_NewReading);




/***********************************************************************************************************************
*                                                     GLOBAL OBJECTS                                                   *
***********************************************************************************************************************/






/***********************************************************************************************************************
*                                                     STATIC OBJECTS                                                   *
***********************************************************************************************************************/
static float_t Last_N_Readings_ul0[OUTLIER_WINDOW_SIZE] = {0};
static float_t Last_N_Readings_ul45[OUTLIER_WINDOW_SIZE] = {0};
static float_t Last_N_Readings_ul90[OUTLIER_WINDOW_SIZE] = {0};
static float_t Last_N_Readings_ul135[OUTLIER_WINDOW_SIZE] = {0};
static float_t Last_N_Readings_ul180[OUTLIER_WINDOW_SIZE] = {0};
static float_t Last_N_Readings_ul225[OUTLIER_WINDOW_SIZE] = {0};
static float_t Last_N_Readings_ul270[OUTLIER_WINDOW_SIZE] = {0};
static float_t Last_N_Readings_ul315[OUTLIER_WINDOW_SIZE] = {0};


/***********************************************************************************************************************
*                                                      DATA TYPES                                                      *
***********************************************************************************************************************/




/***********************************************************************************************************************
*                                                  FUNCTION DECLARATION                                                *
***********************************************************************************************************************/

/**
 * @brief this function initialize the task of reading from all ultrasonics
 * @param p_AllUltrasonics pointer to the Ultrasonics of the car
 * @return app_status_t status of operation
 */
app_status_t ultrasonics_task_init(ultrasonics_t *p_AllUltrasonics)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_AllUltrasonics)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        ecu_status_t l_EcuStatus = Ultrasonic_Init(8,
                                                   p_AllUltrasonics->UL_0,
                                                   p_AllUltrasonics->UL_180,
                                                   p_AllUltrasonics->UL_90,
                                                   p_AllUltrasonics->UL_270,
                                                   p_AllUltrasonics->UL_45,
                                                   p_AllUltrasonics->UL_225,
                                                   p_AllUltrasonics->UL_135,
                                                   p_AllUltrasonics->UL_315
                                                   
        );
        if (ECU_OK != l_EcuStatus)
        {
            l_AppStatus = APP_ERROR;
        }
    }
    return l_AppStatus;
}

/**
 * @brief this function requests for update the value of 0-180 ultrasonics
 * @param p_AllUltrasonics pointer to the Ultrasonics of the car
 * @return app_status_t status of operation
 */
app_status_t ultrasonics_update_task_X(ultrasonics_t *p_AllUltrasonics)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_AllUltrasonics)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        ecu_status_t l_EcuStatus = Ultrasonic_ReadDistance(2, 
                                                           1,
                                                           p_AllUltrasonics->UL_0,
                                                           p_AllUltrasonics->UL_180
                                                           
        );
        if (ECU_OK != l_EcuStatus)
        {
        	l_AppStatus = APP_ERROR;
        }
    }
    return l_AppStatus;
}

/**
 * @brief this function requests for update the value of 45-135-225-315 ultrasonics
 * @param p_AllUltrasonics pointer to the Ultrasonics of the car
 * @return app_status_t status of operation
 */
app_status_t ultrasonics_update_task_U(ultrasonics_t *p_AllUltrasonics)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_AllUltrasonics)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        ecu_status_t l_EcuStatus = Ultrasonic_ReadDistance(2, 
                                                           5,
                                                           p_AllUltrasonics->UL_45,
                                                           p_AllUltrasonics->UL_225
                                                           
        );
        if (ECU_OK != l_EcuStatus)
        {
        	l_AppStatus = APP_ERROR;
        }
    }
    return l_AppStatus;
}

/**
 * @brief this function requests for update the value of 90-270 ultrasonics
 * @param p_AllUltrasonics pointer to the Ultrasonics of the car
 * @return app_status_t status of operation
 */
app_status_t ultrasonics_update_task_Y(ultrasonics_t *p_AllUltrasonics)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_AllUltrasonics)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        ecu_status_t l_EcuStatus = Ultrasonic_ReadDistance(2, 
                                                           3,
                                                           p_AllUltrasonics->UL_90,
                                                           p_AllUltrasonics->UL_270
                                                        
        );
        if (ECU_OK != l_EcuStatus)
        {
        	l_AppStatus = APP_ERROR;
        }
    }
    return l_AppStatus;
}

/**
 * @brief this function requests for update the value of 45-135-225-315 ultrasonics
 * @param p_AllUltrasonics pointer to the Ultrasonics of the car
 * @return app_status_t status of operation
 */
app_status_t ultrasonics_update_task_V(ultrasonics_t *p_AllUltrasonics)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_AllUltrasonics)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        ecu_status_t l_EcuStatus = Ultrasonic_ReadDistance(2, 
                                                           7,
                                                           p_AllUltrasonics->UL_135,
                                                           p_AllUltrasonics->UL_315
                                                           
        );
        if (ECU_OK != l_EcuStatus)
        {
        	l_AppStatus = APP_ERROR;
        }
    }
    return l_AppStatus;
}

/**
 * @brief update the value of the reading of ultrasonics 0-180 depending on the outlier detection method
 * 
 * @param p_AllUltrasonics pointer to all ultrasonics
 * @return app_status_t status of operations
 */
app_status_t ultrasonics_outlier_detect_X(ultrasonics_t *p_AllUltrasonics)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_AllUltrasonics)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t temp = 0.0f;
        temp = outlier_detect(Last_N_Readings_ul0, *(p_AllUltrasonics->UL_0->Distance));
        shift_readings(Last_N_Readings_ul0, temp);
        *(p_AllUltrasonics->UL_0->Distance) = temp;

        temp = outlier_detect(Last_N_Readings_ul180, *(p_AllUltrasonics->UL_180->Distance));
        shift_readings(Last_N_Readings_ul180, temp);
        *(p_AllUltrasonics->UL_180->Distance) = temp;
    }
    return l_AppStatus;
}

/**
 * @brief update the value of the reading of ultrasonics 45-225 depending on the outlier detection method
 * 
 * @param p_AllUltrasonics pointer to all ultrasonics
 * @return app_status_t status of operations
 */
app_status_t ultrasonics_outlier_detect_U(ultrasonics_t *p_AllUltrasonics)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_AllUltrasonics)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t temp = 0.0f;
        temp = outlier_detect(Last_N_Readings_ul45, *(p_AllUltrasonics->UL_45->Distance));
        shift_readings(Last_N_Readings_ul45, temp);
        *(p_AllUltrasonics->UL_45->Distance) = temp;

        temp = outlier_detect(Last_N_Readings_ul225, *(p_AllUltrasonics->UL_225->Distance));
        shift_readings(Last_N_Readings_ul225, temp);
        *(p_AllUltrasonics->UL_225->Distance) = temp;
    }
    return l_AppStatus;
}

/**
 * @brief update the value of the reading of ultrasonics 90-270 depending on the outlier detection method
 * 
 * @param p_AllUltrasonics pointer to all ultrasonics
 * @return app_status_t status of operations
 */
app_status_t ultrasonics_outlier_detect_Y(ultrasonics_t *p_AllUltrasonics)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_AllUltrasonics)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t temp = 0.0f;
        temp = outlier_detect(Last_N_Readings_ul90, *(p_AllUltrasonics->UL_90->Distance));
        shift_readings(Last_N_Readings_ul90, temp);
        *(p_AllUltrasonics->UL_90->Distance) = temp;

        temp = outlier_detect(Last_N_Readings_ul270, *(p_AllUltrasonics->UL_270->Distance));
        shift_readings(Last_N_Readings_ul315, temp);
        *(p_AllUltrasonics->UL_270->Distance) = temp;
    }
    return l_AppStatus;
}

/**
 * @brief update the value of the reading of ultrasonics 135-315 depending on the outlier detection method
 * 
 * @param p_AllUltrasonics pointer to all ultrasonics
 * @return app_status_t status of operations
 */
app_status_t ultrasonics_outlier_detect_V(ultrasonics_t *p_AllUltrasonics)
{
    app_status_t l_AppStatus = APP_OK;
    if (NULL == p_AllUltrasonics)
    {
        l_AppStatus = APP_ERROR;
    }
    else
    {
        float_t temp = 0.0f;
        temp = outlier_detect(Last_N_Readings_ul135, *(p_AllUltrasonics->UL_135->Distance));
        shift_readings(Last_N_Readings_ul135, temp);
        *(p_AllUltrasonics->UL_135->Distance) = temp;

        temp = outlier_detect(Last_N_Readings_ul315, *(p_AllUltrasonics->UL_315->Distance));
        shift_readings(Last_N_Readings_ul315, temp);
        *(p_AllUltrasonics->UL_315->Distance) = temp;
        
    }
    return l_AppStatus;
}
/***********************************************************************************************************************
*                                               STATIC FUNCTION DECLARATION                                            *
***********************************************************************************************************************/

/**
 * @brief shifts the reading in the buffer
 * 
 * @param p_ReadingBuffer buffer will be shifted
 * @param p_NewReading the new value should be shiffted in the buffer
 */
static void shift_readings(float_t *p_ReadingBuffer, float_t p_NewReading)
{
    for (uint8_t counter = OUTLIER_WINDOW_SIZE - 1; counter > 0; counter--)
    {
        p_ReadingBuffer[counter] = p_ReadingBuffer[counter - 1];
    }
    p_ReadingBuffer[0] = p_NewReading;
}

/**
 * @brief detect the outlier depending on the window of the last reading values
 * 
 * @param p_ReadingBuffer the buffer he will take the action depending on 
 * @param p_NewReading the value want to be checked
 * @return float_t best reading decision
 * @note this function returns the new reading if no outlier detected and returns the median of 
 * last N (OUTLIER_WINDOW_SIZE) reading if detected
 */
static float_t outlier_detect(float_t *p_ReadingBuffer, float_t p_NewReading)
{
    float_t l_RetReading = 0.0f;
    float_t l_Median = 0.0f;
    for (uint8_t counter = 0; counter < OUTLIER_WINDOW_SIZE; counter++)
    {
        l_Median += p_ReadingBuffer[counter];
    }
    l_Median /= (float_t)OUTLIER_WINDOW_SIZE;
    if ((p_NewReading > (l_Median + OUTLIER_THRESHOLD)) || 
        (p_NewReading < (l_Median - OUTLIER_THRESHOLD)) ||
        (p_NewReading > ULTRASONIC_MAX_READING))
    {
        l_RetReading = l_Median;
    }
    else
    {
        l_RetReading = p_NewReading;
    }
    return l_RetReading;
}



/***********************************************************************************************************************
* AUTHOR                |* NOTE                                                                                        *
************************************************************************************************************************
*                       |                                                                                              * 
*                       |                                                                                              * 
***********************************************************************************************************************/
