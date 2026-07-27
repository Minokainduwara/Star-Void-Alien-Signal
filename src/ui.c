#include "game.h"
#include <stdio.h>

static void DrawCenteredText(const char *text, int y, int fontSize, Color color)
{
    int tw = MeasureText(text, fontSize);
    DrawText(text, (SCREEN_WIDTH - tw) / 2, y, fontSize, color);
}

static void DrawNeonButton(const char *text, int x, int y, int w, int h, Color color, bool hover, Rectangle *outRect)
{
    Rectangle rect = {x, y, w, h};
    if (outRect) *outRect = rect;
    Color bg = hover ? (Color){color.r, color.g, color.b, 60} : (Color){color.r, color.g, color.b, 30};
    DrawRectangleRounded(rect, 0.2f, 8, bg);
    DrawRectangleRoundedLines(rect, 0.2f, 8, color);
    int tw = MeasureText(text, 16);
    DrawText(text, x + (w - tw) / 2, y + (h - 20) / 2, 16, color);
}

// ---------------------------------------------------------------------------
// MAIN MENU
// ---------------------------------------------------------------------------

void UI_DrawMainMenu(void)
{
    DrawCenteredText("STAR VOID", 40, 50, (Color){0, 200, 255, 255});
    DrawCenteredText("Alien Signal", 90, 24, (Color){200, 220, 255, 200});
    DrawLine(SCREEN_WIDTH / 2 - 150, 120, SCREEN_WIDTH / 2 + 150, 120, (Color){0, 200, 255, 100});

    int btnW = 220;
    int btnH = 38;
    int startX = (SCREEN_WIDTH - btnW) / 2;
    int startY = 140;
    int spacing = 44;

    Vector2 mouse = GetMousePosition();
    Rectangle r;

    // Column 1 - Game modes
    DrawNeonButton("Campaign", startX - 110, startY, btnW, btnH,
        (Color){0, 200, 255, 255},
        CheckCollisionPointRec(mouse, (Rectangle){startX - 110, startY, btnW, btnH}), &r);
    if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        g.state = GAME_STATE_LEVEL_SELECT;

    DrawNeonButton("Survival", startX - 110, startY + spacing, btnW, btnH,
        (Color){255, 200, 100, 255},
        CheckCollisionPointRec(mouse, (Rectangle){startX - 110, startY + spacing, btnW, btnH}), &r);
    if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        g.state = GAME_STATE_SURVIVAL_MENU;

    Color bossColor = g.save.unlockedBossRush ? (Color){255, 100, 100, 255} : (Color){100, 100, 100, 100};
    DrawNeonButton(g.save.unlockedBossRush ? "Boss Rush" : "Boss Rush [LOCKED]",
        startX - 110, startY + spacing * 2, btnW, btnH, bossColor,
        CheckCollisionPointRec(mouse, (Rectangle){startX - 110, startY + spacing * 2, btnW, btnH}) && g.save.unlockedBossRush, &r);
    if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && g.save.unlockedBossRush)
    {
        Game_Reset();
        g.mode = GAME_MODE_BOSS_RUSH;
        g.state = GAME_STATE_PLAYING;
        Level_StartWave(1);
    }

    // Column 2 - System buttons
    DrawNeonButton("Settings", startX + 110, startY, btnW, btnH,
        (Color){150, 150, 255, 255},
        CheckCollisionPointRec(mouse, (Rectangle){startX + 110, startY, btnW, btnH}), &r);
    if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        g.state = GAME_STATE_SETTINGS;

    DrawNeonButton("How to Play", startX + 110, startY + spacing, btnW, btnH,
        (Color){150, 255, 150, 255},
        CheckCollisionPointRec(mouse, (Rectangle){startX + 110, startY + spacing, btnW, btnH}), &r);
    if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        g.state = GAME_STATE_HOW_TO_PLAY;

    DrawNeonButton("Credits", startX + 110, startY + spacing * 2, btnW, btnH,
        (Color){255, 150, 255, 255},
        CheckCollisionPointRec(mouse, (Rectangle){startX + 110, startY + spacing * 2, btnW, btnH}), &r);
    if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        g.state = GAME_STATE_CREDITS;

    // Bottom row - Exit
    DrawNeonButton("Exit Game", startX, startY + spacing * 3 + 10, btnW, btnH,
        (Color){150, 150, 150, 255},
        CheckCollisionPointRec(mouse, (Rectangle){startX, startY + spacing * 3 + 10, btnW, btnH}), &r);
    if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        g.exitRequested = true;
    }

    // Difficulty selection
    int diffY = startY + spacing * 4 + 20;
    DrawCenteredText("DIFFICULTY", diffY, 14, (Color){150, 200, 255, 200});

    int diffBtnW = 80;
    int diffBtnH = 30;
    int diffStartX = (SCREEN_WIDTH - (diffBtnW * 3 + 20)) / 2;
    int diffBtnY = diffY + 22;

    const char *diffNames[] = {"Easy", "Normal", "Hard"};
    Color diffColors[] = {{0, 255, 100, 255}, {0, 200, 255, 255}, {255, 100, 100, 255}};

    for (int i = 0; i < 3; i++)
    {
        int bx = diffStartX + i * (diffBtnW + 10);
        r = (Rectangle){bx, diffBtnY, diffBtnW, diffBtnH};
        bool isSelected = (int)g.difficulty == i;
        bool hover = CheckCollisionPointRec(mouse, r);
        Color col = isSelected ? diffColors[i] : (Color){100, 100, 100, 150};
        Color bg = hover ? (Color){col.r, col.g, col.b, 60} : (Color){col.r, col.g, col.b, 20};
        DrawRectangleRounded(r, 0.2f, 8, bg);
        DrawRectangleRoundedLines(r, 0.2f, 8, col);
        int tw = MeasureText(diffNames[i], 14);
        DrawText(diffNames[i], bx + (diffBtnW - tw) / 2, diffBtnY + 7, 14, col);

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            g.difficulty = (Difficulty)i;
            switch (i) {
                case 0: g.difficultyMult = 0.6f; break;
                case 1: g.difficultyMult = 1.0f; break;
                case 2: g.difficultyMult = 1.5f; break;
            }
        }
    }

    DrawCenteredText("ESC: Quit", SCREEN_HEIGHT - 22, 11, (Color){100, 100, 100, 150});

    if (g.save.highScore > 0)
    {
        char hs[64];
        snprintf(hs, sizeof(hs), "High Score: %d", g.save.highScore);
        DrawCenteredText(hs, SCREEN_HEIGHT - 80, 12, (Color){255, 200, 50, 200});
    }

    DrawText("v1.3", 10, SCREEN_HEIGHT - 20, 10, (Color){50, 50, 50, 100});
}

