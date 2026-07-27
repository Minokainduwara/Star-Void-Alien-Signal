#include "game.h"

// Forward declarations
static void SpawnNextEnemy(void);
static EnemyType GetRandomEnemyType(int wave);

void Level_Init(void)
{
    g.wave.currentWave = 0;
    g.wave.totalWaves = 10;
    g.wave.enemiesSpawned = 0;
    g.wave.enemiesInWave = 0;
    g.wave.spawnTimer = 0;
    g.wave.spawnInterval = 0.8f;
    g.wave.waveComplete = false;
    g.wave.bossWave = false;
    g.wave.waveDelay = 2.0f;
    g.wave.difficulty = 1.0f;
}

void Level_StartWave(int waveNum)
{
    g.wave.currentWave = waveNum;
    g.wave.enemiesSpawned = 0;
    g.wave.waveComplete = false;
    g.wave.spawnTimer = 0;

    // Clear remaining enemies
    for (int i = 0; i < MAX_ENEMIES; i++)
        g.enemies[i].active = false;

    // Determine wave composition based on difficulty
    float diffScale = g.difficultyMult;
    int baseEnemies = (int)((3 + waveNum * 2) * diffScale);
    if (baseEnemies > MAX_WAVE_ENEMIES)
        baseEnemies = MAX_WAVE_ENEMIES;
    if (baseEnemies < 2) baseEnemies = 2;
    g.wave.enemiesInWave = baseEnemies;
    g.wave.bossWave = (waveNum % 5 == 0);

    // Set spawn interval based on difficulty
    g.wave.spawnInterval = (1.0f - waveNum * 0.03f) / diffScale;
    if (g.wave.spawnInterval < 0.2f)
        g.wave.spawnInterval = 0.2f;

    // For boss rush mode
    if (g.mode == GAME_MODE_BOSS_RUSH)
    {
        g.wave.bossWave = true;
        g.wave.enemiesInWave = 1;
        g.wave.spawnInterval = 0.5f;
    }

    // Survival mode: continuous scaling
    if (g.mode == GAME_MODE_SURVIVAL)
    {
        g.wave.enemiesInWave = (int)((5 + waveNum) * diffScale);
        if (g.wave.enemiesInWave > MAX_WAVE_ENEMIES)
            g.wave.enemiesInWave = MAX_WAVE_ENEMIES;
    }

    // Spawn some enemies immediately
    int immediate = baseEnemies / 3;
    if (immediate < 1) immediate = 1;
    for (int i = 0; i < immediate; i++)
    {
        SpawnNextEnemy();
        g.wave.enemiesSpawned++;
    }

    Level_ScaleDifficulty();
}

static EnemyType GetRandomEnemyType(int wave)
{
    int r = rand() % 100;
    float scale = g.difficultyMult;
    int adjustedWave = (int)(wave * scale);

    if (adjustedWave <= 2)
    {
        if (r < 70) return ENEMY_SCOUT;
        return ENEMY_FIGHTER;
    }
    else if (adjustedWave <= 5)
    {
        if (r < 40) return ENEMY_SCOUT;
        if (r < 70) return ENEMY_FIGHTER;
        if (r < 85) return ENEMY_HUNTER;
        return ENEMY_SHIELD;
    }
    else
    {
        if (r < 25) return ENEMY_SCOUT;
        if (r < 50) return ENEMY_FIGHTER;
        if (r < 70) return ENEMY_HUNTER;
        if (r < 85) return ENEMY_SHIELD;
        return ENEMY_BOSS;
    }
}

void SpawnNextEnemy(void)
{
    Vector2 pos;
    EnemyType type;

    if (g.wave.bossWave && g.wave.enemiesSpawned == 0)
    {
        type = ENEMY_BOSS;
        pos = (Vector2){SCREEN_WIDTH + 40, SCREEN_HEIGHT / 2.0f};
        Audio_PlayBossWarning();
    }
    else
    {
        type = GetRandomEnemyType(g.wave.currentWave);
        pos = (Vector2){
            SCREEN_WIDTH + 20,
            (float)(rand() % (SCREEN_HEIGHT - 100)) + 50
        };
    }

    Enemy_Spawn(type, pos);
}

void Level_Update(float dt)
{
    if (g.wave.waveComplete)
    {
        g.wave.waveDelay -= dt;
        if (g.wave.waveDelay <= 0)
        {
            if (g.wave.currentWave % 3 == 0 && g.mode == GAME_MODE_CAMPAIGN)
            {
                g.state = GAME_STATE_UPGRADE;
            }

            int nextWave = g.wave.currentWave + 1;

            if (g.mode == GAME_MODE_CAMPAIGN && nextWave > 10)
            {
                g.state = GAME_STATE_WIN;
                return;
            }

            if (g.mode == GAME_MODE_BOSS_RUSH && nextWave > 5)
            {
                g.state = GAME_STATE_WIN;
                return;
            }

            Level_StartWave(nextWave);

            if (g.mode == GAME_MODE_CAMPAIGN)
            {
                if (nextWave == 2 || nextWave == 4 || nextWave == 6 || nextWave == 8 || nextWave == 10)
                {
                    Story_Advance();
                }
            }
        }
        return;
    }

    g.wave.spawnTimer -= dt;
    if (g.wave.spawnTimer <= 0 && g.wave.enemiesSpawned < g.wave.enemiesInWave)
    {
        SpawnNextEnemy();
        g.wave.enemiesSpawned++;
        g.wave.spawnTimer = g.wave.spawnInterval;
    }

    if (g.wave.enemiesSpawned >= g.wave.enemiesInWave)
    {
        bool anyAlive = false;
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (g.enemies[i].active)
            {
                anyAlive = true;
                break;
            }
        }
        if (!anyAlive)
        {
            g.wave.waveComplete = true;
            g.wave.waveDelay = 3.0f;
        }
    }
}

bool Level_IsWaveComplete(void)
{
    return g.wave.waveComplete;
}

void Level_ScaleDifficulty(void)
{
    g.wave.difficulty = 1.0f + g.wave.currentWave * 0.2f * g.difficultyMult;
}