#include "game.h"

void Game_Init(void)
{
    memset(&g, 0, sizeof(GameData));
    g.state = GAME_STATE_MENU;
    g.mode = GAME_MODE_CAMPAIGN;
    g.difficulty = 1.0f;
    g.screenRect = (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    Player_Init(&g.player);
    Level_Init();

    // Init upgrades
    strcpy(g.upgrades[0].name, "Firing Speed");
    strcpy(g.upgrades[0].desc, "Increase fire rate");
    g.upgrades[0].cost = 50;
    g.upgrades[0].upgradeLevel = 0;

    strcpy(g.upgrades[1].name, "Damage Boost");
    strcpy(g.upgrades[1].desc, "Increase bullet damage");
    g.upgrades[1].cost = 75;
    g.upgrades[1].upgradeLevel = 0;

    strcpy(g.upgrades[2].name, "Shield Regen");
    strcpy(g.upgrades[2].desc, "Passive shield recovery");
    g.upgrades[2].cost = 60;
    g.upgrades[2].upgradeLevel = 0;

    strcpy(g.upgrades[3].name, "Speed Boost");
    strcpy(g.upgrades[3].desc, "Faster ship movement");
    g.upgrades[3].cost = 40;
    g.upgrades[3].upgradeLevel = 0;

    strcpy(g.upgrades[4].name, "Drone Companion");
    strcpy(g.upgrades[4].desc, "Auto-firing drone");
    g.upgrades[4].cost = 100;
    g.upgrades[4].upgradeLevel = 0;

    strcpy(g.upgrades[5].name, "Void Special");
    strcpy(g.upgrades[5].desc, "Screen-clearing attack");
    g.upgrades[5].cost = 150;
    g.upgrades[5].upgradeLevel = 0;

    // Load dummy texture (1 pixel white)
    Image img = GenImageColor(1, 1, WHITE);
    g.dummyTex = LoadTextureFromImage(img);
    UnloadImage(img);
}

void Game_Reset(void)
{
    // Reset all entities
    for (int i = 0; i < MAX_BULLETS; i++)
        g.bullets[i].active = false;
    for (int i = 0; i < MAX_ENEMIES; i++)
        g.enemies[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++)
        g.particles[i].active = false;
    for (int i = 0; i < MAX_POWERUPS; i++)
        g.powerups[i].active = false;

    Player_Init(&g.player);
    Level_Init();
    g.gameTime = 0;
    g.screenShake = 0;
    g.comboCount = 0;
    g.comboDisplayTimer = 0;
    g.storyTimer = 0;
    g.currentStoryIndex = 0;
    Story_Init();
}

void Game_Update(float dt)
{
    g.gameTime += dt;

    // Update starfield always
    Starfield_Update(dt);

    if (g.screenShake > 0)
        g.screenShake -= dt * 2.0f;
    if (g.comboDisplayTimer > 0)
        g.comboDisplayTimer -= dt;

    switch (g.state)
    {
        case GAME_STATE_MENU:
        {
            UI_HandleMenuInput();

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            {
                Game_Reset();
                g.state = GAME_STATE_STORY;
                g.storyTimer = 0;
            }
            if (IsKeyPressed(KEY_S))
            {
                Game_Reset();
                g.mode = GAME_MODE_SURVIVAL;
                g.state = GAME_STATE_PLAYING;
            }
            if (IsKeyPressed(KEY_B) && g.save.unlockedBossRush)
            {
                Game_Reset();
                g.mode = GAME_MODE_BOSS_RUSH;
                g.state = GAME_STATE_PLAYING;
            }
            break;
        }

        case GAME_STATE_STORY:
        {
            g.storyTimer += dt;
            if (g.storyTimer > 3.0f || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            {
                if (Story_IsComplete())
                {
                    g.state = GAME_STATE_PLAYING;
                    Level_StartWave(1);
                }
                else
                {
                    Story_Advance();
                    g.storyTimer = 0;
                }
            }
            break;
        }

        case GAME_STATE_PLAYING:
        {
            // Update player
            Player_Update(&g.player, dt);

            // Auto-fire if holding fire button
            if (g.firePressed)
                Weapon_Fire(&g.player, g.bullets, dt);

            // Update entities
            Bullet_UpdateAll(dt);
            for (int i = 0; i < MAX_ENEMIES; i++)
                if (g.enemies[i].active)
                    Enemy_Update(&g.enemies[i], dt);
            PowerUp_UpdateAll(dt);
            Particle_UpdateAll(dt);

            // Collision detection
            Collision_CheckAll();

            // Level/wave management
            Level_Update(dt);

            // Check game over
            if (g.player.hp <= 0)
            {
                g.state = GAME_STATE_GAMEOVER;
                Audio_PlayGameOver();
                if (g.player.score > g.save.highScore)
                    g.save.highScore = g.player.score;
            }

            // Win condition - campaign complete
            if (g.mode == GAME_MODE_CAMPAIGN && g.wave.currentWave > 10)
            {
                g.state = GAME_STATE_WIN;
            }
            break;
        }

        case GAME_STATE_PAUSED:
        {
            // Resume on fire press
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
                g.state = GAME_STATE_PLAYING;
            if (IsKeyPressed(KEY_M))
                g.state = GAME_STATE_MENU;
            break;
        }

        case GAME_STATE_GAMEOVER:
        {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
                g.state = GAME_STATE_MENU;
            break;
        }

        case GAME_STATE_WIN:
        {
            g.save.unlockedBossRush = true;
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
                g.state = GAME_STATE_MENU;
            break;
        }

        case GAME_STATE_UPGRADE:
        {
            // Navigate upgrades with number keys
            for (int i = 0; i < 6; i++)
            {
                if (IsKeyPressed(KEY_ONE + i))
                {
                    if (g.player.energyFragments >= g.upgrades[i].cost &&
                        g.upgrades[i].upgradeLevel < 3)
                    {
                        g.player.energyFragments -= g.upgrades[i].cost;
                        g.upgrades[i].upgradeLevel++;
                        g.upgrades[i].cost = (int)(g.upgrades[i].cost * 1.5f);

                        // Apply upgrade
                        switch (i)
                        {
                            case 0: break; // Firing speed handled in weapon.c
                            case 1: break; // Damage handled in weapon.c
                            case 2: g.player.maxShield += 25; break;
                            case 3: g.player.speed += 20; break;
                            case 4: g.player.hasDrone = true; break;
                            case 5: g.player.specialCooldown = 0; break;
                        }
                        Audio_PlayLevelUp();
                    }
                }
            }
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER))
                g.state = GAME_STATE_PLAYING;
            break;
        }

        default:
            break;
    }
}

