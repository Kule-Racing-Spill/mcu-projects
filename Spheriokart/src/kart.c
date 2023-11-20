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
	float distance_to_camera;
} typedef entity_t;

struct moving_entity_t
{
	entity_t *entity;
	float direction;
	float radial_speed;		// change in direction = x * some factor
	float tangential_speed; // change in position along the current direction = y * some factor
} typedef moving_entity_t;

// Player

#define PLAYER_RADIAL_ACC 0.0003
#define PLAYER_MAX_RADIAL_SPEED 0.02
#define PLAYER_TANGENTIAL_ACC 1
#define PLAYER_MAX_TANGENTIAL_SPEED 10
#define PLAYER_RADIAL_DAMPING 0.9
#define PLAYER_TANGENTIAL_DAMPING 0.96

struct player_t
{
	moving_entity_t moving;
} typedef player_t;

// Declarations
#if DEBUG
static void WritePosToScreen(player_t *player);
#endif

void validate_angle(float *angle)
{
	while (*angle < -PI)
		*angle += TAU;
	while (*angle > PI)
		*angle -= TAU;
}

void player_step(player_t *player, vec2int input_vector)
{
	moving_entity_t *moving = &(player->moving);
	entity_t *entity = moving->entity;

	// Tangential speed
	moving->tangential_speed += input_vector.y;

	// Radial speed
	moving->radial_speed += input_vector.x * 0.01;
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

#define NUM_ENTITIES 200
#define NUM_ROAD_ENTITIES 1000
#define CAMERA_PLAYER_DISTANCE 256
#define CAMERA_RENDER_DISTANCE (4096)

canvas_t canvas = (canvas_t){
	.size = {800, 480}};

player_t player;
entity_t entities[NUM_ENTITIES];
entity_t road_entities_l[NUM_ROAD_ENTITIES];
int road_l_start = 0;
int road_l_end = 0;
entity_t road_entities_r[NUM_ROAD_ENTITIES];
int road_r_start = 0;
int road_r_end = 0;
entity_t *entity_pointers[NUM_ENTITIES];
int visible_count = NUM_ENTITIES;

int compare_distance_to_camera(const void *pa, const void *pb)
{
	entity_t *a = *((entity_t **)pa);
	entity_t *b = *((entity_t **)pb);

	if (!a->visible)
		return 1;
	if (!b->visible)
		return -1;

	if (a->distance_to_camera == b->distance_to_camera)
		return 0;
	else if (a->distance_to_camera < b->distance_to_camera)
		return -1;
	else
		return 1;
}

extern inline void kart_draw()
{
	qsort(entity_pointers, visible_count, sizeof(entity_t *), compare_distance_to_camera);

	for (int i = visible_count - 1; i >= 0; i--)
	{
		if (!entity_pointers[i]->visible)
		{
			continue;
		}

		spi_draw_sprite(entity_pointers[i]->draw_info);
	}
}

int set_entity_render_data(entity_t *entity, vec2 camera_pos, vec2 origin)
{
	entity->visible = 0;

	float distance_to_camera = distance_between(camera_pos, entity->position);

	entity->distance_to_camera = distance_to_camera;
	if (distance_to_camera > CAMERA_RENDER_DISTANCE)
	{
		return 1;
	}

	float distance_to_player = distance_between(player.moving.entity->position, entity->position);
	if (distance_to_player > distance_to_camera)
	{
		return 2;
	}

	float angle =
		fast_atan2_b(entity->position.y - camera_pos.y, entity->position.x - camera_pos.x) - player.moving.direction;
	validate_angle(&angle);

	if (angle < -PI_HALF || PI_HALF < angle)
	{
		return 3;
	}

	vec2 offset = {
		32 * fast_pow(distance_to_camera, 0.4) * sin(angle),
		16 * fast_pow(distance_to_camera, 0.35) * sin(angle - PI_HALF)};

	int x = origin.x + offset.x;
	int y = origin.y + offset.y;

	entity->draw_info.x = x;
	entity->draw_info.y = y;

	int border = 80; // TODO: why does it crash without this?
	if (x < border || canvas.size.x - border < x || y < border || canvas.size.y - border < y)
	{
		return 4;
	}

	int scale = round(clamp(10000 / fast_pow(distance_to_camera, 0.8), 1, 256));
	entity->draw_info.scale = scale;

	if (scale < 4)
	{
		return 5;
	}

	entity_pointers[visible_count] = entity;
	visible_count += 1;
	entity->visible = 1;
	return 0;
}

extern inline void kart_step(vec2int input_vector, int frames)
{
	for (int i = 0; i < frames; i++)
	{
		player_step(&player, input_vector);
	}

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

	for (int i = 0; i < NUM_ENTITIES; i++)
	{
		set_entity_render_data(&entities[i], camera_pos, origin);
	}

	for (int i = road_l_start; i < road_l_end; i++)
	{
		int res = set_entity_render_data(&road_entities_l[i], camera_pos, origin);
		if (res != 0)
			road_l_start = i;
	}
	int i = road_l_end;
	while (set_entity_render_data(&road_entities_l[i], camera_pos, origin) == 0)
	{
		road_l_end++;
		i++;
	}

	for (int i = road_r_start; i < road_r_end; i++)
	{
		int res = set_entity_render_data(&road_entities_r[i], camera_pos, origin);
		if (res != 0)
			road_r_start = i;
	}
	i = road_r_end;
	while (set_entity_render_data(&road_entities_r[i], camera_pos, origin) == 0)
	{
		road_r_end++;
		i++;
	}
}

void kart_init()
{
	for (int i = 0; i < NUM_ROAD_ENTITIES; i++)
	{
		road_entities_l[i].position.x = i * 512;
		road_entities_l[i].position.y = 0;
		road_entities_l[i].draw_info.sprite_id = 1;
		road_entities_l[i].draw_info.x = 0;
		road_entities_l[i].draw_info.y = 0;
		road_entities_l[i].draw_info.scale = 100;

		road_entities_r[i].position.x = i * 512;
		road_entities_r[i].position.y = 512;
		road_entities_r[i].draw_info.sprite_id = 1;
		road_entities_r[i].draw_info.x = 0;
		road_entities_r[i].draw_info.y = 0;
		road_entities_r[i].draw_info.scale = 100;
	}

	for (int i = 0; i < NUM_ENTITIES; i++)
	{
		entities[i].position.x = i * (128 + 64);
		entities[i].position.y = -100;
		if (i % 2 == 0)
		{
			entities[i].position.y += 712;
		}

		entities[i].draw_info.sprite_id = 1;
		entities[i].draw_info.x = 0;
		entities[i].draw_info.y = 0;
		entities[i].draw_info.scale = 100;

		entity_pointers[i] = &entities[i];
	}

	player.moving.entity = &entities[0];
	player.moving.direction = 0;
	player.moving.entity->draw_info.sprite_id = 0;
	player.moving.entity->position.x = -512;
	player.moving.entity->position.y = 256;
	player.moving.direction = 0.1;

	vec2int v = {0, 0};
	kart_step(v, 1);
	qsort(entity_pointers, NUM_ENTITIES, sizeof(entity_t *), compare_distance_to_camera);
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
