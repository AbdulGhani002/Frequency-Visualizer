#include <stdio.h>
#include <raylib.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ARRAY_LENGTH(xs) (sizeof(xs) / sizeof(xs[0]))
#define MAX_FRAMES 2048

int16_t global_frames[MAX_FRAMES] = {0};
size_t global_frames_count = 0;

void callback(void* bufferData, unsigned int frames) {
    int16_t* samples = (int16_t*)bufferData;
    size_t sampleCount = frames * 2;

    if (sampleCount > MAX_FRAMES) {
        sampleCount = MAX_FRAMES;
    }

    memcpy(global_frames, samples, sampleCount * sizeof(int16_t));
    global_frames_count = sampleCount / 2;
}

int main(void) {
    freopen("output.log", "w", stdout);
    InitWindow(800, 600, "Frequency Visualizer");
    SetTargetFPS(60);
    InitAudioDevice();

    Music music = LoadMusicStream("./assets/alone.mp3");
    assert(music.stream.sampleSize == 32);
    assert(music.stream.channels == 2);
    
    PlayMusicStream(music);
    AttachAudioStreamProcessor(music.stream, callback);

    float volume = 0.5f;
    SetMusicVolume(music, volume);
    
    printf("music.frameCount = %u\n", music.frameCount);
    printf("music.stream.sampleRate = %u\n", music.stream.sampleRate);
    printf("music.stream.channels = %u\n", music.stream.channels);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) {
            if (IsMusicStreamPlaying(music)) {
                PauseMusicStream(music);
            } else {
                ResumeMusicStream(music);
            }
        }

        if (IsKeyPressed(KEY_UP)) {
            volume = (volume < 1.0f) ? volume + 0.1f : 1.0f;
            SetMusicVolume(music, volume);
        }
        
        if (IsKeyPressed(KEY_DOWN)) {
            volume = (volume > 0.0f) ? volume - 0.1f : 0.0f;
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
        float cell_width = (float)width / (float)(global_frames_count > 0 ? global_frames_count : 1);

        for (size_t i = 0; i < global_frames_count; i++) {
            int16_t sample = global_frames[i * 2];
            float t = (float)sample / INT16_MAX;

            if (sample > 0) {
                DrawRectangle(i * cell_width, height / 2 - height / 2 * t, cell_width, height / 2 * t, RED);
            } else {
                DrawRectangle(i * cell_width, height / 2, cell_width, -height / 2 * t, GREEN);
            }
        }
        
        EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    fflush(stdout);
    
    return 0;
}
