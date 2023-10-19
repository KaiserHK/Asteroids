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


typedef struct Player {
    Vector2 position;
    float speed;
} Player;

typedef struct Asteroid {
    Vector2 position;
    float speed;
    float size;
} Asteroid;


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

    int n_asteroids = 10;
    Asteroid asteroids[n_asteroids];

    for (int i = 0; i < n_asteroids; i++) {
        asteroids[i] = (Asteroid){(Vector2){GetRandomValue(0, screenWidth), 0}, GetRandomValue(1, 5), GetRandomValue(5, 20)};
    }
    
    //GAME SETUP END
    
    while (!WindowShouldClose()) {

        //UPDATES
        scrolling += 0.175f;
        if (scrolling >= screenHeight) scrolling = 0;

        if (IsKeyDown(KEY_D)) player.position.x += player.speed;
        else if (IsKeyDown(KEY_A)) player.position.x -= player.speed;

        Rectangle playerCollisionBox = {player.position.x - 20, player.position.y - 20, 40, 40};
        Color playerColor = GREEN;
        
        for (int i = 0; i < n_asteroids; i++) {
            asteroids[i].position.y += asteroids[i].speed;

            if (CheckCollisionCircleRec(asteroids[i].position, asteroids[i].size, playerCollisionBox)) {
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

        //Draw Player
        Vector2 v1 = {player.position.x, player.position.y - 20};
        Vector2 v2 = {player.position.x - 20, player.position.y + 20};
        Vector2 v3 = {player.position.x + 20, player.position.y + 20};
        DrawRectangleRec(playerCollisionBox, BLUE);
        DrawTriangle(v1, v2, v3, playerColor);

        //Draw Asteroids
        for (int i = 0; i < n_asteroids; i++) {
            DrawCircle(asteroids[i].position.x, asteroids[i].position.y, asteroids[i].size, DARKBROWN);
        }
        //END DRAW

        EndDrawing();
    }
    
    UnloadTexture(noise_texture);
    UnloadTexture(perlin_texture);
    CloseWindow();

    return 0;
}