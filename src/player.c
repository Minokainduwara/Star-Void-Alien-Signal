#include "game.h"

void Player_Init(Player *p)
{
    p->pos = (Vector2){80, SCREEN_HEIGHT / 2.0f};
    p->speed = PLAYER_SPEED;
    p->hp = PLAYER_MAX_HP;
    p->maxHp = PLAYER_MAX_HP;
    p->shield = PLAYER_MAX_SHIELD;
    p->maxShield = PLAYER_MAX_SHIELD;
    p->fireTimer = 0;
    p->weaponLevel = WEAPON_SINGLE;
    p->killCount = 0;
    p->energyFragments = 0;
    p->score = 0;
    p->hasDrone = false;
    p->droneAngle = 0;
    p->specialCooldown = 5.0f;
    p->invincibleTimer = 0;
    p->lives = 3;
    p->trailIndex = 0;
    for (int i = 0; i < 12; i++)
        p->trail[i].pos = p->pos;
}

void Player_Update(Player *p, float dt)
{
    // Input handling
    Vector2 moveDir = {0, 0};

    // Keyboard movement
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    moveDir.y -= 1;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  moveDir.y += 1;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  moveDir.x -= 1;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) moveDir.x += 1;

    // Touch/mouse movement (left half of screen)
    if (g.touchActive && g.touchPos.x < SCREEN_WIDTH * 0.7f)
    {
        Vector2 delta;
        delta.x = g.touchPos.x - p->pos.x;
        delta.y = g.touchPos.y - p->pos.y;
        float len = sqrtf(delta.x * delta.x + delta.y * delta.y);
        if (len > 5.0f)
        {
            moveDir.x = delta.x / len;
            moveDir.y = delta.y / len;
        }
        else
        {
            moveDir.x = 0;
            moveDir.y = 0;
        }
    }

    // Normalize diagonal movement
    float len = sqrtf(moveDir.x * moveDir.x + moveDir.y * moveDir.y);
    if (len > 1.0f)
    {
        moveDir.x /= len;
        moveDir.y /= len;
    }

    p->pos.x += moveDir.x * p->speed * dt;
    p->pos.y += moveDir.y * p->speed * dt;

    // Clamp to play area
    if (p->pos.x < PLAY_AREA_LEFT + PLAYER_SIZE)
        p->pos.x = PLAY_AREA_LEFT + PLAYER_SIZE;
    if (p->pos.x > PLAY_AREA_RIGHT - PLAYER_SIZE)
        p->pos.x = PLAY_AREA_RIGHT - PLAYER_SIZE;
    if (p->pos.y < PLAY_AREA_TOP + PLAYER_SIZE)
        p->pos.y = PLAY_AREA_TOP + PLAYER_SIZE;
    if (p->pos.y > PLAY_AREA_BOTTOM - PLAYER_SIZE)
        p->pos.y = PLAY_AREA_BOTTOM - PLAYER_SIZE;

    // Update fire timer
    if (p->fireTimer > 0)
        p->fireTimer -= dt;

    // Update invincibility
    if (p->invincibleTimer > 0)
        p->invincibleTimer -= dt;

    // Drone orbit
    if (p->hasDrone)
        p->droneAngle += dt * 2.0f;

    // Special cooldown
    if (p->specialCooldown > 0)
        p->specialCooldown -= dt;

    // Shield passive regen (if upgrade purchased)
    if (g.upgrades[2].upgradeLevel > 0 && p->shield < p->maxShield)
        p->shield += g.upgrades[2].upgradeLevel * 2.0f * dt;

    // Weapon evolution based on kills
    if (p->killCount >= 5 && p->weaponLevel < WEAPON_DOUBLE)
    {
        p->weaponLevel = WEAPON_DOUBLE;
        Audio_PlayLevelUp();
    }
    if (p->killCount >= 15 && p->weaponLevel < WEAPON_SPREAD)
    {
        p->weaponLevel = WEAPON_SPREAD;
        Audio_PlayLevelUp();
    }
    if (p->killCount >= 35 && p->weaponLevel < WEAPON_BEAM)
    {
        p->weaponLevel = WEAPON_BEAM;
        Audio_PlayLevelUp();
    }

    // Update trail
    p->trail[p->trailIndex].pos = p->pos;
    p->trailIndex = (p->trailIndex + 1) % 12;
}

