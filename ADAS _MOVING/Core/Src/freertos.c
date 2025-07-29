/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../APP_Layer/inc/APP.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for PID_task */
osThreadId_t PID_taskHandle;
const osThreadAttr_t PID_task_attributes = {
  .name = "PID_task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for CAN_task */
osThreadId_t CAN_taskHandle;
const osThreadAttr_t CAN_task_attributes = {
  .name = "CAN_task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityRealtime1,
};
/* Definitions for Encoder_task */
osThreadId_t Encoder_taskHandle;
const osThreadAttr_t Encoder_task_attributes = {
  .name = "Encoder_task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityHigh7,
};
/* Definitions for CAN_mutex */
osMutexId_t CAN_mutexHandle;
const osMutexAttr_t CAN_mutex_attributes = {
  .name = "CAN_mutex"
};
/* Definitions for ROBOT_INITsema */
osSemaphoreId_t ROBOT_INITsemaHandle;
const osSemaphoreAttr_t ROBOT_INITsema_attributes = {
  .name = "ROBOT_INITsema"
};
/* Definitions for CAN_sema */
osSemaphoreId_t CAN_semaHandle;
const osSemaphoreAttr_t CAN_sema_attributes = {
  .name = "CAN_sema"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void PIDtask(void *argument);
void CANtask(void *argument);
void EncoderTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of CAN_mutex */
  CAN_mutexHandle = osMutexNew(&CAN_mutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of ROBOT_INITsema */
  ROBOT_INITsemaHandle = osSemaphoreNew(1, 0, &ROBOT_INITsema_attributes);

  /* creation of CAN_sema */
  CAN_semaHandle = osSemaphoreNew(2, 0, &CAN_sema_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of PID_task */
  PID_taskHandle = osThreadNew(PIDtask, NULL, &PID_task_attributes);

  /* creation of CAN_task */
  CAN_taskHandle = osThreadNew(CANtask, NULL, &CAN_task_attributes);

  /* creation of Encoder_task */
  Encoder_taskHandle = osThreadNew(EncoderTask, NULL, &Encoder_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_PIDtask */
/**
  * @brief  Function implementing the PID_task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_PIDtask */
void PIDtask(void *argument)
{
  /* USER CODE BEGIN PIDtask */
  ecu_status_t t_EcuStatus = robot_init(&ADAS_ROBOT, MOTORS_PID_TIME_STEP);
  osSemaphoreRelease(ROBOT_INITsemaHandle);
  /* Infinite loop */
  for(;;)
  {
    t_EcuStatus |= robot_PID(&ADAS_ROBOT, MOTORS_PID_TIME_STEP);
    osDelay(MOTORS_PID_TIME_STEP);
  }
  /* USER CODE END PIDtask */
}

/* USER CODE BEGIN Header_CANtask */
/**
* @brief Function implementing the CAN_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CANtask */
void CANtask(void *argument)
{
  /* USER CODE BEGIN CANtask */
  app_status_t t_AppStatus = CAN_task_init(&Main_CAN);
  t_AppStatus |= messages_init();
  /* Infinite loop */
  for(;;)
  {
    osSemaphoreAcquire(CAN_semaHandle, osWaitForever);
    osMutexAcquire(CAN_mutexHandle, osWaitForever);
    t_AppStatus |= CAN_rx_task(&Main_CAN);
    osMutexRelease(CAN_mutexHandle);
  }
  /* USER CODE END CANtask */
}

/* USER CODE BEGIN Header_EncoderTask */
/**
* @brief Function implementing the Encoder_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_EncoderTask */
void EncoderTask(void *argument)
{
  /* USER CODE BEGIN EncoderTask */
  app_status_t t_AppStatus = APP_OK;
  /* Infinite loop */
  for(;;)
  {
    if (ADAS_ROBOT.FL.Encoder.Position != EncoderLeftTemp ||
        ADAS_ROBOT.FR.Encoder.Position != EncoderRightTemp  ) 
    {
      osMutexAcquire(CAN_mutexHandle, osWaitForever);
      t_AppStatus |= CAN_send_message(&Main_CAN, &msg_update_encoder);
      osMutexRelease(CAN_mutexHandle);
      EncoderLeftTemp = ADAS_ROBOT.FL.Encoder.Position;
      EncoderRightTemp = ADAS_ROBOT.FR.Encoder.Position;
    }

    osDelay(50);
  }
  /* USER CODE END EncoderTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == CAN_INT_Pin) {
	    // Call your custom callback function here
	    osSemaphoreRelease(CAN_semaHandle);
	}
}
/* USER CODE END Application */