// ---------------------------------------------------------------------------
// DASHBOARD
// ---------------------------------------------------------------------------

void UI_DrawDashboard(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});

    DrawCenteredText("DASHBOARD", 80, 40, (Color){0, 200, 255, 255});

    char info[128];
    snprintf(info, sizeof(info), "Wave: %d | Score: %d | Kills: %d",
        g.wave.currentWave, g.player.score, g.player.killCount);
    DrawCenteredText(info, 140, 16, (Color){200, 200, 200, 200});

    int btnW = 250;
    int btnH = 45;
    int startX = (SCREEN_WIDTH - btnW) / 2;
    int startY = 200;
    int spacing = 55;

    Vector2 mouse = GetMousePosition();
    Rectangle r;

    DrawNeonButton("Resume Game", startX, startY, btnW, btnH,
        (Color){0, 200, 255, 255},
        CheckCollisionPointRec(mouse, (Rectangle){startX, startY, btnW, btnH}), &r);
    if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        g.state = GAME_STATE_PLAYING;

    DrawNeonButton("Save & Exit to Menu", startX, startY + spacing, btnW, btnH,
        (Color){0, 255, 100, 255},
        CheckCollisionPointRec(mouse, (Rectangle){startX, startY + spacing, btnW, btnH}), &r);
    if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Game_SaveState();
        g.state = GAME_STATE_MENU;
    }

    DrawNeonButton("Quit (No Save)", startX, startY + spacing * 2, btnW, btnH,
        (Color){255, 100, 100, 255},
        CheckCollisionPointRec(mouse, (Rectangle){startX, startY + spacing * 2, btnW, btnH}), &r);
    if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        g.state = GAME_STATE_MENU;

    DrawNeonButton("Exit Application", startX, startY + spacing * 3, btnW, btnH,
        (Color){150, 150, 150, 255},
        CheckCollisionPointRec(mouse, (Rectangle){startX, startY + spacing * 3, btnW, btnH}), &r);
    if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        g.exitRequested = true;
    }

    DrawCenteredText("R: Resume | S: Save & Exit | Q: Quit to Menu | X: Exit", SCREEN_HEIGHT - 40, 14, (Color){150, 150, 150, 200});
}

