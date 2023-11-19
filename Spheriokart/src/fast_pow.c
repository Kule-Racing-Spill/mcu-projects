
// src: https://martin.ankerl.com/2007/10/04/optimized-pow-approximation-for-java-and-c-c/
extern inline double fast_pow(double a, double b) {
  // calculate approximation with fraction of the exponent
  int e = (int) b;
  union {
	  double d;
    int x[2];
  } u = { a };
  u.x[1] = (int)((b - e) * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;

  // exponentiation by squaring with the exponent's integer part
  // double r = u.d makes everything much slower, not sure why
  double r = 1.0;
  while (e) {
    if (e & 1) {
      r *= a;
    }
    a *= a;
    e >>= 1;
  }

  return r * u.d;
}
