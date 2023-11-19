/* uncomment the next line if you're on a big-endian system */
/* #define BIG_ENDIAN */

/* uncomment the next line if you can not assume double-precision FPU or IEEE754 */
/* #define NO_FAST_TRUNCATE */

/* we need to do some custom hacking for MSVC */
#ifdef _MSC_VER
typedef __int32 int32_t;
#else
#include <stdint.h>
#endif

extern inline int32_t fast_round(double x) {
	#ifndef NO_FAST_TRUNCATE

	const double MAGIC_ROUND = 6755399441055744.0; /* http://stereopsis.com/sree/fpu2006.html */

	union {
	double d;

	struct {
	#ifdef BIG_ENDIAN
	int32_t hw;
	int32_t lw;
	#else
	int32_t lw;
	int32_t hw;
	#endif
	};
	} fast_trunc;

	fast_trunc.d = x;
	fast_trunc.d += MAGIC_ROUND;

	return fast_trunc.lw;

	#else

	if (x < 0) {
	return (int32_t) (x - 0.5);
	} else {
	return (int32_t) (x + 0.5);
	}

	#endif
}

extern inline double fast_sin(double x) {
	const double PI = 3.14159265358979323846264338327950288;
	const double INVPI = 0.31830988618379067153776752674502872;
	const double A = 0.00735246819687011731341356165096815;
	const double B = -0.16528911397014738207016302002888890;
	const double C = 0.99969198629596757779830113868360584;

	int32_t k;
	double x2;

	/* find offset of x from the range -pi/2 to pi/2 */
	k = fast_round(INVPI * x);

	/* bring x into range */
	x -= k * PI;

	/* calculate sine */
	x2 = x * x;
	x = x*(C + x2*(B + A*x2));

	/* if x is in an odd pi count we must flip */
	if (k % 2) x = -x;

	return x;
}