// ---------------------------------------------------------------------------
// LEVEL SELECT
// ---------------------------------------------------------------------------

void UI_DrawLevelSelect(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 20, 230});

    DrawCenteredText("SELECT LEVEL", 30, 30, (Color){0, 200, 255, 255});

    Vector2 mouse = GetMousePosition();
    Rectangle r;

    int btnW = 60;
    int btnH = 50;
    int startX = (SCREEN_WIDTH - (5 * (btnW + 10)) + 10) / 2;
    int startY = 100;
    int cols = 5;
    int spacingX = btnW + 10;
    int spacingY = btnH + 10;

    for (int i = 1; i <= MAX_LEVELS; i++)
    {
        int row = (i - 1) / cols;
        int col = (i - 1) % cols;
        int bx = startX + col * spacingX;
        int by = startY + row * spacingY;

        bool unlocked = (i <= g.save.unlockedLevel);
        bool hover = CheckCollisionPointRec(mouse, (Rectangle){bx, by, btnW, btnH}) && unlocked;

        Color colC = unlocked ? (Color){0, 200, 255, 255} : (Color){80, 80, 80, 100};
        Color bg = hover ? (Color){colC.r, colC.g, colC.b, 60} : (Color){colC.r, colC.g, colC.b, 20};
        DrawRectangleRounded((Rectangle){bx, by, btnW, btnH}, 0.2f, 8, bg);
        DrawRectangleRoundedLines((Rectangle){bx, by, btnW, btnH}, 0.2f, 8, colC);

        char lvlText[8];
        snprintf(lvlText, sizeof(lvlText), "%d", i);
        int tw = MeasureText(lvlText, 20);
        DrawText(lvlText, bx + (btnW - tw) / 2, by + 14, 20, colC);

        if (i % 5 == 0 && unlocked)
        {
            DrawText("BOSS", bx + 5, by + btnH - 12, 8, (Color){255, 50, 50, 200});
        }

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Game_Reset();
            g.mode = GAME_MODE_CAMPAIGN;
            g.state = GAME_STATE_PLAYING;
            Level_StartWave(i);
        }
    }

    DrawCenteredText("ESC: Back to Main Menu", SCREEN_HEIGHT - 30, 14, (Color){150, 150, 150, 200});
}

// ---------------------------------------------------------------------------
// SURVIVAL MENU
// ---------------------------------------------------------------------------

