#include "game.h"

void Game_Init(void)
{
    memset(&g, 0, sizeof(GameData));
    g.state = GAME_STATE_MENU;
    g.mode = GAME_MODE_CAMPAIGN;
    g.difficulty = DIFFICULTY_NORMAL;
    g.difficultyMult = 1.0f;
    g.screenRect = (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    // Load persistent save data
    g.save.unlockedLevel = 1;
    g.save.unlockedBossRush = false;
    g.save.survivalSaveExists = false;

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

    Image img = GenImageColor(1, 1, WHITE);
    g.dummyTex = LoadTextureFromImage(img);
    UnloadImage(img);
}

void Game_Reset(void)
{
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

void Game_SaveState(void)
{
    // Ensure saves directory exists
    // On most systems this will work; if not, save silently fails
    GameSave save;
    memset(&save, 0, sizeof(GameSave));
    save.valid = true;
    save.mode = g.mode;
    save.difficulty = g.difficulty;
    save.currentWave = g.wave.currentWave;
    save.score = g.player.score;
    save.killCount = g.player.killCount;
    save.energyFragments = g.player.energyFragments;
    save.hp = g.player.hp;
    save.maxHp = g.player.maxHp;
    save.shield = g.player.shield;
    save.maxShield = g.player.maxShield;
    save.weaponLevel = g.player.weaponLevel;
    save.missiles = g.player.missiles;
    save.laserBeams = g.player.laserBeams;
    save.soundwaves = g.player.soundwaves;
    save.hasDrone = g.player.hasDrone;
    save.gameTime = g.gameTime;
    for (int i = 0; i < 6; i++)
        save.upgradeLevels[i] = g.upgrades[i].upgradeLevel;

    FILE *f = fopen(SAVE_FILE, "wb");
    if (f)
    {
        fwrite(&save, sizeof(GameSave), 1, f);
        fclose(f);
    }

    // Update save flags
    if (g.mode == GAME_MODE_SURVIVAL)
        g.save.survivalSaveExists = true;
}

bool Game_LoadState(void)
{
    FILE *f = fopen(SAVE_FILE, "rb");
    if (!f) return false;

    GameSave save;
    if (fread(&save, sizeof(GameSave), 1, f) != 1)
    {
        fclose(f);
        return false;
    }
    fclose(f);

    if (!save.valid) return false;

    g.gameSave = save;
    return true;
}

void Game_DeleteSave(void)
{
    remove(SAVE_FILE);
    g.save.survivalSaveExists = false;
}

static void SetDifficulty(Difficulty d)
{
    g.difficulty = d;
    switch (d)
    {
        case DIFFICULTY_EASY:   g.difficultyMult = 0.6f; break;
        case DIFFICULTY_NORMAL: g.difficultyMult = 1.0f; break;
        case DIFFICULTY_HARD:   g.difficultyMult = 1.5f; break;
    }
}

void ApplyGameSave(void)
{
    GameSave *s = &g.gameSave;
    if (!s->valid) return;

    g.mode = s->mode;
    SetDifficulty(s->difficulty);
    g.player.score = s->score;
    g.player.killCount = s->killCount;
    g.player.energyFragments = s->energyFragments;
    g.player.hp = s->hp;
    g.player.maxHp = s->maxHp;
    g.player.shield = s->shield;
    g.player.maxShield = s->maxShield;
    g.player.weaponLevel = s->weaponLevel;
    g.player.missiles = s->missiles;
    g.player.laserBeams = s->laserBeams;
    g.player.soundwaves = s->soundwaves;
    g.player.hasDrone = s->hasDrone;
    g.gameTime = s->gameTime;
    for (int i = 0; i < 6; i++)
        g.upgrades[i].upgradeLevel = s->upgradeLevels[i];

    Level_StartWave(s->currentWave);
}

void Game_Update(float dt)
{
    g.gameTime += dt;

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

            if (IsKeyPressed(KEY_ONE))
                SetDifficulty(DIFFICULTY_EASY);
            if (IsKeyPressed(KEY_TWO))
                SetDifficulty(DIFFICULTY_NORMAL);
            if (IsKeyPressed(KEY_THREE))
                SetDifficulty(DIFFICULTY_HARD);

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            {
                Game_Reset();
                g.mode = GAME_MODE_CAMPAIGN;
                g.state = GAME_STATE_STORY;
                g.storyTimer = 0;
            }
            if (IsKeyPressed(KEY_S))
            {
                Game_Reset();
                g.mode = GAME_MODE_SURVIVAL;
                g.state = GAME_STATE_PLAYING;
                Level_StartWave(1);
            }
            if (IsKeyPressed(KEY_B) && g.save.unlockedBossRush)
            {
                Game_Reset();
                g.mode = GAME_MODE_BOSS_RUSH;
                g.state = GAME_STATE_PLAYING;
                Level_StartWave(1);
            }
            break;
        }

        case GAME_STATE_DASHBOARD:
        {
            // Dashboard handled in UI - keyboard shortcuts
            if (IsKeyPressed(KEY_R))
            {
                // Resume game
                g.state = GAME_STATE_PLAYING;
            }
            if (IsKeyPressed(KEY_S))
            {
                // Save & Exit
                Game_SaveState();
                g.state = GAME_STATE_MENU;
            }
            if (IsKeyPressed(KEY_Q))
            {
                // Quit to menu without saving
                g.state = GAME_STATE_MENU;
            }
            if (IsKeyPressed(KEY_X))
            {
                // Exit application
                g.exitRequested = true;
            }
            break;
        }

        case GAME_STATE_LEVEL_SELECT:
        {
            // Level select handled in UI
            if (IsKeyPressed(KEY_ESCAPE))
                g.state = GAME_STATE_MENU;
            break;
        }

        case GAME_STATE_SURVIVAL_MENU:
        {
            if (IsKeyPressed(KEY_N))
            {
                Game_Reset();
                g.mode = GAME_MODE_SURVIVAL;
                g.state = GAME_STATE_PLAYING;
                Level_StartWave(1);
            }
            if (IsKeyPressed(KEY_C))
            {
                if (Game_LoadState() && g.gameSave.mode == GAME_MODE_SURVIVAL)
                {
                    Game_Reset();
                    ApplyGameSave();
                    g.state = GAME_STATE_PLAYING;
                }
            }
            if (IsKeyPressed(KEY_ESCAPE))
                g.state = GAME_STATE_MENU;
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
            if (IsKeyPressed(KEY_Q))
            {
                if (g.player.missiles > 0 || g.player.laserBeams > 0 || g.player.soundwaves > 0)
                {
                    do {
                        g.player.selectedSpecial = (SpecialWeaponType)((g.player.selectedSpecial + 1) % 4);
                    } while (g.player.selectedSpecial != SPECIAL_NONE &&
                             ((g.player.selectedSpecial == SPECIAL_MISSILE && g.player.missiles <= 0) ||
                              (g.player.selectedSpecial == SPECIAL_LASER && g.player.laserBeams <= 0) ||
                              (g.player.selectedSpecial == SPECIAL_SOUNDWAVE && g.player.soundwaves <= 0)));
                }
                else
                {
                    g.player.selectedSpecial = SPECIAL_NONE;
                }
            }

            g.specialFirePressed = IsKeyPressed(KEY_E);

            Player_Update(&g.player, dt);

            if (g.firePressed)
                Weapon_Fire(&g.player, g.bullets, dt);

            if (g.specialFirePressed && g.player.selectedSpecial != SPECIAL_NONE)
            {
                Weapon_FireSpecial(&g.player);
            }

            Bullet_UpdateAll(dt);
            for (int i = 0; i < MAX_ENEMIES; i++)
                if (g.enemies[i].active)
                    Enemy_Update(&g.enemies[i], dt);
            PowerUp_UpdateAll(dt);
            Particle_UpdateAll(dt);

            Collision_CheckAll();

            Level_Update(dt);

            if (g.player.hp <= 0)
            {
                g.state = GAME_STATE_GAMEOVER;
                Audio_PlayGameOver();
                if (g.player.score > g.save.highScore)
                    g.save.highScore = g.player.score;
            }

            if (g.mode == GAME_MODE_CAMPAIGN && g.wave.currentWave > 10)
            {
                g.state = GAME_STATE_WIN;
            }
            break;
        }

        case GAME_STATE_PAUSED:
        {
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
            if (g.mode == GAME_MODE_CAMPAIGN && g.wave.currentWave > g.save.unlockedLevel)
                g.save.unlockedLevel = g.wave.currentWave;
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
                g.state = GAME_STATE_MENU;
            break;
        }

        case GAME_STATE_UPGRADE:
        {
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

                        switch (i)
                        {
                            case 0: break;
                            case 1: break;
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
    Starfield_Draw();

    switch (g.state)
    {
        case GAME_STATE_MENU:
            UI_DrawMainMenu();
            break;

        case GAME_STATE_DASHBOARD:
            UI_DrawDashboard();
            break;

        case GAME_STATE_LEVEL_SELECT:
            UI_DrawLevelSelect();
            break;

        case GAME_STATE_SURVIVAL_MENU:
            UI_DrawSurvivalMenu();
            break;

        case GAME_STATE_STORY:
            Starfield_Draw();
            Story_Init();
            UI_DrawStoryOverlay();
            break;

        case GAME_STATE_PLAYING:
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, 2, (Color){0, 100, 255, 100});
            DrawRectangle(0, SCREEN_HEIGHT - 2, SCREEN_WIDTH, 2, (Color){0, 100, 255, 100});

            for (int i = 0; i < MAX_POWERUPS; i++)
                if (g.powerups[i].active)
                    PowerUp_DrawAll();

            for (int i = 0; i < MAX_ENEMIES; i++)
                if (g.enemies[i].active)
                    Enemy_Draw(&g.enemies[i]);

            Bullet_DrawAll();

            for (int i = 0; i < MAX_PARTICLES; i++)
                if (g.particles[i].active)
                    Particle_DrawAll();

            Player_Draw(&g.player);

            if (g.player.hasDrone)
            {
                float dx = cosf(g.player.droneAngle) * 30;
                float dy = sinf(g.player.droneAngle) * 30;
                Vector2 dpos = {g.player.pos.x + dx, g.player.pos.y + dy};
                DrawCircleV(dpos, 6, (Color){0, 255, 255, 255});
                DrawCircleV(dpos, 4, (Color){0, 200, 255, 150});
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