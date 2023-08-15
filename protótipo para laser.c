typedef struct laser {
    Vector2 position;
    bool isFiring;
} Laser;

typedef struct game {
    int mole_num;
    char map[MAP_HEIGHT][MAP_WIDTH + 1];
    Player player;
    Laser laser;
    Mole moles[MAX_TROPHIES];
    Collectable powerUps[MAX_TROPHIES],
                emeralds[MAX_TROPHIES],
                golds[MAX_TROPHIES];
} Game;

//...
void updateGame(Game *session) {
    int dx = 0;
    int dy = 0;

    //Switch case para processar as entradas do teclado
    if (IsKeyDown(KEY_UP))      dy = -TILE_SIZE;
    if (IsKeyDown(KEY_DOWN))    dy = TILE_SIZE;
    if (IsKeyDown(KEY_LEFT))    dx = -TILE_SIZE;
    if (IsKeyDown(KEY_RIGHT))   dx = TILE_SIZE;

    if (IsKeyDown(KEY_SPACE)) {
        session->laser.isFiring = true;
        session->laser.position = session->player.position;
    } else {
        session->laser.isFiring = false;
    }

    //função movePlayer com base nas entradas do teclado
    movePlayer(session, dx, dy);
    handleMoles(session);
    updateMoles(session);
    handleLaser(session);
}

void drawObject(Vector2 position, Color color) {
    DrawRectangle(position.x, position.y, TILE_SIZE, TILE_SIZE, color);
    if (session->laser.isFiring) {
        DrawLine(session->player.position.x, session->player.position.y, session->laser.position.x, session->laser.position.y, RED);
    }
}

bool DrawGameMap(Game session) {
    bool continueRunning = true;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        int x = 0;
        while (x < MAP_WIDTH) {
            Vector2 position = { x * TILE_SIZE, y * TILE_SIZE};
            switch (session.map[y][x]) {
                //...
                case 'W':
                    // Desenhar parede destrutível
                    drawObject(position, BROWN);
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

void handleMoles(Game *session) {
    for (int i = 0; i < MAX_TROPHIES; i++) {
        if (session->moles[i].isAlive &&
            session->moles[i].position.x == session->player.position.x &&
            session->moles[i].position.y == session->player.position.y) {
            // O jogador foi atacado por uma toupeira.
            session->player.lives -= 1;
            if (session->player.lives <= 0) {
                resetGame(session);
            }
        }
    }
}

void handleLaser(Game *session) {
    if (!session->laser.isFiring) return;

    int x = session->laser.position.x / TILE_SIZE;
    int y = session->laser.position.y / TILE_SIZE;

    if (x < 0 || x >= MAP_WIDTH || 
        y < 0 || y >= MAP_HEIGHT)
        return;

    char* tile = &session->map[y][x];

    switch (*tile) {
        case 'T':
            // Mata a mole
            *tile = ' ';
            break;
        case 'W':
            // Quebra a parede
            *tile = ' ';
            break;
        case 'O':
            // Coleta ouro
            *tile = ' ';
            session->player.score += 10;  // precisa de ajuste se necessário
            session->player.golds += 1;
            break;
        case 'E':
            // Coleta esmeralda
            *tile = ' ';
            session->player.score += 20;  // precisa de ajuste se necessario
            session->player.emeralds += 1;
            break;
        default: break;
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

    // Atualiza a posição do laser para a posição do jogador.
    if (session->laser.isFiring) {
        session->laser.position.x = newX;
        session->laser.position.y = newY;
    }
}


void handleMoles(Game *session) {
    for (int i = 0; i < session->mole_num; i++) {
        if(session->moles[i].isAlive && 
           session->moles[i].position.x == session->player.position.x &&
           session->moles[i].position.y == session->player.position.y) {
            // O jogador colidiu com uma mole.
            session->player.lives -= 1;
            if(session->player.lives <= 0) {
                // Se o jogador não tiver mais vidas, resetar o jogo.
                resetGame(session);
            }
        }
    }
}

void movePlayer(Game *session, int dx, int dy) {
    // Primeiro, calcule a nova posição do jogador.
    int newX = session->player.position.x + dx;
    int newY = session->player.position.y + dy;

    // Verifica se a nova posição está dentro dos limites do terreno e não é uma parede.
    if (!IsWalkable(session, newX, newY)) return;

    // Mova o jogador para a nova posição.
    session->player.position.x = newX;
    session->player.position.y = newY;

    // Verifique a colisão com as moles
    handleMoles(session);

    // Verifica o tesouro na nova posição e coleta se houver.
}
