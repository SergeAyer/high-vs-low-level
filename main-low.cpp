#include "mbed.h"
#include <stdint.h>

// low-level structure representing a GPIO (valid for STM32L4XX)
struct GPIO
{
  uint32_t MODER;       /*!< GPIO port mode register,               Address offset: 0x00      */
  uint32_t OTYPER;      /*!< GPIO port output type register,        Address offset: 0x04      */
  uint32_t OSPEEDR;     /*!< GPIO port output speed register,       Address offset: 0x08      */
  uint32_t PUPDR;       /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
  uint32_t IDR;         /*!< GPIO port input data register,         Address offset: 0x10      */
  uint32_t ODR;         /*!< GPIO port output data register,        Address offset: 0x14      */
  uint32_t BSRR;        /*!< GPIO port bit set/reset  register,     Address offset: 0x18      */
  uint32_t LCKR;        /*!< GPIO port configuration lock register, Address offset: 0x1C      */
  uint32_t AFR[2];      /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
  uint32_t BRR;         /*!< GPIO Bit Reset register,               Address offset: 0x28      */
  uint32_t ASCR;        /*!< GPIO analog switch control register,   Address offset: 0x2C     */
};

// definition of low level addresses (valid for STM32L4XX)
#define PERIPH_BASE               (0x40000000UL) /*!< Peripheral base address */ 
#define AHB1PERIPH_BASE           (PERIPH_BASE + 0x00020000UL)
#define RCC_BASE                  (AHB1PERIPH_BASE + 0x1000UL)
#define AHB2ENR_OFFSET            0x4C
#define AHB2PERIPH_BASE           (PERIPH_BASE + 0x08000000UL)
#define GPIOA_BASE                (AHB2PERIPH_BASE + 0x0000UL)

// RCC position/flag for enabling clock for GPIO port A
#define RCC_AHB2ENR_GPIOAEN_Pos   (0U)
#define RCC_AHB2ENR_GPIOAEN_Msk   (0x1UL << RCC_AHB2ENR_GPIOAEN_Pos) /*!< 0x00000001 */
#define RCC_AHB2ENR_GPIOAEN       RCC_AHB2ENR_GPIOAEN_Msk

// mode definitions
#define LL_GPIO_MODE_OUTPUT   GPIO_MODER_MODE0_0
#define LL_GPIO_PULL_NO       (0x00000000U) 
    
// definitions for selecting pin #5
#define GPIO_BSRR_BS5_Pos     (5U)
#define GPIO_BSRR_BS5_Msk     (0x1UL << GPIO_BSRR_BS5_Pos)            /*!< 0x00000020 */
#define GPIO_BSRR_BS5         GPIO_BSRR_BS5_Msk
// STM32 helpers
#define STM_PORT(X) (((uint32_t)(X) >> 4) & 0xF)
#define STM_PIN(X)  ((uint32_t)(X) & 0xF)

int main() 
{
    // led 1 is on pin 0x05
    constexpr unsigned int ledPin = 0x05;
    constexpr unsigned int ledPinMask = 1 << ledPin;

    // Pin 0x5 is on port A  
    volatile GPIO* ledGPIO = (GPIO*) (GPIOA_BASE);
    
    // Enable RCC Clock for GPIO Port A
    volatile uint32_t* RCC_AHB2ENR = (uint32_t*) (RCC_BASE + AHB2ENR_OFFSET);
    *RCC_AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    
    // configure the pin as output (mode LL_GPIO_MODE_OUTPUT) 
    uint32_t dirClearMask = (GPIO_MODER_MODE0 << (GPIO_BSRR_BS5_Pos * 2U));
    uint32_t dirSetMask = (LL_GPIO_MODE_OUTPUT << (GPIO_BSRR_BS5_Pos * 2U));
    MODIFY_REG(ledGPIO->MODER, dirClearMask, dirSetMask);
    
    // set pull mode to no pull
    uint32_t pullClearMask = (GPIO_PUPDR_PUPD0 << (GPIO_BSRR_BS5_Pos * 2U));
    uint32_t pullSetMask =(LL_GPIO_PULL_NO << (GPIO_BSRR_BS5_Pos * 2U));    
    MODIFY_REG(ledGPIO->PUPDR, pullClearMask, pullSetMask);

    while (true) {
        // toggle the led by setting and resetting it
        // alternatively in each iteration
        uint32_t pin_state = ((ledGPIO->ODR & ledPinMask) ? 1 : 0);
        if (pin_state) {
            ledGPIO->BSRR = ledPinMask << 16;  // turning off
        } else {
            ledGPIO->BSRR = ledPinMask;  // turning on
        }

        // busy wait for the blinking interval time
        // using a rough estimation...
        // and dependent on the CPU speed.
        for (uint64_t i = 0; i < 10000000; i++) {
            asm("nop");
        }
    }    
    
    return 0;
}
