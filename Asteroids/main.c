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
#include <math.h>

//MACROS

//STRUCTS
struct Player {
    Vector2 position;
    Vector2 velocity;
};

struct Asteroid {
    Vector2 position;
    Vector2 velocity;
};

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Asteroids");
    SetTargetFPS(60);
    
    //CONTENT START
    float playerSize = 1.0;
    float h = (0.5 * playerSize) * tan((60.0 * (PI / 180.0)));

    int playerSpeed = 4;
    Vector2 startPosition = {screenWidth/2, screenHeight - (screenHeight/4)};
    struct Player player = {startPosition, (Vector2){0, 0}};

    int asteroidSpeed = 4;
    Vector2 asteroidStartPosition = {screenWidth/2, 0};
    struct Asteroid asteroid = {asteroidStartPosition, (Vector2){0, asteroidSpeed}};
    //CONTENT END
    
    while (!WindowShouldClose()) {

        //UPDATES
        if (IsKeyDown(KEY_RIGHT)) player.position.x += playerSpeed;
        else if (IsKeyDown(KEY_LEFT)) player.position.x -= playerSpeed;

        asteroid.position.y += asteroidSpeed;
        //END UPDATES

        BeginDrawing();
        ClearBackground(BLACK);
        
        //DRAW
        Vector2 v1 = {player.position.x, player.position.y - 20};
        Vector2 v2 = {player.position.x - 20, player.position.y + 20};
        Vector2 v3 = {player.position.x + 20, player.position.y + 20};
        DrawTriangle(v1, v2, v3, GREEN);

        DrawCircle(asteroid.position.x, asteroid.position.y, 20.0, DARKBROWN);
        //END DRAW

        EndDrawing();
    }

    CloseWindow();

    return 0;
}