void UI_DrawSurvivalMenu(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 20, 230});

    DrawCenteredText("SURVIVAL MODE", 50, 30, (Color){255, 200, 100, 255});

    bool hasSave = Game_LoadState() && g.gameSave.mode == GAME_MODE_SURVIVAL;

    if (hasSave)
    {
        char saveInfo[128];
        snprintf(saveInfo, sizeof(saveInfo), "Saved Progress: Wave %d, Score %d",
            g.gameSave.currentWave, g.gameSave.score);
        DrawCenteredText(saveInfo, 100, 14, (Color){200, 200, 200, 200});
    }

    int btnW = 250;
    int btnH = 45;
    int startX = (SCREEN_WIDTH - btnW) / 2;
    int startY = 200;

    Vector2 mouse = GetMousePosition();
    Rectangle r;

    DrawNeonButton("New Game", startX, startY, btnW, btnH,
        (Color){0, 255, 100, 255},
        CheckCollisionPointRec(mouse, (Rectangle){startX, startY, btnW, btnH}), &r);
    if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Game_Reset();
        g.mode = GAME_MODE_SURVIVAL;
        g.state = GAME_STATE_PLAYING;
        Level_StartWave(1);
    }

    Color continueCol = hasSave ? (Color){0, 200, 255, 255} : (Color){80, 80, 80, 100};
    bool canContinue = hasSave && CheckCollisionPointRec(mouse, (Rectangle){startX, startY + 55, btnW, btnH});
    DrawNeonButton(hasSave ? "Continue" : "No Save Found", startX, startY + 55, btnW, btnH,
        continueCol, canContinue, &r);
    if (canContinue && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hasSave)
    {
        Game_Reset();
        ApplyGameSave();
        g.state = GAME_STATE_PLAYING;
    }

    if (hasSave)
    {
        DrawNeonButton("Delete Save", startX, startY + 110, btnW, btnH,
            (Color){255, 100, 100, 255},
            CheckCollisionPointRec(mouse, (Rectangle){startX, startY + 110, btnW, btnH}), &r);
        if (CheckCollisionPointRec(mouse, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Game_DeleteSave();
        }
    }

    DrawCenteredText("N: New Game | C: Continue | ESC: Back", SCREEN_HEIGHT - 40, 14, (Color){150, 150, 150, 200});
    DrawCenteredText("ESC: Back to Main Menu", SCREEN_HEIGHT - 20, 14, (Color){150, 150, 150, 200});
}

// ---------------------------------------------------------------------------
// HUD
// ---------------------------------------------------------------------------

