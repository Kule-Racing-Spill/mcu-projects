#include "lib/spidrv.h"
#include "lib/segmentlcd.h"
#include "spi.h"

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

SPIDRV_HandleData_t handleData;
SPIDRV_Handle_t handle = &handleData;

void toggle_led(void);

// a pixel is 4 bits
// a sprite is 32x32 pixels

uint8_t sprite_sphere[SPRITE_BYTES] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0xdd, 0xdd, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xed, 0xdd, 0xdd, 0xdd, 0xdd, 0xde, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xed, 0xdc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcd, 0xde, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0d, 0xdc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcd, 0xd0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xdd, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xdd, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0d, 0xdd, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xdd, 0xd0, 0x00, 0x00,
	0x00, 0x00, 0xdd, 0xdd, 0xdc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcd, 0xdd, 0xdd, 0x00, 0x00,
	0x00, 0x0e, 0xdd, 0xdd, 0xdd, 0xdc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcd, 0xdd, 0xdd, 0xdd, 0xe0, 0x00,
	0x00, 0x0d, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xd0, 0x00,
	0x00, 0xed, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xde, 0x00,
	0x00, 0xed, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xde, 0x00,
	0x00, 0xed, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xde, 0x00,
	0x0f, 0xfd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdf, 0xf0,
	0x0f, 0xff, 0xfd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdf, 0xff, 0xf0,
	0x0e, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xff, 0xff, 0xff, 0xe0,
	0x0e, 0xdf, 0xff, 0xff, 0xff, 0xfd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdf, 0xff, 0xff, 0xff, 0xfd, 0xe0,
	0x0e, 0xdd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdd, 0xe0,
	0x0e, 0xdd, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xdd, 0xe0,
	0x00, 0xdd, 0xdd, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0x00,
	0x00, 0xed, 0xdd, 0xdf, 0xff, 0xff, 0xff, 0xfd, 0xdf, 0xff, 0xff, 0xff, 0xfd, 0xdd, 0xde, 0x00,
	0x00, 0xed, 0xdd, 0xdd, 0xff, 0xff, 0xff, 0xfd, 0xdf, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xde, 0x00,
	0x00, 0x0e, 0xdd, 0xdd, 0xdf, 0xff, 0xff, 0xdd, 0xdd, 0xff, 0xff, 0xfd, 0xdd, 0xdd, 0xe0, 0x00,
	0x00, 0x0e, 0xed, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xde, 0xe0, 0x00,
	0x00, 0x00, 0xee, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xee, 0x00, 0x00,
	0x00, 0x00, 0x0e, 0xee, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xee, 0xe0, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xee, 0xee, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xee, 0xee, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0e, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0xee, 0xee, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#define N_SPRITES 1
uint8_t *sprites[N_SPRITES] = {
//		&sprite_sphere
};

void spi_send_sprite(uint8_t sprite_id) {
	SegmentLCD_Number(SPI_CMD_SEND_SPRITE);
	int buffer_size = 3 + SPRITE_BYTES;
	uint8_t buffer[3 + SPRITE_BYTES];
	buffer[0] = SPI_CMD_SEND_SPRITE;
	buffer[1] = sprite_id;
	for (int i = 0; i < SPRITE_BYTES; i++) {
		buffer[2 + i] = sprites[sprite_id][i];
	}
	buffer[buffer_size - 1] = 0;
	SPIDRV_MTransmitB( handle, buffer, buffer_size );
}

void spi_draw_sprite(sprite_draw_info sprite_info) {
	int buffer_size = 8;
	uint8_t buffer[8];
	buffer[0] = SPI_CMD_DRAW_SPRITE;
	buffer[1] = sprite_info.sprite_id;
	buffer[2] = sprite_info.x >> 8;
	buffer[3] = sprite_info.x & 0x00FF;
	buffer[4] = sprite_info.y >> 8;
	buffer[5] = sprite_info.y & 0x00FF;
	buffer[6] = sprite_info.scale;
	buffer[7] = 0;
	SPIDRV_MTransmitB( handle, buffer, buffer_size );
}

void spi_send_test(int i) {
	SegmentLCD_Number(i);
	int buffer_size = 2;
	uint8_t buffer[2];
	buffer[0] = i;
	buffer[1] = 0;
	SPIDRV_MTransmitB( handle, buffer, buffer_size );
}

void spi_init(void) {

	SPIDRV_Init_t initData = SPIDRV_MASTER_USART1;
	initData.bitRate = 1000000;

	// Initialize a SPI driver instance
	SPIDRV_Init( handle, &initData );
}

void busy_sleep(int i) {
	int c = 0;
	while (i * 1000 > c) {
		c++;
	}
}

void spi_send_sprites( void ) {
	int N = 1;
	for (int i = 0; i < N; i++) {
		spi_send_sprite(i);
	}
}

#define RTC_FREQ    32768
#define LED_PORT gpioPortE
#define BUTTON_PORT gpioPortB
#define LED0 2
#define LED1 3

#define I2C_PORT gpioPortC
#define PC4 4

int led_on = 0;
void toggle_led() {
	led_on = !led_on;
	if (led_on) {
		GPIO_PinOutSet(LED_PORT, LED1);
	} else {
		GPIO_PinOutClear(LED_PORT, LED1);
	}
}

int x = 0;
int y = 0;
int dx = 5;
int dy = 5;
int scale = 0;

void draw() {

	sprite_draw_info sprite_info_a = {
		0,
		x,
		y,
		scale,
	};

	spi_draw_sprite(sprite_info_a);

	x += dx;
	y += dy;
	if (x <= 0 || 800 <= x) {
		dx *= -1;
	}
	if (y <= 0 || 480 <= y) {
		dy *= -1;
	}

	//if (x % 100 == 0) {
		SegmentLCD_Number(x);
	//}
		scale += 1;
}

void GPIO_ODD_IRQHandler(void)
{
	/* Acknowledge interrupt */
}

void GPIO_EVEN_IRQHandler(void)
{
	/* Acknowledge interrupt */

	draw();
	GPIO_IntClear(1 << PC4);
}

void gpio_setup(void) {
	CMU_ClockEnable(cmuClock_GPIO, true);

    GPIO_PinModeSet(I2C_PORT, PC4, gpioModeInput, 0);
    GPIO_ExtIntConfig(I2C_PORT, PC4, PC4, false, true, true);

	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

int main( void )
{
	/* Chip errata */
	CHIP_Init();

	/* Ensure core frequency has been updated */
	SystemCoreClockUpdate();

	gpio_setup();

	SegmentLCD_Init(false);
	SegmentLCD_Number(-1);

	spi_init();

	while (1) {
		//draw();
	}

	return 0;
}
