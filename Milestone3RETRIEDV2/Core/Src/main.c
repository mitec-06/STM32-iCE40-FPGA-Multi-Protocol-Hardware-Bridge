/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
void UART_SendAndReadData(USART_TypeDef *USARTX, const uint8_t *data, size_t length);
void I2C_Bus_Recovery();

typedef enum {
  ERROR_TIMEOUT,
  ERROR_NACK,
  ERROR_BUS_STUCK,
  I2C_SUCCESS
  } I2C_Status;

  I2C_Status I2C_Wait_With_Timeout(uint32_t, uint32_t);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
#define I2C_TIMEOUT_CYCLES 144000
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
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  

  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  DWT -> CYCCNT = 0;
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */
  const char *msg = "System booting..\n";
  UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  if (!LL_I2C_IsEnabled(I2C1)){
      LL_I2C_Enable(I2C1);
    }
    
  while (1)
  {
    /* USER CODE END WHILE */
    
    uint32_t busy_timeout = DWT->CYCCNT;
    while (LL_I2C_IsActiveFlag_BUSY(I2C1)){
      if ((DWT->CYCCNT - busy_timeout) >= I2C_TIMEOUT_CYCLES){
        msg = "ERROR: Bus stuck BUSY upon entry, trying to free it...\n";
        UART_SendAndReadData(USART2, (const uint8_t * )msg, strlen(msg));
        I2C_Bus_Recovery();
        break;
      } 
    }

    LL_I2C_GenerateStartCondition(I2C1);

    
    if (I2C_Wait_With_Timeout(LL_I2C_SR1_SB, I2C_TIMEOUT_CYCLES) != I2C_SUCCESS){
      msg = "ERROR: START CONDITION Timeout! \n";
      UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));
      I2C_Bus_Recovery();
      LL_mDelay(50); // retry loop...
      continue;
    } // already checking...

    

    msg = "Start Condition Detected..\n";
    UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));

    volatile uint8_t shifted = 0x48 << 1;
  LL_I2C_TransmitData8(I2C1, shifted);
  shifted = 0x00;

  uint32_t add_start = DWT -> CYCCNT;
  bool add_ok = false;

  while ((DWT->CYCCNT - add_start) < I2C_TIMEOUT_CYCLES){
  if (LL_I2C_IsActiveFlag_AF(I2C1)){
        msg = "ERROR: NACK not detected...";
        UART_SendAndReadData(USART2, (const uint8_t * )msg, strlen(msg));
        LL_I2C_ClearFlag_AF(I2C1);
        LL_I2C_GenerateStopCondition(I2C1);
        break;
      } 
      if (LL_I2C_IsActiveFlag_ADDR(I2C1)){
        LL_I2C_ClearFlag_ADDR(I2C1);
        add_ok = true;
        break;
      }
    }

    if (!add_ok){
      msg = "ERROR: Address Timeout or NACK, recovering..\n";
      UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));
      I2C_Bus_Recovery();
      LL_mDelay(50);
      continue;
    }

    msg = "Address Detected!\n";
    UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));

    LL_I2C_TransmitData8(I2C1, 0x69);

    if (I2C_Wait_With_Timeout(LL_I2C_SR1_BTF, I2C_TIMEOUT_CYCLES) != I2C_SUCCESS){
      msg = "ERROR: First Byte Transmission Failed (BTF Timeout).\n";
      UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));
      I2C_Bus_Recovery();
      LL_mDelay(50);
      continue;
    }

    msg = "1st Message Sent!...\n";
    UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));



    LL_I2C_TransmitData8(I2C1, 0x00);

    if (I2C_Wait_With_Timeout(LL_I2C_SR1_BTF, I2C_TIMEOUT_CYCLES) != I2C_SUCCESS){
      msg = "ERROR: Second Byte Transmission Failed (BTF Timeout).\n";
      UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));
      I2C_Bus_Recovery();
      LL_mDelay(50);
      continue;
    }

    msg = "2nd Message Sent!...\n";
    UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));





  
  // do nothing while busy
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  
  // normally SDA and SCL lines are highest here
  
  
  // unless start bit is active, program wont proceed
  
  // program transmission

  /*
  while (!LL_I2C_IsActiveFlag_ADDR(I2C1) && !LL_I2C_IsActiveFlag_AF(I2C1)){

  }
  // waits until either condition is reached

  // failed condition, no address detected  
  if (LL_I2C_IsActiveFlag_AF(I2C1)){
     msg = "ERROR: NACK: Address not detected...\n";
    UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));
      LL_I2C_GenerateStopCondition(I2C1);
     // while(LL_I2C_IsActiveFlag_STOP(I2C1));
      LL_I2C_ClearFlag_AF(I2C1);
      // LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
      break;
    } else {
       msg = "Address Detected!..\n";
       UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));
      // an address was detected! 
      volatile uint32_t temp;
      temp = I2C1 -> SR1;
      temp = I2C1 -> SR2;
      (void)temp;
      */ /*
      LL_I2C_ClearFlag_ADDR(I2C1);

      LL_I2C_TransmitData8(I2C1, shifted);
      while(!LL_I2C_IsActiveFlag_BTF(I2C1)){}

      LL_I2C_TransmitData8(I2C1, 0x69);

      while(!LL_I2C_IsActiveFlag_BTF(I2C1)){}

      if (I2C_Wait_With_Timeout(LL_I2C_SR1_TXE, 14400) != I2C_SUCCESS){
        msg = "ERROR: Second Byte Transmission Failed. \n";
        UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));
        LL_I2C_GenerateStopCondition(I2C1);
        LL_mDelay(50);
      }else {
        msg = "1st Data Sent!..\n";
      UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));
      }

      shifted = 0x00;

      LL_I2C_TransmitData8(I2C1, shifted);
      

      while(!LL_I2C_IsActiveFlag_BTF(I2C1)){}

      if (I2C_Wait_With_Timeout(LL_I2C_SR1_TXE, 14400) != I2C_SUCCESS){
        msg = "ERROR : Second Byte Transmission Failed. \n";
        UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));
        LL_I2C_GenerateStopCondition(I2C1);
        LL_mDelay(50);
      }else {
        msg = "2nd Data Sent!..\n";
      UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));
      }
      

      LL_I2C_GenerateStopCondition(I2C1);
      // while(!LL_I2C_IsActiveFlag_STOP(I2C1));

      while(LL_I2C_IsActiveFlag_BUSY(I2C1)){}
      // LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
    }
    */

    LL_I2C_GenerateStopCondition(I2C1);
    while(LL_I2C_IsActiveFlag_BUSY(I2C1)){}
    
    /* USER CODE BEGIN 3 */
    msg = "Stop Condition Generated..\n";
    UART_SendAndReadData(USART2, (const uint8_t *)msg, strlen(msg));

    // LL_I2C_GenerateStopCondition(I2C1);

    LL_mDelay(500);
  }
  /* USER CODE END 3 */
}

