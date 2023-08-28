#pragma once
#include "structs.h"
#include <raymath.h>

void drawObject(Vector2 position, Color color);

void nextLevel(Game *session);
void resetGame(Game *session);
void updateGame(Game *session);

bool LoadGameMap(int level, Game* session);
bool DrawGameMap(Game session);
void drawEndGameScreen(Game *session);
void UnloadGameMap(Game* session);