#include "game.h"
#include <stdio.h>

// Helper: draw centered text
static void DrawCenteredText(const char *text, int y, int fontSize, Color color)
{
    int tw = MeasureText(text, fontSize);
    DrawText(text, (SCREEN_WIDTH - tw) / 2, y, fontSize, color);
}

// Helper: draw a neon button
static Rectangle DrawNeonButton(const char *text, int x, int y, int w, int h, Color color, bool hover)
{
    Rectangle rect = {x, y, w, h};
    Color bg = hover ? (Color){color.r, color.g, color.b, 60} : (Color){color.r, color.g, color.b, 30};
    DrawRectangleRounded(rect, 0.2f, 8, bg);
    DrawRectangleRoundedLines(rect, 0.2f, 8, color);
    int tw = MeasureText(text, 16);
    DrawText(text, x + (w - tw) / 2, y + (h - 20) / 2, 16, color);
    return rect;
}

void UI_DrawMainMenu(void)
{
    // Title
    DrawCenteredText("STAR VOID", 50, 50, (Color){0, 200, 255, 255});
    DrawCenteredText("Alien Signal", 100, 24, (Color){200, 220, 255, 200});

    // Subtitle
    DrawCenteredText("A 2D Space Shooter", 130, 14, (Color){150, 200, 255, 150});

    // Decorative line
    DrawLine(SCREEN_WIDTH / 2 - 150, 150, SCREEN_WIDTH / 2 + 150, 150, (Color){0, 200, 255, 100});

    // Menu options
    int btnW = 250;
    int btnH = 40;
    int startX = (SCREEN_WIDTH - btnW) / 2;
    int startY = 170;
    int spacing = 48;

    Vector2 mouse = GetMousePosition();
    Rectangle r = {0, 0, btnW, btnH};

    // Campaign
    r = (Rectangle){startX, startY, btnW, btnH};
    bool hover1 = CheckCollisionPointRec(mouse, r);
    DrawNeonButton("Campaign Mode", startX, startY, btnW, btnH,
        (Color){0, 200, 255, 255}, hover1);
    if (hover1 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Game_Reset();
        g.mode = GAME_MODE_CAMPAIGN;
        g.state = GAME_STATE_STORY;
        g.storyTimer = 0;
    }

    // Survival
    r = (Rectangle){startX, startY + spacing, btnW, btnH};
    bool hover2 = CheckCollisionPointRec(mouse, r);
    DrawNeonButton("Survival Mode", startX, startY + spacing, btnW, btnH,
        (Color){255, 200, 100, 255}, hover2);
    if (hover2 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Game_Reset();
        g.mode = GAME_MODE_SURVIVAL;
        g.state = GAME_STATE_PLAYING;
        Level_StartWave(1);
    }

    // Boss Rush
    Color bossColor = g.save.unlockedBossRush ? (Color){255, 100, 100, 255} : (Color){100, 100, 100, 100};
    r = (Rectangle){startX, startY + spacing * 2, btnW, btnH};
    bool hover3 = CheckCollisionPointRec(mouse, r) && g.save.unlockedBossRush;
    DrawNeonButton(g.save.unlockedBossRush ? "Boss Rush" : "Boss Rush [LOCKED]",
        startX, startY + spacing * 2, btnW, btnH, bossColor, hover3);
    if (hover3 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Game_Reset();
        g.mode = GAME_MODE_BOSS_RUSH;
        g.state = GAME_STATE_PLAYING;
        Level_StartWave(1);
    }

    // Difficulty selection
    int diffY = startY + spacing * 3 + 10;
    DrawCenteredText("DIFFICULTY", diffY, 14, (Color){150, 200, 255, 200});

    int diffBtnW = 80;
    int diffBtnH = 30;
    int diffStartX = (SCREEN_WIDTH - (diffBtnW * 3 + 20)) / 2;
    int diffBtnY = diffY + 22;

    const char *diffNames[] = {"Easy", "Normal", "Hard"};
    Color diffColors[] = {
        (Color){0, 255, 100, 255},
        (Color){0, 200, 255, 255},
        (Color){255, 100, 100, 255}
    };

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
            switch (i)
            {
                case 0: g.difficultyMult = 0.6f; break;
                case 1: g.difficultyMult = 1.0f; break;
                case 2: g.difficultyMult = 1.5f; break;
            }
        }
    }

    // Controls hint
    DrawCenteredText("WASD/Arrows: Move | Space: Fire | Q: Select Special | E: Use Special", SCREEN_HEIGHT - 55, 11, (Color){100, 150, 200, 150});
    DrawCenteredText("Touch: Drag to move | Tap right side to fire", SCREEN_HEIGHT - 38, 11, (Color){100, 150, 200, 150});

    // High score
    if (g.save.highScore > 0)
    {
        char hs[64];
        snprintf(hs, sizeof(hs), "High Score: %d", g.save.highScore);
        DrawCenteredText(hs, SCREEN_HEIGHT - 80, 12, (Color){255, 200, 50, 200});
    }

    // Version
    DrawText("v1.1", 10, SCREEN_HEIGHT - 20, 10, (Color){50, 50, 50, 100});
}

