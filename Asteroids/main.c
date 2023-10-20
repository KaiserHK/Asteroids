/*******************************************************************************************
*
*   raylib [core] example - Basic 3d example
*
*   Welcome to raylib!
*
*   To compile example, just press F5.
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <math.h>


#define SCOREPATH "score.txt"


typedef struct Player {
    Vector2 position;
    float speed;
} Player;

typedef struct Asteroid {
    Vector2 position;
    float speed;
    float size;
} Asteroid;

typedef enum GameState {
    MENU = 0,
    SCOREBOARD,
    INGAME,
    GAMEOVER
} GameState;


int saveScore(float score) {
    FILE *file = fopen(SCOREPATH, "w");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "FILE WAS NOT OPENED DURING SAVE");
        return 1;
    }
    fprintf(file, "%0.2f", score);
    fclose(file);
    return 0;
}

float loadScore() {
    FILE *file = fopen(SCOREPATH, "r");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "FILE WAS NOT OPENED DURING LOAD");
    }
    float score = 0.0;
    fscanf(file, "%f", &score);
    fclose(file);
    return score;
}


int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Asteroids");
    SetTargetFPS(60);
    
    //GAME SETUP
    Image white = GenImageWhiteNoise(screenWidth, screenHeight, 0.01f);
    Image perlin = GenImagePerlinNoise(screenWidth, screenHeight, 50, 50, 4.0f);
    Texture2D noise_texture = LoadTextureFromImage(white);
    Texture2D perlin_texture = LoadTextureFromImage(perlin);
    UnloadImage(white);
    UnloadImage(perlin);
    float scrolling = 0.0;

    Player player = {(Vector2){screenWidth/2, screenHeight - (screenHeight/4)}, 4};
    float score = 0.0;
    int hit = 0;

    int n_asteroids = 10;
    Asteroid asteroids[n_asteroids];

    for (int i = 0; i < n_asteroids; i++) {
        asteroids[i] = (Asteroid){(Vector2){GetRandomValue(0, screenWidth), 0}, GetRandomValue(1, 5), GetRandomValue(5, 20)};
    }
    
    GameState state = MENU;

    float storedScore = 0.0;
    int hasStoredScoreBeenReadFromFile = 0;
    //GAME SETUP END
    
    while (!WindowShouldClose()) {

        switch (state) {

            case MENU: {
                if (IsKeyDown(KEY_ENTER)) state = INGAME;
                if (IsKeyDown(KEY_S)) state = SCOREBOARD;
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("Press Enter to Start", (screenWidth / 2) - MeasureText("Press Enter to Start", 20), (screenHeight / 2), 40, LIGHTGRAY);
                EndDrawing();
            } break;
            
            case SCOREBOARD: {
                if (!hasStoredScoreBeenReadFromFile) {
                    storedScore = loadScore();
                    hasStoredScoreBeenReadFromFile = 1;
                }
                if (IsKeyDown(KEY_R)) {
                    state = MENU;
                    hasStoredScoreBeenReadFromFile = 0;
                }
                BeginDrawing();
                ClearBackground(BLACK);
                char score[128];
                snprintf(score, 128, "LAST SCORE: %0.2f", storedScore);
                DrawText(score, (screenWidth/2) - MeasureText(score, 20), (screenHeight/2), 40, LIGHTGRAY);
                EndDrawing();
            } break;

            case INGAME: {
                //UPDATES
                score += player.speed;

                scrolling += 0.175f;
                if (scrolling >= screenHeight) scrolling = 0;

                if (IsKeyDown(KEY_D)) player.position.x += player.speed;
                else if (IsKeyDown(KEY_A)) player.position.x -= player.speed;

                if (player.position.x >= screenWidth) player.position.x = screenWidth;
                else if (player.position.x <= 0) player.position.x = 0;

                Vector2 v1 = {player.position.x, player.position.y - 20};
                Vector2 v2 = {player.position.x - 20, player.position.y + 20};
                Vector2 v3 = {player.position.x + 20, player.position.y + 20};

                Rectangle playerCollisionBox = {player.position.x - 20, player.position.y - 20, 40, 40};
                Color playerColor = GREEN;
                
                for (int i = 0; i < n_asteroids; i++) {
                    asteroids[i].position.y += asteroids[i].speed;
                    double theta = atan(((asteroids[i].position.y - player.position.y)/(asteroids[i].position.x - player.position.x)));
                    double dx = asteroids[i].size * cos(theta);
                    double dy = asteroids[i].size * sin(theta);

                    if (asteroids[i].position.x < player.position.x) {
                        dx = -1 * dx;
                        dy = -1 * dy;
                    }

                    double x3 = asteroids[i].position.x - dx;
                    double y3 = asteroids[i].position.y - dy;

                    if (CheckCollisionPointTriangle((Vector2){x3, y3}, v1, v2, v3)) {
                        hit = 1;
                        playerColor = RED;
                    }

                    if (asteroids[i].position.y >= screenHeight) {
                        asteroids[i].position = (Vector2){GetRandomValue(0, screenWidth), 0};
                        asteroids[i].speed = GetRandomValue(1, 5);
                        asteroids[i].size = GetRandomValue(5, 20);
                    }
                }
                //END UPDATES

                BeginDrawing();
                ClearBackground(BLACK);
                
                //DRAW

                //Draw Background
                DrawTextureEx(noise_texture, (Vector2){0, scrolling}, 0.0, 1.0f, WHITE);
                BeginBlendMode(BLEND_MULTIPLIED);
                DrawTextureEx(perlin_texture, (Vector2){0, scrolling}, 0.0, 1.0f, WHITE);
                EndBlendMode();

                DrawTextureEx(noise_texture, (Vector2){0, (-1 * screenHeight) + scrolling}, 0.0, 1.0f, WHITE);
                BeginBlendMode(BLEND_MULTIPLIED);
                DrawTextureEx(perlin_texture, (Vector2){0, (-1 * screenHeight) + scrolling}, 0.0, 1.0f, WHITE);
                EndBlendMode();

                //Draw Score
                char scoreString[128];
                snprintf(scoreString, 128, "Score: %0.2f", score);
                DrawText(scoreString, 10, 10, 20, WHITE);

                //Draw Player
                DrawTriangle(v1, v2, v3, playerColor);

                //Draw Asteroids
                for (int i = 0; i < n_asteroids; i++) {
                    DrawCircle(asteroids[i].position.x, asteroids[i].position.y, asteroids[i].size, DARKBROWN);
                }

                //Draw End Game
                if (hit) {
                    state = GAMEOVER;

                    saveScore(score);

                    hit = 0;
                    scrolling = 0;
                    score = 0;

                    player.position = (Vector2){screenWidth/2, screenHeight - (screenHeight/4)};

                    for (int i = 0; i < n_asteroids; i++) {
                        asteroids[i] = (Asteroid){(Vector2){GetRandomValue(0, screenWidth), 0}, GetRandomValue(1, 5), GetRandomValue(5, 20)};
                    }
                }

                //END DRAW
                EndDrawing();
            } break;

            case GAMEOVER: {
                if (IsKeyDown(KEY_R)) {
                    state = MENU;
                }

                BeginDrawing();
                DrawText("GAME OVER", (screenWidth/2) - (MeasureText("GAME OVER", 20)), (screenHeight/2), 40, RED);
                DrawText("Press R for Main Menu", (screenWidth/2) - (MeasureText("Press R for Main Menu", 10)), (screenHeight/2) + 60, 20, RED);
                EndDrawing();
            } break;

            default: break;
        }
    }
    
    UnloadTexture(noise_texture);
    UnloadTexture(perlin_texture);
    CloseWindow();

    return 0;
}