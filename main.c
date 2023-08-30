#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "libs/structs.h"
#include "libs/mechanics.h"
#include "libs/session.h"

Sound laserShoot;
Sound moleHit;
Sound playerMove;
Sound playerDeath;
Sound levelUp;
Sound moleHitPlayer; 

int main() {
    // Initialize the game
    const int screenWidth = MAP_WIDTH * TILE_SIZE;
    const int screenHeight = MAP_HEIGHT * TILE_SIZE;

    InitAudioDevice();
    laserShoot = LoadSound("sounds/laserShoot.wav");
    moleHit = LoadSound("sounds/moleHit.wav");
    playerMove = LoadSound("sounds/playerMove.wav");
    playerDeath = LoadSound("sounds/playerDeath.wav");
    levelUp = LoadSound("sounds/levelUp.wav");
    moleHitPlayer = LoadSound("sounds/moleHitPlayer.wav");

    InitWindow(screenWidth, screenHeight, "MineSlug");
    SetTargetFPS(8);

    Game session;
    bool gameRunning = false;
    bool loaded = false;

    MenuState currentMenu = MENU_MAIN;
    MenuItem selectedMenuItem = MENU_NONE;

    while (!WindowShouldClose()) {
        // Detect mouse position and clicks
        Vector2 mousePoint = GetMousePosition();

        // Handle input and menus
        if (!gameRunning) {
            // Main menu
            if (currentMenu == MENU_MAIN) {
                selectedMenuItem = MENU_NONE;

                Rectangle newGameRect = { 10, 40, MeasureText("1. New Game", 20), 20 };
                Rectangle loadGameRect = { 10, 70, MeasureText("2. Load Game", 20), 20 };

                if (CheckCollisionPointRec(mousePoint, newGameRect)) {
                    selectedMenuItem = MENU_NEW_GAME;
                } else if (CheckCollisionPointRec(mousePoint, loadGameRect)) {
                    selectedMenuItem = MENU_LOAD_GAME;
                }

                BeginDrawing();
                ClearBackground(RAYWHITE);

                DrawText("MineSlug", 10, 10, 20, BLACK);
                DrawText("New Game", 10, 40, 20, (selectedMenuItem == MENU_NEW_GAME) ? RED : DARKGRAY);
                DrawText("Load Game", 10, 70, 20, (selectedMenuItem == MENU_LOAD_GAME) ? RED : DARKGRAY);
                DrawText("Press ESC to quit game", 10, 200, 20, RED);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (selectedMenuItem == MENU_NEW_GAME) {
                        gameRunning = true;
                        resetGame(&session);
                        LoadGameMap(1, &session);
                    } else if (selectedMenuItem == MENU_LOAD_GAME) {
                        gameRunning = true;
                        loadGame(&session);
                    }
                }

                if (IsKeyPressed(KEY_TAB)) {
                    if (gameRunning) {
                        currentMenu = MENU_SECONDARY;
                    }
                }

                EndDrawing();
        }}
            // Secondary menu
            else if (IsKeyPressed(KEY_TAB) || (currentMenu == MENU_SECONDARY)) {
                currentMenu = MENU_SECONDARY;
                selectedMenuItem = MENU_NONE;

                Rectangle loadGameRect = { 10, 40, MeasureText("1. Load Game", 20), 20 };
                Rectangle saveGameRect = { 10, 70, MeasureText("2. Save Game", 20), 20 };
                Rectangle backRect = { 10, 100, MeasureText("3. Back", 20), 20 };

                if (CheckCollisionPointRec(mousePoint, loadGameRect)) {
                    selectedMenuItem = MENU_LOAD_GAME;
                } else if (CheckCollisionPointRec(mousePoint, saveGameRect)) {
                    selectedMenuItem = MENU_SAVE_GAME;
                } else if (CheckCollisionPointRec(mousePoint, backRect)) {
                    selectedMenuItem = MENU_BACK;
                }

                BeginDrawing();
                ClearBackground(RAYWHITE);

                DrawText("MineSlug", 10, 10, 20, BLACK);
                DrawText("Load Game", 10, 40, 20, (selectedMenuItem == MENU_LOAD_GAME) ? RED : DARKGRAY);
                DrawText("Save Game", 10, 70, 20, (selectedMenuItem == MENU_SAVE_GAME) ? RED : DARKGRAY);
                DrawText("Back", 10, 100, 20, (selectedMenuItem == MENU_BACK) ? RED : DARKGRAY);
                DrawText("Press ESC to quit game", 10, 200, 20, RED);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (selectedMenuItem == MENU_LOAD_GAME) {
                        if(loadGame(&session))
                            currentMenu = MENU_NONE;
                        else
                            DrawText("Failed to load game", 10, 40, 100, RED);
                    } else if (selectedMenuItem == MENU_SAVE_GAME) {
                        if(saveGame(&session))
                            DrawText("Game saved", 10, 70, 100, RED);
                        else
                            DrawText("Failed to save game", 10, 70, 100, RED);
                    } else if (selectedMenuItem == MENU_BACK) {
                        currentMenu = MENU_NONE;
                    }
                }

                EndDrawing();
        } else {
            // Game loop
            updateGame(&session);
            BeginDrawing();
            
            ClearBackground(RAYWHITE);
            loaded = DrawGameMap(session);
            
            // Desenha elementos
            drawLaser(session);
            drawObject(session.player.position, RED);
            for (int i = 0; i < session.mole_num; i++) {
                int distx = (session.player.position.x - session.moles[i].position.x) / TILE_SIZE, 
                    disty = (session.player.position.y - session.moles[i].position.y) / TILE_SIZE;
                int distsqr = (distx * distx) + (disty * disty);
                if (session.moles[i].isAlive && ((distsqr <= 81) || session.powered) ) {
                    drawObject(session.moles[i].position, PURPLE);
                }
            }

            DrawText(TextFormat("Level: %i", session.player.level), 1 * TILE_SIZE, 10, 20, WHITE);
            DrawText(TextFormat("Lives: %i", session.player.lives), 4 * TILE_SIZE, 10, 20, WHITE);
            DrawText(TextFormat("Score: %i", session.player.score), 7 * TILE_SIZE, 10, 20, WHITE);
            DrawText(TextFormat("Emeralds: %i", session.player.emeralds), 23 * TILE_SIZE, 10, 20, DARKGREEN);
            DrawText(TextFormat("Golds: %i", session.player.golds), 27 * TILE_SIZE, 10, 20, GOLD);
            DrawText(TextFormat("Total Emeralds: %i", session.total_emeralds), 25 * TILE_SIZE, 19.25 * TILE_SIZE, 20, DARKGREEN);

            if (!loaded || session.activity != RUNNING) {
                gameRunning = false;
            }

            EndDrawing();
        }
    }

    drawEndGameScreen(&session);
    UnloadSound(laserShoot);
    UnloadSound(moleHit);
    UnloadSound(playerMove);
    UnloadSound(playerDeath);
    UnloadSound(levelUp);
    UnloadSound(moleHitPlayer);
    CloseAudioDevice();
    return 0;
}