/** **************************************************************************
 * @file    button.c
 * @brief   Button HAL for EFM32GG STK
 * @version 1.0
 *****************************************************************************/

#include <stdint.h>
/*
 * Including this file, it is possible to define which processor using command line
 * E.g. -DEFM32GG995F1024
 * The alternative is to include the processor specific file directly
 * #include "efm32gg995f1024.h"
 */
#include "em_device.h"
#include "config.h"

#if !DEV
#include "efm32gg332f1024.h"
#endif

#include "button.h"


static GPIO_P_TypeDef * const GPIOA = &(GPIO->P[0]);    // GPIOA

static uint32_t lastread = 0;
static uint32_t inputpins = 0;

void Button_Init(uint32_t buttons) {

    /* Enable Clock for GPIO */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO

    if ( buttons&BUTTON1 ) {
        GPIOA->MODEL &= ~_GPIO_P_MODEL_MODE0_MASK;      // Clear bits
        GPIOA->MODEL |= GPIO_P_MODEL_MODE0_INPUT;       // Set bits
        inputpins |= BUTTON1;
    }

    if ( buttons&BUTTON2 ) {
        GPIOA->MODEL &= ~_GPIO_P_MODEL_MODE1_MASK;      // Clear bits
        GPIOA->MODEL |= GPIO_P_MODEL_MODE1_INPUT;       // Set bits
        inputpins |= BUTTON2;
    }

    if ( buttons&BUTTON3) {
		GPIOA->MODEL &= ~_GPIO_P_MODEL_MODE2_MASK;      // Clear bits
		GPIOA->MODEL |= GPIO_P_MODEL_MODE2_INPUT;       // Set bits
		inputpins |= BUTTON3;
    }

	if ( buttons&BUTTON4) {
		GPIOA->MODEL &= ~_GPIO_P_MODEL_MODE3_MASK;      // Clear bits
		GPIOA->MODEL |= GPIO_P_MODEL_MODE3_INPUT;       // Set bits
		inputpins |= BUTTON4;
	}
    // First read
    lastread = GPIOA->DIN;

}

uint32_t Button_Read(void) {

    lastread = GPIOA->DIN;
    return lastread&inputpins;
}

uint32_t Button_ReadChanges(void) {
uint32_t newread;
uint32_t changes;

    newread = GPIOA->DIN;
    changes = newread^lastread;
    lastread = newread;

    return changes&inputpins;
}

uint32_t Button_ReadReleased(void) {
uint32_t newread;
uint32_t changes;

    newread = GPIOA->DIN;
    changes = newread&~lastread;
    lastread = newread;

    return changes&inputpins;
}

uint32_t Button_ReadPressed(void) {
uint32_t newread;
uint32_t changes;

    newread = GPIOA->DIN;
    changes = ~newread&lastread;
    lastread = newread;

    return changes&inputpins;
}
