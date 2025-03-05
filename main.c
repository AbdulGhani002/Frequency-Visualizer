#include <stdio.h>
#include <raylib.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define ARRAY_LENGTH(xs) sizeof(xs)/sizeof(xs[0])

// uint32_t global_frames[2048] = NULL;

uint32_t global_frames[2048] = {0};
size_t global_frames_count = 0;

void callback (void* bufferData, unsigned int frames){

	if(frames > ARRAY_LENGTH(global_frames)){
		frames = ARRAY_LENGTH(global_frames);
	}

	memcpy(global_frames, bufferData, frames*sizeof(int16_t)*2);
	global_frames_count = frames * 2;

	// (void) bufferData;

	// int16_t* frames = bufferData;
	// (void) frames;
	// printf("Callback Frames: %u\n", frames);
}

int main(void){
	freopen("output.log", "w", stdout);
	InitWindow(800, 600, "Frequency Visualizer");
	SetTargetFPS(90);
    	InitAudioDevice();

    	Music music = LoadMusicStream("./assets/alone.mp3");

    	assert(music.stream.sampleSize == 32); // 32 bit sample size is permitted only for now...
    	assert(music.stream.channels == 2); // 2 channels are permitted. I don't know what are channels for now!!!
    	PlayMusicStream(music);
    	AttachAudioStreamProcessor(music.stream,callback);


    	float volume = 0.5f; // Storing volume separately
    	SetMusicVolume(music, volume); 
    	printf("music.frameCount= %u\n", music.frameCount);
    	printf("music.stream.sampleRate= %u\n", music.stream.sampleRate);
    	printf("music.stream.channels= %u\n", music.stream.channels);

    	

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
        // ClearBackground(GREEN);

	ClearBackground(CLITERAL(Color) {0x18, 0x18, 0x18, 0xFF});
        
        DrawText("Press SPACE to Play/Pause", 20, 20, 20, WHITE);
        DrawText("Press UP to Increase Volume", 20, 50, 20, WHITE);
        DrawText("Press DOWN to Decrease Volume", 20, 80, 20, WHITE);
        DrawText(TextFormat("Volume: %.2f", volume), 20, 110, 20, WHITE);

	int width = GetRenderWidth(); // GetRenderWidth gives width of screen
	int height = GetRenderHeight(); // GetRenderHeight gives height of screen
	
	float cell_width = (float) width / (float) global_frames_count; // GetRenderWidth is the width of screen
	
	for(size_t i = 0; i < global_frames_count; i++){
		int16_t sample = *(int16_t*)&global_frames[i];

		if (sample > 0){
			float t = (float) sample / (float) INT16_MAX;
			DrawRectangle(i*cell_width, height / 2 - height / 2 * t, cell_width, height / 2 * t, RED);
		} else {
			float t = (float) sample /  (float) INT16_MIN;
			DrawRectangle(i*cell_width, 0, cell_width, height / 2 * t, RED); 
			}
			
		printf("%d\n", sample);
	}
	printf("global_frames_count: %zu\n", global_frames_count);
        EndDrawing();
    }

    // Cleaning up resources
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    fflush(stdout);
    if (global_frames_count > 0) {
	exit(1);
    	}
    return 0;
}
