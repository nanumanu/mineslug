
// Definição de efeitos sonoros
Sound laserShoot;
Sound moleHit;
Sound playerMove;
Sound playerDeath;
Sound levelUp;

int main() {

    InitAudioDevice();  // Inicializar dispositivo de audio

    // Carregar efeitos sonoros
    laserShoot = LoadSound("path_to_laserShoot.wav");
    moleHit = LoadSound("path_to_moleHit.wav");
    playerMove = LoadSound("path_to_playerMove.wav");
    playerDeath = LoadSound("path_to_playerDeath.wav");
    levelUp = LoadSound("path_to_levelUp.wav");


    //descarregar os efeitos e fechar o dispositivo de audio
    UnloadSound(laserShoot);
    UnloadSound(moleHit);
    UnloadSound(playerMove);
    UnloadSound(playerDeath);
    UnloadSound(levelUp);
    
    CloseAudioDevice();

    return 0;
}


void fireLaser(Game *session) {
    //...
    PlaySound(laserShoot);  // Reproduza o som do laser
}


void updateLaser(Game *session) {
    //...
    if (session->moles[i].isAlive && CheckCollisionRecs(...)) {
        session->moles[i].isAlive = false;
        session->player.laser.isActive = false;
        PlaySound(moleHit);  // Reproduza o som quando uma toupeira for atingida
        return;
    }
}

void movePlayer(Game *session, int dx, int dy) {

    if (IsWalkable(session, newX, newY)) {
        session->player.position.x = newX;
        session->player.position.y = newY;
        PlaySound(playerMove);  // Reproduz o som do jogador se movendo
    }
}


void updateMoles(Game *session) {

    if (CheckCollisionRecs(...)) {
        PlaySound(playerDeath);  // Reproduz o som da morte do jogador
    }

}

void checkNextLevel(Game *session) {

    if (session->player.level != 3) {
        PlaySound(levelUp);  // Reproduz o som quando subir de nível
    }

}
