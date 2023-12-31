#include "em_usb.h"
#include "trackball.h"
#include "kart.h"
#include "printing.h"
#include "spi.h"
#include "em_device.h"
#include "button.h"
#include "config.h"

#define LED_PORT gpioPortE
#define LED0 2
#define LED1 3

// #define PD5 5

#if DEV
#define FPGA_INPUT_PORT 2
#define FPGA_INPUT_PIN 4
#else
#include "clock_efm32gg.h"
#define FPGA_INPUT_PORT gpioPortE
#define FPGA_INPUT_PIN 15
#define FPGA_READY_PIN 14
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
#if !DEV
	GPIO_PinModeSet(FPGA_INPUT_PORT, FPGA_READY_PIN, gpioModeInput, 0);
#endif

	GPIO_ExtIntConfig(FPGA_INPUT_PORT, FPGA_INPUT_PIN, FPGA_INPUT_PIN, false, true, true);

	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

int main()
{
#if !DEV
	(void)SystemCoreClockSet(CLOCK_HFXO, 1, 1);
#endif

#if DEBUG
	SWO_SetupForPrint(); /* For adding printing to console in simplicity studio debugger */
#endif

	kart_init();
	spi_init();
	gpio_init();
	spi_send_sprites();

	/* Button stuff */
	Button_Init(BUTTON1 | BUTTON2 | BUTTON3 | BUTTON4);
	uint32_t b;
	input_vector.x = 0;
	input_vector.y = 0;
	vec2int prev_input_vector = {
		.x = 0,
		.y = 0};

	int fpga_reset = 0;
	int fpga_ready = 0;
	// GPIO_PinOutClear(gpioPortE, 13);

	while (1)
	{
#if !DEV
		fpga_ready = !GPIO_PinInGet(FPGA_INPUT_PORT, FPGA_READY_PIN);
		int pe13 = GPIO_PinOutGet(gpioPortE, 13);

		if (fpga_reset && fpga_ready)
		{
			spi_send_sprites();
		}

		fpga_reset = GPIO_PinInGet(FPGA_INPUT_PORT, FPGA_READY_PIN);
		/* BUTTONS */
#endif

		b = b | Button_Read();

		if (b & BUTTON1)
		{
			input_vector.y = +100; // BOOST
#if DEBUG
			printf("Button1 pressed!\n");
#endif
		}
		if (b & BUTTON3)
		{
			jump();
		}

#if !DEV
		if (b & BUTTON4)
		{
			NVIC_SystemReset(); // Reset MCU on press
		}

		if (b & BUTTON2)
		{
			GPIO_PinOutSet(gpioPortE, 13); // Reprogram FPGA
#if DEBUG
			printf("Button2 pressed!\n");
#endif
		}
		else
		{
			GPIO_PinOutClear(gpioPortE, 13);
		}
#endif
		/* TRACKBALL*/

#if TRACKBALL

		if (USBH_DeviceConnected())
		{
			GetTrackballValues(&input_vector);
		}
		else
		{
			InitUSBHStack(); /* Initialize usb stack */
			InitTrackball(); /* Initialize the trackball */
		}

#endif
		/* KART */

		if (frames)
		{
			if (prev_input_vector.x == input_vector.x)
			{
				input_vector.x = 0;
			}
			else
			{
				prev_input_vector.x = input_vector.x;
			}
			if (prev_input_vector.y == input_vector.y)
			{
				input_vector.y = 0;
			}
			else
			{
				prev_input_vector.y = input_vector.y;
			}
			kart_step(input_vector, frames);
			frames = 0;
			b = 0;
		}
	}
}
