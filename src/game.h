#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

#define SCREEN_WIDTH        800
#define SCREEN_HEIGHT       600
#define GAME_NAME           "Star Void: Alien Signal"
#define MAX_BULLETS         128
#define MAX_ENEMIES         64
#define MAX_PARTICLES       512
#define MAX_POWERUPS        24
#define MAX_STARS           120
#define MAX_WAVE_ENEMIES    20
#define MAX_STORY_LINES     5
#define MAX_SAVE_SLOTS      3
#define FPS                 60
#define MAX_LEVELS          10
#define SAVE_FILE           "saves/game_save.dat"

// Game areas
#define PLAY_AREA_TOP       60.0f
#define PLAY_AREA_BOTTOM   (SCREEN_HEIGHT - 20.0f)
#define PLAY_AREA_LEFT      10.0f
#define PLAY_AREA_RIGHT    (SCREEN_WIDTH - 10.0f)

// Player defaults
#define PLAYER_MAX_HP       100
#define PLAYER_MAX_SHIELD   50
#define PLAYER_SPEED        280.0f
#define PLAYER_SIZE         18.0f
#define PLAYER_FIRE_RATE    0.18f

// Special weapon limits
#define MAX_MISSILES        3
#define MAX_LASER_BEAMS     2
#define MAX_SOUNDWAVES      4

// ---------------------------------------------------------------------------
// Enums
// ---------------------------------------------------------------------------

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_DASHBOARD,
    GAME_STATE_LEVEL_SELECT,
    GAME_STATE_SURVIVAL_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAMEOVER,
    GAME_STATE_UPGRADE,
    GAME_STATE_STORY,
    GAME_STATE_WIN,
    GAME_STATE_SETTINGS,
    GAME_STATE_HOW_TO_PLAY,
    GAME_STATE_CREDITS
} GameState;

typedef enum {
    WEAPON_SINGLE = 1,
    WEAPON_DOUBLE,
    WEAPON_SPREAD,
    WEAPON_BEAM
} WeaponLevel;

typedef enum {
    SPECIAL_NONE,
    SPECIAL_MISSILE,
    SPECIAL_LASER,
    SPECIAL_SOUNDWAVE
} SpecialWeaponType;

typedef enum {
    ENEMY_SCOUT,
    ENEMY_FIGHTER,
    ENEMY_HUNTER,
    ENEMY_SHIELD,
    ENEMY_BOSS
} EnemyType;

typedef enum {
    POWERUP_ENERGY,
    POWERUP_HEALTH,
    POWERUP_SHIELD,
    POWERUP_SPEED_BOOST,
    POWERUP_DRONE,
    POWERUP_SPECIAL,
    POWERUP_MISSILE,
    POWERUP_LASER_BEAM,
    POWERUP_SOUNDWAVE
} PowerUpType;

typedef enum {
    BULLET_PLAYER,
    BULLET_ENEMY,
    BULLET_BEAM,
    BULLET_MISSILE,
    BULLET_SOUNDWAVE
} BulletType;

typedef enum {
    GAME_MODE_CAMPAIGN,
    GAME_MODE_SURVIVAL,
    GAME_MODE_BOSS_RUSH
} GameMode;

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD
} Difficulty;

// ---------------------------------------------------------------------------
// Structs
// ---------------------------------------------------------------------------

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float radius;
    bool active;
    int damage;
    BulletType type;
    Color color;
    float lifetime;
    float lifeMax;
} Bullet;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float radius;
    bool active;
    int hp;
    int maxHp;
    EnemyType type;
    float fireTimer;
    float fireRate;
    int scoreValue;
    bool hasShield;
    float shieldHP;
    float phaseTimer;
    int phase;
    Color tint;
} Enemy;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float radius;
    bool active;
    int lifetime;
    Color color;
    int value;
    PowerUpType type;
} PowerUp;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float radius;
    Color color;
    int lifetime;
    int maxLifetime;
    bool active;
    bool gravity;
} Particle;

typedef struct {
    Vector2 pos;
    float radius;
    Color color;
    float brightness;
    float phase;
} Star;

typedef struct {
    Vector2 pos;
    float angle;
} TrailPoint;

typedef struct {
    Vector2 pos;
    float speed;
    int hp;
    int maxHp;
    float shield;
    float maxShield;
    float fireTimer;
    WeaponLevel weaponLevel;
    int killCount;
    int energyFragments;
    int score;
    bool hasDrone;
    float droneAngle;
    float specialCooldown;
    float invincibleTimer;
    int lives;
    TrailPoint trail[12];
    int trailIndex;
    int missiles;
    int laserBeams;
    int soundwaves;
    SpecialWeaponType selectedSpecial;
    float specialFireTimer;
} Player;

typedef struct {
    int currentWave;
    int totalWaves;
    int enemiesSpawned;
    int enemiesInWave;
    float spawnTimer;
    float spawnInterval;
    bool waveComplete;
    bool bossWave;
    float waveDelay;
    float difficulty;
    EnemyType waveTypes[MAX_WAVE_ENEMIES];
} WaveManager;

typedef struct {
    int upgradeLevel;
    bool purchased;
    int cost;
    char name[32];
    char desc[64];
} Upgrade;

typedef struct {
    char text[128];
    float displayTimer;
    bool shown;
} StoryLine;

typedef struct {
    int highScore;
    int wavesCompleted;
    int totalKills;
    int weaponLevel;
    int shieldLevel;
    int speedLevel;
    int droneLevel;
    bool unlockedBossRush;
    int unlockedLevel;       // Highest campaign level unlocked
    bool survivalSaveExists; // Whether survival save exists
} SaveData;

