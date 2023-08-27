#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <unistd.h>
#include "libs/structs.h"
#include "libs/mechanics.h"
#include "libs/session.h"

int main() {
    // Inicialização do jogo
    const int screenWidth = MAP_WIDTH * 40;
    const int screenHeight = MAP_HEIGHT * 40;

    Game session;
    LoadGameMap(levels[0], &session);
    resetGame(&session);
    InitWindow(screenWidth, screenHeight, "MineSlug");
    SetTargetFPS(7);
    bool loaded = true;
    
    // Loop principal do jogo
    while (!WindowShouldClose() && loaded && (session.activity == RUNNING)) {
        
        updateGame(&session);

        // Renderização do jogo
        BeginDrawing();
        ClearBackground(RAYWHITE);
        loaded = DrawGameMap(session);
        
        // Desenha elementos
        drawLaser(session);
        drawObject(session.player.position, RED);
        for (int i = 0; i < session.mole_num; i++) {
            if (session.moles[i].isAlive) {
                drawObject(session.moles[i].position, PURPLE);
            }
        }
        
        DrawText(TextFormat("Level: %i", session.player.level), 1*TILE_SIZE, 10, 20, WHITE);
        DrawText(TextFormat("Lives: %i", session.player.lives), 4*TILE_SIZE, 10, 20, WHITE);
        DrawText(TextFormat("Score: %i", session.player.score), 7*TILE_SIZE, 10, 20, WHITE);
        DrawText(TextFormat("Emeralds: %i", session.player.emeralds), 23*TILE_SIZE, 10, 20, DARKGREEN);
        DrawText(TextFormat("Golds: %i", session.player.golds), 27*TILE_SIZE, 10, 20, GOLD);
        EndDrawing();
    }

    // Encerramento do jogo
    drawEndGameScreen(&session);
    return 0;
}