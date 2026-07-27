#include "game.h"

void PowerUp_Spawn(Vector2 pos, PowerUpType type)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!g.powerups[i].active)
        {
            g.powerups[i].pos = pos;
            g.powerups[i].active = true;
            g.powerups[i].type = type;
            g.powerups[i].radius = 8;
            g.powerups[i].vel = (Vector2){-30, 0};
            g.powerups[i].lifetime = 300; // 5 seconds at 60fps
            g.powerups[i].value = 1;

            switch (type)
            {
                case POWERUP_ENERGY:
                    g.powerups[i].color = (Color){0, 255, 200, 255};
                    g.powerups[i].value = 5 + (int)g.difficulty;
                    break;
                case POWERUP_HEALTH:
                    g.powerups[i].color = (Color){0, 255, 50, 255};
                    g.powerups[i].value = 25;
                    break;
                case POWERUP_SHIELD:
                    g.powerups[i].color = (Color){0, 200, 255, 255};
                    g.powerups[i].value = 20;
                    break;
                case POWERUP_SPEED_BOOST:
                    g.powerups[i].color = (Color){255, 255, 0, 255};
                    g.powerups[i].value = 5; // seconds
                    break;
                case POWERUP_DRONE:
                    g.powerups[i].color = (Color){200, 0, 255, 255};
                    g.powerups[i].value = 1;
                    break;
                case POWERUP_SPECIAL:
                    g.powerups[i].color = (Color){255, 50, 50, 255};
                    g.powerups[i].value = 1;
                    break;
            }
            return;
        }
    }
}

void PowerUp_UpdateAll(float dt)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!g.powerups[i].active) continue;

        g.powerups[i].pos.x += g.powerups[i].vel.x * dt;
        g.powerups[i].pos.y += sinf(g.gameTime * 3.0f + i) * 30.0f * dt;
        g.powerups[i].lifetime--;

        if (g.powerups[i].lifetime <= 0 || g.powerups[i].pos.x < -20)
            g.powerups[i].active = false;
    }
}

void PowerUp_DrawAll(void)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!g.powerups[i].active) continue;

        PowerUp *p = &g.powerups[i];
        float pulse = 1.0f + 0.2f * sinf(g.gameTime * 5.0f + i);

        // Glow
        DrawCircleV(p->pos, p->radius * pulse * 1.5f,
                    (Color){p->color.r, p->color.g, p->color.b, 50});
        // Core
        DrawCircleV(p->pos, p->radius * pulse, p->color);
        DrawCircleV(p->pos, p->radius * pulse * 0.5f, WHITE);

        // Icon indicator
        const char *icon = "";
        switch (p->type)
        {
            case POWERUP_ENERGY: icon = "E"; break;
            case POWERUP_HEALTH: icon = "+"; break;
            case POWERUP_SHIELD: icon = "S"; break;
            case POWERUP_SPEED_BOOST: icon = ">"; break;
            case POWERUP_DRONE: icon = "D"; break;
            case POWERUP_SPECIAL: icon = "!"; break;
        }
        DrawText(icon, p->pos.x - 4, p->pos.y - 6, 10, BLACK);
    }
}

void PowerUp_Collect(Player *p, PowerUpType type)
{
    switch (type)
    {
        case POWERUP_ENERGY:
            p->energyFragments += 5 + (int)g.difficulty;
            break;
        case POWERUP_HEALTH:
            Player_Heal(p, 25);
            break;
        case POWERUP_SHIELD:
            Player_AddShield(p, 20);
            break;
        case POWERUP_SPEED_BOOST:
            p->speed += 50;
            break;
        case POWERUP_DRONE:
            p->hasDrone = true;
            break;
        case POWERUP_SPECIAL:
            // Screen clear - damage all enemies
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (g.enemies[i].active)
                {
                    g.enemies[i].hp -= 50;
                    if (g.enemies[i].hp <= 0)
                    {
                        g.enemies[i].active = false;
                        g.player.score += g.enemies[i].scoreValue;
                        g.player.killCount++;
                        Particle_SpawnBurst(g.enemies[i].pos, 15, (Color){255, 100, 50, 255}, 200);
                    }
                }
            }
            g.screenShake = 10.0f;
            break;
    }
}

const char* PowerUp_GetName(PowerUpType type)
{
    switch (type)
    {
        case POWERUP_ENERGY: return "Energy Crystal";
        case POWERUP_HEALTH: return "Health Restore";
        case POWERUP_SHIELD: return "Shield Boost";
        case POWERUP_SPEED_BOOST: return "Speed Boost";
        case POWERUP_DRONE: return "Drone Companion";
        case POWERUP_SPECIAL: return "Void Blast!";
        default: return "Unknown";
    }
}