// Persistent save for in-progress game
typedef struct {
    bool valid;
    GameMode mode;
    Difficulty difficulty;
    int currentWave;
    int score;
    int killCount;
    int energyFragments;
    int hp;
    int maxHp;
    float shield;
    float maxShield;
    WeaponLevel weaponLevel;
    int missiles;
    int laserBeams;
    int soundwaves;
    int upgradeLevels[6];
    bool hasDrone;
    float gameTime;
} GameSave;

// ---------------------------------------------------------------------------
// Game Globals (extern)
// ---------------------------------------------------------------------------

typedef struct {
    GameState state;
    GameMode mode;
    Difficulty difficulty;
    Player player;
    Bullet bullets[MAX_BULLETS];
    Enemy enemies[MAX_ENEMIES];
    Particle particles[MAX_PARTICLES];
    PowerUp powerups[MAX_POWERUPS];
    Star stars[MAX_STARS];
    WaveManager wave;
    Upgrade upgrades[6];
    StoryLine story[MAX_STORY_LINES];
    int currentStoryIndex;
    float storyTimer;
    float screenShake;
    float gameTime;
    float difficultyMult;
    Rectangle screenRect;
    bool firePressed;
    bool specialFirePressed;
    bool exitRequested;
    Vector2 touchPos;
    bool touchActive;
    bool lastFrameTouchActive;
    int comboSFX;
    float comboDisplayTimer;
    int comboCount;
    SaveData save;
    GameSave gameSave;
    Texture2D dummyTex;
} GameData;

extern GameData g;

// ---------------------------------------------------------------------------
// Function declarations
// ---------------------------------------------------------------------------

// game.c
void Game_Init(void);
void Game_Update(float dt);
void Game_Draw(void);
void Game_Shutdown(void);
void Game_Reset(void);
void Game_SaveState(void);
bool Game_LoadState(void);
void Game_DeleteSave(void);
void ApplyGameSave(void);

// player.c
void Player_Init(Player *p);
void Player_Update(Player *p, float dt);
void Player_Draw(const Player *p);
void Player_TakeDamage(Player *p, int damage);
void Player_Heal(Player *p, int amount);
void Player_AddShield(Player *p, float amount);

// enemy.c
void Enemy_Spawn(EnemyType type, Vector2 pos);
void Enemy_Update(Enemy *e, float dt);
void Enemy_Draw(const Enemy *e);
void Enemy_SpawnWave(void);

// bullet.c
void Bullet_Fire(Vector2 pos, Vector2 vel, int damage, BulletType type, Color color);
void Bullet_UpdateAll(float dt);
void Bullet_DrawAll(void);

// collision.c
bool Collision_CircleCircle(Vector2 a, float ra, Vector2 b, float rb);
bool Collision_CircleRect(Vector2 c, float r, Rectangle rect);
void Collision_CheckAll(void);
void DealDamageToEnemy(Enemy *e, int damage, Bullet *bullet);

// weapon.c
void Weapon_Fire(Player *p, Bullet *bullets, float dt);
void Weapon_FireSpecial(Player *p);
const char* Weapon_GetName(WeaponLevel wl);
int Weapon_GetDamage(WeaponLevel wl);
float Weapon_GetFireRate(WeaponLevel wl);
const char* Weapon_GetSpecialName(SpecialWeaponType sw);

// particles.c
void Particle_Spawn(Vector2 pos, Vector2 vel, float radius, Color color, int lifetime, bool gravity);
void Particle_SpawnBurst(Vector2 pos, int count, Color color, float speed);
void Particle_SpawnTrail(Vector2 pos, Color color);
void Particle_UpdateAll(float dt);
void Particle_DrawAll(void);
void Starfield_Init(void);
void Starfield_Update(float dt);
void Starfield_Draw(void);

// level.c
void Level_Init(void);
void Level_Update(float dt);
void Level_StartWave(int waveNum);
bool Level_IsWaveComplete(void);
void Level_ScaleDifficulty(void);

// audio.c
void Audio_Init(void);
void Audio_PlayShoot(void);
void Audio_PlayHit(void);
void Audio_PlayExplosion(void);
void Audio_PlayPowerUp(void);
void Audio_PlayBossWarning(void);
void Audio_PlayLevelUp(void);
void Audio_PlayGameOver(void);
void Audio_PlaySpecial(void);
void Audio_Shutdown(void);

// ui.c
void UI_DrawMainMenu(void);
void UI_DrawDashboard(void);
void UI_DrawLevelSelect(void);
void UI_DrawSurvivalMenu(void);
void UI_DrawHUD(void);
void UI_DrawPauseMenu(void);
void UI_DrawGameOver(void);
void UI_DrawUpgradeMenu(void);
void UI_DrawStoryOverlay(void);
void UI_DrawTouchControls(void);
void UI_DrawWinScreen(void);
void UI_DrawSettings(void);
void UI_DrawHowToPlay(void);
void UI_DrawCredits(void);
void UI_HandleMenuInput(void);

// powerup.c
void PowerUp_Spawn(Vector2 pos, PowerUpType type);
void PowerUp_UpdateAll(float dt);
void PowerUp_DrawAll(void);
void PowerUp_Collect(Player *p, PowerUpType type);
const char* PowerUp_GetName(PowerUpType type);

// story.c
void Story_Init(void);
void Story_Advance(void);
const char* Story_GetCurrentText(void);
bool Story_IsComplete(void);

#endif // GAME_H