void UI_DrawHUD(void)
{
    char waveText[32];
    snprintf(waveText, sizeof(waveText), "Wave %d", g.wave.currentWave);
    DrawText(waveText, 10, 10, 20, (Color){0, 200, 255, 200});

    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", g.player.score);
    DrawText(scoreText, 10, 35, 16, (Color){255, 255, 200, 200});

    DrawText("HP", SCREEN_WIDTH - 200, 10, 14, (Color){255, 100, 100, 200});
    DrawRectangle(SCREEN_WIDTH - 170, 12, 150, 12, (Color){50, 50, 50, 150});
    float hpPct = (float)g.player.hp / g.player.maxHp;
    DrawRectangle(SCREEN_WIDTH - 170, 12, (int)(150 * hpPct), 12,
        (Color){255 * (1 - hpPct), 255 * hpPct, 50, 200});
    char hpText[16];
    snprintf(hpText, sizeof(hpText), "%d/%d", g.player.hp, g.player.maxHp);
    DrawText(hpText, SCREEN_WIDTH - 165, 13, 10, WHITE);

    DrawText("SH", SCREEN_WIDTH - 200, 28, 10, (Color){100, 200, 255, 200});
    DrawRectangle(SCREEN_WIDTH - 170, 28, 150, 8, (Color){50, 50, 50, 150});
    float shPct = g.player.maxShield > 0 ? g.player.shield / g.player.maxShield : 0;
    DrawRectangle(SCREEN_WIDTH - 170, 28, (int)(150 * shPct), 8, (Color){0, 200, 255, 200});

    const char *wpn = Weapon_GetName(g.player.weaponLevel);
    DrawText(wpn, SCREEN_WIDTH / 2 - 50, 10, 16, (Color){200, 200, 255, 200});

    char energyText[32];
    snprintf(energyText, sizeof(energyText), "Energy: %d", g.player.energyFragments);
    DrawText(energyText, SCREEN_WIDTH / 2 - 50, 30, 14, (Color){0, 255, 200, 200});

    char killText[32];
    snprintf(killText, sizeof(killText), "Kills: %d", g.player.killCount);
    DrawText(killText, SCREEN_WIDTH / 2 - 50, 45, 12, (Color){200, 200, 200, 150});

    int specialY = SCREEN_HEIGHT - 80;
    DrawText("SPECIAL", 10, specialY, 12, (Color){200, 200, 200, 150});

    Color missileCol = g.player.missiles > 0 ? (Color){255, 150, 50, 255} : (Color){100, 100, 100, 100};
    if (g.player.selectedSpecial == SPECIAL_MISSILE) DrawRectangle(8, specialY + 14, 60, 18, (Color){255, 150, 50, 30});
    DrawText(TextFormat("M:%d", g.player.missiles), 10, specialY + 15, 14, missileCol);

    Color laserCol = g.player.laserBeams > 0 ? (Color){255, 50, 255, 255} : (Color){100, 100, 100, 100};
    if (g.player.selectedSpecial == SPECIAL_LASER) DrawRectangle(58, specialY + 14, 60, 18, (Color){255, 50, 255, 30});
    DrawText(TextFormat("L:%d", g.player.laserBeams), 60, specialY + 15, 14, laserCol);

    Color soundCol = g.player.soundwaves > 0 ? (Color){200, 255, 100, 255} : (Color){100, 100, 100, 100};
    if (g.player.selectedSpecial == SPECIAL_SOUNDWAVE) DrawRectangle(108, specialY + 14, 60, 18, (Color){200, 255, 100, 30});
    DrawText(TextFormat("W:%d", g.player.soundwaves), 110, specialY + 15, 14, soundCol);

    if (g.player.selectedSpecial != SPECIAL_NONE)
    {
        const char *selName = Weapon_GetSpecialName(g.player.selectedSpecial);
        DrawText(TextFormat("[%s]", selName), 10, specialY + 34, 10, (Color){255, 255, 200, 200});
        DrawText("Q:Cycle E:Fire", 10, specialY + 46, 9, (Color){150, 150, 150, 150});
    }

    if (g.comboDisplayTimer > 0 && g.comboCount > 1)
    {
        char comboText[32];
        snprintf(comboText, sizeof(comboText), "COMBO x%d!", g.comboCount);
        int alpha = (int)(g.comboDisplayTimer / 2.0f * 255);
        DrawCenteredText(comboText, SCREEN_HEIGHT / 2 - 50, 30,
            (Color){255, 255, 100, (unsigned char)alpha});
    }

    if (g.wave.bossWave)
    {
        float pulse = 0.5f + 0.5f * sinf(g.gameTime * 4.0f);
        DrawCenteredText("WARNING: BOSS INCOMING", 70, 20,
            (Color){255, 50, 50, (unsigned char)(pulse * 255)});
    }

    const char *diffNames[] = {"Easy", "Normal", "Hard"};
    Color diffColors[] = {{0, 255, 100, 150}, {0, 200, 255, 150}, {255, 100, 100, 150}};
    DrawText(TextFormat("[%s]", diffNames[g.difficulty]), SCREEN_WIDTH - 100, 45, 10, diffColors[g.difficulty]);

    DrawText("ESC: Dashboard", 10, SCREEN_HEIGHT - 20, 10, (Color){100, 100, 100, 150});
}

// ---------------------------------------------------------------------------
// PAUSE MENU
// ---------------------------------------------------------------------------

void UI_DrawPauseMenu(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 150});

    DrawCenteredText("PAUSED", SCREEN_HEIGHT / 2 - 60, 40, (Color){0, 200, 255, 255});
    DrawCenteredText("Press P or SPACE to resume", SCREEN_HEIGHT / 2, 16, (Color){200, 200, 200, 200});
    DrawCenteredText("Press ESC for Dashboard", SCREEN_HEIGHT / 2 + 30, 16, (Color){200, 200, 200, 150});

    Rectangle btn = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 70, 200, 50};
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, btn);
    DrawNeonButton("RESUME", btn.x, btn.y, btn.width, btn.height,
        (Color){0, 200, 255, 255}, hover, NULL);
    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        g.state = GAME_STATE_PLAYING;
}

// ---------------------------------------------------------------------------
// GAME OVER
// ---------------------------------------------------------------------------

