#include <stdio.h>
#include "plug.h"
#include <raylib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <rlgl.h>

#define N (1<<10)

typedef struct {
  Music music;
  bool error;
  Shader circle;
} Plug;

Plug *plug = NULL;

float in_raw[N];
float in_win[N];
float complex out_raw[N];
float out_log[N];
float out_smooth[N];

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
  float a = crealf(z);
  float b = cimagf(z);
  return logf(a*a + b*b);
}

void callback (void *bufferData, unsigned int frames)
{
  //pointer to array of 2 float [(float1), /float2), (float1), (float2),..]
  float (*fs)[2] = bufferData;

  for(size_t i = 0; i < frames; ++i) {
	memmove(in_raw, in_raw + 1, (N - 1)*sizeof(in_raw[0]));
	in_raw[N-1] = fs[i][0];
  }
}

void plug_init(void)
{
  plug = malloc(sizeof(*plug));
  assert(plug != NULL && "Oops");
  memset(plug, 0, sizeof(*plug));

  plug->circle = LoadShader(NULL, "../shaders/circle.fs");
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
  UnloadShader(plug->circle);
  plug->circle = LoadShader(NULL, "../shaders/circle.fs");
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

	float dt = GetFrameTime();

	BeginDrawing();
	ClearBackground(BLACK);

	if (IsMusicReady(plug->music)) {
	  for (size_t i = 0; i < N; ++i) {
		float t = (float)i/(N -1);
		float hann = 0.5 - 0.5*cosf(2*PI*t);
		in_win[i] = in_raw[i]*hann;
	  }

	  // FFT
	  fft(in_win, 1, out_raw, N);

	  float step = 1.06;
	  float lowf = 1.0f;
	  size_t m = 0;
	  float max_amp = 1.0f;

	  // "Squash" into the Logarithmic Scale
	  for(float f = lowf; (size_t) f < N/2; f = ceilf(f*step)) {
		float f1 = ceilf(f*step);
		float a = 0.0f;
		for(size_t q = (size_t) f; q < N/2 && q < (size_t) f1; ++q) {
		  float b = amp(out_raw[q]);
		  if (b >a) a = b;
		}
		if(max_amp < a) max_amp = a;
		out_log[m++] = a;
	  }

	  // Normalize Frequencies to 0..1 range
	  for (size_t i = 0; i < m; ++i) {
		out_log[i] /= max_amp;
	  }

	  float smoothness = 8;
	  for(size_t i = 0; i < m; ++i) {
		out_smooth[i] += (out_log[i] - out_smooth[i])*smoothness*dt;
	  }

	  float cell_width = (float)w/m;

      //Display the Bars
	  for (size_t i = 0; i < m; ++i) {
		float hue = (float)i/m;
		float t = out_smooth[i];
		float saturation = 0.75f;
		float value = 1.0f;
		Color color = ColorFromHSV(hue*360, saturation, value);
		Vector2 start_pos = {
		  i*cell_width + cell_width/2,
		  h - h*2/3*t,
		};

		Vector2 end_pos = {
		  i*cell_width + cell_width/2,
		  h,
		};
		float thick = cell_width/2*sqrtf(t);
		DrawLineEx(start_pos, end_pos, thick, color);
	  }

	  Texture2D texture = {rlGetTextureIdDefault(), 1, 1, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

      //Display circles
      BeginShaderMode(plug->circle);
	  for (size_t i = 0; i < m; ++i) {
		float hue = (float)i/m;
		float t = out_smooth[i];
		float saturation = 0.75f;
		float value = 1.0f;
		Color color = ColorFromHSV(hue*360, saturation, value);
		Vector2 center = {
		  i*cell_width + cell_width/2,
		  h - h*2/3*t,
		};
		float radius = cell_width*5*sqrtf(t);
		/*Rectangle rec = {
		  .x = center.x - radius,
		  .y = center.y - radius,
		  .width = 2*radius,
		  .height = 2*radius,
		  };*/
		Vector2 position = {
		  .x = center.x - radius,
		  .y = center.y - radius,
		};
	    DrawTextureEx(texture, position, 0, 2*radius, color);
		// DrawRectangleRec(rec, color);
	  }
	  EndShaderMode();
	}else {
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
