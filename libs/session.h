#pragma once
#include "structs.h"

void drawObject(Vector2 position, Color color);

void resetGame(Game *session);
void updateGame(Game *session);
void checkNextLevel(Game *session);

void LoadGameMap(const char* filename, Game* session);
bool DrawGameMap(Game session);
void drawEndGameScreen(Game *session);
void UnloadGameMap(Game* session);