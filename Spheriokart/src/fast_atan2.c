#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846

// src: https://gist.github.com/volkansalma/2972237
float fast_atan2(float y, float x) {
	float abs_y = fabs(y) + 1e-10f;      // kludge to prevent 0/0 condition
	float r = (x - copysign(abs_y, x)) / (abs_y + fabs(x));
	float angle = PI/2.f - copysign(PI/4.f, x);

	angle += (0.1963f * r * r - 0.9817f) * r;
	return copysign(angle, y);
}
