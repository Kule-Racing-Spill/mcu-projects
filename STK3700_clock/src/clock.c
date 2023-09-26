/***************************************************************************//**
 * @file
 * @brief LCD controller and Energy Mode/RTC demo for EFM32GG_STK3700
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_rtc.h"
#include "em_lcd.h"
#include "vddcheck.h"
#include "segmentlcd.h"
#include "bsp_trace.h"

#define RTC_FREQ    32768

#define LED_PORT gpioPortE
#define BUTTON_PORT gpioPortB
#define LED0 2
#define LED1 3
#define PB0 9
#define PB1 10

#define   I2C_PORT     gpioPortC     // gpioPortC
#define   PC4      4     // PC4

/* Initial setup to 12:00 */
uint32_t hours   = 12;
uint32_t minutes = 0;
int led_on = 0;

/* This flag enables/disables vboost on the LCD */
bool oldBoost = false;

/***************************************************************************//**
 * @brief GPIO Interrupt handler (PB9)
 *        Sets the hours
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << 9);

}

/***************************************************************************//**
 * @brief GPIO Interrupt handler (PB10)
 *        Sets the minutes
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << 10);
  GPIO_IntClear(1 << 4);

  led_on = !led_on;
	if (led_on) {
	  GPIO_PinOutSet(LED_PORT, LED1);
	} else {
	  GPIO_PinOutClear(LED_PORT, LED1);
	}
}

/***************************************************************************//**
 * @brief Setup GPIO interrupt to set the time
 ******************************************************************************/
void gpioSetup(void)
{
  /* Enable GPIO in CMU */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PB9 and PB10 as input */
  	GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
    GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);

    GPIO_PinModeSet(I2C_PORT, PC4, gpioModeInput, 0);

    /* Set falling edge interrupt for both ports */
  	GPIO_IntConfig(gpioPortB, 9, false, true, true);
    GPIO_IntConfig(gpioPortB, 10, false, true, true);

    GPIO_IntConfig(I2C_PORT, PC4, false, true, true);

  /* Enable interrupt in core for even and odd gpio interrupts */
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

int gpio_main() {
	/* Chip errata */
  CHIP_Init();

  /* Ensure core frequency has been updated */
  SystemCoreClockUpdate();

  /* Init LCD with no voltage boost */
  SegmentLCD_Init(oldBoost);

  /* Setup GPIO with interrupts to serve the pushbuttons */
  gpioSetup();

  /* Main function loop */
  while (1) {
	  SegmentLCD_Number(hours * 100 + minutes);
	  minutes += 1;
  }

  return 0;
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
	gpio_main();
}

