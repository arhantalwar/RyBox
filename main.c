#include <math.h>
#include <stdlib.h>
#include <raylib.h>

#define WIDTH 1024 * 1.5
#define HEIGHT 900/2

#define KICK_NUM 8
#define SNARE_NUM 9
#define LONG_NUM 23

#define DIST WIDTH
#define SPEED 500
#define PERFECT_SPEED 30

#define BEAT_OFFSET (2.41)
#define OFFSCREEN_DIST -40

typedef enum BeatType {
    KICK,
    SNARE,
    LONG_HOLD,
} BeatType;

typedef struct BeatCheckerBox {
    BeatType Type;
    Rectangle Rec;
    Color color;
} BeatCheckerBox;

typedef struct BeatBox {
    BeatType Type;
    float BeatAt;
    Rectangle Rec;
    Color color;
    bool active;
    bool isPerfect;
} BeatBox;

BeatBox* GenerateBeatTiming(BeatType type, int num, float timings[num]) {

    BeatBox* beat_timings = (BeatBox*) malloc(sizeof(BeatBox) * num);

    for(int i = 0; i < num; i++) {

        beat_timings[i].Rec.x = DIST;
        beat_timings[i].Rec.width = 20;
        beat_timings[i].Rec.height = (float) HEIGHT/3;
        beat_timings[i].BeatAt = (float) timings[i] - BEAT_OFFSET;
        beat_timings[i].active = false;
        beat_timings[i].isPerfect = false;

        switch (type) {

            case KICK:

                beat_timings[i].Type = KICK;
                beat_timings[i].Rec.y = 0;
                beat_timings[i].color = ColorAlpha(PURPLE, 0.5);
                break;

            case SNARE:

                beat_timings[i].Type = SNARE;
                beat_timings[i].Rec.y = (float) HEIGHT/3;
                beat_timings[i].color = (Color){197, 33, 132, 255*0.5};
                break;

            case LONG_HOLD:

                beat_timings[i].Type = LONG_HOLD;
                beat_timings[i].Rec.y = (float) 2 * HEIGHT/3;
                beat_timings[i].color = ColorAlpha(ORANGE, 0.5);
                break;
                
            default:
                break;
        
        }
    }

    return beat_timings;

}

