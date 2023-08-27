#pragma once
#include "structs.h"

void drawLaser(Game session);
void fireLaser(Game *session);
void updateLaser(Game *session);

void movePlayer(Game *session, int dx, int dy);
bool IsWalkable(Game *session, int x, int y);
void updateMoles(Game *session);
