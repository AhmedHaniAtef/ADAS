/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../ECU_Layer/inc/ecu.h"
#include "ultrasonic.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
float dis_1,dis_2,dis_3,dis_4,dis_5,dis_6,dis_7,dis_8;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
UltrasonicSensor sensor_1={
		.Channel=TIM_CHANNEL_1,
		.TRIG_PORT=GPIOA,
		.TRIG_PIN=GPIO_PIN_7,
		.htim=&htim3,
		.Distance=&dis_1
};
UltrasonicSensor sensor_2={
		.Channel=TIM_CHANNEL_2,
		.TRIG_PORT=GPIOA,
		.TRIG_PIN=GPIO_PIN_7,
		.htim=&htim3,
		.Distance=&dis_2
};
UltrasonicSensor sensor_3={
		.Channel=TIM_CHANNEL_3,
		.TRIG_PORT=GPIOA,
		.TRIG_PIN=GPIO_PIN_7,
		.htim=&htim3,
		.Distance=&dis_3
};
UltrasonicSensor sensor_4={
		.Channel=TIM_CHANNEL_4,
		.TRIG_PORT=GPIOA,
		.TRIG_PIN=GPIO_PIN_7,
		.htim=&htim3,
		.Distance=&dis_4
};
UltrasonicSensor sensor_5={
		.Channel=TIM_CHANNEL_1,
		.TRIG_PORT=GPIOA,
		.TRIG_PIN=GPIO_PIN_7,
		.htim=&htim4,
		.Distance=&dis_5
};
UltrasonicSensor sensor_6={
		.Channel=TIM_CHANNEL_2,
		.TRIG_PORT=GPIOA,
		.TRIG_PIN=GPIO_PIN_7,
		.htim=&htim4,
		.Distance=&dis_6
};
UltrasonicSensor sensor_7={
		.Channel=TIM_CHANNEL_3,
		.TRIG_PORT=GPIOA,
		.TRIG_PIN=GPIO_PIN_7,
		.htim=&htim4,
		.Distance=&dis_7
};
UltrasonicSensor sensor_8={
		.Channel=TIM_CHANNEL_4,
		.TRIG_PORT=GPIOA,
		.TRIG_PIN=GPIO_PIN_7,
		.htim=&htim4,
		.Distance=&dis_8
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */


  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  Ultrasonic_Init(8,&sensor_1,&sensor_2,&sensor_3,&sensor_4,&sensor_5,&sensor_6,&sensor_7,&sensor_8);
  //HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  	  //Ultrasonic_ReadDistance(4,1,&sensor_1,&sensor_2,&sensor_3,&sensor_4);
	  	  //Ultrasonic_ReadDistance(4,5,&sensor_5,&sensor_6,&sensor_7,&sensor_8);
	  	  Ultrasonic_ReadDistance(8,1,&sensor_1,&sensor_2,&sensor_3,&sensor_4,&sensor_5,&sensor_6,&sensor_7,&sensor_8);
	  	  HAL_Delay(100);
	  	  


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
