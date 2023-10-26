#include "lib/spidrv.h"
#include "lib/segmentlcd.h"

void TransferComplete( SPIDRV_Handle_t handle,
                       Ecode_t transferStatus,
                       int itemsTransferred )
{
	if ( transferStatus == ECODE_EMDRV_SPIDRV_OK )
	{
	// Success !
		SegmentLCD_Number(2);
	}
}

SPIDRV_HandleData_t handleData;
SPIDRV_Handle_t handle = &handleData;

#define NUM_SPRITE_IDS 4
#define SPRITE_BYTES (32*16)
#define SPI_CMD_SEND_SPRITE 0
#define SPI_CMD_DRAW_SPRITES 1
#define SPI_CMD_TEST_2 2
#define SPI_CMD_TEST_3 3

struct sprite_draw_info {
	uint8_t sprite_id;
	uint16_t x;
	uint16_t y;
	uint8_t scale;
} typedef sprite_draw_info;

// a pixel is 4 bits
// a sprite is 32x32 pixels
uint8_t sprite_0[SPRITE_BYTES];
uint8_t sprite_1[SPRITE_BYTES];
uint8_t sprite_2[SPRITE_BYTES];
uint8_t sprite_3[SPRITE_BYTES];

uint8_t *sprites[4] = {
		sprite_0,
		sprite_1,
		sprite_2,
		sprite_3
};

void spi_send_sprite(uint8_t sprite_id) {
	SegmentLCD_Number(SPI_CMD_SEND_SPRITE);
	int buffer_size = 2 + SPRITE_BYTES;
	uint8_t buffer[buffer_size];
	buffer[0] = SPI_CMD_SEND_SPRITE;
	buffer[1] = sprite_id;
	buffer[2] = *sprites[sprite_id];
	SPIDRV_MTransmitB( handle, buffer, buffer_size );
}

void spi_draw_sprites(sprite_draw_info sprite_info) {
	SegmentLCD_Number(SPI_CMD_DRAW_SPRITES);
	int buffer_size = 7;
	uint8_t buffer[buffer_size];
	buffer[0] = SPI_CMD_DRAW_SPRITES;
	//buffer[1] = (uint8_t)sprite_info;
	SPIDRV_MTransmitB( handle, buffer, buffer_size );
}

void spi_send_test(int i) {
	SegmentLCD_Number(i);
	int buffer_size = 1;
	uint8_t buffer[1];
	buffer[0] = i;
	SPIDRV_MTransmitB( handle, buffer, buffer_size );
}

void busy_sleep(int i) {
	int c = 0;
	while (i * 1000 > c) {
		c++;
	}
}

int main( void )
{
	SegmentLCD_Init(false);
	SegmentLCD_Number(-1);

	//uint8_t buffer[10];
	SPIDRV_Init_t initData = SPIDRV_MASTER_USART1;
	initData.bitRate = 1;

	// Initialize a SPI driver instance
	SPIDRV_Init( handle, &initData );

	// Transmit data using a blocking transmit function
	//SPIDRV_MTransmitB( handle, buffer, 10 );
	//SegmentLCD_Number(1);

	// Transmit data using a callback to catch transfer completion.
	//SPIDRV_MTransmit( handle, buffer, 10, TransferComplete );

	// for (int i = 0; i < NUM_SPRITE_IDS; i++) {
	// 	spi_send_sprite(i);
	// }

	sprite_draw_info sprite_info;

	while (1) {
		spi_send_sprite(0);
		busy_sleep(1000);
		spi_draw_sprites(sprite_info);
		busy_sleep(1000);
		for (int i = 2; i < 10; i++) {
			spi_send_test(i);
			busy_sleep(1000);
		}
	}
}
