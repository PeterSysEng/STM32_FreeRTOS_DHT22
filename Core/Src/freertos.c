/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_uart.h"
#include "task.h"
#include "cmsis_os.h"
#include "tim.h"
#include "usart.h"
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct {
    int temperature;
    int humidity;
} DHT_Data_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

extern TIM_HandleTypeDef htim2;
 
/* USER CODE END Variables */
/* Definitions for SensorTask */
osThreadId_t SensorTaskHandle;
const osThreadAttr_t SensorTask_attributes = {
  .name = "SensorTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UartTask */
osThreadId_t UartTaskHandle;
const osThreadAttr_t UartTask_attributes = {
  .name = "UartTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for dhtQueue */
osMessageQueueId_t dhtQueueHandle;
const osMessageQueueAttr_t dhtQueue_attributes = {
  .name = "dhtQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void Set_Pin_Output(void);
void Set_Pin_Input(void);
uint8_t Read_DHT22(int *Temperature, int *Humidity);
void delay_us(uint16_t us);

/* USER CODE END FunctionPrototypes */

void StartSensorTask(void *argument);
void StartUartTask(void *argument);

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

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of dhtQueue */
  dhtQueueHandle = osMessageQueueNew (16, sizeof(DHT_Data_t), &dhtQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of SensorTask */
  SensorTaskHandle = osThreadNew(StartSensorTask, NULL, &SensorTask_attributes);

  /* creation of UartTask */
  UartTaskHandle = osThreadNew(StartUartTask, NULL, &UartTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartSensorTask */
/**
  * @brief  Function implementing the SensorTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartSensorTask */
void StartSensorTask(void *argument)
{
  /* USER CODE BEGIN StartSensorTask */
  /* Infinite loop */
  
    DHT_Data_t data;

    for(;;)
    {
        // Start measurement
        if (Read_DHT22(&data.temperature, &data.humidity) ){
            // Send to queue
            osMessageQueuePut(dhtQueueHandle, &data, 0, 0);

        }

        osDelay(3000);
    }

  /* USER CODE END StartSensorTask */
}

/* USER CODE BEGIN Header_StartUartTask */
/**
* @brief Function implementing the UartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUartTask */
void StartUartTask(void *argument)
{
  /* USER CODE BEGIN StartUartTask */
  /* Infinite loop */
  
    DHT_Data_t received;
    char buf[64];

    for(;;)
    {
        if (osMessageQueueGet(dhtQueueHandle, &received, NULL, osWaitForever) == osOK){
            int temp_int = received.temperature / 10;
            int temp_dec = received.temperature % 10;
            
            int hum_int = received.humidity / 10;
            int hum_dec = received.humidity % 10;

            int len = sprintf(buf, "Temp: %d.%d C, Hum: %d.%d %%\r\n", temp_int, temp_dec, hum_int, hum_dec);
            HAL_UART_Transmit(&huart2, (uint8_t*)buf, len, 100);
        }

        osDelay(1);
    }
  /* USER CODE END StartUartTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


void Set_Pin_Output(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // Push-pull output
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
    
void Set_Pin_Input(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;     // Input
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

uint8_t Read_DHT22(int *Temperature, int *Humidity) {
    uint8_t data[5] = {0,0,0,0,0};
    uint8_t i, j;

    vTaskSuspendAll(); // FreeRTOS stop for precise timing

    // 1. START signal
    Set_Pin_Output();
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 0); // Pull down to GND
    delay_us(18000);                         // 18ms waiting
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 1); // Pull up to VDD
    delay_us(30);                            // 30us waiting
    Set_Pin_Input();                         // Switch to input

    // 2. Waiting for the response
    // There is no error handler here
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0));    // Waiting for the LOW
    while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0))); // Waiting for the HIGH
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0));    // Waiting for the LOW (start of data)

    // 3. 40 BIT reading
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 8; j++) {
            while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0))); // Waiting for the HIGH
            delay_us(40); // If it is still hight after 40us, then it is a "1" bit
            if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0))) {
                data[i] &= ~(1 << (7 - j)); // "0" bit
            } else {
                data[i] |= (1 << (7 - j));  // "1" bit
                while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0)); // Waiting for the end of the nex bit 
            }
        }
    }

    xTaskResumeAll(); // FreeRTOS continue

    // 4.DATA CONVERSION (without checksum verification) 
    *Humidity = (int)((data[0] << 8) | data[1]);
    *Temperature = (int)((data[2] << 8) | data[3]);

    return 1; // Success!
}


void delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim2, 0);            // Reset counter
    HAL_TIM_Base_Start(&htim2);                  // Timer start
    while (__HAL_TIM_GET_COUNTER(&htim2) < us);  // Waiting
    HAL_TIM_Base_Stop(&htim2);                   // Timer stop
}


/* USER CODE END Application */




