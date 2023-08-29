while (!WindowShouldClose()) {

        if (gameover = false) {


            // pipipi pópópó


           if (AQUI EU FIQUEI EM DUVIDA PQ EU N SEI O NOME DA VARIAVEL Q TU BOTOU PRA VIDAS=0) {
                gameover = true;
            }
        } else {

            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("Game Over", SCREEN_WIDTH / 2 - MeasureText("Game Over", 50) / 2, SCREEN_HEIGHT / 2 - 50, 50, RED);
            DrawText("aperte SPACE para recomeçar", SCREEN_WIDTH / 2 - MeasureText("Press SPACE to restart", 20) / 2, SCREEN_HEIGHT - 100, 20, GRAY);

            EndDrawing();


            if (IsKeyPressed(KEY_SPACE)) {
                gameover = false;
                // a partir daqui a parte de save entra