#include "game.h"

GameData g;

int main(void)
{
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_NAME);
    SetTargetFPS(FPS);
    InitAudioDevice();

    // Initialize game
    Game_Init();
    Audio_Init();
    Starfield_Init();
    Story_Init();

    // Main game loop
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f; // Clamp delta-time

        // Handle escape - go to dashboard in-game, does nothing on menu
        if (IsKeyPressed(KEY_ESCAPE))
        {
            if (g.state == GAME_STATE_PLAYING || g.state == GAME_STATE_PAUSED)
                g.state = GAME_STATE_DASHBOARD;
            else if (g.state == GAME_STATE_LEVEL_SELECT || g.state == GAME_STATE_SURVIVAL_MENU)
                g.state = GAME_STATE_MENU;
        }
        // P key toggles pause
        if (IsKeyPressed(KEY_P))
        {
            if (g.state == GAME_STATE_PLAYING)
                g.state = GAME_STATE_PAUSED;
            else if (g.state == GAME_STATE_PAUSED)
                g.state = GAME_STATE_PLAYING;
        }

        // Update touch/mouse input
        g.firePressed = false;
        g.lastFrameTouchActive = g.touchActive;

        // Desktop controls (keyboard + mouse fallback)
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            g.touchPos = GetMousePosition();
            g.touchActive = true;
            if (g.touchPos.x > SCREEN_WIDTH * 0.7f)
                g.firePressed = true;
        }
        else
        {
            g.touchActive = false;
        }

        // Touch input (mobile)
        if (GetTouchPointCount() > 0)
        {
            Vector2 t = GetTouchPosition(0);
            if (t.x >= 0 && t.y >= 0)
            {
                g.touchPos = t;
                g.touchActive = true;
                if (t.x > SCREEN_WIDTH * 0.7f)
                    g.firePressed = true;
            }
        }

        // Keyboard fire
        if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_ENTER))
            g.firePressed = true;

        // Update
        Game_Update(dt);

        // Check if exit was requested from dashboard
        if (g.exitRequested)
            break;

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        Game_Draw();
        EndDrawing();
    }

    // Cleanup
    Game_Shutdown();
    Audio_Shutdown();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}