void Game_Draw(void)
{
    // Draw starfield background
    Starfield_Draw();

    switch (g.state)
    {
        case GAME_STATE_MENU:
            UI_DrawMainMenu();
            break;

        case GAME_STATE_STORY:
            Starfield_Draw();
            Story_Init(); // Ensures story lines are loaded
            UI_DrawStoryOverlay();
            break;

        case GAME_STATE_PLAYING:
        {
            // Apply screen shake
            Vector2 offset = {0, 0};
            if (g.screenShake > 0)
            {
                offset.x = (float)(rand() % 10 - 5) * g.screenShake;
                offset.y = (float)(rand() % 10 - 5) * g.screenShake;
            }

            // Draw game objects using rlPushMatrix-style offset
            // raylib doesn't have rlPushMatrix in the simple API,
            // so we just draw with offset

            DrawRectangle(0, 0, SCREEN_WIDTH, 2, (Color){0, 100, 255, 100}); // Top border
            DrawRectangle(0, SCREEN_HEIGHT - 2, SCREEN_WIDTH, 2, (Color){0, 100, 255, 100}); // Bot border

            // Draw game objects
            for (int i = 0; i < MAX_POWERUPS; i++)
                if (g.powerups[i].active)
                    PowerUp_DrawAll(); // draws all

            for (int i = 0; i < MAX_ENEMIES; i++)
                if (g.enemies[i].active)
                    Enemy_Draw(&g.enemies[i]);

            Bullet_DrawAll();

            for (int i = 0; i < MAX_PARTICLES; i++)
                if (g.particles[i].active)
                    Particle_DrawAll(); // draws all

            Player_Draw(&g.player);

            // Drone companion
            if (g.player.hasDrone)
            {
                float dx = cosf(g.player.droneAngle) * 30;
                float dy = sinf(g.player.droneAngle) * 30;
                Vector2 dpos = {g.player.pos.x + dx, g.player.pos.y + dy};
                DrawCircleV(dpos, 6, (Color){0, 255, 255, 255});
                DrawCircleV(dpos, 4, (Color){0, 200, 255, 150});
                // Drone fires periodically
                if ((int)(g.gameTime * 3) % 2 == 0)
                {
                    Bullet_Fire(dpos, (Vector2){400, 0}, 5, BULLET_PLAYER, (Color){0, 255, 255, 255});
                }
            }

            UI_DrawHUD();
            UI_DrawTouchControls();
            break;
        }

        case GAME_STATE_PAUSED:
            // Draw game world in background
            UI_DrawPauseMenu();
            break;

        case GAME_STATE_GAMEOVER:
            UI_DrawGameOver();
            break;

        case GAME_STATE_UPGRADE:
            UI_DrawUpgradeMenu();
            break;

        case GAME_STATE_WIN:
            UI_DrawWinScreen();
            break;

        default:
            break;
    }
}

void Game_Shutdown(void)
{
    UnloadTexture(g.dummyTex);
}