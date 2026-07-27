#include "game.h"

void Bullet_Fire(Vector2 pos, Vector2 vel, int damage, BulletType type, Color color)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!g.bullets[i].active)
        {
            g.bullets[i].pos = pos;
            g.bullets[i].vel = vel;
            g.bullets[i].radius = type == BULLET_BEAM ? 6.0f : 3.0f;
            g.bullets[i].active = true;
            g.bullets[i].damage = damage;
            g.bullets[i].type = type;
            g.bullets[i].color = color;
            g.bullets[i].lifeMax = type == BULLET_BEAM ? 0.5f : 2.0f;
            g.bullets[i].lifetime = 0;
            return;
        }
    }
}

void Bullet_UpdateAll(float dt)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!g.bullets[i].active) continue;

        g.bullets[i].pos.x += g.bullets[i].vel.x * dt;
        g.bullets[i].pos.y += g.bullets[i].vel.y * dt;
        g.bullets[i].lifetime += dt;

        // Deactivate if out of bounds or expired
        if (g.bullets[i].pos.x < -20 || g.bullets[i].pos.x > SCREEN_WIDTH + 20 ||
            g.bullets[i].pos.y < -20 || g.bullets[i].pos.y > SCREEN_HEIGHT + 20 ||
            g.bullets[i].lifetime >= g.bullets[i].lifeMax)
        {
            g.bullets[i].active = false;
        }
    }
}

void Bullet_DrawAll(void)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!g.bullets[i].active) continue;

        Bullet *b = &g.bullets[i];

        if (b->type == BULLET_BEAM)
        {
            // Beam effect - elongated ellipse
            DrawCircleV(b->pos, b->radius, b->color);
            DrawCircleV(b->pos, b->radius * 0.6f, (Color){255, 255, 255, 200});
            // Glow
            DrawCircleV(b->pos, b->radius * 2, (Color){b->color.r, b->color.g, b->color.b, 50});
        }
        else
        {
            // Regular bullet with glow
            DrawCircleV(b->pos, b->radius, b->color);
            DrawCircleV(b->pos, b->radius * 0.5f, (Color){255, 255, 255, 200});
            // Trail
            DrawCircleV((Vector2){b->pos.x - b->vel.x * 0.02f, b->pos.y - b->vel.y * 0.02f},
                       b->radius * 0.7f,
                       (Color){b->color.r, b->color.g, b->color.b, 100});
        }
    }
}