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
    fprintf(file, "level=%d\n",         session->player.level);
    fprintf(file, "score=%d\n",         session->player.score);
    fprintf(file, "lives=%d\n",         session->player.lives);
    fprintf(file, "emeralds=%d\n",      session->player.emeralds);
    fprintf(file, "total_emeralds=%d\n",session->total_emeralds);
    fprintf(file, "golds=%d\n",         session->player.golds);
    fprintf(file, "positionx=%.0f\n",   session->player.position.x);
    fprintf(file, "positiony=%.0f\n",   session->player.position.y);
    fprintf(file, "moles=%d\n", session->mole_num);
    for(int i = 0; i < session->mole_num; i++) {
        fprintf(file, "mole_alive=%d", session->moles[i].isAlive);
        fprintf(file, " mole_x=%.0f ", session->moles[i].position.x);
        fprintf(file, "mole_y=%.0f ", session->moles[i].position.y);
        fprintf(file, "mole_dir=%d ", session->moles[i].direction);
        fprintf(file, "mole_cng=%d\n", session->moles[i].change);
    }
    fprintf(file, "------\n");
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            fprintf(file, "%c", session->map[y][x]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "---\n");
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            if((session->minerals[y][x].type == EMERALD) ||
               (session->minerals[y][x].type == GOLDD) ||
               (session->minerals[y][x].type == POWERUP)) {
                fprintf(file, "min_type=%d ", session->minerals[y][x].type);
                fprintf(file, "min_x=%d ", x);
                fprintf(file, "min_y=%d ", y);
                fprintf(file, "min_sot=%d ", session->minerals[y][x].soterrado);
                fprintf(file, "min_col=%d\n", session->minerals[y][x].coletado);
            }
        }
    }

    fclose(file);
    return true;
}


