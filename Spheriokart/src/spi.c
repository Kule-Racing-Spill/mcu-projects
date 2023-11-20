#include "lib/spidrv.h"
#include "spi.h"

#define DEBUG 0

SPIDRV_HandleData_t handleData;
SPIDRV_Handle_t handle = &handleData;

// a pixel is 4 bits (a half byte)
// a sprite is 16x16 pixels


#define NUM_SPRITE_IDS 1
#define SPI_CMD_DRAW_SPRITE 1
#define SPI_CMD_SEND_SPRITE 2

sprite_data_t sprite_sphere = {
	.sprite_id = 0,
	.data = {
			0xff, 0xff, 0xff, 0x88, 0x88, 0xff, 0xff, 0xff,
			0xff, 0xff, 0x88, 0x88, 0x88, 0x88, 0xff, 0xff,
			0xff, 0xf8, 0x88, 0x88, 0x88, 0x88, 0x8f, 0xff,
			0xff, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0xff,
			0xf8, 0x88, 0x66, 0x88, 0x88, 0x66, 0x88, 0x8f,
			0xf8, 0x86, 0x66, 0x68, 0x86, 0x66, 0x68, 0x8f,
			0x88, 0x86, 0x66, 0x68, 0x86, 0x66, 0x68, 0x88,
			0x88, 0x88, 0x66, 0x88, 0x88, 0x66, 0x88, 0x88,
			0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
			0x88, 0x88, 0x66, 0x66, 0x66, 0x66, 0x88, 0x88,
			0xf8, 0x88, 0x86, 0x66, 0x66, 0x68, 0x88, 0x8f,
			0xf8, 0x88, 0x88, 0x66, 0x66, 0x88, 0x88, 0x8f,
			0xff, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0xff,
			0xff, 0xf8, 0x88, 0x88, 0x88, 0x88, 0x8f, 0xff,
			0xff, 0xff, 0x88, 0x88, 0x88, 0x88, 0xff, 0xff,
			0xff, 0xff, 0xff, 0x88, 0x88, 0xff, 0xff, 0xff,
		}
};


void spi_send_sprite(sprite_data_t sprite) {
	int buffer_size = 3 + SPRITE_BYTES;
	uint8_t buffer[3 + SPRITE_BYTES];
	buffer[0] = SPI_CMD_SEND_SPRITE;
	buffer[1] = sprite.sprite_id;
	for (int i = 0; i < SPRITE_BYTES; i++) {
		buffer[2 + i] = sprite.data[i];
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
	int buffer_size = 2;
	uint8_t buffer[2];
	buffer[0] = i;
	buffer[1] = 0;
	SPIDRV_MTransmitB( handle, buffer, buffer_size );
}

void spi_init(void) {
	SPIDRV_Init_t initData = SPIDRV_MASTER_USART1;
	#if !DEV
		initData.portLocation = _USART_ROUTE_LOCATION_LOC0;
	#endif
	initData.bitRate = 6000000;
	SPIDRV_Init( handle, &initData );
}

void busy_sleep(int i) {
	int c = 0;
	while (i * 1000 > c) {
		c++;
	}
}

void spi_send_sprites( void ) {
	spi_send_sprite(sprite_sphere);
}
