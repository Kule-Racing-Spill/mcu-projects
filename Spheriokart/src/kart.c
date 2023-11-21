#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "fast_math.h"
#include "trackball.h"
#include "spi.h"

// Maths

#define PI 3.14159265358979323846
float TAU = 2 * PI;
float PI_HALF = PI / 2;

struct vec2
{
	float x;
	float y;
} typedef vec2;

vec2 vec2_add(vec2 a, vec2 b)
{
	vec2 sum = {a.x + b.x, a.y + b.y};
	return sum;
}

static inline float distance_between(vec2 a, vec2 b)
{
	float dx = b.x - a.x;
	float dy = b.y - a.y;
	return fabs(dx) + fabs(dy);
}

float clamp(float value, float min, float max)
{
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}

// Sprite

#define SPRITE_MARIO_RIGHT 0
#define SPRITE_MARIO_UP 1
#define SPRITE_MARIO_LEFT 2
#define SPRITE_HALF 64 // this considers scaling

// Entities

struct entity_t
{
	vec2 position;
	sprite_draw_info draw_info;
	uint8_t visible;
	uint8_t disabled;
	float distance_to_camera;
} typedef entity_t;

struct moving_entity_t
{
	entity_t *entity;
	float direction;
	float radial_speed;		// change in direction = x * some factor
	float tangential_speed; // change in position along the current direction = y * some factor
	float z;
	float z_speed;
} typedef moving_entity_t;

#define CHUNK_WIDTH 1024
#define WORLD_WIDTH 24
#define MAX_ENTITIES_PER_CHUNK 16

struct chunk_t
{
	int i;
	entity_t *entities[MAX_ENTITIES_PER_CHUNK];
} typedef chunk_t;

chunk_t chunks[WORLD_WIDTH * WORLD_WIDTH];

static bool is_coin(entity_t *e)
{
	return e->draw_info.sprite_id >= 16 && e->draw_info.sprite_id < 20;
}

static bool is_bush(entity_t *e)
{
	return e->draw_info.sprite_id >= 8 && e->draw_info.sprite_id < 16;
}

int chunk_index(float x, float y)
{
	int offset = CHUNK_WIDTH * WORLD_WIDTH / 2;
	int cx = (x + offset) / CHUNK_WIDTH;
	if (cx < 0)
	{
		cx = 0;
	}
	else if (cx > (WORLD_WIDTH - 1))
	{
		cx = WORLD_WIDTH - 1;
	}

	int cy = (y + offset) / CHUNK_WIDTH;
	if (cy < 0)
	{
		cy = 0;
	}
	else if (cy > (WORLD_WIDTH - 1))
	{
		cy = WORLD_WIDTH - 1;
	}
	return cy * WORLD_WIDTH + cx;
}

// Player

#define PLAYER_RADIAL_ACC 0.0003
#define PLAYER_MAX_RADIAL_SPEED 0.02
#define PLAYER_MAX_TANGENTIAL_SPEED 100
#define PLAYER_RADIAL_DAMPING 0.94
#define PLAYER_TANGENTIAL_DAMPING 0.96
#define PLAYER_JUMP_UP 24
#define PLAYER_JUMP_FORWARD 0.3
#define PLAYER_GRAVITY 3

struct player_t
{
	moving_entity_t moving;
	float rotation;
} typedef player_t;

// Declarations
#if DEBUG
static void WritePosToScreen(player_t *player);
#endif

extern inline void validate_angle(float *angle)
{
	while (*angle < -PI)
		*angle += TAU;
	while (*angle > PI)
		*angle -= TAU;
}

