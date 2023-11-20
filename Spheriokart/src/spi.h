#include "lib/spidrv.h"

#define SPRITE_BYTES 128

struct sprite_draw_info
{
	uint8_t sprite_id;
	uint16_t x;
	uint16_t y;
	uint8_t scale;
} typedef sprite_draw_info;

struct sprite_data_t {
	uint8_t sprite_id;
	uint8_t data[SPRITE_BYTES];
} typedef sprite_data_t;

void spi_init(void);
void spi_send_sprite(sprite_data_t sprite);
void spi_draw_sprite(sprite_draw_info sprite_info);
void spi_send_test(int i);
void busy_sleep(int i);
void spi_send_sprites(void);
