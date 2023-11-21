
#define PI 3.14159265358979323846

extern inline float fast_sin_f(float x)
{
    const float B = 4 / PI;
    const float C = -4 / (PI*PI);

    return -(B * x + C * x * ((x < 0) ? -x : x));
} 
