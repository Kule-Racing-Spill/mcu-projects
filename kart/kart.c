#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

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
    float radial_speed;     // change in direction
    float tangential_speed; // change in position along the current direction
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

void print_player_debug(player_t player)
{
    moving_entity_t moving = player.moving;
    vec2 position = moving.entity.position;
    printf("Player[x:%f, y:%f, dir:%f, speed:%f]", position.x,
           position.y, moving.direction, moving.tangential_speed);
}

void player_step(player_t *player)
{
    moving_entity_t moving = player->moving;
    entity_t entity = moving.entity;
    vec2 position = moving.entity.position;

    // Tangential speed

    if (get_input('w'))
    {
        entity.sprite_index = SPRITE_MARIO_UP;
        moving.tangential_speed += PLAYER_TANGENTIAL_ACC;
    }
    if (get_input('s'))
    {
        entity.sprite_index = SPRITE_MARIO_UP;
        moving.tangential_speed -= PLAYER_TANGENTIAL_ACC;
    }
    moving.tangential_speed = clamp(moving.tangential_speed,
                                    -PLAYER_MAX_TANGENTIAL_SPEED, PLAYER_MAX_TANGENTIAL_SPEED);
    if (
        (moving.tangential_speed > 0 && !get_input('w')) ||
        (moving.tangential_speed < 0 && !get_input('s')))
    {
        moving.tangential_speed *= PLAYER_TANGENTIAL_DAMPING;
    }

    // Movement

    entity.position.x += cos(moving.direction) * moving.tangential_speed;
    entity.position.y += sin(moving.direction) * moving.tangential_speed;

    // Radial speed

    if (get_input('d'))
    {
        entity.sprite_index = SPRITE_MARIO_RIGHT;
        moving.radial_speed += PLAYER_RADIAL_ACC;
    }
    if (get_input('a'))
    {
        entity.sprite_index = SPRITE_MARIO_LEFT;
        moving.radial_speed -= PLAYER_RADIAL_ACC;
    }

    moving.radial_speed = clamp(moving.radial_speed,
                                -PLAYER_MAX_RADIAL_SPEED, PLAYER_MAX_RADIAL_SPEED);
    if (
        (moving.radial_speed > 0 && !get_input('d')) ||
        (moving.radial_speed < 0 && !get_input('a')))
    {
        moving.radial_speed *= PLAYER_RADIAL_DAMPING;
    }
    moving.direction += moving.radial_speed;

    while (moving.direction < -M_PI)
        moving.direction += 2 * M_PI;
    while (moving.direction > M_PI)
        moving.direction -= 2 * M_PI;
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

void draw_image(int sprite_index, vec2 position, float scale)
{
    // TODO!
}

// Program

#define NUM_ENTITIES 50

int main(void)
{
    canvas_t canvas;
    player_t player;
    entity_t entities[NUM_ENTITIES];
    int camera_distance = 256;

    while (1)
    {
        if (get_input('q'))
        {
            break;
        }

        print_player_debug(player);

        draw_background(canvas);

        vec2 player_pos = player.moving.entity.position;
        vec2 camera_pos = {
            player_pos.x -
                camera_distance * cos(player.moving.direction),
            player_pos.y -
                camera_distance * sin(player.moving.direction)};

        // TODO: Sort entities by distance to camera.

        for (int i = 0; i < NUM_ENTITIES; i++)
        {
            entity_t entity = entities[i];
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

            draw_image(
                entity.sprite_index,
                vec2_add(origin, offset),
                scale);
        }
    }

    return 0;
}