int main() {

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(WIDTH, HEIGHT, "GAME");
    InitAudioDevice();

    BeatCheckerBox KickChecker = {
        .Type = KICK,
        .Rec.x = 300,
        .Rec.y = 0,
        .Rec.width = 40,
        .Rec.height = (float) HEIGHT/3,
        .color = ColorAlpha(PURPLE, 0.3),
    };

    BeatCheckerBox SnareChecker = {
        .Type = SNARE,
        .Rec.x = 300,
        .Rec.y = (float) HEIGHT/3,
        .Rec.width = 40,
        .Rec.height = (float) HEIGHT/3,
        .color = (Color){229, 116, 188, 255},
    };

    BeatCheckerBox LongChecker = {
        .Type = LONG_HOLD,
        .Rec.x = 300,
        .Rec.y = (float) 2 * HEIGHT/3,
        .Rec.width = 40,
        .Rec.height = (float) HEIGHT/3,
        .color = ColorAlpha(ORANGE, 0.3),
    };

    float kick_timings[KICK_NUM] = {32.05, 33.67, 35.26, 36.97, 38.51, 40.13, 41.72, 43.31};
    BeatBox* KickRhythm = GenerateBeatTiming(KICK, KICK_NUM, kick_timings);

    float snare_timings[SNARE_NUM] = {32.88, 34.50, 36.10, 37.69, 39.35, 40.96, 42.56, 44.10, 46.35};
    BeatBox* SnareRhythm = GenerateBeatTiming(SNARE, SNARE_NUM, snare_timings);
    
    float long_timings[LONG_NUM] = {13.11, 13.92, 14.71, 15.51, 16.30, 17.11, 17.90, 18.71, 19.57, 20.38, 21.17, 21.96, 22.76,
    23.57, 24.36, 25.15, 26.03, 26.84, 27.63, 28.42, 29.23, 30.03, 30.80};
    BeatBox* LongRhythm = GenerateBeatTiming(LONG_HOLD, LONG_NUM, long_timings);

    Music track = LoadMusicStream("./Shine.wav");
    PlayMusicStream(track);

    bool isPerfectKick = false;
    bool isPerfectSnare = false;
    bool isPerfectLong = false;

    int KickRhythmCount = 0;
    int SnareRhythmCount = 0;
    int LongRhythmCount = 0;

    while(!WindowShouldClose()) {

        float deltaTime = GetFrameTime();
        float track_marker = GetMusicTimePlayed(track);

        ClearBackground(RAYWHITE);
        UpdateMusicStream(track);

        DrawRectangle(0, 0, WIDTH, HEIGHT/3, ColorAlpha(PURPLE, 0.2));
        DrawRectangle(0, HEIGHT/3, WIDTH, HEIGHT/3, (Color){197, 33, 132, 255*0.2});
        DrawRectangle(0, 2 * HEIGHT/3, WIDTH, HEIGHT/3, ColorAlpha(ORANGE, 0.2));

        BeginDrawing();

        DrawRectangle(KickChecker.Rec.x, KickChecker.Rec.y, KickChecker.Rec.width, KickChecker.Rec.height, KickChecker.color);

        for(int i = KickRhythmCount; i < KICK_NUM; i++) {

            if (!KickRhythm[i].active && fabs(track_marker - KickRhythm[i].BeatAt) < 0.05f) {
                KickRhythm[i].active = true;
            }

            isPerfectKick = CheckCollisionRecs(KickRhythm[i].Rec, KickChecker.Rec);

            if(isPerfectKick && IsKeyPressed(KEY_F)) {
                KickRhythm[i].color = ColorAlpha(PURPLE, 1);
                KickRhythm[i].isPerfect = true;
            }

            if (KickRhythm[i].active && KickRhythm[i].Rec.x > OFFSCREEN_DIST) {

                KickRhythm[i].Rec.x -= SPEED * deltaTime;

                if (KickRhythm[i].isPerfect) {
                    KickRhythm[i].Rec.width += PERFECT_SPEED * deltaTime;
                }

                DrawRectangleRounded(KickRhythm[i].Rec, 50, 10, KickRhythm[i].color);

            }

            if (KickRhythm[i].Rec.x < OFFSCREEN_DIST) {
                KickRhythmCount++;
            }

        }

        DrawRectangle(SnareChecker.Rec.x, SnareChecker.Rec.y, SnareChecker.Rec.width, SnareChecker.Rec.height, SnareChecker.color);

        for(int i = SnareRhythmCount; i < SNARE_NUM; i++) {

            if (!SnareRhythm[i].active && fabs(track_marker - SnareRhythm[i].BeatAt) < 0.05f) {
                SnareRhythm[i].active = true;
            }

            isPerfectSnare = CheckCollisionRecs(SnareRhythm[i].Rec, SnareChecker.Rec);

            if(isPerfectSnare && IsKeyPressed(KEY_J)) {
                SnareRhythm[i].color = (Color){197, 33, 132, 255};
                SnareRhythm[i].isPerfect = true;
            }

            if (SnareRhythm[i].active && SnareRhythm[i].Rec.x > OFFSCREEN_DIST) {

                SnareRhythm[i].Rec.x -= SPEED * deltaTime;

                if (SnareRhythm[i].isPerfect) {
                    SnareRhythm[i].Rec.width += PERFECT_SPEED * deltaTime;
                }

                DrawRectangleRounded(SnareRhythm[i].Rec, 50, 10, SnareRhythm[i].color);

            }

            if (SnareRhythm[i].Rec.x < OFFSCREEN_DIST) {
                SnareRhythmCount++;
            }

        }

        DrawRectangle(LongChecker.Rec.x, LongChecker.Rec.y, LongChecker.Rec.width, LongChecker.Rec.height, LongChecker.color);

        for(int i = LongRhythmCount; i < LONG_NUM; i++) {

            if (!LongRhythm[i].active && fabs(track_marker - LongRhythm[i].BeatAt) < 0.05f) {
                LongRhythm[i].active = true;
            }

            isPerfectLong = CheckCollisionRecs(LongRhythm[i].Rec, LongChecker.Rec);

            if(isPerfectLong && IsKeyPressed(KEY_K)) {
                LongRhythm[i].color = ColorAlpha(ORANGE, 1);
                LongRhythm[i].isPerfect = true;
            }

            if (LongRhythm[i].active && LongRhythm[i].Rec.x > OFFSCREEN_DIST) {

                LongRhythm[i].Rec.x -= SPEED * deltaTime;

                if (LongRhythm[i].isPerfect) {
                    LongRhythm[i].Rec.width += PERFECT_SPEED * deltaTime;
                }

                DrawRectangleRounded(LongRhythm[i].Rec, 50, 0, LongRhythm[i].color);

                if (LongRhythm[i].Rec.x < OFFSCREEN_DIST) {
                    LongRhythmCount++;
                }

            }


        }

        EndDrawing();

    }

    UnloadMusicStream(track);
    free(KickRhythm);
    free(SnareRhythm);
    free(LongRhythm);

    return 0;

}