extern inline void player_step(player_t *player, vec2int input_vector, int frames)
{
	moving_entity_t *moving = &(player->moving);
	entity_t *entity = moving->entity;

	// Tangential speed
	moving->tangential_speed += input_vector.y * frames;

	// Radial speed
	moving->radial_speed += input_vector.x * 0.01 * frames;
	moving->direction += moving->radial_speed;
	validate_angle(&(moving->direction));

	// Movement
	entity->position.x += fast_sin(PI_HALF - moving->direction) * moving->tangential_speed;
	entity->position.y += fast_sin(moving->direction) * moving->tangential_speed;

	// Clamp
	moving->tangential_speed *= PLAYER_TANGENTIAL_DAMPING;
	moving->tangential_speed = clamp(moving->tangential_speed, -PLAYER_MAX_TANGENTIAL_SPEED, PLAYER_MAX_TANGENTIAL_SPEED);
	moving->radial_speed *= PLAYER_RADIAL_DAMPING;
	moving->radial_speed = clamp(moving->radial_speed, -PLAYER_MAX_RADIAL_SPEED, PLAYER_MAX_RADIAL_SPEED);

	// Animation
	player->rotation += player->moving.tangential_speed / 50.0;
	if (player->rotation > 16)
	{
		player->rotation -= 16;
	}
	if (player->rotation < 0)
	{
		player->rotation += 16;
	}

	if (player->rotation <= 8)
	{
		player->moving.entity->draw_info.sprite_id = (int)player->rotation;
	}
	else
	{
		player->moving.entity->draw_info.sprite_id = 0;
	}

	player->moving.z += player->moving.z_speed;

	if (player->moving.z > 0) {
		player->moving.z_speed -= PLAYER_GRAVITY;
	} else {
		player->moving.z = 0;
		player->moving.z_speed = 0;
	}
}

// Canvas

struct canvas_t
{
	vec2 size;
} typedef canvas_t;

void draw_rect(int x0, int y0, int x1, int y1, char *color)
{
	// TODO!
}

// Program

#define NUM_ENTITIES 450
#define CAMERA_PLAYER_DISTANCE 256
#define CAMERA_RENDER_DISTANCE 8096

canvas_t canvas = (canvas_t){
	.size = {800, 480}};

player_t player;
entity_t entities[NUM_ENTITIES];
entity_t *entity_draw_order[NUM_ENTITIES];
int visible_count = 0;
int coin_rotation = 0;
int coin_count = 0;
int timer = 0;

void jump() {
	if (player.moving.z == 0) {
		player.moving.z_speed = PLAYER_JUMP_UP;
		player.moving.tangential_speed += PLAYER_JUMP_FORWARD;
	}
}

int compare_distance_to_camera(const void *pa, const void *pb)
{
	entity_t *a = *((entity_t **)pa);
	entity_t *b = *((entity_t **)pb);

	if (a->distance_to_camera == b->distance_to_camera)
		return 0;
	else if (a->distance_to_camera < b->distance_to_camera)
		return -1;
	else
		return 1;
}

sprite_draw_info overlay_coin = {
	.sprite_id = 16,
	.x = 20,
	.y = 20,
	.scale = 48,
};
sprite_draw_info overlay_coin_digit0 = {
	.sprite_id = 32,
	.x = 84,
	.y = 20,
	.scale = 48,
};
sprite_draw_info overlay_coin_digit1 = {
	.sprite_id = 32,
	.x = 134,
	.y = 20,
	.scale = 48,
};

sprite_draw_info overlay_speedometer = {
	.sprite_id = 20,
	.x = 20,
	.y = 428,
	.scale = 32,
};
sprite_draw_info overlay_speed_digit0 = {
	.sprite_id = 32,
	.x = 20 + 32,
	.y = 428,
	.scale = 32,
};
sprite_draw_info overlay_speed_digit1 = {
	.sprite_id = 32,
	.x = 20 + 32 + 28,
	.y = 428,
	.scale = 32,
};

