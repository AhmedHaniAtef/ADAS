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
#include "../../ECU_Layer/inc/ecu.h"
#include "../../APP_Layer/inc/CAN_task.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
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

union twoFloats temp;
uint8_t tempBuffer[8];

uint8_t flag = 0;
Can_t CAN = {
	.Speed = MCP_8MHz_1000kBPS,
	.UsedSPI = &hspi2,
};

monitoring_t monitor = {
  .Size = 8,
  .UsedUART = &huart6,
  .MonitorUpdateData_CALLBACK = update_monitor,
  .Data = &temp.originData,
};

CAN_bus_t Main_CAN =
{
  .UsedCAN = &CAN,
};

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
/* Definitions for PID_task */
osThreadId_t PID_taskHandle;
const osThreadAttr_t PID_task_attributes = {
  .name = "PID_task",
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
/* Definitions for Temp_Move_Task */
osThreadId_t Temp_Move_TaskHandle;
const osThreadAttr_t Temp_Move_Task_attributes = {
  .name = "Temp_Move_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for CAN_INIT_mutex */
osMutexId_t CAN_INIT_mutexHandle;
const osMutexAttr_t CAN_INIT_mutex_attributes = {
  .name = "CAN_INIT_mutex"
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
void MONITORINGtask(void *argument);
void TempMoveTask(void *argument);

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

  /* creation of MONITORING_task */
  MONITORING_taskHandle = osThreadNew(MONITORINGtask, NULL, &MONITORING_task_attributes);

  /* creation of Temp_Move_Task */
  Temp_Move_TaskHandle = osThreadNew(TempMoveTask, NULL, &Temp_Move_Task_attributes);

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
  ecu_status_t t_EcuStatus = robot_init(&ADAS_ROBOT, 100);
  /* Infinite loop */
  for(;;)
  {
    t_EcuStatus |= robot_PID(&ADAS_ROBOT, 100);
    osDelay(100);
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

/* USER CODE BEGIN Header_TempMoveTask */
/**
* @brief Function implementing the Temp_Move_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TempMoveTask */
void TempMoveTask(void *argument)
{
  /* USER CODE BEGIN TempMoveTask */
  osDelay(10);
  /* Infinite loop */
  for(;;)
  {
    robot_move(&ADAS_ROBOT, 0, 0.3);
    osDelay(5000);
    robot_stop(&ADAS_ROBOT);
    osDelay(1000);
    robot_move(&ADAS_ROBOT, 180, 0.3);
    osDelay(5000);
    robot_stop(&ADAS_ROBOT);
    osDelay(1000);
    robot_move(&ADAS_ROBOT, 90, 0.3);
    osDelay(5000);
    robot_stop(&ADAS_ROBOT);
    osDelay(1000);
    robot_move(&ADAS_ROBOT, 270, 0.3);
    osDelay(5000);
    robot_stop(&ADAS_ROBOT);
    osDelay(1000);
  }
  /* USER CODE END TempMoveTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == CAN_INT_Pin) {
	        // Call your custom callback function here
		//CAN_CLEAR_INT(&Main_CAN);
	    osSemaphoreRelease(CAN_semaHandle);
		//flag = 1;
	}
}

void messagerx_clb(uCAN_MSG *p_Message)
{
  memcpy(tempBuffer, p_Message->frame.data, 8);
}

void update_monitor(void)
{
  memcpy(temp.data, tempBuffer, 8);
}

void messagetx_clb(uCAN_MSG *p_Message)
{
  p_Message->frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
  p_Message->frame.dlc = 4;
  p_Message->frame.data[0] ++;
  p_Message->frame.data[1] = p_Message->frame.data[0] + 1;
  p_Message->frame.data[2] = p_Message->frame.data[1] + 1;
  p_Message->frame.data[3] = p_Message->frame.data[2] + 1;
}
/* USER CODE END Application */

