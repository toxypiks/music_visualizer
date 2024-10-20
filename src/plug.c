#include <stdio.h>
#include "plug.h"
#include <raylib.h>
#include <assert.h>
#include <math.h>

float in[N];
float complex out[N];

typedef struct {
  float left;
  float right;
} Frame;

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
	float complex v = cexp(-2*I*PI*t)*out[k + n/2];
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
}

void plug_hello(void)
{
  printf("Hello from Plugin\n");
}

void plug_init(Plug *plug, const char *file_path)
{
  plug->music = LoadMusicStream(file_path);
  assert(plug->music.stream.sampleSize == 32);
  assert(plug->music.stream.channels == 2);
  printf("music.frameCount = %u\n", plug->music.frameCount);
  printf("music.stream.sampleRate = %u\n", plug->music.stream.sampleRate);
  printf("music.stream.sampleSize = %u\n", plug->music.stream.sampleSize);
  printf("music.stream.channels = %u\n", plug->music.stream.channels);

  PlayMusicStream(plug->music);
  AttachAudioStreamProcessor(plug->music.stream, callback);
}

void plug_update(Plug *plug)
{
  UpdateMusicStream(plug->music);

	if(IsKeyPressed(KEY_SPACE)) {
	  if(IsMusicStreamPlaying(plug->music)) {
		PauseMusicStream(plug->music);
	  } else {
		ResumeMusicStream(plug->music);
	  }
	}

	int w = GetRenderWidth();
	int h = GetRenderHeight();

	BeginDrawing();
	ClearBackground(BLACK);

	fft(in, 1, out, N);

  float max_amp = 0.0f;
  for (size_t i = 0; i < N; ++i) {
	float a = amp(out[i]);
	if (max_amp < a) max_amp = a;
  }

  float cell_width = (float)w/N;
	for (size_t i = 0; i < N; ++i) {
	  float t = amp(out[i])/max_amp;
	  DrawRectangle(i*cell_width, h/2 - h/2*t, cell_width, h/2*t, RED);
	}
	EndDrawing();
}
