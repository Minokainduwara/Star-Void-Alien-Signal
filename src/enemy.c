#include "game.h"

void Enemy_Spawn(EnemyType type, Vector2 pos)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!g.enemies[i].active)
        {
            Enemy *e = &g.enemies[i];
            e->pos = pos;
            e->active = true;
            e->type = type;
            e->fireTimer = (float)(rand() % 100) / 100.0f * 2.0f;
            e->phaseTimer = 0;
            e->phase = 0;
            e->tint = WHITE;

            switch (type)
            {
                case ENEMY_SCOUT:
                    e->hp = 15 + (int)(g.difficulty * 5);
                    e->maxHp = e->hp;
                    e->radius = 10;
                    e->vel = (Vector2){-200 - g.difficulty * 20, 0};
                    e->fireRate = 0;
                    e->scoreValue = 10;
                    e->hasShield = false;
                    e->tint = (Color){255, 200, 100, 255};
                    break;

                case ENEMY_FIGHTER:
                    e->hp = 30 + (int)(g.difficulty * 8);
                    e->maxHp = e->hp;
                    e->radius = 14;
                    e->vel = (Vector2){-150 - g.difficulty * 15, 0};
                    e->fireRate = 1.5f;
                    e->scoreValue = 20;
                    e->hasShield = false;
                    e->tint = (Color){255, 100, 100, 255};
                    break;

                case ENEMY_HUNTER:
                    e->hp = 25 + (int)(g.difficulty * 6);
                    e->maxHp = e->hp;
                    e->radius = 12;
                    e->vel = (Vector2){-120 - g.difficulty * 10, 0};
                    e->fireRate = 2.0f;
                    e->scoreValue = 30;
                    e->hasShield = false;
                    e->tint = (Color){200, 100, 255, 255};
                    break;

                case ENEMY_SHIELD:
                    e->hp = 50 + (int)(g.difficulty * 10);
                    e->maxHp = e->hp;
                    e->radius = 16;
                    e->vel = (Vector2){-100 - g.difficulty * 10, 0};
                    e->fireRate = 2.5f;
                    e->scoreValue = 40;
                    e->hasShield = true;
                    e->shieldHP = 30 + g.difficulty * 5;
                    e->tint = (Color){100, 200, 255, 255};
                    break;

                case ENEMY_BOSS:
                    e->hp = 200 + (int)(g.difficulty * 50);
                    e->maxHp = e->hp;
                    e->radius = 30;
                    e->vel = (Vector2){-60, 0};
                    e->fireRate = 0.8f;
                    e->scoreValue = 200;
                    e->hasShield = true;
                    e->shieldHP = 100 + g.difficulty * 20;
                    e->tint = (Color){255, 50, 50, 255};
                    break;
            }
            return;
        }
    }
}

void Enemy_Update(Enemy *e, float dt)
{
    e->phaseTimer += dt;

    switch (e->type)
    {
        case ENEMY_SCOUT:
        {
            // Move directly left, slight sine wave
            e->pos.x += e->vel.x * dt;
            e->pos.y += sinf(e->phaseTimer * 4.0f) * 60.0f * dt;
            break;
        }

        case ENEMY_FIGHTER:
        {
            // Move left with sine pattern, shoot at player
            e->pos.x += e->vel.x * dt;
            e->pos.y += sinf(e->phaseTimer * 2.0f) * 40.0f * dt;

            e->fireTimer -= dt;
            if (e->fireTimer <= 0 && e->pos.x < SCREEN_WIDTH - 50)
            {
                e->fireTimer = e->fireRate;
                Vector2 dir = {g.player.pos.x - e->pos.x, g.player.pos.y - e->pos.y};
                float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
                if (len > 0)
                {
                    dir.x /= len;
                    dir.y /= len;
                }
                Vector2 bvel = {dir.x * 250, dir.y * 250};
                Bullet_Fire(e->pos, bvel, 8, BULLET_ENEMY, RED);
            }
            break;
        }

        case ENEMY_HUNTER:
        {
            // Track player
            Vector2 targetDir = {g.player.pos.x - e->pos.x, g.player.pos.y - e->pos.y};
            float len = sqrtf(targetDir.x * targetDir.x + targetDir.y * targetDir.y);
            if (len > 0)
            {
                targetDir.x /= len;
                targetDir.y /= len;
            }
            float speed = 150 + g.difficulty * 10;
            e->pos.x += targetDir.x * speed * dt;
            e->pos.y += targetDir.y * speed * dt;

            e->fireTimer -= dt;
            if (e->fireTimer <= 0 && e->pos.x < SCREEN_WIDTH - 50)
            {
                e->fireTimer = e->fireRate;
                Vector2 bvel = {-200, 0};
                Bullet_Fire(e->pos, bvel, 5, BULLET_ENEMY, (Color){200, 100, 255, 255});
            }
            break;
        }

        case ENEMY_SHIELD:
        {
            // Slow movement, shield absorbs hits
            e->pos.x += e->vel.x * dt;
            e->pos.y += sinf(e->phaseTimer * 1.5f) * 30.0f * dt;

            e->fireTimer -= dt;
            if (e->fireTimer <= 0)
            {
                e->fireTimer = e->fireRate;
                Vector2 bvel = {-180, 0};
                Bullet_Fire(e->pos, bvel, 10, BULLET_ENEMY, (Color){100, 200, 255, 255});
            }
            break;
        }

        case ENEMY_BOSS:
        {
            // Boss movement patterns
            e->pos.x += e->vel.x * dt;
            if (e->pos.x < SCREEN_WIDTH * 0.6f)
                e->vel.x = 0;

            // Phase-based attack patterns
            float hpPercent = (float)e->hp / e->maxHp;
            if (hpPercent < 0.3f) e->phase = 2;
            else if (hpPercent < 0.6f) e->phase = 1;

            switch (e->phase)
            {
                case 0: // Phase 1: slow sine
                    e->pos.y += sinf(e->phaseTimer * 1.0f) * 50.0f * dt;
                    break;
                case 1: // Phase 2: faster movement
                    e->pos.y += sinf(e->phaseTimer * 2.0f) * 80.0f * dt;
                    e->fireRate = 0.5f;
                    break;
                case 2: // Phase 3: aggressive
                    e->pos.y += sinf(e->phaseTimer * 3.0f) * 100.0f * dt;
                    e->fireRate = 0.3f;
                    break;
            }

            e->fireTimer -= dt;
            if (e->fireTimer <= 0)
            {
                e->fireTimer = e->fireRate;
                // Boss fires spread
                for (int a = -2; a <= 2; a++)
                {
                    Vector2 bvel = {-200 + a * 40, a * 30};
                    Bullet_Fire(e->pos, bvel, 12, BULLET_ENEMY, (Color){255, 50, 50, 255});
                }
            }
            break;
        }
    }

    // Deactivate if off screen
    if (e->pos.x < -50 || e->pos.x > SCREEN_WIDTH + 50 ||
        e->pos.y < -50 || e->pos.y > SCREEN_HEIGHT + 50)
    {
        e->active = false;
    }
}