void UI_DrawHUD(void)
{
    // Wave info
    char waveText[32];
    snprintf(waveText, sizeof(waveText), "Wave %d", g.wave.currentWave);
    DrawText(waveText, 10, 10, 20, (Color){0, 200, 255, 200});

    // Score
    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", g.player.score);
    DrawText(scoreText, 10, 35, 16, (Color){255, 255, 200, 200});

    // Health bar
    DrawText("HP", SCREEN_WIDTH - 200, 10, 14, (Color){255, 100, 100, 200});
    DrawRectangle(SCREEN_WIDTH - 170, 12, 150, 12, (Color){50, 50, 50, 150});
    float hpPct = (float)g.player.hp / g.player.maxHp;
    DrawRectangle(SCREEN_WIDTH - 170, 12, (int)(150 * hpPct), 12,
        (Color){255 * (1 - hpPct), 255 * hpPct, 50, 200});
    char hpText[16];
    snprintf(hpText, sizeof(hpText), "%d/%d", g.player.hp, g.player.maxHp);
    DrawText(hpText, SCREEN_WIDTH - 165, 13, 10, WHITE);

    // Shield bar
    DrawText("SH", SCREEN_WIDTH - 200, 28, 10, (Color){100, 200, 255, 200});
    DrawRectangle(SCREEN_WIDTH - 170, 28, 150, 8, (Color){50, 50, 50, 150});
    float shPct = g.player.maxShield > 0 ? g.player.shield / g.player.maxShield : 0;
    DrawRectangle(SCREEN_WIDTH - 170, 28, (int)(150 * shPct), 8, (Color){0, 200, 255, 200});

    // Weapon level
    const char *wpn = Weapon_GetName(g.player.weaponLevel);
    DrawText(wpn, SCREEN_WIDTH / 2 - 50, 10, 16, (Color){200, 200, 255, 200});

    // Energy fragments
    char energyText[32];
    snprintf(energyText, sizeof(energyText), "Energy: %d", g.player.energyFragments);
    DrawText(energyText, SCREEN_WIDTH / 2 - 50, 30, 14, (Color){0, 255, 200, 200});

    // Kills
    char killText[32];
    snprintf(killText, sizeof(killText), "Kills: %d", g.player.killCount);
    DrawText(killText, SCREEN_WIDTH / 2 - 50, 45, 12, (Color){200, 200, 200, 150});

    // Special weapon HUD (bottom left)
    int specialY = SCREEN_HEIGHT - 80;
    DrawText("SPECIAL", 10, specialY, 12, (Color){200, 200, 200, 150});

    // Missile ammo
    Color missileCol = g.player.missiles > 0 ? (Color){255, 150, 50, 255} : (Color){100, 100, 100, 100};
    bool missileSelected = (g.player.selectedSpecial == SPECIAL_MISSILE);
    if (missileSelected) DrawRectangle(8, specialY + 14, 60, 18, (Color){255, 150, 50, 30});
    DrawText(TextFormat("M:%d", g.player.missiles), 10, specialY + 15, 14, missileCol);

    // Laser ammo
    Color laserCol = g.player.laserBeams > 0 ? (Color){255, 50, 255, 255} : (Color){100, 100, 100, 100};
    bool laserSelected = (g.player.selectedSpecial == SPECIAL_LASER);
    if (laserSelected) DrawRectangle(58, specialY + 14, 60, 18, (Color){255, 50, 255, 30});
    DrawText(TextFormat("L:%d", g.player.laserBeams), 60, specialY + 15, 14, laserCol);

    // Soundwave ammo
    Color soundCol = g.player.soundwaves > 0 ? (Color){200, 255, 100, 255} : (Color){100, 100, 100, 100};
    bool soundSelected = (g.player.selectedSpecial == SPECIAL_SOUNDWAVE);
    if (soundSelected) DrawRectangle(108, specialY + 14, 60, 18, (Color){200, 255, 100, 30});
    DrawText(TextFormat("W:%d", g.player.soundwaves), 110, specialY + 15, 14, soundCol);

    // Selected special indicator
    if (g.player.selectedSpecial != SPECIAL_NONE)
    {
        const char *selName = Weapon_GetSpecialName(g.player.selectedSpecial);
        DrawText(TextFormat("[%s]", selName), 10, specialY + 34, 10, (Color){255, 255, 200, 200});
        DrawText("Q:Cycle E:Fire", 10, specialY + 46, 9, (Color){150, 150, 150, 150});
    }

    // Combo display
    if (g.comboDisplayTimer > 0 && g.comboCount > 1)
    {
        char comboText[32];
        snprintf(comboText, sizeof(comboText), "COMBO x%d!", g.comboCount);
        int alpha = (int)(g.comboDisplayTimer / 2.0f * 255);
        DrawCenteredText(comboText, SCREEN_HEIGHT / 2 - 50, 30,
            (Color){255, 255, 100, (unsigned char)alpha});
    }

    // Boss warning
    if (g.wave.bossWave)
    {
        float pulse = 0.5f + 0.5f * sinf(g.gameTime * 4.0f);
        DrawCenteredText("WARNING: BOSS INCOMING", 70, 20,
            (Color){255, 50, 50, (unsigned char)(pulse * 255)});
    }

    // Difficulty indicator
    const char *diffNames[] = {"Easy", "Normal", "Hard"};
    Color diffColors[] = {{0, 255, 100, 150}, {0, 200, 255, 150}, {255, 100, 100, 150}};
    DrawText(TextFormat("[%s]", diffNames[g.difficulty]), SCREEN_WIDTH - 100, 45, 10, diffColors[g.difficulty]);
}

