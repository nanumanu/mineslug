#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../libs/structs.h"
#include "../libs/session.h"
#include "../libs/mechanics.h"

Vector2 initial_position = {0, 0};

// Function to save the game state to a text file
bool saveGame(const Game *session) {
    FILE *file = fopen("game.sav", "w");
    
    if (file == NULL) {
        return false; // Failed to open the file
    }

    // Save game data to the file
    fprintf(file, "level=%d\n", session->player.level);
    fprintf(file, "score=%d\n", session->player.score);
    fprintf(file, "lives=%d\n", session->player.lives);
    fprintf(file, "emeralds=%d\n", session->player.emeralds);
    fprintf(file, "total_emeralds=%d\n", session->total_emeralds);
    fprintf(file, "golds=%d\n", session->player.golds);
    fprintf(file, "positionx=%.0f\n", session->player.position.x);
    fprintf(file, "positiony=%.0f\n", session->player.position.y);
    fprintf(file, "moles=%d\n", session->mole_num);
    for(int i = 0; i < session->mole_num; i++) {
        fprintf(file, "mole_alive=%d\n", session->moles[i].isAlive);
        if(session->moles[i].isAlive){
            fprintf(file, "mole_x=%.0f\n", session->player.position.x);
            fprintf(file, "mole_y=%.0f\n", session->player.position.y);
            fprintf(file, "mole_dir=%d\n", session->moles[i].direction);
            fprintf(file, "mole_cng=%d\n", session->moles[i].change);
        }
    }
    fprintf(file, "-----\n");
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            fprintf(file, "%c", session->map[y][x]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "-----\n");
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            if((session->minerals[y][x].type == EMERALD) ||
               (session->minerals[y][x].type == GOLDD) ||
               (session->minerals[y][x].type == POWERUP))
            fprintf(file, "min_type=%d\n", session->minerals[y][x].type);
            fprintf(file, "min_x=%d\n", x * TILE_SIZE);
            fprintf(file, "min_y=%d\n", y * TILE_SIZE);
            fprintf(file, "min_sot=%d\n", session->minerals[y][x].soterrado);
            fprintf(file, "min_col=%d\n", session->minerals[y][x].coletado);
        }
    }

    fclose(file);
    return true;
}

// Function to load the game state from a text file
bool loadGame(Game *session) {
    FILE *file = fopen("game.sav", "r");
    
    if (file == NULL) {
        return false; // Failed to open the file
    }

    // Read and update game data from the file
    char buffer[100]; // Adjust the buffer size as needed
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Parse each line to extract data
        char variable[50];
        int value;
        if (sscanf(buffer, "%[^=]=%d", variable, &value) == 2) {
            if (strcmp(variable, "level") == 0) {
                session->player.level = value;
            } else if (strcmp(variable, "score") == 0) {
                session->player.score = value;
            } else if (strcmp(variable, "lives") == 0) {
                session->player.lives = value;
            } else if (strcmp(variable, "emeralds") == 0) {
                session->player.emeralds = value;
            } else if (strcmp(variable, "total_emeralds") == 0) {
                session->player.lives = value;
            } else if (strcmp(variable, "golds") == 0) {
                session->player.golds = value;
            } else if (strcmp(variable, "positionx") == 0) {
                session->player.position.x = value;
            } else if (strcmp(variable, "positiony") == 0) {
                session->player.position.y = value;
            } 
        }
    }
    nextLevel(session);
    session->activity = RUNNING;

    fclose(file);
    return true;
}

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
    for (int y = 0; y < MAP_HEIGHT; y++) {
        fgets(session->map[y], MAP_WIDTH+2, file);
        for(int x = 0; x < MAP_WIDTH; x++) {
            if (session->map[y][x] == 'J') {
                initial_position.x = x * TILE_SIZE;
                initial_position.y = y * TILE_SIZE;
                session->player.position = initial_position;
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
        DrawText("You won! :D", (MAP_WIDTH * TILE_SIZE)/2 - 120, (MAP_HEIGHT * TILE_SIZE)/2 - 70, 55, DARKGREEN);
    } else {
        PlaySound(playerDeath);
        DrawText("You lost! :(", (MAP_WIDTH * TILE_SIZE)/2 - 120, (MAP_HEIGHT * TILE_SIZE)/2 - 70, 55, DARKPURPLE);
    }
    DrawText(TextFormat("Final Score: %i", session->player.score), 
                        (MAP_WIDTH* TILE_SIZE)/2 - 135, (MAP_HEIGHT * TILE_SIZE)/2 + 20, 40, BLACK);
    
    DrawText(TextFormat("Level: %i", session->player.level), 
                        (MAP_WIDTH* TILE_SIZE)/2 - 60, (MAP_HEIGHT * TILE_SIZE)/2 + 70, 40, BLACK);
    EndDrawing();
    WaitTime(5);
    CloseWindow();
    exit(0);
}