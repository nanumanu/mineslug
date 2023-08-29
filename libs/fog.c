#define MAP_WIDTH  30 
#define MAP_HEIGHT 20

typedef enum {
    CLEAR = 0,
    PARTIAL_FOG,
    FULL_FOG
} FogState;

typedef struct {
 

    FogState fogMap[MAP_HEIGHT][MAP_WIDTH]; //fogMap array
} Game;

for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
        session->fogMap[y][x] = FULL_FOG;
    }
}

bool DrawGameMap(Game session) {
    bool continueRunning = true;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            Vector2 position = { x * TILE_SIZE, y * TILE_SIZE };
            switch (session.map[y][x]) {
                // ...
            }

            switch (session.fogMap[y][x]) {
                case CLEAR:
                    // não fazer nada, tile está visivel
                    break;
                case PARTIAL_FOG:
                    DrawRectangle(position.x, position.y, TILE_SIZE, TILE_SIZE, Fade(BLACK, 0.5f));
                    break;
                case FULL_FOG:
                    DrawRectangle(position.x, position.y, TILE_SIZE, TILE_SIZE, BLACK);
                    break;
            }
        }
    }

    return continueRunning;
}
