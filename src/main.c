#include <stdio.h>
#include <unistd.h>
#include <raylib.h>


int main(void)
{
  InitWindow(800, 600, "Musializer");
  SetTargetFPS(60);

  InitAudioDevice();
  Music music = LoadMusicStream("../sounds/crystalcastles_air_war.mp3");
  PlayMusicStream(music);

  while(!WindowShouldClose()) {
	UpdateMusicStream(music);

	BeginDrawing();
	ClearBackground(BLACK);
	EndDrawing();
  }

  return 0;
}
