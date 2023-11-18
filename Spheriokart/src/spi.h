#include "lib/spidrv.h"

#define NUM_SPRITE_IDS 4
#define SPRITE_BYTES (32*16)
#define SPI_CMD_SEND_SPRITE 0
#define SPI_CMD_DRAW_SPRITE 1

struct sprite_draw_info {
	uint8_t sprite_id;
	uint16_t x;
	uint16_t y;
	uint8_t scale;
} typedef sprite_draw_info;

void spi_init(void);
void spi_send_sprite(uint8_t sprite_id);
void spi_draw_sprite(sprite_draw_info sprite_info);
void spi_send_test(int i);
void busy_sleep(int i);
void spi_loop( void );
void spi_send_sprites( void );
