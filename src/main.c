#include "main.h"

void loop(void)
{
    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_Delay(1000);
}

void main(void)
{
    // Reset of all peripherals, Initializes the Flash interface and the Systick.
    HAL_Init();
    // Configure the system clock
    SystemClock_Config();
    // Initialize all configured peripherals
    MX_GPIO_Init();

    /*
     * Configure GPIO pin : LED LD1 (green, next to reset switch).
     * The default config is SCK for the Arduino SPI (Arduino pin D13)
     */
    {
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.Pin = GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(true) loop();
#pragma clang diagnostic pop
}