sprite_draw_info overlay_stopwatch = {
	.sprite_id = 21,
	.x = 550,
	.y = 20,
	.scale = 48,
};
sprite_draw_info overlay_timer_min_digit0 = {
	.sprite_id = 32,
	.x = 550 + 44,
	.y = 20,
	.scale = 48,
};
sprite_draw_info overlay_timer_min_digit1 = {
	.sprite_id = 32,
	.x = 550 + 44 + 40,
	.y = 20,
	.scale = 48,
};
sprite_draw_info overlay_timer_colon = {
	.sprite_id = 42,
	.x = 550 + 44 + 40 + 32,
	.y = 20,
	.scale = 48,
};
sprite_draw_info overlay_timer_sec_digit0 = {
	.sprite_id = 32,
	.x = 550 + 44 + 40 + 32 + 32,
	.y = 20,
	.scale = 48,
};
sprite_draw_info overlay_timer_sec_digit1 = {
	.sprite_id = 32,
	.x = 550 + 44 + 40 + 32 + 32 + 40,
	.y = 20,
	.scale = 48,
};
void draw_overlay()
{
	overlay_coin_digit0.sprite_id = 32 + coin_count / 10;
	overlay_coin_digit1.sprite_id = 32 + coin_count % 10;
	spi_draw_sprite(overlay_coin);
	spi_draw_sprite(overlay_coin_digit0);
	spi_draw_sprite(overlay_coin_digit1);

	int speed = abs(player.moving.tangential_speed / 5);
	overlay_speed_digit0.sprite_id = 32 + speed / 10;
	overlay_speed_digit1.sprite_id = 32 + speed % 10;
	spi_draw_sprite(overlay_speedometer);
	spi_draw_sprite(overlay_speed_digit0);
	spi_draw_sprite(overlay_speed_digit1);

	int seconds = timer / 30;
	int minutes = seconds / 60;
	overlay_timer_min_digit0.sprite_id = 32 + minutes / 10;
	overlay_timer_min_digit1.sprite_id = 32 + minutes % 10;
	overlay_timer_sec_digit0.sprite_id = 32 + (seconds % 60) / 10;
	overlay_timer_sec_digit1.sprite_id = 32 + seconds % 10;
	spi_draw_sprite(overlay_stopwatch);
	spi_draw_sprite(overlay_timer_min_digit0);
	spi_draw_sprite(overlay_timer_min_digit1);
	spi_draw_sprite(overlay_timer_colon);
	spi_draw_sprite(overlay_timer_sec_digit0);
	spi_draw_sprite(overlay_timer_sec_digit1);
}

extern inline void kart_draw()
{
	qsort(entity_draw_order, visible_count, sizeof(entity_t *), compare_distance_to_camera);

	for (int i = visible_count - 1; i >= 0; i--)
	{
		if (!entity_draw_order[i]->visible)
		{
			continue;
		}

		spi_draw_sprite(entity_draw_order[i]->draw_info);
	}

	draw_overlay();
}

int set_draw_info(entity_t *entity, vec2 camera_pos, vec2 origin, int scale_offset, float z)
{
	entity->visible = 0;

	float distance_to_camera = distance_between(camera_pos, entity->position);

	entity->distance_to_camera = distance_to_camera;

	if (distance_to_camera > CAMERA_RENDER_DISTANCE)
	{
		return 0;
	}

	float distance_to_player = distance_between(player.moving.entity->position, entity->position);
	if (distance_to_player > distance_to_camera)
	{
		return 1;
	}

	float angle =
		fast_atan2_b(entity->position.y - camera_pos.y, entity->position.x - camera_pos.x) - player.moving.direction;
	validate_angle(&angle);

	if (angle < -PI_HALF || PI_HALF < angle)
	{
		return 1;
	}

	vec2 offset = {
		32 * fast_pow(distance_to_camera, 0.4) * sin(angle),
		16 * fast_pow(distance_to_camera, 0.34) * sin(angle - PI_HALF)};

	int x = origin.x + offset.x;
	int y = origin.y + offset.y;

	entity->draw_info.x = x;
	entity->draw_info.y = y - z;

	if (x < 0 || canvas.size.x - 0 < x || y < 0 || canvas.size.y - 0 < y)
	{
		return 1;
	}

	int scale = round(clamp(scale_offset + 10000 / fast_pow(distance_to_camera, 0.8), 1, 256));
	entity->draw_info.scale = scale;

	entity_draw_order[visible_count] = entity;
	visible_count += 1;
	entity->visible = 1;

	return 0;
}

