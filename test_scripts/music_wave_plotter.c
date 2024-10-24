#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <raylib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))


#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

typedef struct {
  float left;
  float right;
} Frame;

Frame global_frames[48000] = {0};
size_t global_frames_count = 0;

void callback (void *bufferData, unsigned int frames)
{
  size_t capacity = ARRAY_LEN(global_frames);
  if(frames <= capacity - global_frames_count) {
	memcpy(global_frames + global_frames_count, bufferData, sizeof(Frame)*frames);
	global_frames_count += frames;
  } else if (frames <= capacity) {
	memmove(global_frames, global_frames + frames, sizeof(Frame)*(capacity - frames));
	memcpy(global_frames + (capacity - frames), bufferData, sizeof(Frame)*frames);
  } else {
	memcpy(global_frames, bufferData, sizeof(Frame)* capacity);
	global_frames_count = capacity;
  }
}

int main(void)
{
  InitWindow(1600, 600, "Musializer");
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
	size_t capacity = ARRAY_LEN(global_frames);
	float cell_width = (float)GetRenderWidth()/capacity;

	for (size_t i = 0; i < capacity; ++i) {
	  float t = global_frames[i].left;
	  if (t > 0) {
		DrawPixel(i * cell_width, h/2 - h/2*t, RED);
	  } else {
		DrawPixel(i * cell_width, h/2 - h/2*t, BLUE);
	  }
	}
	EndDrawing();
  }
  return 0;
}
