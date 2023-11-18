#include <stdio.h>

#ifndef TRACKBALL_H
#define TRACKBALL_H

typedef struct
{
	int8_t x;
	int8_t y;
} vec2int;

void runTrackballExample(void);
void InitTrackball(void);
void GetTrackballValues(vec2int *coords);
void InitUSBHStack(void);

#endif // TRACKBALL_H
