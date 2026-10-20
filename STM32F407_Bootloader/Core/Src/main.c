/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Bootloader main program with CRC benchmark
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

#include "boot_validate.h"
#include "boot_jump.h"
#include "led.h"
#include "app_header.h"
#include "flash_layout.h"
#include "crc32.h"
#include "crc_benchmark.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/*
 * Set to 1U to stop in the Bootloader after printing benchmark results.
 * Set to 0U to jump automatically when validation and benchmark pass.
 */
#define CRC_BENCHMARK_HOLD_BEFORE_JUMP    1U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* Benchmark values can also be inspected in Live Expressions. */
volatile crc_benchmark_result_t g_crc_benchmark;

/* Result returned by validate_app(). */
volatile int g_app_status = -1;

/* Set to 1 after all three CRC implementations have been measured. */
volatile uint8_t g_crc_benchmark_finished = 0U;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* USER CODE BEGIN PFP */
//static void print_crc_benchmark(const crc_benchmark_result_t *result,
//                                uint32_t app_size);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief Redirect printf() output to SWV ITM stimulus port 0.
  * @param ch Character to transmit.
  * @retval Transmitted character.
  */
int __io_putchar(int ch)
{
    ITM_SendChar((uint32_t)ch);
    return ch;
}

/**
  * @brief Print the three CRC results and DWT cycle counts.
  * @param result Benchmark result structure.
  * @param app_size Number of Application bytes included in CRC calculation.
  */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */
    const app_header_t *app_header;
    /* USER CODE END 1 */

    /* MCU Configuration------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and SysTick. */
    HAL_Init();

    /* USER CODE BEGIN Init */
    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */
    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();

    /* USER CODE BEGIN 2 */

    /* Header is stored at APP_HEADER_START_ADDRESS. */
    app_header = app_header_get();

    /*
     * validate_app() must check the Header and Application before benchmark:
     * Magic Number, Size, selected CRC method, MSP and Reset Handler.
     */
    g_app_status = validate_app();

    if (APP_VALID == g_app_status)
    {
        /*
         * Run all three implementations regardless of CRC_SELECTED_ALGORITHM:
         * 1. Software bit-by-bit
         * 2. Software lookup table
         * 3. STM32F407 hardware CRC
         */
        crc_benchmark_run(
            (const uint8_t *)APP_START_ADDRESS,
            app_header->app_size,
            (crc_benchmark_result_t *)&g_crc_benchmark);

#if (CRC_BENCHMARK_HOLD_BEFORE_JUMP == 1U)
        /*
         * Stay here for benchmark inspection.
         * Set CRC_BENCHMARK_HOLD_BEFORE_JUMP to 0U for automatic jump.
         */
        while (1)
        {
            __NOP();
        }
#else
        /* Jump only when all three CRC implementations return the same CRC. */
        if (g_crc_benchmark.all_results_match != 0U)
        {
            jump_to_app();
        }
#endif
    }

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        /* Validation failed, CRC results mismatched, or jump returned. */
        led_toggle();
        HAL_Delay(LED_BLINK_PERIOD_MS);
        /* USER CODE END 3 */
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage. */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initialize the HSI oscillator. */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initialize CPU, AHB and APB clocks. */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
                                  RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* USER CODE BEGIN MX_GPIO_Init_1 */
    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

    /* Configure GPIO pin: PD14 */
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USER CODE BEGIN MX_GPIO_Init_2 */
    /* USER CODE END MX_GPIO_Init_2 */
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
    __disable_irq();

    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief Reports the source file name and source line number.
  * @param file Pointer to source file name.
  * @param line Source line number.
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    (void)file;
    (void)line;
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
