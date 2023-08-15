#define NUM_LEVELS 3 // O número total de níveis

char* levels[NUM_LEVELS] = {
    "map1.txt", 
    "map2.txt", 
    "map3.txt"
};

typedef struct game {
    ...
    int total_emeralds; // A quantidade total de esmeraldas no nível atual
    int total_golds; // A quantidade total de ouro no nível atual
    ...
} Game;
void LoadGameMap(const char* filename, Game* session) {
    
    session->total_emeralds = 0;
    session->total_golds = 0;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        fgets(session->map[y], MAP_WIDTH+2, file);
        for(int x = 0; x < MAP_WIDTH; x++) {
            ...
            else if (session->map[y][x] == 'E') {
                session->total_emeralds++;
            }
            else if (session->map[y][x] == 'O') {
                session->total_golds++;
            }
        }
    }
    ...
}

void checkLevelCompletion(Game* session) {
    if (session->player.emeralds == session->total_emeralds && session->player.golds == session->total_golds) {
        session->player.level++;
        if (session->player.level > NUM_LEVELS) {
            // O jogador ganhou o jogo!
            // Você pode exibir uma mensagem ou tela de vitória aqui.
        } else {
            // Carrega o próximo nível
            LoadGameMap(levels[session->player.level - 1], session);
            resetGame(session); // Ressete o jogo mantendo o score, level e vidas
        }
    }
}

void updateGame(Game *session) {
    ...
    //função movePlayer com base nas entradas do teclado
    movePlayer(session, dx, dy);
    updateMoles(session);
    checkLevelCompletion(session);
}
void displayVictoryMessage() {
    printf("\n\n************ Parabéns! Você venceu o jogo! ************\n\n");
}

void displayGameOverMessage() {
    printf("\n\n************ Infelizmente, você perdeu. Tente novamente! ************\n\n");
}
void checkLevelCompletion(Game* session) {
    ...
    if (session->player.level > NUM_LEVELS) {
        // O jogador ganhou o jogo!
        displayVictoryMessage();
        // encerrar o jogo ou oferecer a opção para reiniciar
    } else {
        ...
    }
}


