#include <stdio.h>

#ifndef TRACKBALL_H
#define TRACKBALL_H

typedef struct {
	int8_t x;
	int8_t y;
} TrackballValues;

void runTrackballExample(void);
void InitTrackball(void);
void GetTrackballValues(TrackballValues* coords);
void InitUSBHStack(void);

#endif // TRACKBALL_H