void UI_DrawGameOver(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});

    DrawCenteredText("GAME OVER", SCREEN_HEIGHT / 2 - 80, 50, (Color){255, 50, 50, 255});

    char scoreText[64];
    snprintf(scoreText, sizeof(scoreText), "Final Score: %d", g.player.score);
    DrawCenteredText(scoreText, SCREEN_HEIGHT / 2 - 20, 24, (Color){255, 255, 200, 200});

    char waveText[64];
    snprintf(waveText, sizeof(waveText), "Waves Survived: %d", g.wave.currentWave);
    DrawCenteredText(waveText, SCREEN_HEIGHT / 2 + 10, 18, (Color){200, 200, 200, 150});

    char killText[64];
    snprintf(killText, sizeof(killText), "Enemies Destroyed: %d", g.player.killCount);
    DrawCenteredText(killText, SCREEN_HEIGHT / 2 + 35, 16, (Color){200, 200, 200, 120});

    if (g.player.score >= g.save.highScore)
    {
        DrawCenteredText("NEW HIGH SCORE!", SCREEN_HEIGHT / 2 + 60, 20, (Color){255, 200, 50, 255});
    }

    DrawCenteredText("Press ENTER or SPACE to continue", SCREEN_HEIGHT / 2 + 100, 14, (Color){150, 150, 150, 200});
}

// ---------------------------------------------------------------------------
// UPGRADE MENU
// ---------------------------------------------------------------------------

void UI_DrawUpgradeMenu(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 20, 230});

    DrawCenteredText("UPGRADE STATION", 30, 30, (Color){0, 200, 255, 255});

    char energyText[64];
    snprintf(energyText, sizeof(energyText), "Energy Fragments: %d", g.player.energyFragments);
    DrawCenteredText(energyText, 70, 16, (Color){0, 255, 200, 200});

    int startY = 110;
    int spacing = 55;

    for (int i = 0; i < 6; i++)
    {
        int y = startY + i * spacing;
        Color col;

        if (g.upgrades[i].upgradeLevel >= 3)
        {
            col = (Color){100, 100, 100, 100};
        }
        else if (g.player.energyFragments >= g.upgrades[i].cost)
        {
            col = (Color){0, 200, 255, 255};
        }
        else
        {
            col = (Color){150, 150, 150, 150};
        }

        DrawRectangle(50, y, SCREEN_WIDTH - 100, 45, (Color){col.r, col.g, col.b, 20});
        DrawRectangleLines(50, y, SCREEN_WIDTH - 100, 45, col);

        char nameText[64];
        snprintf(nameText, sizeof(nameText), "%d. %s [Lv.%d]", i + 1, g.upgrades[i].name, g.upgrades[i].upgradeLevel);
        DrawText(nameText, 60, y + 5, 16, col);

        DrawText(g.upgrades[i].desc, 60, y + 25, 12, (Color){col.r, col.g, col.b, 150});

        if (g.upgrades[i].upgradeLevel < 3)
        {
            char costText[32];
            snprintf(costText, sizeof(costText), "Cost: %d", g.upgrades[i].cost);
            DrawText(costText, SCREEN_WIDTH - 150, y + 10, 14,
                g.player.energyFragments >= g.upgrades[i].cost ? (Color){0, 255, 200, 200} : RED);
        }
        else
        {
            DrawText("MAXED", SCREEN_WIDTH - 120, y + 10, 14, (Color){100, 100, 100, 200});
        }
    }

    DrawCenteredText("Press 1-6 to purchase | ESC to continue", SCREEN_HEIGHT - 30, 14, (Color){150, 150, 150, 200});
}

// ---------------------------------------------------------------------------
// STORY OVERLAY
// ---------------------------------------------------------------------------

void UI_DrawStoryOverlay(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});

    const char *text = Story_GetCurrentText();
    int fontSize = 24;
    int tw = MeasureText(text, fontSize);
    DrawText(text, (SCREEN_WIDTH - tw) / 2, SCREEN_HEIGHT / 2 - 20, fontSize, (Color){0, 200, 255, 255});

    DrawLine(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + 20, SCREEN_WIDTH / 2 + 200, SCREEN_HEIGHT / 2 + 20,
        (Color){0, 200, 255, 50});

    DrawCenteredText("Press ENTER or SPACE to continue", SCREEN_HEIGHT / 2 + 50, 14, (Color){150, 150, 150, 200});
}

