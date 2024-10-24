#include <stdio.h>
#include "plug.h"
#include <raylib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define N (1<<10)

typedef struct {
  Music music;
  bool error;
} Plug;

Plug *plug = NULL;

float in1[N];
float in2[N];
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
  /*float a = fabs(crealf(z));
  float b = fabsf(cimagf(z));
  if (a < b) return b;*/
  float a = crealf(z);
  float b = cimagf(z);
  return logf(a*a + b*b);
}

void callback (void *bufferData, unsigned int frames)
{
  //pointer to array of 2 float [(float1), /float2), (float1), (float2),..]
  float (*fs)[2] = bufferData;

  for(size_t i = 0; i < frames; ++i) {
	memmove(in1, in1 + 1, (N - 1)*sizeof(in1[0]));
	in1[N-1] = fs[i][0];
  }
}

void plug_init(void)
{
  plug = malloc(sizeof(*plug));
  assert(plug != NULL && "Oops");
  memset(plug, 0, sizeof(*plug));
}

Plug *plug_pre_reload(void)
{
  if(IsMusicReady(plug->music)) {
	DetachAudioStreamProcessor(plug->music.stream, callback);
	return plug;
  }
}

void plug_post_reload(Plug *prev)
{
  plug = prev;
  if(IsMusicReady(plug->music)) {
    AttachAudioStreamProcessor(plug->music.stream, callback);
  }
}

void plug_update(void)
{
  if(IsMusicReady(plug->music)) {
	UpdateMusicStream(plug->music);
  }

	if(IsKeyPressed(KEY_SPACE)) {
	  if(IsMusicReady(plug->music)) {
	    if(IsMusicStreamPlaying(plug->music)) {
		PauseMusicStream(plug->music);
		} else {
		  ResumeMusicStream(plug->music);
		}
	  }
	}

	if(IsKeyPressed(KEY_Q)) {
	  if (IsMusicReady(plug->music)) {
		StopMusicStream(plug->music);
		PlayMusicStream(plug->music);
	  }
	}

	if(IsFileDropped()) {
	  FilePathList droppedFiles = LoadDroppedFiles();
	  if (droppedFiles.count > 0) {
		printf("NEW FILES JUST DROPPED!\n");
		const char *file_path = droppedFiles.paths[0];

		if(IsMusicReady(plug->music)) {
		  StopMusicStream(plug->music);
		  UnloadMusicStream(plug->music);
		}

		//weird
		plug->music = LoadMusicStream(file_path);

		if(IsMusicReady(plug->music)) {
		  plug->error = false;
		  printf("music.frameCount = %u\n", plug->music.frameCount);
		  printf("music.stream.sampleRate = %u\n", plug->music.stream.sampleRate);
		  printf("music.stream.sampleSize = %u\n", plug->music.stream.sampleSize);
		  printf("music.stream.channels = %u\n", plug->music.stream.channels);

		  SetMusicVolume(plug->music, 0.5f);
		  AttachAudioStreamProcessor(plug->music.stream, callback);
		  PlayMusicStream(plug->music);
		} else {
		  plug->error = true;
		}
	  }
	  UnloadDroppedFiles(droppedFiles);
	}

	int w = GetRenderWidth();
	int h = GetRenderHeight();

	BeginDrawing();
	ClearBackground(BLACK);

	if (IsMusicReady(plug->music)) {
	  for (size_t i = 0; i < N; ++i) {
		float t = (float)i/(N -1);
		float hann = 0.5 - 0.5*cosf(2*PI*t);
		in2[i] = in1[i]*hann;
	  }
	  fft(in2, 1, out, N);

	  float max_amp = 0.0f;
	  for (size_t i = 0; i < N; ++i) {
		float a = amp(out[i]);
		if (max_amp < a) max_amp = a;
	  }

	  float step = 1.06;
	  float lowf = 1.0f;
	  size_t m = 0;
	  for (float f = lowf; (size_t) f < N/2; f = ceilf(f*step)) {
		m += 1;
	  }

	  float cell_width = (float)w/m;
	  m = 0;
	  for (float f = lowf; (size_t) f < N/2; f = ceilf(f*step)) {
		float f1 = ceilf(f*step);
		float a = 0.0f;
		for (size_t q = (size_t) f; q < N/2 && q < (size_t) f1; ++q) {
		   float b = amp(out[q]);
		   if (b > a) a = b;
		}
		// a /= (size_t) f1 - (size_t) f + 1;
		float t = a/max_amp;
		DrawRectangle(m*cell_width, h - h/2*t, cell_width, h/2*t, BLUE);
		m += 1;
	  }
	}
	else {
	  const char *label;
	  Color color;
	  int height = 69;
	  if (plug->error) {
		label = "Could not load file";
		color = RED;
	  } else {
		label = "Drag&Drop Music Here";
		color = WHITE;
	  }
	  int width = MeasureText(label, height);
	  DrawText(label, w/2 - width/2, h/2 - height/2, height, color);
	}
	EndDrawing();
}
