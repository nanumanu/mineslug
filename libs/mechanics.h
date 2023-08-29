#pragma once
#include "structs.h"

extern Sound laserShoot;
extern Sound moleHit;
extern Sound playerMove;
extern Sound playerDeath;
extern Sound levelUp;
extern Sound moleHitPlayer; 

void drawLaser(Game session);
void fireLaser(Game *session);
void updateLaser(Game *session);

void movePlayer(Game *session, int dx, int dy);
void breakWall(Game *session, int x, int y);
bool IsWalkable(Game *session, int x, int y);
void updateMoles(Game *session);
