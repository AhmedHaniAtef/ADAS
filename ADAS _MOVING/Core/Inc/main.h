/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CAN_INT_Pin GPIO_PIN_13
#define CAN_INT_GPIO_Port GPIOC
#define CAN_INT_EXTI_IRQn EXTI15_10_IRQn
#define FL_PWM_Pin GPIO_PIN_1
#define FL_PWM_GPIO_Port GPIOA
#define FR_PWM_Pin GPIO_PIN_2
#define FR_PWM_GPIO_Port GPIOA
#define RL_PWM_Pin GPIO_PIN_3
#define RL_PWM_GPIO_Port GPIOA
#define RL_2_Pin GPIO_PIN_4
#define RL_2_GPIO_Port GPIOA
#define RL_1_Pin GPIO_PIN_5
#define RL_1_GPIO_Port GPIOA
#define RL_EN_1_Pin GPIO_PIN_6
#define RL_EN_1_GPIO_Port GPIOA
#define RL_EN_2_Pin GPIO_PIN_7
#define RL_EN_2_GPIO_Port GPIOA
#define RR_2_Pin GPIO_PIN_0
#define RR_2_GPIO_Port GPIOB
#define RR_1_Pin GPIO_PIN_1
#define RR_1_GPIO_Port GPIOB
#define CAN_CS_Pin GPIO_PIN_12
#define CAN_CS_GPIO_Port GPIOB
#define FL_EN_1_Pin GPIO_PIN_8
#define FL_EN_1_GPIO_Port GPIOA
#define FL_EN_2_Pin GPIO_PIN_9
#define FL_EN_2_GPIO_Port GPIOA
#define FL_2_Pin GPIO_PIN_11
#define FL_2_GPIO_Port GPIOA
#define FL_1_Pin GPIO_PIN_12
#define FL_1_GPIO_Port GPIOA
#define FR_EN_1_Pin GPIO_PIN_15
#define FR_EN_1_GPIO_Port GPIOA
#define FR_EN_2_Pin GPIO_PIN_3
#define FR_EN_2_GPIO_Port GPIOB
#define FR_2_Pin GPIO_PIN_4
#define FR_2_GPIO_Port GPIOB
#define FR_1_Pin GPIO_PIN_5
#define FR_1_GPIO_Port GPIOB
#define RR_EN_1_Pin GPIO_PIN_6
#define RR_EN_1_GPIO_Port GPIOB
#define RR_EN_2_Pin GPIO_PIN_7
#define RR_EN_2_GPIO_Port GPIOB
#define RR_PWM_Pin GPIO_PIN_9
#define RR_PWM_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
