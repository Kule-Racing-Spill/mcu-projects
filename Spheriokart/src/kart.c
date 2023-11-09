#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "trackball.h"
#include "segmentlcd.h"
#include "spi.h"

// Maths

#define M_PI 3.14159265358979323846

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

float distance_between(vec2 a, vec2 b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrt(dx * dx + dy * dy);
}

float clamp(float value, float min, float max)
{
    if (value > max)
        return max;
    if (value < min)
        return min;
    return value;
}

// Input

int get_input(char c)
{
    return 1; // TODO!
}

// Sprite

#define SPRITE_MARIO_RIGHT 0
#define SPRITE_MARIO_UP 1
#define SPRITE_MARIO_LEFT 2

// Entities

struct entity_t
{
    vec2 position;
    int sprite_index;
} typedef entity_t;

struct moving_entity_t
{
    entity_t entity;
    float direction;
    float radial_speed;     // change in direction = x * some factor
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
static void WritePosToScreen(player_t* player);


void print_player_debug(player_t player)
{
    moving_entity_t moving = player.moving;
    vec2 position = moving.entity.position;
}

int8_t prevValueX;
int8_t prevValueY;

void player_step(player_t *player, TrackballValues v)
{
    moving_entity_t* moving = &(player->moving);
    entity_t* entity = &(moving->entity);

	moving->tangential_speed = clamp(moving->tangential_speed,
	                                    -PLAYER_MAX_TANGENTIAL_SPEED, PLAYER_MAX_TANGENTIAL_SPEED);
	moving->tangential_speed *= PLAYER_TANGENTIAL_DAMPING;
    moving->radial_speed = clamp(moving->radial_speed,
                                -PLAYER_MAX_RADIAL_SPEED, PLAYER_MAX_RADIAL_SPEED);
	moving->radial_speed *= PLAYER_RADIAL_DAMPING;

    if(prevValueX == v.x || prevValueY == v.y) return;

    // Tangential speed
    entity->sprite_index = SPRITE_MARIO_UP;
	moving->tangential_speed += v.y;

    // Movement

    entity->position.x += cos(moving->direction) * moving->tangential_speed;
    entity->position.y += sin(moving->direction) * moving->tangential_speed;

    // Radial speed
    entity->sprite_index = SPRITE_MARIO_RIGHT;
	moving->radial_speed += (v.x * 0.01);
    moving->direction += moving->radial_speed;

    while (moving->direction < -M_PI)
        moving->direction += 2 * M_PI;
    while (moving->direction > M_PI)
        moving->direction -= 2 * M_PI;

    prevValueX = v.x;
	prevValueY = v.y;
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

void draw_background(canvas_t canvas)
{
    int sky_height = canvas.size.y / 3;
    draw_rect(0, 0, canvas.size.x, sky_height, "blue");
    draw_rect(0, sky_height, canvas.size.x, canvas.size.y, "green");
}

void draw_sprite(int sprite_index, vec2 position, float scale)
{
	sprite_draw_info sprite_info = (sprite_draw_info) {
		.scale = scale,
		.sprite_id = sprite_index,
		.x = position.x,
		.y = position.y,
	};

	spi_draw_sprite(sprite_info);
}

// Program

#define NUM_ENTITIES 1

canvas_t canvas = (canvas_t) {
	.size = {800, 480}
};

player_t player;
entity_t entities[NUM_ENTITIES];
int camera_distance = 256;

int kart(TrackballValues v)
{
	print_player_debug(player);

	draw_background(canvas);

	vec2 player_pos = player.moving.entity.position;
	vec2 camera_pos = {
		player_pos.x -
			camera_distance * cos(player.moving.direction),
		player_pos.y -
			camera_distance * sin(player.moving.direction)};

	player_step(&player, v);
	WritePosToScreen(&player);
	// TODO: Sort entities by distance to camera.

	for (int i = 0; i < NUM_ENTITIES; i++)
	{
		// entity_t entity = entities[i];
		entity_t entity = player.moving.entity;
		float angle =
			atan2(entity.position.y - camera_pos.y, entity.position.x - camera_pos.x) -
			player.moving.direction;

		while (angle > M_PI)
			angle -= 2 * M_PI;
		while (angle < -M_PI)
			angle += 2 * M_PI;
		if (!(-M_PI / 2 < angle && angle < M_PI / 2))
			continue;
		;

		float distance = distance_between(camera_pos, entity.position);
		float scale = clamp(100 / powf(distance, 0.8), 0.01, 2.5);
		scale = round(scale * 100) / 100;

		vec2 origin = {
			canvas.size.x / 2 - 16,
			canvas.size.y};

		vec2 offset = {
			32 * powf(distance, 0.4) * cos(angle - M_PI / 2),
			16 * powf(distance, 0.3) * sin(angle - M_PI / 2)};

		draw_sprite(
			entity.sprite_index,
			vec2_add(origin, offset),
			scale);
	}

    return 0;
}

static void WritePosToScreen(player_t* player){
	float x = player->moving.radial_speed;
	float y = player->moving.tangential_speed;
	float direction = player->moving.direction;
	float position = player->moving.entity.position.x;

	// Determine pad direction
	if (x == 0 && y == 0) {
		// No movement detected
		return;
	}

	// Display direction and values
	SegmentLCD_Number(round(direction));
	SegmentLCD_LowerNumber(round(position));
}
