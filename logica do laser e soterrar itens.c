
//Laser

#include "raylib.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

typedef struct Player {
    Rectangle rect;
    bool isLaserActive;
} Player;

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jogo Simples com Raylib");

    Player player = { { SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 40, 50, 30 }, false };

    SetTargetFPS(60);
    
    while (!WindowShouldClose()) 
    {
        // Update
        if (IsKeyDown(KEY_RIGHT)) player.rect.x += 5;
        if (IsKeyDown(KEY_LEFT)) player.rect.x -= 5;
        if (player.rect.x < 0) player.rect.x = 0;
        if (player.rect.x > SCREEN_WIDTH - 50) player.rect.x = SCREEN_WIDTH - 50;

        if (IsKeyDown(KEY_G)) player.isLaserActive = true;
        else player.isLaserActive = false;

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(player.rect, RED);
        if (player.isLaserActive) DrawLine(player.rect.x + 25, player.rect.y, player.rect.x + 25, 0, RED);

        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}


//funcionalidade para soterrar itens 

void movePlayer(Game *session, int dx, int dy) {
    int newX = session->player.position.x + dx;
    int newY = session->player.position.y + dy;

    if (!IsWalkable(session, newX, newY)) return;

    session->player.position.x = newX;
    session->player.position.y = newY;

    int tileX = newX / TILE_SIZE;
    int tileY = newY / TILE_SIZE;
    char tile = session->map[tileY][tileX];

    if (tile == 'E') {
        session->player.emeralds++;
        session->player.score += 10;
        session->map[tileY][tileX] = 'S';  // Esmeralda é soterrada depois de coletada
    }
    // Lógica similar para outros itens, se necessário...
}
