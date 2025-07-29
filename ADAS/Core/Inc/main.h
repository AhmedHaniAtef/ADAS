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
#define UL_TRIGGER_Y_Pin GPIO_PIN_13
#define UL_TRIGGER_Y_GPIO_Port GPIOC
#define UL_TRIGGER_V_Pin GPIO_PIN_14
#define UL_TRIGGER_V_GPIO_Port GPIOC
#define UL_TRIGGER_U_Pin GPIO_PIN_1
#define UL_TRIGGER_U_GPIO_Port GPIOA
#define BT_TX_Pin GPIO_PIN_2
#define BT_TX_GPIO_Port GPIOA
#define BT_RX_Pin GPIO_PIN_3
#define BT_RX_GPIO_Port GPIOA
#define CAN_CS_Pin GPIO_PIN_4
#define CAN_CS_GPIO_Port GPIOA
#define CAN_SCK_Pin GPIO_PIN_5
#define CAN_SCK_GPIO_Port GPIOA
#define CAN_SO_Pin GPIO_PIN_6
#define CAN_SO_GPIO_Port GPIOA
#define CAN_SI_Pin GPIO_PIN_7
#define CAN_SI_GPIO_Port GPIOA
#define UL_90_Pin GPIO_PIN_0
#define UL_90_GPIO_Port GPIOB
#define UL_135_Pin GPIO_PIN_1
#define UL_135_GPIO_Port GPIOB
#define UL_TRIGGER_X_Pin GPIO_PIN_2
#define UL_TRIGGER_X_GPIO_Port GPIOB
#define MPU_SCL_Pin GPIO_PIN_10
#define MPU_SCL_GPIO_Port GPIOB
#define MPU_INT_Pin GPIO_PIN_12
#define MPU_INT_GPIO_Port GPIOB
#define MPU_INT_EXTI_IRQn EXTI15_10_IRQn
#define CAN_INT_Pin GPIO_PIN_13
#define CAN_INT_GPIO_Port GPIOB
#define CAN_INT_EXTI_IRQn EXTI15_10_IRQn
#define MPU_RESET_Pin GPIO_PIN_8
#define MPU_RESET_GPIO_Port GPIOA
#define ESP32_TX_Pin GPIO_PIN_9
#define ESP32_TX_GPIO_Port GPIOA
#define ESP32_RX_Pin GPIO_PIN_10
#define ESP32_RX_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_11
#define BUZZER_GPIO_Port GPIOA
#define LED_LEFT_Pin GPIO_PIN_12
#define LED_LEFT_GPIO_Port GPIOA
#define LED_RIGHT_Pin GPIO_PIN_15
#define LED_RIGHT_GPIO_Port GPIOA
#define MPU_SDA_Pin GPIO_PIN_3
#define MPU_SDA_GPIO_Port GPIOB
#define UL_0_Pin GPIO_PIN_4
#define UL_0_GPIO_Port GPIOB
#define UL_45_Pin GPIO_PIN_5
#define UL_45_GPIO_Port GPIOB
#define UL_180_Pin GPIO_PIN_6
#define UL_180_GPIO_Port GPIOB
#define UL_225_Pin GPIO_PIN_7
#define UL_225_GPIO_Port GPIOB
#define UL_270_Pin GPIO_PIN_8
#define UL_270_GPIO_Port GPIOB
#define UL_315_Pin GPIO_PIN_9
#define UL_315_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
