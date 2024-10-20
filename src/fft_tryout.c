#include <stdio.h>
#include <math.h>

float pi;

int main (void)
{
  pi = atan2f(1, 1)*4; //vector of 45 degree mulitplied by 4

  size_t n = 8;
  float in[n];
  float out[n];

  for(size_t i = 0; i < n; ++i) {
	float t = (float)i/n;
	in[i] = sinf(2*pi*t*1) + sinf(2*pi*t*3);
  }

  for(size_t f = 0; f < n; ++f) {
	out[f] = 0;
	for (size_t i = 0; i < n; ++i) {
	  float t = (float)i/n;
	  out[f] += in[i]*sinf(2*pi*f*t);
	}
  }

  for (size_t f = 0; f < n; ++f) {
	printf("%02zu: %.2f\n", f, out[f]);
  }

  /*for (size_t i = 0; i < n; ++i) {
	printf("%f\n", in[i]);
	}*/
  return 0;
}
