#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../libs/session.h"
#include "../libs/mechanics.h"

char* levels[] = {
    "maps/map1.txt", 
    "maps/map2.txt", 
    "maps/map3.txt"
};

Vector2 initial_position = {0, 0};

void resetGame(Game *session) {
    session->player.level = 1;
    session->player.score = 0;
    session->player.lives = 3;
    session->player.emeralds = 0;
    session->total_emeralds = 0;
    session->player.golds = 0;
    session->player.position = initial_position;
    session->player.direction = DOWN;
    session->powered = false;
    session->activity = RUNNING;
}

bool DrawGameMap(Game session) {
    bool continueRunning = true;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        int x = 0;
        while (x < MAP_WIDTH) {
            Vector2 position = { x * TILE_SIZE, y * TILE_SIZE};
            switch (session.map[y][x]) {
                case '#':
                    // Desenhar parede indestrutivel
                    drawObject(position, BLACK);
                    break;
                case 'O':
                    // Desenhar minério de ouro
                    drawObject(position, GOLD);
                    break;
                case 'E':
                    // Desenhar esmeralda
                    drawObject(position, DARKGREEN);
                    break;
                case 'S':
                    // Desenhar área soterrada
                    drawObject(position, LIGHTGRAY);
                    break;
                case 'A':
                    // Desenhar power-up
                    drawObject(position, SKYBLUE);
                    break;
                default: break;
            }
            x++;
        }
    }

    return continueRunning;
}

void drawObject(Vector2 position, Color color) {
    DrawRectangle(position.x, position.y, TILE_SIZE, TILE_SIZE, color);
}

void updateGame(Game *session) {
    int dx = 0;
    int dy = 0;

    //Switch case para processar as entradas do teclado
    if (IsKeyDown(KEY_UP))    { dy = -TILE_SIZE; session->player.direction = UP; }
    if (IsKeyDown(KEY_DOWN))  { dy = TILE_SIZE;  session->player.direction = DOWN; }
    if (IsKeyDown(KEY_LEFT))  { dx = -TILE_SIZE; session->player.direction = LEFT; }
    if (IsKeyDown(KEY_RIGHT)) { dx = TILE_SIZE;  session->player.direction = RIGHT; }
    if (IsKeyDown(KEY_G))       fireLaser(session); 

    //função movePlayer com base nas entradas do teclado
    updateLaser(session);
    movePlayer(session, dx, dy);
    updateMoles(session);
}

void LoadGameMap(const char* filename, Game* session) {
    FILE* file = fopen(filename, "r");
    int mole_num = 0;
    
    if (!file) {
        printf("Erro ao abrir arquivo do mapa.\n");
        exit(EXIT_FAILURE);
    }
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        fgets(session->map[y], MAP_WIDTH+2, file);
        for(int x = 0; x < MAP_WIDTH; x++) {
            if (session->map[y][x] == 'J') {
                initial_position.x = x * TILE_SIZE;
                initial_position.y = y * TILE_SIZE;
                session->map[y][x] = ' '; // Remove a posição inicial do jogador do mapa
            }
            else if (session->map[y][x] == 'T') {
                session->moles[mole_num].position.x = x * TILE_SIZE;
                session->moles[mole_num].position.y = y * TILE_SIZE;
                session->map[y][x] = ' ';
                mole_num++;
            }
            else if (session->map[y][x] == 'E') session->total_emeralds += 1;
        }
    }
    session->mole_num = mole_num;
    fclose(file);
}

void drawEndGameScreen(Game *session) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    if (session->activity == WON) {
        DrawText("You won!", 10, 10, 20, DARKGREEN);
    } else {
        DrawText("You lost!", 10, 10, 20, DARKPURPLE);
    }
    DrawText(TextFormat("Final Score: %i", session->player.score), 10, 40, 20, BLACK);
    EndDrawing();
    CloseWindow();
    exit(0);
}

void checkNextLevel(Game *session) {
    if (session->player.emeralds != session->total_emeralds) 
        return;
    else if(session->player.level != 3) {
        LoadGameMap(levels[session->player.level], session); // To be replaced with the next level's map
        session->player.level += 1;
        resetGame(session);
    }
    else
        session->activity = WON;
}

void UnloadGameMap(Game* session) {
    // Libera recursos se necessário
}