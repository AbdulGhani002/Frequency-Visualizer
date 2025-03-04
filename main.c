#include <stdio.h>
#include <unistd.h>
#include <raylib.h>

int main(void){
	InitWindow(800,600,"Frequency Visualizer");
	SetTargetFPS(90);
	InitAudioDevice();

	Music music = LoadMusicStream("./assets/alone.mp3");
	PlayMusicStream(music);

	while(!WindowShouldClose()){

		if(IsKeyPressed(KEY_SPACE)){
			if(IsMusicStreamPlaying(music)){
				PauseMusicStream(music);
			} else {
				ResumeMusicStream(music);
				}
}

		UpdateMusicStream(music);
		BeginDrawing();
		ClearBackground(GREEN);
		EndDrawing();
}
	return 0;
}
