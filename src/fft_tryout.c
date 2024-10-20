#include <stdio.h>
#include <math.h>

int main (void)
{
  double pi = atan2f(1, 1)*4; //vector of 45 degree mulitplied by 4
  printf("%f\n", pi);

  size_t n = 8;
  float in[n];

  for(size_t i = 0; i < n; ++i) {
	float t = (float)i/n;
	in[i] = sinf(2*pi*t);
  }
  return 0;
}
