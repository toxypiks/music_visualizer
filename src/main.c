#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <raylib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <complex.h>
#include <math.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

#define N 256

float pi;
float in [N];
float complex out[N];
float max_amp;

typedef struct {
  float left;
  float right;
} Frame;

Frame global_frames[4800] = {0};
size_t global_frames_count = 0;

void fft(float in[], size_t stride, float complex out[], size_t n)
{
  assert(n > 0);

  if (n == 1) {
	out[0] = in[0];
	return;
  }

  fft(in, stride*2, out, n/2);
  fft(in + stride, stride*2, out + n/2, n/2);

  for (size_t k = 0; k < n/2; ++k) {
	float t = (float) k/n;
	float complex v = cexp(-2*I*pi*t)*out[k + n/2];
	float complex e = out[k];
	out[k] = e + v;
	out[k + n/2] = e - v;
  }
}

float amp (float complex z)
{
  float a = fabs(crealf(z));
  float b = fabsf(cimagf(z));
  if (a < b) return b;
  return a;
}

void callback (void *bufferData, unsigned int frames)
{
  if (frames < N) return;

  Frame *fs = bufferData;

  for(size_t i = 0; i < frames; ++i) {
	in[i] = fs[i].left;
  }

  fft(in, 1, out, N);

  max_amp = 0.0f;
  for (size_t i = 0; i < frames; ++i) {
	float a = amp(out[i]);
	if (max_amp < a) max_amp = a;
  }
}

int main(void)
{
  pi = atan2f(1, 1)*4;
  InitWindow(800, 600, "Musializer");
  SetTargetFPS(60);

  InitAudioDevice();
  Music music = LoadMusicStream("../sounds/crystalcastles_air_war.mp3");
  assert(music.stream.sampleSize == 32);
  assert(music.stream.channels == 2);
  printf("music.frameCount = %u\n", music.frameCount);
  printf("music.stream.sampleRate = %u\n", music.stream.sampleRate);
  printf("music.stream.sampleSize = %u\n", music.stream.sampleSize);
  printf("music.stream.channels = %u\n", music.stream.channels);

  PlayMusicStream(music);

  AttachAudioStreamProcessor(music.stream, callback);

  while(!WindowShouldClose()) {
	UpdateMusicStream(music);

	if(IsKeyPressed(KEY_SPACE)) {
	  if(IsMusicStreamPlaying(music)) {
		PauseMusicStream(music);
	  } else {
		ResumeMusicStream(music);
	  }
	}

	int w = GetRenderWidth();
	int h = GetRenderHeight();

	BeginDrawing();
	ClearBackground(BLACK);
	float cell_width = (float)w/N;
	for (size_t i = 0; i < N; ++i) {
	  float t = amp(out[i])/max_amp;
	  DrawRectangle(i*cell_width, h/2 - h/2*t, cell_width, h/2*t, RED);
	}
	EndDrawing();
  }

  return 0;
}
