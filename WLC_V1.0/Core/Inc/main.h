/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

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
#define PERIOD_LCC 47418
#define PERIOD_FSBB 27200
#define PERIOD_MASTERTIMER 13600
#define DeathTime_LCC 50
#define DeathTime_FSBB 30
#define ACDC_SW_Pin GPIO_PIN_13
#define ACDC_SW_GPIO_Port GPIOC
#define KEY_Pin GPIO_PIN_0
#define KEY_GPIO_Port GPIOC
#define KEY_EXTI_IRQn EXTI0_IRQn
#define ADC_FSBB_VIN_Pin GPIO_PIN_3
#define ADC_FSBB_VIN_GPIO_Port GPIOC
#define ADC_FSBB_IIN_Pin GPIO_PIN_0
#define ADC_FSBB_IIN_GPIO_Port GPIOA
#define ADC_FSBB_IOUT_Pin GPIO_PIN_1
#define ADC_FSBB_IOUT_GPIO_Port GPIOA
#define ADC_FSBB_VOUT_Pin GPIO_PIN_2
#define ADC_FSBB_VOUT_GPIO_Port GPIOA
#define ADC_COIL_SNS_Pin GPIO_PIN_7
#define ADC_COIL_SNS_GPIO_Port GPIOA
#define ADC_NTC_Pin GPIO_PIN_0
#define ADC_NTC_GPIO_Port GPIOB
#define FAN_Pin GPIO_PIN_2
#define FAN_GPIO_Port GPIOB
#define EN_12V_Pin GPIO_PIN_12
#define EN_12V_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_12
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOD
#define BUZZER_Pin GPIO_PIN_7
#define BUZZER_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