// ---------------------------------------------------------------------------
// TOUCH CONTROLS
// ---------------------------------------------------------------------------

void UI_DrawTouchControls(void)
{
    Rectangle fireBtn = {SCREEN_WIDTH - 80, SCREEN_HEIGHT - 80, 60, 60};
    Color fireCol = g.firePressed ? (Color){255, 100, 100, 100} : (Color){255, 50, 50, 50};
    DrawCircleV((Vector2){fireBtn.x + 30, fireBtn.y + 30}, 30, fireCol);
    DrawCircleLines(fireBtn.x + 30, fireBtn.y + 30, 30, (Color){255, 100, 100, 150});
    DrawText("FIRE", fireBtn.x + 8, fireBtn.y + 22, 12, (Color){255, 200, 200, 200});

    if (g.player.selectedSpecial != SPECIAL_NONE)
    {
        Rectangle specBtn = {SCREEN_WIDTH - 80, SCREEN_HEIGHT - 150, 60, 60};
        Color specCol = (Color){255, 200, 100, 80};
        DrawCircleV((Vector2){specBtn.x + 30, specBtn.y + 30}, 25, specCol);
        DrawCircleLines(specBtn.x + 30, specBtn.y + 30, 25, (Color){255, 200, 100, 150});
        DrawText("SP", specBtn.x + 12, specBtn.y + 22, 12, (Color){255, 255, 200, 200});

        Vector2 touch = g.touchPos;
        if (g.touchActive && CheckCollisionPointRec(touch, specBtn))
        {
            g.specialFirePressed = true;
        }
    }

    DrawRectangleLines(5, (int)PLAY_AREA_TOP, (int)(SCREEN_WIDTH * 0.7f) - 5,
        (int)(PLAY_AREA_BOTTOM - PLAY_AREA_TOP), (Color){0, 100, 255, 20});
}

// ---------------------------------------------------------------------------
// WIN SCREEN
// ---------------------------------------------------------------------------

void UI_DrawWinScreen(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});

    DrawCenteredText("VICTORY!", SCREEN_HEIGHT / 2 - 80, 50, (Color){0, 255, 200, 255});
    DrawCenteredText("The Star Void has been pacified.", SCREEN_HEIGHT / 2 - 20, 20, (Color){200, 255, 200, 200});
    DrawCenteredText("The ancient signal has been silenced.", SCREEN_HEIGHT / 2 + 10, 16, (Color){200, 200, 200, 150});

    char scoreText[64];
    snprintf(scoreText, sizeof(scoreText), "Final Score: %d", g.player.score);
    DrawCenteredText(scoreText, SCREEN_HEIGHT / 2 + 50, 20, (Color){255, 255, 200, 200});

    DrawCenteredText("Boss Rush mode unlocked!", SCREEN_HEIGHT / 2 + 90, 16, (Color){255, 200, 50, 255});

    DrawCenteredText("Press ENTER or SPACE to continue", SCREEN_HEIGHT / 2 + 130, 14, (Color){150, 150, 150, 200});
}

// ---------------------------------------------------------------------------
// SETTINGS
// ---------------------------------------------------------------------------

void UI_DrawSettings(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 20, 230});

    DrawCenteredText("SETTINGS", 50, 40, (Color){0, 200, 255, 255});
    DrawCenteredText("(Coming Soon)", 100, 20, (Color){150, 150, 150, 200});

    DrawCenteredText("Future settings will include:", 160, 16, (Color){200, 200, 200, 150});
    DrawCenteredText("- Sound volume", 190, 14, (Color){150, 150, 150, 120});
    DrawCenteredText("- Music volume", 210, 14, (Color){150, 150, 150, 120});
    DrawCenteredText("- Touch control size", 230, 14, (Color){150, 150, 150, 120});
    DrawCenteredText("- Graphics quality", 250, 14, (Color){150, 150, 150, 120});

    DrawCenteredText("Press ESC or BACKSPACE to return", SCREEN_HEIGHT - 40, 14, (Color){150, 150, 150, 200});
}

