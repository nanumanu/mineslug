




//Alteração na função updateLaser

void updateLaser(Game *session) {

    // Verifica colisão com minérios
    int x = session->player.laser.position.x / TILE_SIZE;
    int y = session->player.laser.position.y / TILE_SIZE;
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        char tile = session->map[y][x];
        switch (tile) {
            case 'S': 
                breakWall(session, x, y);
                session->player.laser.isActive = false; 
                break;
            case 'E': // Esmeralda coletada
                session->player.emeralds += 1;
                session->player.score += 10;  //exemplo adicione 10 pontos ao placar
                session->map[y][x] = ' ';
                session->player.laser.isActive = false; 
                break;
            case 'O': // Ouro coletado
                session->player.golds += 1;
                session->player.score += 5;   //exemplo: adicione 5 pontos ao placar
                session->map[y][x] = ' ';
                session->player.laser.isActive = false; 
                break;
            default: break;
        }
    }
}

//Alterações nas funções de desenho e atualização:

void DrawGameMap(Game session) {

    for (int i = 0; i < MAX_TERRAIN_SIZE; i++) {
        if (!minerals[i].coletado && !minerals[i].soterrado) {
            Color color = (minerals[i].type == EMERALD) ? DARKGREEN : GOLD;
            drawObject(minerals[i].position, color);
        }
    }
}

//função moveplayer editada para o jogador coletar as esmeraldas e golds
void movePlayer(Game *session, int dx, int dy) {
    int newX = session->player.position.x + dx;
    int newY = session->player.position.y + dy;
    
    if (IsWalkable(session, newX, newY)) {
        // Coletando esmeraldas e ouro
        int tileX = newX / TILE_SIZE;
        int tileY = newY / TILE_SIZE;
        
        if (session->map[tileY][tileX] == 'E') {
            session->player.emeralds++;
            session->map[tileY][tileX] = ' ';
        } else if (session->map[tileY][tileX] == 'O') {
            session->player.golds++;
            session->map[tileY][tileX] = ' ';
        }
        
        session->player.position.x = newX;
        session->player.position.y = newY;
    }
    
    checkNextLevel(session);
}

void checkNextLevel(Game *session) {
    if (session->player.emeralds == session->total_emeralds) {
        if (session->player.level < NUM_LEVELS - 1) {
            session->player.level += 1;
            LoadGameMap(levels[session->player.level], session);
            resetGame(session);
        } else {
            session->activity = WON;
        }
    }
}
 //função check next level serve para quando o jogador coletar a esmeralda 