void UART_SendAndReadData(USART_TypeDef *USARTX, const uint8_t *data, size_t length){

  for (size_t i = 0; i < length; i++){
   while (!LL_USART_IsActiveFlag_TXE(USARTX));
   LL_USART_TransmitData8(USARTX, data[i]);
   //while(!LL_USART_IsActiveFlag_RXNE(USART1)){}
  //  LL_USART_ReceiveData8(USART1);
  }
  while (!LL_USART_IsActiveFlag_TC(USARTX)){}
  //while(!LL_USART_IsActiveFlag_RXNE(USART1)){}
  //LL_USART_ReceiveData8(USART1);
}

I2C_Status I2C_Wait_With_Timeout(uint32_t Flag, uint32_t TimeoutCycles){
  uint32_t start_cycle = DWT-> CYCCNT;

  while ((I2C1 ->SR1 & Flag)==0){

    
    if ((DWT->CYCCNT) - start_cycle >= TimeoutCycles){
      return ERROR_TIMEOUT;
    }

    else if (LL_I2C_IsActiveFlag_AF(I2C1)){
      LL_I2C_ClearFlag_AF(I2C1);

     // LL_I2C_IsActiveFlag_AF(I2C1);

      return ERROR_NACK;
    }
  }

  return I2C_SUCCESS;
}

void I2C_Bus_Recovery(){
  LL_I2C_Disable(I2C1); // disables the I2C peripheral from running

      LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_OPENDRAIN); // sets SCL as open_drain
      LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT); // SCL becomes the output while SDA becomes the input
      LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_INPUT);

      // we essentially need to toggle SCL 9 times as there are 8 bits in a byte + the one bit for ACK
      // since the SCL line is left high if the master glitches (for whatever reason), the SDA is clamped to 0 until it detects 
      // the SCL line moving up or down 
      // Ultimately, this is done until the SDA line is detected to be logic 1, as that would signify the SCL bit that held it down
      // would have been released, thus recovering the bus.
      for (int i = 0; i < 9; i++){
        LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
        LL_mDelay(5);
        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
        LL_mDelay(5);
        if (LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_7)){
          break;
        }
      }
      // In the above for loop, the SDA line is set as an input so that it can be monitored, below its turned into an output
      // so that we can create a STOP condition for this byte, hence allowing both SDA and SCL lines to be reset.

      LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_OPENDRAIN);
      LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);

      LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6); // SCL LOW
      LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7); // SDA LOW
      
      LL_mDelay(1);
      LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6); // SCL HIGH
      LL_mDelay(1);
      LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_7); // SDA HIGH
      LL_mDelay(5);

      // LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_OPENDRAIN);
      LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_OPENDRAIN);
      
      // configuring back SCL and SDA lines to their previous arrangements...
      LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
      LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);

      LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_6, LL_GPIO_AF_4);
      LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_7, LL_GPIO_AF_4);

      LL_I2C_EnableReset(I2C1);
      LL_I2C_DisableReset(I2C1);

      MX_I2C1_Init();
      LL_I2C_Enable(I2C1);
}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_16, 336, LL_RCC_PLLP_DIV_4);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(84000000);
  LL_SetSystemCoreClock(84000000);
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**I2C1 GPIO Configuration
  PB8   ------> I2C1_SCL
  PB9   ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8|LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */

  /** I2C Initialization
  */
  LL_I2C_DisableOwnAddress2(I2C1);
  LL_I2C_DisableGeneralCall(I2C1);
  LL_I2C_EnableClockStretching(I2C1);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.ClockSpeed = 100000;
  I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C1, 0);
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**USART2 GPIO Configuration
  PA2   ------> USART2_TX
  PA3   ------> USART2_RX
  */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART2, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART2);
  LL_USART_Enable(USART2);
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(LD2_GPIO_Port, LD2_Pin);

  /**/
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_13;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(B1_GPIO_Port, B1_Pin, LL_GPIO_PULL_NO);

  /**/
  LL_GPIO_SetPinMode(B1_GPIO_Port, B1_Pin, LL_GPIO_MODE_INPUT);

  /**/
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

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
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
