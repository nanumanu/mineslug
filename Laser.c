#define LASER_LENGTH 200

typedef struct {
    Vector2 position;
    Vector2 direction;
    bool isActive;
} Laser;

// ...

void fireLaser(Game *session) {
    session->player.laser.position = session->player.position;
    session->player.laser.direction = session->player.lastDirection;
    session->player.laser.isActive = true;
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
            session->player.score += 200;
            session->player.laser.isActive = false; // Laser desativado após acertar uma toupeira
            return;
        }
    }

    // Verifica colisão com minérios
    int x = session->player.laser.position.x / TILE_SIZE;
    int y = session->player.laser.position.y / TILE_SIZE;
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        char tile = session->map[y][x];
        if (tile == 'E' || tile == 'O') {
            // Minera o minério
            session->map[y][x] = ' ';
            session->player.score += (tile == 'E') ? 100 : 50;
            session->player.laser.isActive = false; // Laser desativado após minerar
        }
    }

    // Desativa o laser após percorrer uma certa distância
    if (Vector2Distance(session->player.position, session->player.laser.position) > LASER_LENGTH) {
        session->player.laser.isActive = false;
    }
}

void processPlayerInput(Game *session) {
    if (IsKeyDown(KEY_UP)) {
        movePlayer(session, 0, -1);
    }
    // ... (outros movimentos)

    if (IsKeyPressed(KEY_G)) {
        fireLaser(session);
    }
}

void updateGame(Game *session) {
    processPlayerInput(session);
    updateLaser(session);
    // ... (outras atualizações)
}

void drawLaser(Game session) {
    if (session.player.laser.isActive) {
        DrawLineEx(session.player.position, session.player.laser.position, 3.0, RED);
    }
}

bool DrawGameMap(Game session) {
    // ... (outro código de desenho)
    
    drawLaser(session);
    
    // ...
}

