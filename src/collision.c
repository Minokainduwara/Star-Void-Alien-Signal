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
        if (g.bullets[i].type != BULLET_PLAYER && g.bullets[i].type != BULLET_BEAM &&
            g.bullets[i].type != BULLET_MISSILE && g.bullets[i].type != BULLET_SOUNDWAVE)
            continue;
        if (g.bullets[i].damage <= 0) continue;

        // Soundwave hits ALL enemies in range
        if (g.bullets[i].type == BULLET_SOUNDWAVE)
        {
            for (int j = 0; j < MAX_ENEMIES; j++)
            {
                if (!g.enemies[j].active) continue;
                Enemy *e = &g.enemies[j];

                if (Collision_CircleCircle(g.bullets[i].pos, g.bullets[i].radius,
                                           e->pos, e->radius))
                {
                    DealDamageToEnemy(e, g.bullets[i].damage, &g.bullets[i]);
                }
            }
            continue; // Soundwave persists for its duration
        }

        for (int j = 0; j < MAX_ENEMIES; j++)
        {
            if (!g.enemies[j].active) continue;

            if (Collision_CircleCircle(g.bullets[i].pos, g.bullets[i].radius,
                                       g.enemies[j].pos, g.enemies[j].radius))
            {
                Enemy *e = &g.enemies[j];
                DealDamageToEnemy(e, g.bullets[i].damage, &g.bullets[i]);

                // Missile explodes on impact and damages nearby enemies
                if (g.bullets[i].type == BULLET_MISSILE)
                {
                    Particle_SpawnBurst(g.bullets[i].pos, 20, (Color){255, 150, 50, 255}, 200);
                    g.screenShake = 6.0f;
                    // AOE damage to nearby enemies
                    for (int k = 0; k < MAX_ENEMIES; k++)
                    {
                        if (!g.enemies[k].active || k == j) continue;
                        if (Collision_CircleCircle(g.bullets[i].pos, 60,
                                                   g.enemies[k].pos, g.enemies[k].radius))
                        {
                            DealDamageToEnemy(&g.enemies[k], g.bullets[i].damage / 2, NULL);
                        }
                    }
                }

                g.bullets[i].active = false;
                break;
            }
        }
    }

    // Bullet vs bullet: player bullets destroy enemy bullets
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!g.bullets[i].active) continue;
        if (g.bullets[i].type != BULLET_PLAYER && g.bullets[i].type != BULLET_BEAM &&
            g.bullets[i].type != BULLET_MISSILE) continue;
        if (g.bullets[i].damage <= 0) continue;

        for (int k = 0; k < MAX_BULLETS; k++)
        {
            if (!g.bullets[k].active) continue;
            if (g.bullets[k].type != BULLET_ENEMY) continue;
            if (i == k) continue;

            if (Collision_CircleCircle(g.bullets[i].pos, g.bullets[i].radius,
                                       g.bullets[k].pos, g.bullets[k].radius))
            {
                // Both bullets destroy each other
                g.bullets[i].active = false;
                g.bullets[k].active = false;
                Particle_SpawnBurst(g.bullets[i].pos, 6, (Color){255, 200, 100, 200}, 120);
                break;
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

// Helper: deal damage to an enemy, handle kills and drops
void DealDamageToEnemy(Enemy *e, int damage, Bullet *bullet)
{
    if (!e->active) return;

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
    if (bullet)
        Particle_SpawnBurst(bullet->pos, 4, YELLOW, 100);

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

        // Drop logic
        bool isBoss = (e->type == ENEMY_BOSS);

        // Energy crystal (70% chance)
        if (rand() % 100 < 70)
            PowerUp_Spawn(e->pos, POWERUP_ENERGY);

        // Health/shield
        if (rand() % 100 < 15)
            PowerUp_Spawn(e->pos, POWERUP_HEALTH);
        if (rand() % 100 < 10)
            PowerUp_Spawn(e->pos, POWERUP_SHIELD);

        // Drone (rare)
        if (rand() % 100 < 5 && !g.player.hasDrone)
            PowerUp_Spawn(e->pos, POWERUP_DRONE);

        // Void special (rare)
        if (rand() % 100 < 3)
            PowerUp_Spawn(e->pos, POWERUP_SPECIAL);

        // Special weapon drops (rare from regular enemies, guaranteed from bosses)
        if (isBoss)
        {
            // Boss always drops 1-2 special weapons
            int drops = 1 + (rand() % 2);
            for (int d = 0; d < drops; d++)
            {
                int r = rand() % 3;
                if (r == 0) PowerUp_Spawn(e->pos, POWERUP_MISSILE);
                else if (r == 1) PowerUp_Spawn(e->pos, POWERUP_LASER_BEAM);
                else PowerUp_Spawn(e->pos, POWERUP_SOUNDWAVE);
            }
            // Boss also drops extra energy
            for (int d = 0; d < 3; d++)
                PowerUp_Spawn(e->pos, POWERUP_ENERGY);
        }
        else if (g.wave.currentWave >= 3 && rand() % 100 < 8)
        {
            // Rare special weapon drop from regular enemies (8% after wave 3)
            int r = rand() % 3;
            if (r == 0) PowerUp_Spawn(e->pos, POWERUP_MISSILE);
            else if (r == 1) PowerUp_Spawn(e->pos, POWERUP_LASER_BEAM);
            else PowerUp_Spawn(e->pos, POWERUP_SOUNDWAVE);
        }

        // Guaranteed special drop every 5th wave
        if (g.wave.currentWave % 5 == 0 && g.wave.waveComplete)
        {
            int r = rand() % 3;
            if (r == 0) PowerUp_Spawn(e->pos, POWERUP_MISSILE);
            else if (r == 1) PowerUp_Spawn(e->pos, POWERUP_LASER_BEAM);
            else PowerUp_Spawn(e->pos, POWERUP_SOUNDWAVE);
        }

        Audio_PlayExplosion();
    }
}