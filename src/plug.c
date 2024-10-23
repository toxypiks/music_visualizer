#include <stdio.h>
#include "plug.h"
#include <raylib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define N (1<<13)

typedef struct {
  Music music;
} Plug;

Plug *plug = NULL;

float in[N];
float complex out[N];

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
  //pointer to array of 2 float [(float1), /float2), (float1), (float2),..]
  float (*fs)[plug->music.stream.channels] = bufferData;

  for(size_t i = 0; i < frames; ++i) {
	memmove(in, in + 1, (N - 1)*sizeof(in[0]));
	in[N-1] = fs[i][0];
  }
}

void plug_init(const char *file_path)
{
  plug = malloc(sizeof(*plug));
  assert(plug != NULL && "Oops");
  memset(plug, 0, sizeof(*plug));


  plug->music = LoadMusicStream(file_path);
  printf("music.frameCount = %u\n", plug->music.frameCount);
  printf("music.stream.sampleRate = %u\n", plug->music.stream.sampleRate);
  printf("music.stream.sampleSize = %u\n", plug->music.stream.sampleSize);
  printf("music.stream.channels = %u\n", plug->music.stream.channels);

  SetMusicVolume(plug->music, 0.5f);
  PlayMusicStream(plug->music);
  AttachAudioStreamProcessor(plug->music.stream, callback);
}

Plug *plug_pre_reload(void)
{
  DetachAudioStreamProcessor(plug->music.stream, callback);
  return plug;
}

void plug_post_reload(Plug *prev)
{
  plug = prev;
  AttachAudioStreamProcessor(plug->music.stream, callback);
}

void plug_update(void)
{
  UpdateMusicStream(plug->music);

	if(IsKeyPressed(KEY_SPACE)) {
	  printf("yeah\n");
	  if(IsMusicStreamPlaying(plug->music)) {
		PauseMusicStream(plug->music);
	  } else {
		ResumeMusicStream(plug->music);
	  }
	}

	if(IsFileDropped()) {
	  FilePathList droppedFiles = LoadDroppedFiles();
	  if (droppedFiles.count > 0) {
		printf("NEW FILES JUST DROPPED!\n");
		const char *file_path = droppedFiles.paths[0];
		StopMusicStream(plug->music);
		UnloadMusicStream(plug->music);
		plug->music = LoadMusicStream(file_path);

		printf("music.frameCount = %u\n", plug->music.frameCount);
        printf("music.stream.sampleRate = %u\n", plug->music.stream.sampleRate);
        printf("music.stream.sampleSize = %u\n", plug->music.stream.sampleSize);
        printf("music.stream.channels = %u\n", plug->music.stream.channels);

		SetMusicVolume(plug->music, 0.5f);
        PlayMusicStream(plug->music);
        AttachAudioStreamProcessor(plug->music.stream, callback);
	  }
	  UnloadDroppedFiles(droppedFiles);
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

  float step = 1.06;
  size_t m = 0;
  for (float f = 20.0f; (size_t) f < N; f *= step) {
	m += 1;
  }

  float cell_width = (float)w/m;
  m = 0;
  for (float f = 20.0f; (size_t) f < N; f *= step) {
	float f1 = f*step;
	float a = 0.0f;
	for (size_t q = (size_t) f; q < N && q < (size_t) f1; ++q) {
	  a += amp(out[q]);
	}
	a /= (size_t) f1 - (size_t) f + 1;
	float t = a/max_amp;
	DrawRectangle(m*cell_width, h/2 - h/2*t, cell_width, h/2*t, BLUE);
	m += 1;
  }

  EndDrawing();
}