extern inline void kart_step(vec2int input_vector, int frames)
{
	timer++;
	player_step(&player, input_vector, frames);

	coin_rotation = (coin_rotation + 1) % 16;

#if DEBUG
	WritePosToScreen(&player);
#endif

	vec2 camera_pos = {
		player.moving.entity->position.x - CAMERA_PLAYER_DISTANCE * fast_sin(PI_HALF - player.moving.direction),
		player.moving.entity->position.y - CAMERA_PLAYER_DISTANCE * fast_sin(player.moving.direction)};

	vec2 origin = {
		canvas.size.x / 2 - SPRITE_HALF,
		canvas.size.y};

	visible_count = 0;

	set_draw_info(player.moving.entity, camera_pos, origin, -5, player.moving.z);

	int player_chunk_index = chunk_index(player.moving.entity->position.x, player.moving.entity->position.y);

	// Draw sprites
	for (int x = -5; x <= 5; x++)
	{
		for (int y = -5; y <= 5; y++)
		{
			int ci = player_chunk_index + x + y * WORLD_WIDTH;
			if (ci < 0 || WORLD_WIDTH * WORLD_WIDTH <= ci)
				continue;

			chunk_t *chunk = &chunks[ci];
			for (int i = 0; i < chunk->i; i++)
			{
				entity_t *e = chunk->entities[i];
				if (e->disabled)
					continue;
				if (is_coin(e))
					e->draw_info.sprite_id = 16 + (coin_rotation >> 2);
				int res = set_draw_info(e, camera_pos, origin, 10, 0);
				//if (x != 0 && y != 0 && res != 0)
				//	break;
			}
		}
	}

	// Collisions

	if (player.moving.z < 16) {
		chunk_t *chunk = &chunks[player_chunk_index];
		for (int i = 0; i < chunk->i; i++)
		{
			entity_t *e = chunk->entities[i];
			if (e->disabled)
				continue;

			if (distance_between(e->position, player.moving.entity->position) < 96)
			{
				if (is_coin(e))
				{
					e->disabled = 1;
					coin_count++;
				}
				else if (is_bush(e))
				{
					player.moving.tangential_speed = 0;
				}
				break;
			}
		}
	}
}

void kart_init()
{
	int R = 8192;
	int NUM_ENTITIES_HALF = NUM_ENTITIES / 2;

	for (int i = 0; i < NUM_ENTITIES_HALF; i++)
	{
		entities[i].draw_info.sprite_id = 8 + (rand() % 3);
		int r = R;
		if (i % 2 == 0)
		{
			r = R * 0.92;
		}
		if (i % 10 == 0)
		{
			r = R * 0.96;
			entities[i].draw_info.sprite_id = 16;
		}
		entities[i].position.x = -R + r * sin(2 * PI * i / NUM_ENTITIES_HALF);
		entities[i].position.y = 0 + r * cos(2 * PI * i / NUM_ENTITIES_HALF);
	}

	for (int i = NUM_ENTITIES_HALF; i < NUM_ENTITIES; i++)
	{
		entities[i].draw_info.sprite_id = 8 + (rand() % 3);
		int r = R;
		if (i % 2 == 0)
		{
			r = R * 0.92;
		}
		if (i % 10 == 0)
		{
			r = R * 0.96;
			entities[i].draw_info.sprite_id = 16;
		}
		entities[i].position.x = R + r * sin(2 * PI * i / NUM_ENTITIES_HALF);
		entities[i].position.y = 0 + r * cos(2 * PI * i / NUM_ENTITIES_HALF);
	}

	for (int i = 0; i < (WORLD_WIDTH * WORLD_WIDTH); i++)
	{
		chunk_t *chunk = &chunks[i];
		chunk->i = 0;
	}

	for (int i = 0; i < NUM_ENTITIES; i++)
	{
		float x = entities[i].position.x;
		float y = entities[i].position.y;
		entities[i].visible = 0;
		entities[i].disabled = 0;
		chunk_t *chunk = &chunks[chunk_index(x, y)];

		vec2 origin = {0, 0};
		if (distance_between(entities[i].position, origin) < 512)
			continue;
		if (chunk->i >= MAX_ENTITIES_PER_CHUNK)
			continue;

		chunk->entities[chunk->i] = &entities[i];
		chunk->i++;
	}

	player.moving.entity = malloc(sizeof(entity_t));
	player.moving.direction = 0;
	player.moving.entity->draw_info.sprite_id = 0;
	player.moving.entity->position.x = 0;
	player.moving.entity->position.y = 0;
	player.moving.entity->disabled = 0;
	player.moving.direction = PI / 3;
	player.rotation = 0;
}

#if DEBUG
static void WritePosToScreen(player_t *player)
{
	float x = player->moving.radial_speed;
	float y = player->moving.tangential_speed;
	float direction = player->moving.direction;
	float position = player->moving.entity.position.x;

	// Determine pad direction
	if (x == 0 && y == 0)
	{
		// No movement detected
		return;
	}

	// Display direction and values
	// SegmentLCD_Number(round(direction));
	// SegmentLCD_LowerNumber(round(position));
}
#endif
