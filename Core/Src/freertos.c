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
/* Definitions for MPU_task */
osThreadId_t MPU_taskHandle;
const osThreadAttr_t MPU_task_attributes = {
  .name = "MPU_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime1,
};
/* Definitions for CAN_task */
osThreadId_t CAN_taskHandle;
const osThreadAttr_t CAN_task_attributes = {
  .name = "CAN_task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for MONITORING_task */
osThreadId_t MONITORING_taskHandle;
const osThreadAttr_t MONITORING_task_attributes = {
  .name = "MONITORING_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for Mpu_PID_task */
osThreadId_t Mpu_PID_taskHandle;
const osThreadAttr_t Mpu_PID_task_attributes = {
  .name = "Mpu_PID_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for Ultrasonic_task */
osThreadId_t Ultrasonic_taskHandle;
const osThreadAttr_t Ultrasonic_task_attributes = {
  .name = "Ultrasonic_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh2,
};
/* Definitions for CONTROL_task */
osThreadId_t CONTROL_taskHandle;
const osThreadAttr_t CONTROL_task_attributes = {
  .name = "CONTROL_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for TempPIDmoveTask */
osThreadId_t TempPIDmoveTaskHandle;
const osThreadAttr_t TempPIDmoveTask_attributes = {
  .name = "TempPIDmoveTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for MPU_sema */
osSemaphoreId_t MPU_semaHandle;
const osSemaphoreAttr_t MPU_sema_attributes = {
  .name = "MPU_sema"
};
/* Definitions for MPU_PID_sema */
osSemaphoreId_t MPU_PID_semaHandle;
const osSemaphoreAttr_t MPU_PID_sema_attributes = {
  .name = "MPU_PID_sema"
};
/* Definitions for CONTROL_sema */
osSemaphoreId_t CONTROL_semaHandle;
const osSemaphoreAttr_t CONTROL_sema_attributes = {
  .name = "CONTROL_sema"
};
/* Definitions for PID_tune_sema */
osSemaphoreId_t PID_tune_semaHandle;
const osSemaphoreAttr_t PID_tune_sema_attributes = {
  .name = "PID_tune_sema"
};
/* Definitions for CAN_sema */
osSemaphoreId_t CAN_semaHandle;
const osSemaphoreAttr_t CAN_sema_attributes = {
  .name = "CAN_sema"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Robot_Stop_PID_tunning(void *);
void Robot_Start_PID_tunning(void *);
void Increase_Set_Point_angle(void *);
void Increase_Kp(void *);
void Increase_Ki(void *);
void Increase_Kd(void *);
void Increase_N(void *);
void Reset_PID(void *);
/* USER CODE END FunctionPrototypes */

void MPUtask(void *argument);
void CANtask(void *argument);
void MONITORINGtask(void *argument);
void MpuPIDtask(void *argument);
void ULTRASONICtask(void *argument);
void CONTROLtask(void *argument);
void TempPIDmove_Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of MPU_sema */
  MPU_semaHandle = osSemaphoreNew(1, 0, &MPU_sema_attributes);

  /* creation of MPU_PID_sema */
  MPU_PID_semaHandle = osSemaphoreNew(1, 0, &MPU_PID_sema_attributes);

  /* creation of CONTROL_sema */
  CONTROL_semaHandle = osSemaphoreNew(1, 0, &CONTROL_sema_attributes);

  /* creation of PID_tune_sema */
  PID_tune_semaHandle = osSemaphoreNew(1, 0, &PID_tune_sema_attributes);

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
  MONITORING_taskHandle = osThreadNew(MONITORINGtask, NULL, &MONITORING_task_attributes);

  /* creation of Mpu_PID_task */
  Mpu_PID_taskHandle = osThreadNew(MpuPIDtask, NULL, &Mpu_PID_task_attributes);

  /* creation of Ultrasonic_task */
  Ultrasonic_taskHandle = osThreadNew(ULTRASONICtask, NULL, &Ultrasonic_task_attributes);

  /* creation of CONTROL_task */
  CONTROL_taskHandle = osThreadNew(CONTROLtask, NULL, &CONTROL_task_attributes);

  /* creation of TempPIDmoveTask */
  TempPIDmoveTaskHandle = osThreadNew(TempPIDmove_Task, NULL, &TempPIDmoveTask_attributes);

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
  app_status_t t_AppStatus = MPU_task_init(&Main_MPU);
  osSemaphoreRelease(MPU_PID_semaHandle);
  /* Infinite loop */
  for(;;)
  {
    osSemaphoreAcquire(MPU_semaHandle, osWaitForever);
    t_AppStatus |= MPU_update_task(&Main_MPU);
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
  app_status_t t_AppStatus = monitor_task_init(&Main_Monitor_values);
  /* Infinite loop */
  for(;;)
  {
    t_AppStatus |= monitor_update_task(&Main_Monitor_values);
    osDelay(100);
  }
  /* USER CODE END MONITORINGtask */
}

/* USER CODE BEGIN Header_MpuPIDtask */
/**
* @brief Function implementing the Mpu_PID_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MpuPIDtask */
void MpuPIDtask(void *argument)
{
  /* USER CODE BEGIN MpuPIDtask */
  app_status_t t_AppStatus  = APP_OK;
  osSemaphoreAcquire(MPU_PID_semaHandle, osWaitForever);
  /* Infinite loop */
  for(;;)
  {
    t_AppStatus |= MPU_PID_task(&Main_MPU, &Omega_z, Car_Wanted_Angle);
    CAN_send_message(&Main_CAN, &msg_robot_Wz);
    osDelay(100);
  }
  /* USER CODE END MpuPIDtask */
}

/* USER CODE BEGIN Header_ULTRASONICtask */
/**
* @brief Function implementing the Ultrasonic_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ULTRASONICtask */
void ULTRASONICtask(void *argument)
{
  /* USER CODE BEGIN ULTRASONICtask */
  app_status_t t_AppStatus = ultrasonics_task_init(&Main_Ultrasonics);
  /* Infinite loop */
  for(;;)
  {
    t_AppStatus |= ultrasonics_update_task_X(&Main_Ultrasonics);
    osDelay(20);
    t_AppStatus |= ultrasonics_outlier_detect_X(&Main_Ultrasonics);

    t_AppStatus |= ultrasonics_update_task_Y(&Main_Ultrasonics);
    osDelay(20);
    t_AppStatus |= ultrasonics_outlier_detect_Y(&Main_Ultrasonics);

    t_AppStatus |= ultrasonics_update_task_U(&Main_Ultrasonics);
    osDelay(20);
    t_AppStatus |= ultrasonics_outlier_detect_U(&Main_Ultrasonics);

    t_AppStatus |= ultrasonics_update_task_V(&Main_Ultrasonics);
    osDelay(20);
    t_AppStatus |= ultrasonics_outlier_detect_V(&Main_Ultrasonics);


  }
  /* USER CODE END ULTRASONICtask */
}

/* USER CODE BEGIN Header_CONTROLtask */
/**
* @brief Function implementing the CONTROL_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CONTROLtask */
void CONTROLtask(void *argument)
{
  /* USER CODE BEGIN CONTROLtask */
  app_status_t t_AppStatus = APP_OK;
  t_AppStatus |= controller_add_callback(&Main_Controller, UP, Robot_Stop_PID_tunning);
  t_AppStatus |= controller_add_callback(&Main_Controller, DOWN, Robot_Start_PID_tunning);
  t_AppStatus |= controller_add_callback(&Main_Controller, LEFT, Increase_Set_Point_angle);
  t_AppStatus |= controller_add_callback(&Main_Controller, RIGHT, Increase_Kp);
  t_AppStatus |= controller_add_callback(&Main_Controller, SELECT, Increase_Ki);
  t_AppStatus |= controller_add_callback(&Main_Controller, START, Increase_Kd);
  t_AppStatus |= controller_add_callback(&Main_Controller, LEFT_STICK, Increase_N);
  t_AppStatus |= controller_add_callback(&Main_Controller, RITHT_STICK, Reset_PID);
  /* Infinite loop */
  for(;;)
  {
    t_AppStatus |= controller_wait_receive(&Main_Controller);
    osSemaphoreAcquire(CONTROL_semaHandle, osWaitForever);
    t_AppStatus |= controller_task(&Main_Controller);
  }
  /* USER CODE END CONTROLtask */
}

/* USER CODE BEGIN Header_TempPIDmove_Task */
/**
* @brief Function implementing the TempPIDmoveTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TempPIDmove_Task */
void TempPIDmove_Task(void *argument)
{
  /* USER CODE BEGIN TempPIDmove_Task */
  /* Infinite loop */
  for(;;)
  {
    osSemaphoreAcquire(PID_tune_semaHandle, osWaitForever);
    Car_Wanted_Speed = 0.2;
    Car_Wanted_direction = 0;
    CAN_send_message(&Main_CAN, &msg_robot_strafe);
    osDelay(1000);
  }
  /* USER CODE END TempPIDmove_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == CAN_INT_Pin)
  {
	 osSemaphoreRelease(CAN_semaHandle);
  }
  if (GPIO_Pin == MPU_INT_Pin)
  {
     osSemaphoreRelease(MPU_semaHandle);
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  osSemaphoreRelease(CONTROL_semaHandle);
}

float kp = 0;
float ki = 0;
float kd = 0;
float n = 0;
void Robot_Stop_PID_tunning(void *)
{
  Car_Wanted_Speed = 0;
  Car_Wanted_direction = 0;
  Car_Wanted_Angle = 0;
  PID_Init(&Main_MPU.PID, kp, ki, kd, n, 100, -100, 100);
  CAN_send_message(&Main_CAN, &msg_robot_stop);
}

void Robot_Start_PID_tunning(void *)
{
  osSemaphoreRelease(PID_tune_semaHandle);
}

void Increase_Set_Point_angle(void *)
{
  Car_Wanted_Angle += -10;
}

void Increase_Kp(void *)
{
  kp += -0.001;

  Main_MPU.PID.Kp = kp;
}

void Increase_Ki(void *)
{
  ki += -0.00000001;
  Main_MPU.PID.Ki = ki;
}

void Increase_Kd(void *)
{
  kd += 0.01;
  Main_MPU.PID.Kd = kd;
}

void Increase_N(void *)
{
  n += 0.01;
  Main_MPU.PID.N = n;
}

void Reset_PID(void *)
{
	kp = -0.025;
	ki= 0;
	kd= 0.03;
	n= 0;
  PID_Init(&Main_MPU.PID, kp, ki, kd, n, 100, -10, 10);
}
/* USER CODE END Application */

