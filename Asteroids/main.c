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

//MACROS

//STRUCTS
struct Player {
    Vector2 position;
    float speed;
};

struct Asteroid {
    Vector2 position;
    float speed;
};

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Asteroids");
    SetTargetFPS(60);
    
    //CONTENT START
    float playerSpeed = 4;
    Vector2 startPosition = {screenWidth/2, screenHeight - (screenHeight/4)};
    struct Player player = {startPosition, playerSpeed};

    float asteroidSpeed = 4;
    float asteroidSize = 20;
    Vector2 asteroidStartPosition = {screenWidth/2, 0};
    struct Asteroid asteroid = {asteroidStartPosition, asteroidSpeed};
    //CONTENT END
    
    while (!WindowShouldClose()) {

        //UPDATES
        if (IsKeyDown(KEY_RIGHT)) player.position.x += player.speed;
        else if (IsKeyDown(KEY_LEFT)) player.position.x -= player.speed;

        asteroid.position.y += asteroid.speed;
        if (asteroid.position.y >= screenHeight) asteroid.position = (Vector2){GetRandomValue(0, screenWidth), 0};

        Rectangle playerCollisionBox = {player.position.x - 20, player.position.y - 20, 40, 40};
        Color playerColor = GREEN;
        if (CheckCollisionCircleRec(asteroid.position, asteroidSize, playerCollisionBox)) {
            playerColor = RED;
        }
        //END UPDATES

        BeginDrawing();
        ClearBackground(BLACK);
        
        //DRAW
        Vector2 v1 = {player.position.x, player.position.y - 20};
        Vector2 v2 = {player.position.x - 20, player.position.y + 20};
        Vector2 v3 = {player.position.x + 20, player.position.y + 20};

        DrawRectangleRec(playerCollisionBox, BLUE);
        DrawTriangle(v1, v2, v3, playerColor);
        DrawCircle(asteroid.position.x, asteroid.position.y, asteroidSize, DARKBROWN);
    
        //END DRAW

        EndDrawing();
    }

    CloseWindow();

    return 0;
}