bool loadGame(Game *session) {
    FILE *file = fopen("game.sav", "r");
    
    if (file == NULL) {
        return false; // Failed to open the file
    }

    char buffer[256];
    int mole_index = 0, mapRow = 0, mineralRow = 0;
    bool parsing_moles = false;
    bool parsing_map = false;
    bool parsing_minerals = false;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char variable[256];
        int value;
        
        if (parsing_moles) {
            int mole_alive, mole_dir, mole_cng, mole_x, mole_y;
            // Parse and assign mole data
            if (sscanf(buffer, "mole_alive=%d mole_x=%d mole_y=%d mole_dir=%d mole_cng=%d",
                       &mole_alive, &mole_x, &mole_y, &mole_dir, &mole_cng) == 5) {
                if (mole_index < value && value >= 0) {
                    session->moles[mole_index].isAlive = mole_alive;
                    session->moles[mole_index].position.x = mole_x;
                    session->moles[mole_index].position.y = mole_y;
                    session->moles[mole_index].direction = mole_dir;
                    session->moles[mole_index].change = mole_cng;
                }
                mole_index++;
            } else {
                parsing_moles = false;
                parsing_map = true;
                parsing_minerals = false;
                mapRow = 0;
            }
        } else if (parsing_map) {
            // Parse and assign map data
            if (mapRow < MAP_HEIGHT) {
                strncpy(session->map[mapRow], buffer, MAP_WIDTH);
                mapRow++;
            } else {
                parsing_moles = false;
                parsing_map = false;
                parsing_minerals = true;
                mineralRow = 0;
            }
        } else if (parsing_minerals) {
            // Parse and assign minerals data
            int min_type, min_x, min_y, min_sot, min_col;
            if (sscanf(buffer, "min_type=%d min_x=%d min_y=%d min_sot=%d min_col=%d",
                       &min_type, &min_x, &min_y, &min_sot, &min_col) == 5) {
                session->minerals[min_y][min_x].type = min_type;
                session->minerals[min_y][min_x].soterrado = min_sot;
                session->minerals[min_y][min_x].coletado = min_col;
                mineralRow++;
            }
        } else if (sscanf(buffer, "%[^=]=%d", variable, &value) == 2) {
            if (strcmp(variable, "moles") == 0) {
                session->mole_num = value;
                mole_index = 0;
                parsing_moles = true;
                parsing_map = false;
                parsing_minerals = false;
            } else if (strcmp(variable, "level") == 0) {
                session->player.level = value;
            } else if (strcmp(variable, "score") == 0) {
                session->player.score = value;
            } else if (strcmp(variable, "lives") == 0) {
                session->player.lives = value;
            } else if (strcmp(variable, "emeralds") == 0) {
                session->player.emeralds = value;
            } else if (strcmp(variable, "total_emeralds") == 0) {
                session->total_emeralds = value;
            } else if (strcmp(variable, "golds") == 0) {
                session->player.golds = value;
            } else if (strcmp(variable, "positionx") == 0) {
                initial_position.x = value;
                session->player.position.x = value;
            } else if (strcmp(variable, "positiony") == 0) {
                initial_position.y = value;
                session->player.position.y = value;
            }
        } 
    }

    fclose(file);
    nextLevel(session);
    session->activity = RUNNING;

    return true;
}
/*
// Function to load the game state from a text file
bool loadGame(Game *session) {
    FILE *file = fopen("game.sav", "r");
    
    if (file == NULL) {
        return false; // Failed to open the file
    }

    // Read and update game data from the file
    int mapRow = 0, mineralRow = 0, map = 0, minerals = 0, mole_index = 0;
    char buffer[256]; // Adjust the buffer size as needed
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Parse each line to extract data
        char variable[256];
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
                session->total_emeralds = value;
            } else if (strcmp(variable, "golds") == 0) {
                session->player.golds = value;
            } else if (strcmp(variable, "positionx") == 0) {
                session->player.position.x = value;
            } else if (strcmp(variable, "positiony") == 0) {
                session->player.position.y = value;
            } else if (strcmp(variable, "moles") == 0) {
                session->mole_num = value;
                mole_index = 0;
                mapRow = 0;
                while (fgets(buffer, sizeof(buffer), file) != NULL) {
                    int mole_alive, mole_x, mole_y, mole_dir, mole_cng;
                    
                    if (sscanf(buffer, "mole_alive=%d mole_x=%d mole_y=%d mole_dir=%d mole_cng=%d",
                               &mole_alive, &mole_x, &mole_y, &mole_dir, &mole_cng) == 5) {
                        if (mole_index < value && value >= 0) {
                            session->moles[mole_index].isAlive = mole_alive;
                            session->moles[mole_index].position.x = mole_x;
                            session->moles[mole_index].position.y = mole_y;
                            session->moles[mole_index].direction = mole_dir;
                            session->moles[mole_index].change = mole_cng;
                        }
                        mole_index++;
                    }
                    else if (sscanf(buffer, "mole_alive=%d", &mole_alive) == 1) {
                        if (mole_index < value && value >= 0) {
                            session->moles[mole_index].isAlive = mole_alive;
                        }
                        mole_index++;
                    }
                    else if (strcmp(buffer, "------\n") == 0) {
                        map = 1;
                        break;
                    }
                }
            } else if (strcmp(variable, "mole_alive") == 0) {
                session->mole_num = value;
                if(value == 0)
                    mole_index++;
            } else if (strcmp(variable, "mole_x") == 0) {
                session->moles[mole_index].position.x = value;
            } else if (strcmp(variable, "mole_y") == 0) {
                session->moles[mole_index].position.y = value;
            } else if (strcmp(variable, "mole_dir") == 0) {
                session->moles[mole_index].direction = value;
            } else if (strcmp(variable, "mole_cng") == 0) {
                session->moles[mole_index].change = value;
                mole_index++;
            } else if ((strcmp(buffer, "------\n") == 0) || map) {
                mapRow = 0;
                while (fgets(buffer, sizeof(buffer), file)) {
                    if (mapRow < MAP_HEIGHT) {
                        strncpy(session->map[mapRow], buffer, MAP_WIDTH);
                        mapRow++;
                    }
                    
                    if (strcmp(buffer, "---\n") == 0) {
                        minerals = 1;
                        break;
                    }
                    
                }
            } else if ((strcmp(buffer, "---\n") == 0) || minerals) {
                mineralRow = 0;
                while (fgets(buffer, sizeof(buffer), file) != NULL) {
                    int min_type, min_x, min_y, min_sot, min_col;
                    
                    if (sscanf(buffer, "min_type=%d min_x=%d min_y=%d min_sot=%d min_col=%d",
                               &min_type, &min_x, &min_y, &min_sot, &min_col) == 5) {
                        if (mineralRow < MAP_HEIGHT && min_x >= 0 && min_x < MAP_WIDTH && min_y >= 0 && min_y < MAP_HEIGHT) {
                            session->minerals[min_y][min_x].type = min_type;
                            session->minerals[min_y][min_x].soterrado = min_sot;
                            session->minerals[min_y][min_x].coletado = min_col;
                        }
                        mineralRow++;
                    }
                }
            }
        }
    }
    nextLevel(session);
    session->activity = RUNNING;

    fclose(file);
    return true;
}*/

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