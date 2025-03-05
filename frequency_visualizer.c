#include <stdio.h>
#include <raylib.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ARRAY_LENGTH(xs) (sizeof(xs)/sizeof(xs[0]))

int16_t global_frames[4096] = {0}; // Store audio as int16_t (signed 16-bit PCM)
size_t global_frames_count = 0;

void callback(void* bufferData, unsigned int frames) {
    if (frames * 2 > ARRAY_LENGTH(global_frames)) {
        frames = ARRAY_LENGTH(global_frames) / 2;
    }

    memcpy(global_frames, bufferData, frames * sizeof(int16_t) * 2); // Copy stereo audio
    global_frames_count = frames * 2; // Total number of samples (left + right)
}

int main(void) {
    freopen("output.log", "w", stdout);
    InitWindow(800, 600, "Frequency Visualizer");
    SetTargetFPS(90);
    InitAudioDevice();

    Music music = LoadMusicStream("./assets/alone.mp3");

    assert(music.stream.sampleSize == 32); // Ensuring 32-bit sample size
    assert(music.stream.channels == 2); // Ensuring stereo audio

    PlayMusicStream(music);
    AttachAudioStreamProcessor(music.stream, callback);

    float volume = 0.5f;
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
            if (volume > 1.0f) volume = 1.0f;
            SetMusicVolume(music, volume);
        }
        
        if (IsKeyPressed(KEY_DOWN)) {
            volume -= 0.1f;
            if (volume < 0.0f) volume = 0.0f;
            SetMusicVolume(music, volume);
        }

        UpdateMusicStream(music);

        BeginDrawing();
        ClearBackground((Color){0x18, 0x18, 0x18, 0xFF});

        DrawText("Press SPACE to Play/Pause", 20, 20, 20, WHITE);
        DrawText("Press UP to Increase Volume", 20, 50, 20, WHITE);
        DrawText("Press DOWN to Decrease Volume", 20, 80, 20, WHITE);
        DrawText(TextFormat("Volume: %.2f", volume), 20, 110, 20, WHITE);

        int width = GetRenderWidth();
        int height = GetRenderHeight();
        
        if (global_frames_count > 0) {
            float cell_width = (float) width / (global_frames_count / 2); // Use half because of stereo channels
            
            for (size_t i = 0; i < global_frames_count; i += 2) { // Step by 2 to account for stereo
                int16_t left_sample = global_frames[i];  // Left channel
                int16_t right_sample = global_frames[i + 1]; // Right channel
                
                // Compute amplitude ratio
                float left_t = (float) left_sample / (float) INT16_MAX;
                float right_t = (float) right_sample / (float) INT16_MAX;

                // Draw Left channel (upper part)
                DrawRectangle(i/2 * cell_width, height / 2 - (height / 2 * left_t), cell_width, height / 2 * left_t, RED);

                // Draw Right channel (lower part)
                DrawRectangle(i/2 * cell_width, height / 2, cell_width, height / 2 * right_t, BLUE);
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
