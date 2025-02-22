/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PKA/PKA_ModularExponentiation_IT/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to configure and use PKA through
  *          the STM32WBxx HAL API.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the 
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
PKA_HandleTypeDef hpka;

/* USER CODE BEGIN PV */
PKA_ModExpInTypeDef in = {0};
uint8_t decipheredBuffer[256];
uint8_t cipheredBuffer[256];
__IO uint32_t operationComplete = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_PKA_Init(void);
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
  /* STM32WBxx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Configure LED2 */
  BSP_LED_Init(LED2);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_PKA_Init();
  /* USER CODE BEGIN 2 */
  /* Set input parameters */
  in.expSize = rsa_pub_2048_publicExponent_len;
  in.OpSize  = rsa_pub_2048_modulus_len;
  in.pOp1    = plaintext_bin;
  in.pExp    = rsa_pub_2048_publicExponent;
  in.pMod    = rsa_pub_2048_modulus;
  
  /* Start the modular exponentiation (ciphering step) */
  if(HAL_PKA_ModExp_IT(&hpka, &in) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Wait until the interrupt is triggered */
  while(operationComplete == 0);
  operationComplete = 0;
  
  /* Copy the results to user specified space */
  HAL_PKA_ModExp_GetResult(&hpka , cipheredBuffer);
  
  /* (Facultative) Compare the computed and expected result */
  if (memcmp(cipheredBuffer, ciphertext_bin, ciphertext_bin_len) != 0)
  {
    Error_Handler();
  }
  
  /* Set input parameters */
  in.expSize = rsa_priv_2048_privateExponent_len;
  in.OpSize  = rsa_pub_2048_modulus_len;
  in.pOp1    = ciphertext_bin;
  in.pExp    = rsa_priv_2048_privateExponent;
  in.pMod    = rsa_priv_2048_modulus;

  /* Start the modular exponentiation (deciphering step) */
  if(HAL_PKA_ModExp_IT(&hpka, &in) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Wait until the interrupt is triggered */
  while(operationComplete == 0);
  
  /* Copy the results to user specified space */
  HAL_PKA_ModExp_GetResult(&hpka , decipheredBuffer);

  /* (Facultative) Compare the computed and expected result */
  if (memcmp(decipheredBuffer, plaintext_bin, plaintext_bin_len) != 0)
  {
    Error_Handler();
  }
  
  /* Deinitialize the PKA */
  if(HAL_PKA_DeInit(&hpka) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Success */
  operationComplete = 3;
  BSP_LED_On(LED2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 32;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV5;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK4|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.AHBCLK4Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief PKA Initialization Function
  * @param None
  * @retval None
  */
static void MX_PKA_Init(void)
{

  /* USER CODE BEGIN PKA_Init 0 */

  /* USER CODE END PKA_Init 0 */

  /* USER CODE BEGIN PKA_Init 1 */

  /* USER CODE END PKA_Init 1 */
  hpka.Instance = PKA;
  if (HAL_PKA_Init(&hpka) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN PKA_Init 2 */

  /* USER CODE END PKA_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Process completed callback.
  * @param  hpka PKA handle
  * @retval None
  */
void HAL_PKA_OperationCpltCallback(PKA_HandleTypeDef *hpka)
{
  operationComplete = 1;
}

/**
  * @brief  Error callback.
  * @param  hpka PKA handle
  * @retval None
  */
void HAL_PKA_ErrorCallback(PKA_HandleTypeDef *hpka)
{
  Error_Handler();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  operationComplete = 2;
  while (1)
  {
    /* Error if LED2 is slowly blinking (1 sec. period) */
    BSP_LED_Toggle(LED2); 
    HAL_Delay(1000);   
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