void Player_Draw(const Player *p)
{
    // Draw trail
    for (int i = 0; i < 12; i++)
    {
        int idx = (p->trailIndex - i + 12) % 12;
        float alpha = (1.0f - i / 12.0f) * 0.3f;
        Color col = (Color){0, 150, 255, (unsigned char)(alpha * 255)};
        DrawCircleV(p->trail[idx].pos, PLAYER_SIZE * (1.0f - i / 12.0f), col);
    }

    // Draw engine glow
    Color engineCol = (Color){255, 150, 50, 100};
    DrawCircleV((Vector2){p->pos.x - 8, p->pos.y}, 6, engineCol);
    DrawCircleV((Vector2){p->pos.x - 12, p->pos.y}, 4, (Color){255, 200, 50, 80});

    // Draw invincibility flash
    if (p->invincibleTimer > 0 && (int)(p->invincibleTimer * 10) % 2 == 0)
        return; // Blink

    // Draw ship (neon triangle)
    Vector2 tip = {p->pos.x + PLAYER_SIZE, p->pos.y};
    Vector2 left = {p->pos.x - PLAYER_SIZE * 0.6f, p->pos.y - PLAYER_SIZE * 0.7f};
    Vector2 right = {p->pos.x - PLAYER_SIZE * 0.6f, p->pos.y + PLAYER_SIZE * 0.7f};

    // Main body
    DrawTriangle(tip, left, right, (Color){80, 180, 255, 255});
    DrawTriangleLines(tip, left, right, (Color){150, 220, 255, 255});

    // Cockpit
    Vector2 cockpit = {p->pos.x + 2, p->pos.y};
    DrawCircleV(cockpit, 4, (Color){200, 240, 255, 200});

    // Shield bubble
    if (p->shield > 0)
    {
        float shieldAlpha = (p->shield / p->maxShield) * 0.2f;
        Color shieldCol = (Color){0, 200, 255, (unsigned char)(shieldAlpha * 255)};
        DrawCircleLines(p->pos.x, p->pos.y, PLAYER_SIZE + 8, shieldCol);
    }

    // Health bar above ship
    float hpPercent = (float)p->hp / p->maxHp;
    DrawRectangle(p->pos.x - 15, p->pos.y - PLAYER_SIZE - 10, 30, 4, (Color){50, 50, 50, 150});
    DrawRectangle(p->pos.x - 15, p->pos.y - PLAYER_SIZE - 10,
                  (int)(30 * hpPercent), 4,
                  (Color){255 * (1 - hpPercent), 255 * hpPercent, 50, 200});
}

void Player_TakeDamage(Player *p, int damage)
{
    if (p->invincibleTimer > 0)
        return;

    // Shield absorbs damage first
    if (p->shield > 0)
    {
        float dmg = damage;
        if (p->shield >= dmg)
        {
            p->shield -= dmg;
            dmg = 0;
        }
        else
        {
            dmg -= p->shield;
            p->shield = 0;
        }
        damage = (int)dmg;
    }

    p->hp -= damage;
    if (p->hp < 0) p->hp = 0;
    p->invincibleTimer = 1.5f;
    g.screenShake = 5.0f;

    // Spawn damage particles
    Particle_SpawnBurst(p->pos, 8, RED, 150.0f);
    Audio_PlayHit();
}

void Player_Heal(Player *p, int amount)
{
    p->hp += amount;
    if (p->hp > p->maxHp)
        p->hp = p->maxHp;
}

void Player_AddShield(Player *p, float amount)
{
    p->shield += amount;
    if (p->shield > p->maxShield)
        p->shield = p->maxShield;
}