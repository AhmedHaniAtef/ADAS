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
void messagerx_clb(uCAN_MSG *p_Message);
void messagetx_clb(uCAN_MSG *p_Message);
void update_monitor(void);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
union twoFloats
{
  struct
  {
    float x;
    float y;
  }originData;
  uint8_t data[8];
};
float temp_yaw = 0;

union twoFloats temp;
uint8_t tempBuffer[4];

uint8_t flag = 0;



can_msg_t messagerx =
{
  .ID = 0x0AA,
  .CallBack = messagerx_clb,
};

can_msg_t messagetx =
{
  .ID = 0x1BA,
  .CallBack = messagetx_clb,
};
/* USER CODE END Variables */
/* Definitions for MPU_task */
osThreadId_t MPU_taskHandle;
const osThreadAttr_t MPU_task_attributes = {
  .name = "MPU_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for CAN_task */
osThreadId_t CAN_taskHandle;
const osThreadAttr_t CAN_task_attributes = {
  .name = "CAN_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for MONITORING_task */
osThreadId_t MONITORING_taskHandle;
const osThreadAttr_t MONITORING_task_attributes = {
  .name = "MONITORING_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for CAN_INIT_mutex */
osMutexId_t CAN_INIT_mutexHandle;
const osMutexAttr_t CAN_INIT_mutex_attributes = {
  .name = "CAN_INIT_mutex"
};
/* Definitions for MPU_sema */
osSemaphoreId_t MPU_semaHandle;
const osSemaphoreAttr_t MPU_sema_attributes = {
  .name = "MPU_sema"
};
/* Definitions for CAN_sema */
osSemaphoreId_t CAN_semaHandle;
const osSemaphoreAttr_t CAN_sema_attributes = {
  .name = "CAN_sema"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void MPUtask(void *argument);
void CANtask(void *argument);
void MONITORINGtask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of CAN_INIT_mutex */
  CAN_INIT_mutexHandle = osMutexNew(&CAN_INIT_mutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of MPU_sema */
  MPU_semaHandle = osSemaphoreNew(1, 0, &MPU_sema_attributes);

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
  /* creation of MPU_task */
  MPU_taskHandle = osThreadNew(MPUtask, NULL, &MPU_task_attributes);

  /* creation of CAN_task */
  CAN_taskHandle = osThreadNew(CANtask, NULL, &CAN_task_attributes);

  /* creation of MONITORING_task */
  //MONITORING_taskHandle = osThreadNew(MONITORINGtask, NULL, &MONITORING_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_MPUtask */
/**
  * @brief  Function implementing the MPU_task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_MPUtask */
void MPUtask(void *argument)
{
  /* USER CODE BEGIN MPUtask */
  ecu_status_t t_EcuStatus = MPU6050_Init(&hi2c2,&MPUPin);
  /* Infinite loop */
  for(;;)
  {
    osSemaphoreAcquire(MPU_semaHandle, osWaitForever);
    t_EcuStatus |= MPU6050_Read_All(&hi2c2,&MPUPin);
    temp_yaw = 0.1 * MPUPin.Yaw + (1 - 0.1) * temp_yaw;
  }
  /* USER CODE END MPUtask */
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
  t_AppStatus |= CAN_add_msg_rx(&Main_CAN, &messagerx);
  /* Infinite loop */
  for(;;)
  {
	//while(flag == 0);
    osSemaphoreAcquire(CAN_semaHandle, osWaitForever);
    t_AppStatus |= CAN_rx_task(&Main_CAN);
    //flag = 0;
  }
  /* USER CODE END CANtask */
}

/* USER CODE BEGIN Header_MONITORINGtask */
/**
* @brief Function implementing the MONITORING_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MONITORINGtask */
void MONITORINGtask(void *argument)
{
  /* USER CODE BEGIN MONITORINGtask */
  ecu_status_t t_EcuStatus = ECU_OK;
  /* Infinite loop */
  for(;;)
  {
    t_EcuStatus |= monitoring_send_data(&monitor);
    osDelay(100);
  }
  /* USER CODE END MONITORINGtask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == CAN_INT_Pin) {
	    osSemaphoreRelease(CAN_semaHandle);
	}
  if (GPIO_Pin == MPU_INT_Pin)
  {
     osSemaphoreRelease(MPU_semaHandle);
  }
}

void messagerx_clb(uCAN_MSG *p_Message)
{
  memcpy(tempBuffer, p_Message->frame.data, 8);
}

void update_monitor(void)
{
	memcpy(tempBuffer, &MPUPin.Yaw, 4);
}

float test = 5;
typedef union  {
  float value;
  uint8_t con[4];
}one_float;

one_float temp;
  

void messagetx_clb(uCAN_MSG *p_Message)
{
  p_Message->frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
  p_Message->frame.dlc = 4;
  temp.value = test;
  p_Message->frame.data[0] = temp.con[0];
  p_Message->frame.data[1] = temp.con[1];
  p_Message->frame.data[2] = temp.con[2];
  p_Message->frame.data[3] = temp.con[3];
}
/* USER CODE END Application */

