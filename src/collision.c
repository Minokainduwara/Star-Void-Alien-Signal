#include "game.h"

bool Collision_CircleCircle(Vector2 a, float ra, Vector2 b, float rb)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dist = sqrtf(dx * dx + dy * dy);
    return dist < ra + rb;
}

bool Collision_CircleRect(Vector2 c, float r, Rectangle rect)
{
    float nearestX = fmaxf(rect.x, fminf(c.x, rect.x + rect.width));
    float nearestY = fmaxf(rect.y, fminf(c.y, rect.y + rect.height));
    float dx = c.x - nearestX;
    float dy = c.y - nearestY;
    return (dx * dx + dy * dy) < (r * r);
}

void Collision_CheckAll(void)
{
    // Player bullets vs enemies
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!g.bullets[i].active) continue;
        if (g.bullets[i].type != BULLET_PLAYER && g.bullets[i].type != BULLET_BEAM) continue;
        if (g.bullets[i].damage <= 0) continue; // Visual-only beams

        for (int j = 0; j < MAX_ENEMIES; j++)
        {
            if (!g.enemies[j].active) continue;

            if (Collision_CircleCircle(g.bullets[i].pos, g.bullets[i].radius,
                                       g.enemies[j].pos, g.enemies[j].radius))
            {
                Enemy *e = &g.enemies[j];
                int damage = g.bullets[i].damage;

                // Check shield
                if (e->hasShield && e->shieldHP > 0)
                {
                    e->shieldHP -= damage;
                    if (e->shieldHP <= 0)
                    {
                        e->hasShield = false;
                        e->shieldHP = 0;
                        Particle_SpawnBurst(e->pos, 10, (Color){0, 200, 255, 200}, 200);
                    }
                }
                else
                {
                    e->hp -= damage;
                }

                // Hit particles
                Particle_SpawnBurst(g.bullets[i].pos, 4, YELLOW, 100);

                // Deactivate bullet
                g.bullets[i].active = false;

                // Check enemy killed
                if (e->hp <= 0)
                {
                    e->active = false;
                    g.player.score += e->scoreValue * (1 + g.comboCount);
                    g.player.killCount++;
                    g.comboCount++;
                    g.comboDisplayTimer = 2.0f;

                    // Explosion particles
                    Particle_SpawnBurst(e->pos, 15, (Color){255, 100, 50, 255}, 200);
                    Particle_SpawnBurst(e->pos, 8, YELLOW, 150);
                    g.screenShake = 3.0f;

                    // Spawn energy crystal
                    if (rand() % 100 < 70) // 70% drop rate
                    {
                        PowerUp_Spawn(e->pos, POWERUP_ENERGY);
                    }
                    // Random health/shield drops
                    if (rand() % 100 < 15)
                        PowerUp_Spawn(e->pos, POWERUP_HEALTH);
                    if (rand() % 100 < 10)
                        PowerUp_Spawn(e->pos, POWERUP_SHIELD);
                    if (rand() % 100 < 5 && !g.player.hasDrone)
                        PowerUp_Spawn(e->pos, POWERUP_DRONE);
                    if (rand() % 100 < 3)
                        PowerUp_Spawn(e->pos, POWERUP_SPECIAL);

                    Audio_PlayExplosion();
                }

                break; // One bullet hits one enemy
            }
        }
    }

    // Enemy bullets vs player
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!g.bullets[i].active) continue;
        if (g.bullets[i].type != BULLET_ENEMY) continue;

        if (Collision_CircleCircle(g.bullets[i].pos, g.bullets[i].radius,
                                   g.player.pos, PLAYER_SIZE))
        {
            Player_TakeDamage(&g.player, g.bullets[i].damage);
            g.bullets[i].active = false;
            Particle_SpawnBurst(g.bullets[i].pos, 5, RED, 100);
        }
    }

    // Enemies vs player (collision damage)
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!g.enemies[i].active) continue;

        if (Collision_CircleCircle(g.enemies[i].pos, g.enemies[i].radius,
                                   g.player.pos, PLAYER_SIZE))
        {
            Player_TakeDamage(&g.player, 15);
            // Bounce enemy away
            g.enemies[i].pos.x += 30;
            break;
        }
    }

    // PowerUps vs player
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!g.powerups[i].active) continue;

        if (Collision_CircleCircle(g.powerups[i].pos, g.powerups[i].radius,
                                   g.player.pos, PLAYER_SIZE + 8))
        {
            PowerUp_Collect(&g.player, g.powerups[i].type);
            g.powerups[i].active = false;
            Particle_SpawnBurst(g.powerups[i].pos, 8, GREEN, 100);
            Audio_PlayPowerUp();
        }
    }
}