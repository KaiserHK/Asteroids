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
#define BUFSIZE 128
#define NAMESIZE 3


typedef struct Player {
    Vector2 position;
    float speed;
    char name[NAMESIZE];
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
    GAMEOVER,
    NEWHIGHSCORE
} GameState;

typedef struct Scoreboard {
    int ranks[5];
    float scores[5];
    char names[5][NAMESIZE + 1];
} Scoreboard;


int loadHighScores(Scoreboard *scoreboard) {
    FILE *file = fopen(SCOREPATH, "r");
    if (file != NULL) {
        char buf[BUFSIZE];
        int counter = 0;
        while (fgets(buf, sizeof(buf), file) && counter < 5) {
            sscanf(buf, "%d %f %s", &scoreboard->ranks[counter], &scoreboard->scores[counter], scoreboard->names[counter]);
            counter++;
        }
        
    } else {
        TraceLog(LOG_ERROR, "FILE WAS NOT OPENED DURING LOAD");
    }
    fclose(file);
    return 0;
}

int saveHighScores(Scoreboard *scoreboard) {
    FILE *file = fopen(SCOREPATH, "w");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "FILE WAS NOT OPENED DURING SAVE");
        return 1;
    }
    for (int i = 0; i < 5; i++) {
        fprintf(file, "%d %0.2f %s\n", scoreboard->ranks[i], scoreboard->scores[i], scoreboard->names[i]);
    }
    fclose(file);
    return 0;
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "AstroRocks");
    SetTargetFPS(60);
    
    //GAME SETUP
    Image white = GenImageWhiteNoise(screenWidth, screenHeight, 0.01f);
    Image perlin = GenImagePerlinNoise(screenWidth, screenHeight, 50, 50, 4.0f);
    Texture2D noise_texture = LoadTextureFromImage(white);
    Texture2D perlin_texture = LoadTextureFromImage(perlin);
    UnloadImage(white);
    UnloadImage(perlin);
    float scrolling = 0.0;

    Image playerImage = LoadImage("AstroRocks Spaceship V2.png");
    Texture2D playerTexture = LoadTextureFromImage(playerImage);
    UnloadImage(playerImage);
    float playerTextureScale = 1.35;
    Rectangle frameRectangle = {0, 0, (float)playerTexture.width/2, (float)playerTexture.height/2};
    Rectangle destRectangle = {0, 0, ((float)playerTexture.width/2) * playerTextureScale, ((float)playerTexture.height/2) * playerTextureScale};
    Player player = {(Vector2){screenWidth/2, screenHeight - (screenHeight/4)}, 4};
    float score = 0.0;
    int hit = 0;
    int playerNameLetterIndex = 0;

    int n_asteroids = 10;
    Asteroid asteroids[n_asteroids];

    for (int i = 0; i < n_asteroids; i++) {
        asteroids[i] = (Asteroid){(Vector2){GetRandomValue(0, screenWidth), 0}, GetRandomValue(1, 5), GetRandomValue(5, 20)};
    }
    
    GameState state = MENU;
    Scoreboard scoreboard = {0};
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
                    loadHighScores(&scoreboard);
                    hasStoredScoreBeenReadFromFile = 1;
                }
                if (IsKeyDown(KEY_R)) {
                    state = MENU;
                    hasStoredScoreBeenReadFromFile = 0;
                }
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("High Scores", (screenWidth/2) - MeasureText("High Scores", 20), (screenHeight/4) - 80, 40, LIGHTGRAY);
                int bufsize = 128;
                for (int i = 0; i < 5; i++) {
                    char buf[bufsize];
                    snprintf(buf, bufsize, "%d %0.2f %s", scoreboard.ranks[i], scoreboard.scores[i], scoreboard.names[i]);
                    DrawText(buf, (screenWidth/2) - MeasureText(buf, 20), (screenHeight/4) + (i * 40), 40, LIGHTGRAY);
                }
                EndDrawing();
            } break;

            case INGAME: {
                //UPDATES
                score += player.speed;

                scrolling += 0.175f;
                if (scrolling >= screenHeight) scrolling = 0;

                frameRectangle.x = 0;
                frameRectangle.y = 0;
                if (IsKeyDown(KEY_D)) {
                    player.position.x += player.speed;
                    frameRectangle.y = playerTexture.height/2;
                }
                else if (IsKeyDown(KEY_A)) {
                    player.position.x -= player.speed;
                    frameRectangle.x = playerTexture.width/2;
                }

                if (player.position.x >= screenWidth) player.position.x = screenWidth;
                else if (player.position.x <= 0) player.position.x = 0;

                destRectangle.x = player.position.x - (destRectangle.width / 2);
                destRectangle.y = player.position.y - (destRectangle.height / 2);

                Vector2 v1 = {player.position.x, player.position.y - 20};
                Vector2 v2 = {player.position.x - 20, player.position.y + 20};
                Vector2 v3 = {player.position.x + 20, player.position.y + 20};

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
                //DrawTextureEx(playerTexture, (Vector2){player.position.x - (16 * playerTextureScale), player.position.y - (16 * playerTextureScale)}, 0.0, playerTextureScale, WHITE);
                //DrawTextureRec(playerTexture, frameRectangle, (Vector2){player.position.x - 16, player.position.y - 16}, WHITE);
                DrawTexturePro(playerTexture, frameRectangle, destRectangle, (Vector2){0, 0}, 0, WHITE);

                //Draw Asteroids
                for (int i = 0; i < n_asteroids; i++) {
                    DrawCircle(asteroids[i].position.x, asteroids[i].position.y, asteroids[i].size, DARKBROWN);
                }

                EndDrawing();

                if (hit) {
                    state = GAMEOVER;
                    loadHighScores(&scoreboard);
                    for (int i = 0; i < 5; i++) {
                        if (score >= scoreboard.scores[i]) {
                            state = NEWHIGHSCORE;
                        }
                    }

                    hit = 0;
                    scrolling = 0;
                    player.position = (Vector2){screenWidth/2, screenHeight - (screenHeight/4)};
                    for (int i = 0; i < n_asteroids; i++) {
                        asteroids[i] = (Asteroid){(Vector2){GetRandomValue(0, screenWidth), 0}, GetRandomValue(1, 5), GetRandomValue(5, 20)};
                    }
                }
            } break;

            case NEWHIGHSCORE: {
                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= 32) && (key <= 125) && (playerNameLetterIndex < NAMESIZE)) {
                        player.name[playerNameLetterIndex] = (char)key;
                        player.name[playerNameLetterIndex + 1] = '\0';
                        playerNameLetterIndex++;
                    }
                    key = GetCharPressed();
                }

                if (IsKeyPressed(KEY_BACKSPACE) && (playerNameLetterIndex >= 0)) {
                    player.name[playerNameLetterIndex] = '\0';
                    playerNameLetterIndex--;
                } else if (IsKeyPressed(KEY_ENTER)) {
                    loadHighScores(&scoreboard);
                    for (int i = 0; i < 5; i++) {
                        if (score >= scoreboard.scores[i]) {
                            for (int j = 3; j >= i; j--) {
                                scoreboard.scores[j + 1] = scoreboard.scores[j];
                                for (int k = 0; k < NAMESIZE; k++) {
                                    scoreboard.names[j + 1][k] = scoreboard.names[j][k];
                                }
                            }
                            scoreboard.scores[i] = score;
                            for (int k = 0; k < NAMESIZE; k++) {
                                scoreboard.names[i][k] = player.name[k];
                            }
                            break;
                        }
                    }
                    saveHighScores(&scoreboard);

                    score = 0;
                    state = SCOREBOARD;
                }

                char buf[BUFSIZE];
                snprintf(buf, sizeof(buf), "Enter Initials: %s", player.name);
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("New High Score!", (screenWidth/2) - (MeasureText("New High Score", 20)), (screenHeight/4), 40, GOLD);
                DrawText(buf, (screenWidth/2) - (MeasureText(buf, 20)), (screenHeight/2), 40, LIGHTGRAY);
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
    UnloadTexture(playerTexture);
    CloseWindow();

    return 0;
}