// ---------------------------------------------------------------------------
// HOW TO PLAY
// ---------------------------------------------------------------------------

void UI_DrawHowToPlay(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 20, 230});

    DrawCenteredText("HOW TO PLAY", 40, 35, (Color){0, 200, 255, 255});

    int y = 100;
    int spacing = 28;

    DrawCenteredText("Controls:", y, 18, (Color){200, 200, 255, 200}); y += spacing;
    DrawCenteredText("WASD / Arrow Keys - Move ship", y, 14, (Color){150, 150, 150, 180}); y += spacing;
    DrawCenteredText("SPACE / ENTER - Fire weapon", y, 14, (Color){150, 150, 150, 180}); y += spacing;
    DrawCenteredText("Q - Cycle special weapons", y, 14, (Color){150, 150, 150, 180}); y += spacing;
    DrawCenteredText("E - Use selected special weapon", y, 14, (Color){150, 150, 150, 180}); y += spacing;
    DrawCenteredText("ESC - Dashboard (in-game) / Back (menus)", y, 14, (Color){150, 150, 150, 180}); y += spacing;
    DrawCenteredText("P - Pause / Resume", y, 14, (Color){150, 150, 150, 180}); y += spacing * 2;

    DrawCenteredText("Touch Controls:", y, 18, (Color){200, 200, 255, 200}); y += spacing;
    DrawCenteredText("Drag left side - Move ship", y, 14, (Color){150, 150, 150, 180}); y += spacing;
    DrawCenteredText("Tap right side - Fire weapon", y, 14, (Color){150, 150, 150, 180}); y += spacing;
    DrawCenteredText("Special button - Use special weapon", y, 14, (Color){150, 150, 150, 180}); y += spacing * 2;

    DrawCenteredText("Objective:", y, 18, (Color){200, 200, 255, 200}); y += spacing;
    DrawCenteredText("Destroy enemies, collect energy, upgrade weapons!", y, 14, (Color){150, 150, 150, 180});

    DrawCenteredText("Press ESC or BACKSPACE to return", SCREEN_HEIGHT - 30, 14, (Color){150, 150, 150, 200});
}

// ---------------------------------------------------------------------------
// CREDITS
// ---------------------------------------------------------------------------

void UI_DrawCredits(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 20, 230});

    DrawCenteredText("CREDITS", 50, 40, (Color){0, 200, 255, 255});

    int y = 130;
    int spacing = 35;

    DrawCenteredText("Developed by Minoka Wickramasinghe", y, 20, (Color){200, 220, 255, 255}); y += spacing * 2;

    DrawCenteredText("GitHub:", y, 16, (Color){150, 150, 150, 200}); y += spacing;
    DrawCenteredText("https://github.com/Minokainduwara", y, 14, (Color){0, 200, 255, 255}); y += spacing * 2;

    DrawCenteredText("Website:", y, 16, (Color){150, 150, 150, 200}); y += spacing;
    DrawCenteredText("https://minokainduwara.fwh.is", y, 14, (Color){0, 255, 200, 255}); y += spacing * 2;

    DrawCenteredText("Built with raylib", y, 14, (Color){100, 100, 100, 150}); y += spacing;
    DrawCenteredText("Star Void: Alien Signal v1.3", y, 12, (Color){80, 80, 80, 120});

    DrawCenteredText("Press ESC or BACKSPACE to return", SCREEN_HEIGHT - 30, 14, (Color){150, 150, 150, 200});
}

// ---------------------------------------------------------------------------
// MENU INPUT HANDLER
// ---------------------------------------------------------------------------

void UI_HandleMenuInput(void)
{
    if (IsKeyPressed(KEY_C))
        g.state = GAME_STATE_LEVEL_SELECT;
    if (IsKeyPressed(KEY_S))
        g.state = GAME_STATE_SURVIVAL_MENU;
    if (IsKeyPressed(KEY_B) && g.save.unlockedBossRush)
    {
        Game_Reset();
        g.mode = GAME_MODE_BOSS_RUSH;
        g.state = GAME_STATE_PLAYING;
        Level_StartWave(1);
    }
}