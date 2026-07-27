#include "game.h"

void Bullet_Fire(Vector2 pos, Vector2 vel, int damage, BulletType type, Color color)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!g.bullets[i].active)
        {
            g.bullets[i].pos = pos;
            g.bullets[i].vel = vel;
            g.bullets[i].active = true;
            g.bullets[i].damage = damage;
            g.bullets[i].type = type;
            g.bullets[i].color = color;
            g.bullets[i].lifetime = 0;

            switch (type)
            {
                case BULLET_BEAM:
                    g.bullets[i].radius = 6.0f;
                    g.bullets[i].lifeMax = 0.5f;
                    break;
                case BULLET_MISSILE:
                    g.bullets[i].radius = 8.0f;
                    g.bullets[i].lifeMax = 3.0f;
                    break;
                case BULLET_SOUNDWAVE:
                    g.bullets[i].radius = 10.0f;
                    g.bullets[i].lifeMax = 0.6f;
                    break;
                default:
                    g.bullets[i].radius = 3.0f;
                    g.bullets[i].lifeMax = 2.0f;
                    break;
            }
            return;
        }
    }
}

void Bullet_UpdateAll(float dt)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!g.bullets[i].active) continue;

        Bullet *b = &g.bullets[i];

        // Soundwave expands from center
        if (b->type == BULLET_SOUNDWAVE)
        {
            float pct = b->lifetime / b->lifeMax;
            b->radius = 10.0f + pct * 200.0f;
        }

        b->pos.x += b->vel.x * dt;
        b->pos.y += b->vel.y * dt;
        b->lifetime += dt;

        // Deactivate if out of bounds or expired
        if (b->pos.x < -50 || b->pos.x > SCREEN_WIDTH + 50 ||
            b->pos.y < -50 || b->pos.y > SCREEN_HEIGHT + 50 ||
            b->lifetime >= b->lifeMax)
        {
            b->active = false;
        }
    }
}

void Bullet_DrawAll(void)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!g.bullets[i].active) continue;

        Bullet *b = &g.bullets[i];

        switch (b->type)
        {
            case BULLET_BEAM:
            {
                DrawCircleV(b->pos, b->radius, b->color);
                DrawCircleV(b->pos, b->radius * 0.6f, (Color){255, 255, 255, 200});
                DrawCircleV(b->pos, b->radius * 2, (Color){b->color.r, b->color.g, b->color.b, 50});
                break;
            }

            case BULLET_MISSILE:
            {
                // Large missile with tail flame
                DrawCircleV(b->pos, b->radius, b->color);
                DrawCircleV(b->pos, b->radius * 0.6f, (Color){255, 200, 100, 200});
                // Flame trail
                DrawCircleV((Vector2){b->pos.x - 8, b->pos.y}, 5,
                    (Color){255, 100, 50, 150});
                DrawCircleV((Vector2){b->pos.x - 14, b->pos.y}, 3,
                    (Color){255, 50, 0, 100});
                // Glow
                DrawCircleV(b->pos, b->radius * 2.5f, (Color){b->color.r, b->color.g, b->color.b, 40});
                break;
            }

            case BULLET_SOUNDWAVE:
            {
                // Expanding ring
                float alpha = 1.0f - (b->lifetime / b->lifeMax);
                Color col = b->color;
                col.a = (unsigned char)(alpha * 255);
                DrawCircleLines(b->pos.x, b->pos.y, b->radius, col);
                DrawCircleLines(b->pos.x, b->pos.y, b->radius * 0.7f,
                    (Color){col.r, col.g, col.b, (unsigned char)(col.a * 0.5f)});
                // Inner glow
                DrawCircleV(b->pos, b->radius * 0.3f, (Color){col.r, col.g, col.b, (unsigned char)(col.a * 0.3f)});
                break;
            }

            default:
            {
                // Regular bullet with glow
                DrawCircleV(b->pos, b->radius, b->color);
                DrawCircleV(b->pos, b->radius * 0.5f, (Color){255, 255, 255, 200});
                DrawCircleV((Vector2){b->pos.x - b->vel.x * 0.02f, b->pos.y - b->vel.y * 0.02f},
                           b->radius * 0.7f,
                           (Color){b->color.r, b->color.g, b->color.b, 100});
                break;
            }
        }
    }
}