void Enemy_Draw(const Enemy *e)
{
    Color col = e->tint;

    switch (e->type)
    {
        case ENEMY_SCOUT:
        {
            // Small diamond shape
            Vector2 p1 = {e->pos.x + e->radius, e->pos.y};
            Vector2 p2 = {e->pos.x, e->pos.y - e->radius * 0.7f};
            Vector2 p3 = {e->pos.x - e->radius, e->pos.y};
            Vector2 p4 = {e->pos.x, e->pos.y + e->radius * 0.7f};
            DrawTriangle(p1, p2, p3, col);
            DrawTriangle(p1, p3, p4, col);
            DrawCircleV(e->pos, 3, (Color){255, 255, 200, 200});
            break;
        }

        case ENEMY_FIGHTER:
        {
            // Arrow shape
            Vector2 tip = {e->pos.x - e->radius, e->pos.y};
            Vector2 left = {e->pos.x + e->radius * 0.6f, e->pos.y - e->radius};
            Vector2 right = {e->pos.x + e->radius * 0.6f, e->pos.y + e->radius};
            DrawTriangle(tip, left, right, col);
            DrawTriangleLines(tip, left, right, (Color){255, 200, 200, 200});
            DrawCircleV(e->pos, 3, RED);
            break;
        }

        case ENEMY_HUNTER:
        {
            // Hexagonal shape
            DrawCircleV(e->pos, e->radius, col);
            DrawCircleV(e->pos, e->radius * 0.6f, (Color){150, 50, 200, 200});
            DrawCircleLines(e->pos.x, e->pos.y, e->radius, (Color){200, 150, 255, 200});
            // Eyes
            DrawCircleV((Vector2){e->pos.x - 3, e->pos.y - 3}, 2, WHITE);
            DrawCircleV((Vector2){e->pos.x - 3, e->pos.y + 3}, 2, WHITE);
            break;
        }

        case ENEMY_SHIELD:
        {
            // Shielded enemy
            DrawCircleV(e->pos, e->radius, col);
            DrawCircleV(e->pos, e->radius * 0.5f, (Color){50, 100, 200, 200});
            if (e->hasShield && e->shieldHP > 0)
            {
                DrawCircleLines(e->pos.x, e->pos.y, e->radius + 4, (Color){0, 200, 255, 150});
                DrawCircleLines(e->pos.x, e->pos.y, e->radius + 2, (Color){0, 255, 255, 100});
            }
            break;
        }

        case ENEMY_BOSS:
        {
            // Large boss ship
            float r = e->radius;
            // Main body
            DrawCircleV(e->pos, r, col);
            DrawCircleV(e->pos, r * 0.8f, (Color){200, 30, 30, 200});
            // Inner core
            DrawCircleV(e->pos, r * 0.4f, (Color){255, 200, 50, 200});
            // Shield
            if (e->hasShield && e->shieldHP > 0)
            {
                DrawCircleLines(e->pos.x, e->pos.y, r + 6, (Color){255, 100, 100, 150});
                DrawCircleLines(e->pos.x, e->pos.y, r + 3, (Color){255, 50, 50, 100});
            }
            // Health bar
            float hpPct = (float)e->hp / e->maxHp;
            DrawRectangle(e->pos.x - 25, e->pos.y - r - 12, 50, 5, DARKGRAY);
            DrawRectangle(e->pos.x - 25, e->pos.y - r - 12, (int)(50 * hpPct), 5, RED);
            // Boss indicator
            DrawText("BOSS", e->pos.x - 20, e->pos.y - r - 25, 10, RED);
            break;
        }
    }
}