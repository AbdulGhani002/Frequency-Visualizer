#include <stdio.h> // Standard Input Output library
#include <raylib.h> // Library for graphics and audio
#include <assert.h> // Assertion Testing
#include <stdlib.h> // Standard Library Functions (memcpy)
#include <stdint.h> // uint32_t , int16_t (Fixed-width integer types)
#include <string.h> // String Manipulation Functions

#define ARRAY_LENGTH(xs) (sizeof(xs)/sizeof(xs[0]))

int16_t global_frames[4096] = {0}; // Store audio as int16_t (signed 16-bit PCM)

// uint32_t global_frames[2048] = NULL;

size_t global_frames_count = 0; // Stores number of valid audio samples

void callback(void* bufferData, unsigned int frames) {
    // This function is automatically called by Raylib when new audio frames are available
    // bufferData is a pointer to the audio data (void* means a generic pointer that can point to any type of data).
    // frames is the number of audio frames in the buffer
    if (frames * 2 > ARRAY_LENGTH(global_frames)) {
    	// To avoid buffer overflow we are limiting the number of frames copied
        frames = ARRAY_LENGTH(global_frames) / 2;
    }

    memcpy(global_frames, bufferData, frames * sizeof(int16_t) * 2); // Copy stereo audio. This copies audio buffer into global_frames. Since the audio is stereo (left + right), each frame has 2 samples so we are multipling 2 by number of frames
    global_frames_count = frames * 2; // Total number of samples (left + right)
    // (void) bufferData;

    // int16_t* frames =  bufferData;
    // (void) frames;
    // printf("Callback Frames: %u\n", frames);
}

int main(void) {
    freopen("output.log", "w", stdout); // Redirects stdout to file named output.log. So all printf and other errors will be stored in that file because I'm using WSL and it cant use windows audio card so to run this code I develop it in WSL and then compiles it in windows and then run it in windows and then my WSL terminal cant capture the output of printf statements and any error occured
    InitWindow(800, 600, "Frequency Visualizer");
    SetTargetFPS(90);
    InitAudioDevice();

    Music music = LoadMusicStream("./assets/alone.mp3");

    assert(music.stream.sampleSize == 32); // Ensuring 32-bit sample size
    assert(music.stream.channels == 2); // Ensuring stereo audio

    PlayMusicStream(music);
    AttachAudioStreamProcessor(music.stream, callback);

    float volume = 0.5f; // Storing volume Separately
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
            if (volume > 1.0f) {
            	volume = 1.0f; 
            	} // Max value of volume that is allowed
            SetMusicVolume(music, volume);
        }
        
        if (IsKeyPressed(KEY_DOWN)) {
            volume -= 0.1f;
            if (volume < 0.0f) {
            	volume = 0.0f;
            	} // Minimum value of volume that is allowed
            SetMusicVolume(music, volume);
        }

        UpdateMusicStream(music);

        BeginDrawing();
        // ClearBackground(GREEN);
        ClearBackground((Color){0x18, 0x18, 0x18, 0xFF});

        DrawText("Press SPACE to Play/Pause", 20, 20, 20, WHITE);
        DrawText("Press UP to Increase Volume", 20, 50, 20, WHITE);
        DrawText("Press DOWN to Decrease Volume", 20, 80, 20, WHITE);
        DrawText(TextFormat("Volume: %.2f", volume), 20, 110, 20, WHITE);

        int width = GetRenderWidth(); // Returns width of screen
        int height = GetRenderHeight(); // Returns height of screen
        
        if (global_frames_count > 0) {
            float cell_width = (float) width / (global_frames_count / 2); // Calculating width of each visulaized sample. Using half because of stereo channels
            
            for (size_t i = 0; i < global_frames_count; i += 2) { // Step by 2 to account for stereo
            // Extracting left and right samples and normalizing the samples from 1 to -1.
                int16_t left_sample = global_frames[i];  // Left channel
                int16_t right_sample = global_frames[i + 1]; // Right channel
                
                // Compute amplitude ratio
                float left_t = (float) left_sample / (float) INT16_MAX;
                float right_t = (float) right_sample / (float) INT16_MAX;

                // Draw Left channel (upper part)
                DrawRectangle(i/2 * cell_width, height / 2 - (height / 2 * left_t), cell_width, height / 2 * left_t, RED); // Red bar represents the left channel (upper half part of screen)

                // Draw Right channel (lower part)
                DrawRectangle(i/2 * cell_width, height / 2, cell_width, height / 2 * right_t, BLUE); // Blue bar represnets the right side of channel (lower part of screen)
            }
        }

        EndDrawing();
    }

    // Cleanup
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    fflush(stdout);
    
    return 0;
}
