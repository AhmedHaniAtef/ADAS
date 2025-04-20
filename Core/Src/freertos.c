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
// QMC5883L_t qmc =
// {
//   .UsedI2C = &hi2c2,
//   .DRDY = DRDY_INT_DISABLE, 
//   .FullScale = RNG_8G,
//   .Mode = CONTINUOUS,
//   .OutputDataRate = ODR_200,
//   .OverSampleRatio = OSR_512,
// };
// float_t angle = 0;
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
  .priority = (osPriority_t) osPriorityHigh5,
};
/* Definitions for CAN_task */
osThreadId_t CAN_taskHandle;
const osThreadAttr_t CAN_task_attributes = {
  .name = "CAN_task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh5,
};
/* Definitions for MONITORING_task */
osThreadId_t MONITORING_taskHandle;
const osThreadAttr_t MONITORING_task_attributes = {
  .name = "MONITORING_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh4,
};
/* Definitions for Ultrasonic_task */
osThreadId_t Ultrasonic_taskHandle;
const osThreadAttr_t Ultrasonic_task_attributes = {
  .name = "Ultrasonic_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for CONTROL_task */
osThreadId_t CONTROL_taskHandle;
const osThreadAttr_t CONTROL_task_attributes = {
  .name = "CONTROL_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh7,
};
/* Definitions for YawKalman_task */
osThreadId_t YawKalman_taskHandle;
const osThreadAttr_t YawKalman_task_attributes = {
  .name = "YawKalman_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh2,
};
/* Definitions for YawPID_task */
osThreadId_t YawPID_taskHandle;
const osThreadAttr_t YawPID_task_attributes = {
  .name = "YawPID_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh5,
};
/* Definitions for ACC_Task */
osThreadId_t ACC_TaskHandle;
const osThreadAttr_t ACC_Task_attributes = {
  .name = "ACC_Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for TSR_Task */
osThreadId_t TSR_TaskHandle;
const osThreadAttr_t TSR_Task_attributes = {
  .name = "TSR_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh4,
};
/* Definitions for Timer_test */
osTimerId_t Timer_testHandle;
const osTimerAttr_t Timer_test_attributes = {
  .name = "Timer_test"
};
/* Definitions for MPU_sema */
osSemaphoreId_t MPU_semaHandle;
const osSemaphoreAttr_t MPU_sema_attributes = {
  .name = "MPU_sema"
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
/* Definitions for KalmanReady_sema */
osSemaphoreId_t KalmanReady_semaHandle;
const osSemaphoreAttr_t KalmanReady_sema_attributes = {
  .name = "KalmanReady_sema"
};
/* Definitions for YawPIDReady_sema */
osSemaphoreId_t YawPIDReady_semaHandle;
const osSemaphoreAttr_t YawPIDReady_sema_attributes = {
  .name = "YawPIDReady_sema"
};
/* Definitions for CAN_sema */
osSemaphoreId_t CAN_semaHandle;
const osSemaphoreAttr_t CAN_sema_attributes = {
  .name = "CAN_sema"
};
/* Definitions for Orientation_sema */
osSemaphoreId_t Orientation_semaHandle;
const osSemaphoreAttr_t Orientation_sema_attributes = {
  .name = "Orientation_sema"
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

void Yaw_PID_suspend(void);
void Yaw_PID_resume(void);
/* USER CODE END FunctionPrototypes */

void MPUtask(void *argument);
void CANtask(void *argument);
void MONITORINGtask(void *argument);
void ULTRASONICtask(void *argument);
void CONTROLtask(void *argument);
void YawKalmanTask(void *argument);
void YawPIDtask(void *argument);
void ACCtask(void *argument);
void TSRtask(void *argument);
void callback_test(void *argument);

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

  /* creation of CONTROL_sema */
  CONTROL_semaHandle = osSemaphoreNew(1, 0, &CONTROL_sema_attributes);

  /* creation of PID_tune_sema */
  PID_tune_semaHandle = osSemaphoreNew(1, 0, &PID_tune_sema_attributes);

  /* creation of KalmanReady_sema */
  KalmanReady_semaHandle = osSemaphoreNew(1, 0, &KalmanReady_sema_attributes);

  /* creation of YawPIDReady_sema */
  YawPIDReady_semaHandle = osSemaphoreNew(1, 0, &YawPIDReady_sema_attributes);

  /* creation of CAN_sema */
  CAN_semaHandle = osSemaphoreNew(2, 0, &CAN_sema_attributes);

  /* creation of Orientation_sema */
  Orientation_semaHandle = osSemaphoreNew(2, 0, &Orientation_sema_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of Timer_test */
  Timer_testHandle = osTimerNew(callback_test, osTimerPeriodic, NULL, &Timer_test_attributes);

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

  /* creation of Ultrasonic_task */
  Ultrasonic_taskHandle = osThreadNew(ULTRASONICtask, NULL, &Ultrasonic_task_attributes);

  /* creation of CONTROL_task */
  CONTROL_taskHandle = osThreadNew(CONTROLtask, NULL, &CONTROL_task_attributes);

  /* creation of YawKalman_task */
  YawKalman_taskHandle = osThreadNew(YawKalmanTask, NULL, &YawKalman_task_attributes);

  /* creation of YawPID_task */
  YawPID_taskHandle = osThreadNew(YawPIDtask, NULL, &YawPID_task_attributes);

  /* creation of ACC_Task */
  ACC_TaskHandle = osThreadNew(ACCtask, NULL, &ACC_Task_attributes);

  /* creation of TSR_Task */
  TSR_TaskHandle = osThreadNew(TSRtask, NULL, &TSR_Task_attributes);

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
  osDelay(100);
  app_status_t t_AppStatus = MPU_task_init(&Main_MPU);
  osSemaphoreAcquire(Orientation_semaHandle, osWaitForever);
  for (uint32_t i = 0; i < 1000; i++)
  {
    osSemaphoreAcquire(MPU_semaHandle, osWaitForever);
    t_AppStatus |= MPU_update_task(&Main_MPU);
    MpuGz = Main_MPU.mpu->Gz;
    t_AppStatus |= Orientation_Gyro_calibration(&Main_Orientation);
  }
  Main_Orientation.GyroBias /= 1000.0f;
  osSemaphoreRelease(KalmanReady_semaHandle);
  osSemaphoreRelease(YawPIDReady_semaHandle);
  /* Infinite loop */
  for(;;)
  {
    osSemaphoreAcquire(MPU_semaHandle, osWaitForever);
    t_AppStatus |= MPU_update_task(&Main_MPU);
    MpuGz = Main_MPU.mpu->Gz;
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
  t_AppStatus |= messages_init();
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
  t_AppStatus |= controller_get_yaw_control_init(&Main_Controller, Yaw_PID_suspend);
  t_AppStatus |= controller_give_yaw_control_init(&Main_Controller, Yaw_PID_resume);
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

/* USER CODE BEGIN Header_YawKalmanTask */
/**
* @brief Function implementing the YawKalman_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_YawKalmanTask */
void YawKalmanTask(void *argument)
{
  /* USER CODE BEGIN YawKalmanTask */
  app_status_t t_AppStatus = Orientation_task_init(&Main_Orientation);
  osSemaphoreRelease(Orientation_semaHandle);
  osSemaphoreAcquire(KalmanReady_semaHandle, osWaitForever);
  /* Infinite loop */
  for(;;)
  {
    t_AppStatus |= Orientation_Kf_task(&Main_Orientation);
    osDelay(1);
  }
  /* USER CODE END YawKalmanTask */
}

/* USER CODE BEGIN Header_YawPIDtask */
/**
* @brief Function implementing the YawPID_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_YawPIDtask */
void YawPIDtask(void *argument)
{
  /* USER CODE BEGIN YawPIDtask */
  app_status_t t_AppStatus = APP_OK;
  osSemaphoreAcquire(YawPIDReady_semaHandle, osWaitForever);
  float_t Omega_z_flag = 0;
  /* Infinite loop */
  for(;;)
  {
	Omega_z_flag = Omega_z;
    t_AppStatus |= Orientation_PID_task(&Main_Orientation, &Omega_z, Car_Wanted_Angle);
    if (Omega_z_flag != Omega_z)
    	t_AppStatus |= CAN_send_message(&Main_CAN, &msg_robot_Wz);
    osDelay(100);
  }
  /* USER CODE END YawPIDtask */
}

/* USER CODE BEGIN Header_ACCtask */
/**
* @brief Function implementing the ACC_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ACCtask */
void ACCtask(void *argument)
{
  /* USER CODE BEGIN ACCtask */
  app_status_t t_AppStatus = ACC_task_init(&ACC_Object);
  /* Infinite loop */
  for(;;)
  {
    t_AppStatus |= ACC_relative_task(&ACC_Object);
    osDelay(100);
  }
  /* USER CODE END ACCtask */
}

/* USER CODE BEGIN Header_TSRtask */
/**
* @brief Function implementing the TSR_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TSRtask */
void TSRtask(void *argument)
{
  /* USER CODE BEGIN TSRtask */
  app_status_t t_AppStatus = APP_OK;
  /* Infinite loop */
  for(;;)
  {
    t_AppStatus = TSR_task();
    osDelay(500);
  }
  /* USER CODE END TSRtask */
}

/* callback_test function */
void callback_test(void *argument)
{
  /* USER CODE BEGIN callback_test */

  /* USER CODE END callback_test */
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
float times = 1;
int flag_times = 0;
void Robot_Stop_PID_tunning(void *)
{
  Car_Wanted_Speed = 0;
  Car_Wanted_direction = 0;
  Car_Wanted_Angle = 0;
  CAN_send_message(&Main_CAN, &msg_robot_stop);
}

void Robot_Start_PID_tunning(void *)
{
  Car_Wanted_Speed = 0.45f;
  CAN_send_message(&Main_CAN, &msg_robot_strafe);
}

void Increase_Set_Point_angle(void *)
{
  if (flag_times == 0)
    times = 10;
  else if (flag_times == 1)
    times = -10;
  else if (flag_times == 2)
    times = 1;
  else if (flag_times == 3)
    times = -1;

  flag_times++;
  if (flag_times >= 4)
    flag_times = 0;
}

void Increase_Kp(void *)
{
  kp += 0.1 * times;
  ACC_Object.PID_Vx.Kp = kp;
}

void Increase_Ki(void *)
{
  ki += 0.0001 * times;
  ACC_Object.PID_Vx.Ki = ki;
}

void Increase_Kd(void *)
{
  kd += 0.1 * times;
  ACC_Object.PID_Vx.Kd = kd;
}

void Increase_N(void *)
{
  n += 0.1 * times;
  ACC_Object.PID_Vx.N = n;
}

void Reset_PID(void *)
{
  PID_Init(&ACC_Object.PID_Vx, Vx_Kp, Vx_Ki, Vx_Kd, Vx_N, 100, Vx_MIN_OUT, Vx_MAX_OUT);
}

void Yaw_PID_suspend(void)
{
  osThreadSuspend(YawPID_taskHandle);
}

void Yaw_PID_resume(void)
{
  osThreadResume(YawPID_taskHandle);
}

/* USER CODE END Application */

