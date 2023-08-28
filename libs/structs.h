#pragma once
#include <raylib.h>

extern char* levels[];
extern Vector2 initial_position;

#define MAP_WIDTH           30
#define MAP_HEIGHT          20
#define TILE_SIZE           40
#define MAX_TROPHIES        200
#define PLAYER_VISIBILITY   3

// Enums for directions, map objects, and game status
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

// Definition of the Laser struct
typedef struct {
    Vector2 position;
    Direction direction;
    bool isActive;
} Laser;

typedef struct {
    MapObject type; // EMERALD ou GOLDD
    bool soterrado; // buried
    bool coletado;  // collected
} Mineral;

// Definition of the Player struct
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

// Definition of the Mole struct
typedef struct mole {
    Vector2 position;
    int isAlive;
    Direction direction;
    int change;
} Mole;

// Definition of the Game struct
typedef struct game {
    int powered;
    int mole_num;
    int total_emeralds;
    Status activity;
    char map[MAP_HEIGHT][MAP_WIDTH + 1];
    Player player;
    Mole moles[MAX_TROPHIES];
    Mineral minerals[MAP_HEIGHT][MAP_WIDTH + 1];
} Game;