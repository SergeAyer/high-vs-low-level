// direct access to low level STM32H7XX
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"

/* Common Error codes */
#define BSP_ERROR_NONE                    0

/* Definitions for leds */
#define LED1_GPIO_PORT                   GPIOI
#define LED1_PIN                         GPIO_PIN_12

#define LED2_GPIO_PORT                   GPIOI
#define LED2_PIN                         GPIO_PIN_13

#define LED3_GPIO_PORT                   GPIOI
#define LED3_PIN                         GPIO_PIN_14

#define LED4_GPIO_PORT                   GPIOI
#define LED4_PIN                         GPIO_PIN_15

#define LEDx_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOI_CLK_ENABLE()

typedef enum
{
  LED1 = 0U,
  LED_GREEN = LED1,
  LED2 = 1U,
  LED_ORANGE = LED2,
  LED3 = 2U,
  LED_RED = LED3,
  LED4 = 3U,
  LED_BLUE = LED4,
  LEDn
} Led_TypeDef;

static GPIO_TypeDef* LED_PORT[LEDn] = { LED1_GPIO_PORT,
                                        LED2_GPIO_PORT,
                                        LED3_GPIO_PORT,
                                        LED4_GPIO_PORT};

static const uint32_t LED_PIN[LEDn] = { LED1_PIN,
                                        LED2_PIN,
                                        LED3_PIN,
                                        LED4_PIN};
int32_t BSP_LED_Init(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable the GPIO_LED clock */
  LEDx_GPIO_CLK_ENABLE();

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = LED_PIN[Led];
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_Init(LED_PORT[Led], &GPIO_InitStruct);

  /* By default, turn off LED */
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
 return ret;
}

/**
  * @brief  Turns selected LED On.
  * @param  Led LED to be set on
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval BSP status
  */
int32_t  BSP_LED_On(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;

  HAL_GPIO_WritePin (LED_PORT [Led], (uint16_t)LED_PIN [Led], GPIO_PIN_RESET);
  return ret;
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led LED to be set off
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval BSP status
  */
int32_t  BSP_LED_Off(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  HAL_GPIO_WritePin (LED_PORT [Led], (uint16_t)LED_PIN [Led], GPIO_PIN_SET);
  return ret;
}

int main() 
{
  
    /* STM32H7xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
    HAL_Init();

    // initialize the led
    Led_TypeDef led = LED3;
    BSP_LED_Init(led); 

    // toggle in a loop
    bool on = true;
    while (true) {
        if (on) {
            BSP_LED_On(led);
        } 
        else {
            BSP_LED_Off(led);
        }
        on = !on;

        // busy wait for the blinking interval time
        // using a rough estimation...
        // and dependent on the CPU speed.
        for (uint64_t i = 0; i < 50000000; i++) {
            asm("nop");
        }
    }    
    
    return 0;
}
