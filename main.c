#include <stdio.h>
#include <raylib.h>

void callback (void* bufferData, unsigned int frames){
	(void) bufferData;
	(void) frames;
	printf("Callback Frames: %u\n", frames);
}

int main(void){
	freopen("output.log", "w", stdout);
	InitWindow(800, 600, "Frequency Visualizer");
	SetTargetFPS(90);
    	InitAudioDevice();

    	Music music = LoadMusicStream("./assets/alone.mp3");
    	PlayMusicStream(music);
    	AttachAudioStreamProcessor(music.stream,callback);


    	float volume = 0.5f; // Storing volume separately
    	SetMusicVolume(music, volume); 

    	while (!WindowShouldClose()) {
    
        	if (IsKeyPressed(KEY_SPACE)) {
        
            		if (IsMusicStreamPlaying(music)) {
                		PauseMusicStream(music);
            		} else {
                		ResumeMusicStream(music);
            		}
        	}

        if (IsKeyPressed(KEY_UP)) {
        	volume += 0.1f;
            	if (volume > 1.0f){ 
            		volume = 1.0f;
            	} // Max Value of volume that can be passed is 1
            	SetMusicVolume(music, volume);
        }
        
        if (IsKeyPressed(KEY_DOWN)) {
            	volume -= 0.1f;
            	if (volume < 0.0f){ 
            		volume = 0.0f; // Minimum value of volume that can be passed is 0 
            	}
            	SetMusicVolume(music, volume);
        }

        UpdateMusicStream(music);
        
        BeginDrawing();
        ClearBackground(GREEN);
        DrawText("Press SPACE to Play/Pause", 20, 20, 20, WHITE);
        DrawText("Press UP to Increase Volume", 20, 50, 20, WHITE);
        DrawText("Press DOWN to Decrease Volume", 20, 80, 20, WHITE);
        DrawText(TextFormat("Volume: %.2f", volume), 20, 110, 20, WHITE);
        EndDrawing();
    }

    // Cleaning up resources
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    fflush(stdout);
    return 0;
}
