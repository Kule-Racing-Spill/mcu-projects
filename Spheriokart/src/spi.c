#include "lib/spidrv.h"
//#include "lib/segmentlcd.h"
#include "spi.h"

void TransferComplete( SPIDRV_Handle_t handle,
                       Ecode_t transferStatus,
                       int itemsTransferred )
{
	if ( transferStatus == ECODE_EMDRV_SPIDRV_OK )
	{
	// Success !
		//SegmentLCD_Number(2);
	}
}

SPIDRV_HandleData_t handleData;
SPIDRV_Handle_t handle = &handleData;

// a pixel is 4 bits
// a sprite is 32x32 pixels

#define SPRITE_BYTES 512

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
		&sprite_sphere
};

void spi_send_sprite(uint8_t sprite_id) {
	//SegmentLCD_Number(SPI_CMD_SEND_SPRITE);
	int buffer_size = 2 + SPRITE_BYTES;
	uint8_t buffer[2 + SPRITE_BYTES];
	buffer[0] = SPI_CMD_SEND_SPRITE;
	buffer[1] = sprite_id;
	for (int i = 0; i < SPRITE_BYTES; i++) {
		buffer[2 + i] = sprites[sprite_id][i];
	}
	SPIDRV_MTransmitB( handle, buffer, buffer_size );
}

void spi_draw_sprite(sprite_draw_info sprite_info) {
	//SegmentLCD_Number(SPI_CMD_DRAW_SPRITE);
	int buffer_size = 7;
	uint8_t buffer[7];
	buffer[0] = SPI_CMD_DRAW_SPRITE;
	buffer[1] = sprite_info.sprite_id;
	buffer[2] = sprite_info.x >> 8;
	buffer[3] = sprite_info.x & 0x00FF;
	buffer[4] = sprite_info.y >> 8;
	buffer[5] = sprite_info.y & 0x00FF;
	buffer[6] = sprite_info.scale;
	SPIDRV_MTransmitB( handle, buffer, buffer_size );
}

void spi_send_test(int i) {
	//SegmentLCD_Number(i);
	int buffer_size = 1;
	uint8_t buffer[1];
	buffer[0] = i;
	SPIDRV_MTransmitB( handle, buffer, buffer_size );
}

void spi_init(void) {
	//uint8_t buffer[10];
	SPIDRV_Init_t initData = SPIDRV_MASTER_USART1;
	initData.bitRate = 1;

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

void spi_loop( void )
{
	//SegmentLCD_Init(false);
	//SegmentLCD_Number(-1);

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
		spi_draw_sprite(sprite_info);
		busy_sleep(1000);
		for (int i = 2; i < 10; i++) {
			spi_send_test(i);
			busy_sleep(1000);
		}
	}
}