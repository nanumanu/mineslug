#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../libs/mechanics.h"

void drawLaser(Game session) {
    if (session.player.laser.isActive) {
        DrawCircleV(session.player.laser.position, 7.0, VIOLET);          
    }
}

void fireLaser(Game *session) {
    if (!session->player.laser.isActive) {
        session->player.laser.position.y = session->player.position.y;
        session->player.laser.position.x = session->player.position.x;
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

void movePlayer(Game *session, int dx, int dy) {
    // Primeiro, calcule a nova posição do jogador.
    int newX = session->player.position.x + dx;
    int newY = session->player.position.y + dy;

    // Verificação se a nova posição está dentro dos limites do terreno e não é uma parede.
    if (!IsWalkable(session, newX, newY)) return;

    // Move o jogador para a nova posição.
    session->player.position.x = newX;
    session->player.position.y = newY;
 }