void UI_DrawPauseMenu(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 150});

    DrawCenteredText("PAUSED", SCREEN_HEIGHT / 2 - 60, 40, (Color){0, 200, 255, 255});
    DrawCenteredText("Press P or SPACE to resume", SCREEN_HEIGHT / 2, 16, (Color){200, 200, 200, 200});
    DrawCenteredText("Press ESC for Main Menu", SCREEN_HEIGHT / 2 + 30, 16, (Color){200, 200, 200, 150});

    Rectangle btn = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 70, 200, 50};
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, btn);
    DrawNeonButton("RESUME", btn.x, btn.y, btn.width, btn.height,
        (Color){0, 200, 255, 255}, hover);
    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        g.state = GAME_STATE_PLAYING;
}

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

void UI_DrawTouchControls(void)
{
    // Fire button
    Rectangle fireBtn = {SCREEN_WIDTH - 80, SCREEN_HEIGHT - 80, 60, 60};
    Color fireCol = g.firePressed ? (Color){255, 100, 100, 100} : (Color){255, 50, 50, 50};
    DrawCircleV((Vector2){fireBtn.x + 30, fireBtn.y + 30}, 30, fireCol);
    DrawCircleLines(fireBtn.x + 30, fireBtn.y + 30, 30, (Color){255, 100, 100, 150});
    DrawText("FIRE", fireBtn.x + 8, fireBtn.y + 22, 12, (Color){255, 200, 200, 200});

    // Special weapon button (only if has special ammo)
    if (g.player.selectedSpecial != SPECIAL_NONE)
    {
        Rectangle specBtn = {SCREEN_WIDTH - 80, SCREEN_HEIGHT - 150, 60, 60};
        Color specCol = (Color){255, 200, 100, 80};
        DrawCircleV((Vector2){specBtn.x + 30, specBtn.y + 30}, 25, specCol);
        DrawCircleLines(specBtn.x + 30, specBtn.y + 30, 25, (Color){255, 200, 100, 150});
        DrawText("SP", specBtn.x + 12, specBtn.y + 22, 12, (Color){255, 255, 200, 200});

        // Check touch on special button
        Vector2 touch = g.touchPos;
        if (g.touchActive && CheckCollisionPointRec(touch, specBtn))
        {
            g.specialFirePressed = true;
        }
    }

    // Movement area indicator
    DrawRectangleLines(5, (int)PLAY_AREA_TOP, (int)(SCREEN_WIDTH * 0.7f) - 5,
        (int)(PLAY_AREA_BOTTOM - PLAY_AREA_TOP), (Color){0, 100, 255, 20});
}

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

void UI_HandleMenuInput(void)
{
    if (IsKeyPressed(KEY_C))
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
}