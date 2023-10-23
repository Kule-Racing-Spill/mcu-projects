#include "spidrv.h"
#include "segmentlcd.h"

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

struct sprite_draw_info {
	uint8_t sprite_id;
	uint16_t x;
	uint16_t y;
	uint8_t scale;
} typedef sprite_draw_info;

// a pixel is 4 bits
// a sprite is 32x32 pixels
uint8_t *sprite_0[32*16];
uint8_t *sprite_1[32*16];
uint8_t *sprite_2[32*16];
uint8_t *sprite_3[32*16];

uint8_t **sprites[4] = {
		sprite_0,
		sprite_1,
		sprite_2,
		sprite_3
};

#define NUM_SPRITE_IDS 4

#define SPI_CMD_SEND_SPRITE 0
#define SPI_CMD_DRAW_SPRITES 1

void spi_send_sprite(uint8_t sprite_id) {
	uint8_t buffer[2 + 32*16];
	buffer[0] = SPI_CMD_SEND_SPRITE;
	buffer[1] = sprite_id;
	buffer[2] = **sprites[sprite_id];
	SPIDRV_MTransmitB( handle, buffer, 10 );
}

void spi_draw_sprites(sprite_draw_info *sprites, uint16_t n) {
	uint8_t buffer[3 + n * sizeof(sprite_draw_info)];
	buffer[0] = SPI_CMD_DRAW_SPRITES;
	buffer[1] = n & 0xF0;
	buffer[2] = n & 0x0F;
	buffer[3] = *(uint8_t*)sprites;
	SPIDRV_MTransmitB( handle, buffer, 10 );
}

int main( void )
{
	SegmentLCD_Init(false);
	SegmentLCD_Number(0);

	//uint8_t buffer[10];
	SPIDRV_Init_t initData = SPIDRV_MASTER_USART2;

	// Initialize a SPI driver instance
	SPIDRV_Init( handle, &initData );

	// Transmit data using a blocking transmit function
	//SPIDRV_MTransmitB( handle, buffer, 10 );
	//SegmentLCD_Number(1);

	// Transmit data using a callback to catch transfer completion.
	//SPIDRV_MTransmit( handle, buffer, 10, TransferComplete );

	for (int i = 0; i < NUM_SPRITE_IDS; i++) {
		spi_send_sprite(i);
	}


	int n_sprites = 2;
	sprite_draw_info sprites[n_sprites];

	while (1) {
		spi_draw_sprites(sprites, n_sprites);
	}
}
