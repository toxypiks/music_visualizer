#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <raylib.h>

void callback (void *bufferData, unsigned int frames)
{
  (void) bufferData;
  // printf("callback(frames = %u)\n", frames);
}

int main(void)
{
  InitWindow(800, 600, "Musializer");
  SetTargetFPS(60);

  InitAudioDevice();
  Music music = LoadMusicStream("../sounds/crystalcastles_air_war.mp3");
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

	BeginDrawing();
	ClearBackground(BLACK);
	EndDrawing();
  }

  return 0;
}
