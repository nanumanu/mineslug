#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../libs/session.h"
#include "../libs/mechanics.h"

Vector2 initial_position = {0, 0};

void resetGame(Game *session) {
    session->player.level = 1;
    session->player.score = 0;
    session->player.lives = 3;
    session->player.emeralds = 0;
    session->total_emeralds = 0;
    session->player.golds = 0;
    nextLevel(session);
    session->activity = RUNNING;
}


void nextLevel(Game *session) {
    session->player.position = initial_position;
    session->player.direction = DOWN;
    session->powered = false;
}

bool DrawGameMap(Game session) {
    bool continueRunning = true;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            Vector2 position = { x * TILE_SIZE, y * TILE_SIZE};
            //if(Vector2Distance(session.player.position, (Vector2){x, y}) < 80 &&  || session.powered)) {
                switch (session.map[y][x]) {
                    case '#':
                        // Desenhar parede indestrutivel
                        drawObject(position, BLACK);
                        break;
                    case 'S':
                        // Desenhar área soterrada
                        drawObject(position, LIGHTGRAY);
                        break;
                    case 'E':
                        // Desenhar esmeralda
                        if(session.minerals[y][x].coletado);
                        else if(session.minerals[y][x].soterrado) drawObject(position, LIGHTGRAY);
                        else drawObject(position, DARKGREEN);
                        break;
                    case 'O':
                        // Desenhar ouro
                        if(session.minerals[y][x].coletado);
                        else if(session.minerals[y][x].soterrado) drawObject(position, LIGHTGRAY);
                        else drawObject(position, GOLD);
                        break;
                    case 'A':
                        // Desenhar power-up
                        if(!session.minerals[y][x].coletado)
                            drawObject(position, SKYBLUE);
                        break;
                    default: break;
                }
            //}
            //else {
            //    drawObject(position, BLACK);
            //}
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
    if(session->powered) session->powered--;
    updateLaser(session);
    movePlayer(session, dx, dy);
    updateMoles(session);
}

bool LoadGameMap(int level, Game* session) {
    char fname[20];
    sprintf(fname,"maps/%s%d.%s","map",level,"txt");
    FILE* file = fopen(fname, "r");
    int mole_num = 0;
    
    if (!file) {
        if(level == 1) {
            printf("Erro ao abrir arquivo do mapa.\n");
            exit(EXIT_FAILURE);
        }
        else {
            return false;
        }
    }
    
    if (level > 1){
        for(int i = 0; i < mole_num; i++){
            session->moles[mole_num].isAlive = false;
        }
        
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for(int x = 0; x < MAP_WIDTH; x++) {
                session->map[y][x] = ' ';
                session->minerals[y][x].type = EMPTY;
            }
        }
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
                session->moles[mole_num].direction = GetRandomValue(0, 3); 
                session->moles[mole_num].isAlive = true;
                session->moles[mole_num].change = 5;
                session->map[y][x] = ' ';
                mole_num++;
            }
            else if (session->map[y][x] == 'O') {
                session->minerals[y][x].type = GOLDD;
                session->minerals[y][x].soterrado = true;
                session->minerals[y][x].coletado = false;
            }
            else if (session->map[y][x] == 'E') {
                session->minerals[y][x].type = EMERALD;
                session->minerals[y][x].soterrado = true;
                session->minerals[y][x].coletado = false;
                session->total_emeralds += 1;
            }
            else if (session->map[y][x] == 'A') {
                session->minerals[y][x].type = POWERUP;
                session->minerals[y][x].soterrado = false;
                session->minerals[y][x].coletado = false;
            }
        }
    }
    session->mole_num = mole_num;
    fclose(file);
    return true;
}

void drawEndGameScreen(Game *session) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    if (session->activity == WON) {
        DrawText("You won!", (MAP_WIDTH * TILE_SIZE)/2 - 100, (MAP_HEIGHT * TILE_SIZE)/2 - 50, 50, DARKGREEN);
    } else {
        DrawText("You lost!", (MAP_WIDTH * TILE_SIZE)/2 - 100, (MAP_HEIGHT * TILE_SIZE)/2 - 50, 50, DARKPURPLE);
    }
    DrawText(TextFormat("Final Score: %i", session->player.score), 
                        (MAP_WIDTH* TILE_SIZE)/2 - 130, (MAP_HEIGHT * TILE_SIZE)/2 + 20, 40, BLACK);
    EndDrawing();
    WaitTime(5);
    CloseWindow();
    exit(0);
}


void UnloadGameMap(Game* session) {
    // Libera recursos se necessário
}