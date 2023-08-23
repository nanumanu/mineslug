#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <unistd.h>

#define MAP_WIDTH           30
#define MAP_HEIGHT          20
#define TILE_SIZE           40
#define MAX_TERRAIN_SIZE    100
#define MAX_TROPHIES        200
#define NUM_LEVELS 3 // O número total de níveis

char* levels[NUM_LEVELS] = {
    "map1.txt", 
    "map2.txt", 
    "map3.txt"
};

Vector2 initial_position = {0, 0};

typedef enum {
    UP,
    RIGHT,
    DOWN,
    LEFT
} Direction;

typedef enum {
    EMPTY,
    WALL,
    PLAYER,
    TOUPE,
    GOLDD,
    EMERALD,
    SOTERRADA,
    POWERUP
} MapObject;

typedef enum {
    LOST,
    RUNNING,
    WON
} Status;

typedef struct {
    Vector2 position;
    Direction direction;
    bool isActive;
} Laser;

typedef struct player {
    Vector2 position;
    Direction direction;
    Laser laser;
    int level;
    int score;
    int lives;
    int emeralds;
    int golds;
} Player;

typedef struct mole {
    Vector2 position;
    int isAlive;
} Mole;

typedef struct game {
    bool powered;
    int mole_num;
    int total_emeralds;
    Status activity;
    char map[MAP_HEIGHT][MAP_WIDTH + 1];
    Player player;
    Mole moles[MAX_TROPHIES];
} Game;

void LoadGameMap(const char* filename, Game* session);
void UnloadGameMap(Game *session);
bool DrawGameMap(Game session);
void drawObject(Vector2 position, Color color);
void updateGame(Game *session);
void movePlayer(Game *session, int dx, int dy);
void updateMoles(Game *session);
void handleMoles(Game *session);
bool IsWalkable(Game *session, int x, int y);
void drawEndGameScreen(Game *session);

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

void drawLaser(Game session) {
    if (session.player.laser.isActive) {
        DrawCircleV(session.player.laser.position, 7.0, VIOLET);          
    }
}

void fireLaser(Game *session) {
    if (!session->player.laser.isActive) {
        session->player.laser.position.y = session->player.position.y + 20;
        session->player.laser.position.x = session->player.position.x + 20;
        session->player.laser.direction = session->player.direction;
        session->player.laser.isActive = true;
    }
}

void updateLaser(Game *session) {
    if (!session->player.laser.isActive) return;

    // Avança o laser na direção em que foi disparado
    switch(session->player.laser.direction) {
        case UP:
            session->player.laser.position.y -= TILE_SIZE;
            break;
        case DOWN:
            session->player.laser.position.y += TILE_SIZE;
            break;
        case LEFT:
            session->player.laser.position.x -= TILE_SIZE;
            break;
        case RIGHT:
            session->player.laser.position.x += TILE_SIZE;
            break;
    }

    // Verifica colisão com toupeiras
    for (int i = 0; i < session->mole_num; i++) {
        if (session->moles[i].isAlive && CheckCollisionRecs((Rectangle){session->player.laser.position.x, session->player.laser.position.y, TILE_SIZE, TILE_SIZE},
                                                             (Rectangle){session->moles[i].position.x, session->moles[i].position.y, TILE_SIZE, TILE_SIZE})) {
            session->moles[i].isAlive = false;
            session->player.laser.isActive = false; // Laser desativado após acertar uma toupeira
            return;
        }
    }

    // Verifica colisão com minérios
    int x = session->player.laser.position.x / TILE_SIZE;
    int y = session->player.laser.position.y / TILE_SIZE;
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        char tile = session->map[y][x];
        switch (tile) {
            case 'S': session->map[y][x] = ' ';
            case '#': session->player.laser.isActive = false; // Laser desativado
            default: break;
        }
    }
}

int main() {
    // Inicialização do jogo
    const int screenWidth = MAP_WIDTH * 40;
    const int screenHeight = MAP_HEIGHT * 40;

    Game session;
    LoadGameMap(levels[0], &session);
    resetGame(&session);
    InitWindow(screenWidth, screenHeight, "Pacmine");
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

bool IsWalkable(Game *session, int x, int y) {
    // Verificar se a posição (x, y) pode ser caminhada
    x /= TILE_SIZE; // Corrige as coordenadas para o índice da matriz
    y /= TILE_SIZE;

    if (x < 0 || x >= MAP_WIDTH || 
        y < 0 || y >= MAP_HEIGHT)
        return false; // Fora dos limites do mapa

    char tile = session->map[y][x];
    return (tile != '#') && (tile != 'S');
}

void drawObject(Vector2 position, Color color) {
    DrawRectangle(position.x, position.y, TILE_SIZE, TILE_SIZE, color);
}

void updateMoles(Game *session) {
    for (int i = 0; i < session->mole_num; i++) {
        if(session->moles[i].isAlive) {
            // Decidir um movimento aleatório para a mole (para cima, para baixo, para a esquerda ou para a direita)
            int direction = GetRandomValue(0, 3); 
            int dx = 0;
            int dy = 0;

            switch(direction) {
                case 0: // Para cima
                    dy = -TILE_SIZE;
                    break;
                case 1: // Para baixo
                    dy = TILE_SIZE;
                    break;
                case 2: // Para a esquerda
                    dx = -TILE_SIZE;
                    break;
                case 3: // Para a direita
                    dx = TILE_SIZE;
                    break;
            }

            // Calcula a nova posição da mole.
            int newX = session->moles[i].position.x + dx;
            int newY = session->moles[i].position.y + dy;

            // Verifica se a nova posição é válida, e se for, move a mole.
            if(IsWalkable(session, newX, newY)) {
                session->moles[i].position.x = newX;
                session->moles[i].position.y = newY;
                if(CheckCollisionRecs((Rectangle){newX, newY, TILE_SIZE, TILE_SIZE}, 
                                      (Rectangle){session->player.position.x, session->player.position.y, TILE_SIZE, TILE_SIZE})){
                    if(!session->player.lives){
                        session->activity = LOST;
                        return;
                    }
                    session->player.lives -= 1;
                    session->player.position = initial_position;
                }
            }
        }
    }
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
    // sleep(500); travando
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

void movePlayer(Game *session, int dx, int dy) {
    // Primeiro, calcule a nova posição do jogador.
    int newX = session->player.position.x + dx;
    int newY = session->player.position.y + dy;

    // Verificação se a nova posição está dentro dos limites do terreno e não é uma parede.
    if (!IsWalkable(session, newX, newY)) return;

    // Move o jogador para a nova posição.
    session->player.position.x = newX;
    session->player.position.y = newY;

    // Verificacao de tesouro na nova posição e fazer a coleta se houver.
    switch (session->map[newY][newX]) {
        case 'E': session->player.score += 50;
        case 'O': session->player.score += 50;
        case 'A': session->powered = true;
        default: session->map[newY][newX] = ' ';   ////////////////////ta crashando e precisa
                                                   ////////////////////adcionar o soterro dos items
    }
 }