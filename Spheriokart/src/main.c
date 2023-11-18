#include "em_usb.h"
#include "trackball.h"
#include "kart.h"
#include "printing.h"
#include "spi.h"
#include "em_device.h"
#include "button.h"

#define DEBUG 0
#define DEV 1
#define TRACKBALL 0

#define LED_PORT gpioPortE
#define LED0 2
#define LED1 3

#if DEV
#define FPGA_INPUT_PORT 2
#define FPGA_INPUT_PIN 4
#else
#define FPGA_INPUT_PORT gpioPortE
#define FPGA_INPUT_PIN 15
#endif

#if DEBUG
#include "segmentlcd.h" // For writing to the devboard screen. Can be removed when we are running on our own board.
#endif

vec2int input_vector;

int frames = 0;

void fpga_call()
{
	frames += 1;
	kart_draw();
	GPIO_IntClear(1 << FPGA_INPUT_PIN);
}

void GPIO_ODD_IRQHandler(void) { fpga_call(); }
void GPIO_EVEN_IRQHandler(void) { fpga_call(); }

void gpio_init(void)
{
	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_PinModeSet(FPGA_INPUT_PORT, FPGA_INPUT_PIN, gpioModeInput, 0);
	GPIO_ExtIntConfig(FPGA_INPUT_PORT, FPGA_INPUT_PIN, FPGA_INPUT_PIN, false, true, true);

	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
}


int main()
{
#if DEBUG
	SWO_SetupForPrint(); /* For adding printing to console in simplicity studio debugger */
#endif

	kart_init();
	spi_init();
	gpio_init();

	/* Button stuff */
	Button_Init(BUTTON1 | BUTTON2);
	uint32_t b;

	while (1)
	{
		input_vector.x = 0;
		input_vector.y = 4*5;

		/* BUTTONS */

		b = b | Button_ReadReleased();
		GPIO_PinOutClear(LED_PORT, LED0);
		GPIO_PinOutClear(LED_PORT, LED1);

		if (b & BUTTON1)
		{
			input_vector.x = -1;
			GPIO_PinOutSet(LED_PORT, LED0);
#if DEBUG
			printf("Button1 pressed!\n");
#endif
		}
		if (b & BUTTON2)
		{
			input_vector.x = 1;
			GPIO_PinOutSet(LED_PORT, LED1);
#if DEBUG
			printf("Button2 pressed!\n");
#endif
		}

		/* TRACKBALL*/

#if TRACKBALL

		if (USBH_DeviceConnected())
		{
			GetTrackballValues(&v);
		}
		else
		{
			InitUSBHStack(); /* Initialize usb stack */
			InitTrackball(); /* Initialize the trackball */
		}

#endif
		/* KART */

		if (frames) {
			kart_step(input_vector, frames);
			frames = 0;
			b = 0;
		}
	}
}
