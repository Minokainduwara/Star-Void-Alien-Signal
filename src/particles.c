#include "game.h"

void Particle_Spawn(Vector2 pos, Vector2 vel, float radius, Color color, int lifetime, bool gravity)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!g.particles[i].active)
        {
            g.particles[i].pos = pos;
            g.particles[i].vel = vel;
            g.particles[i].radius = radius;
            g.particles[i].color = color;
            g.particles[i].lifetime = lifetime;
            g.particles[i].maxLifetime = lifetime;
            g.particles[i].active = true;
            g.particles[i].gravity = gravity;
            return;
        }
    }
}

void Particle_SpawnBurst(Vector2 pos, int count, Color color, float speed)
{
    for (int i = 0; i < count; i++)
    {
        float angle = (float)(rand() % 360) * DEG2RAD;
        float spd = (float)(rand() % (int)speed) + speed * 0.3f;
        Vector2 vel = {cosf(angle) * spd, sinf(angle) * spd};
        float radius = (float)(rand() % 4) + 1.0f;
        int lifetime = (rand() % 20) + 15;
        Particle_Spawn(pos, vel, radius, color, lifetime, false);
    }
}

void Particle_SpawnTrail(Vector2 pos, Color color)
{
    for (int i = 0; i < 2; i++)
    {
        Vector2 vel = {-(float)(rand() % 50) - 20, (float)(rand() % 20 - 10)};
        float radius = (float)(rand() % 3) + 1.0f;
        int lifetime = (rand() % 10) + 5;
        Particle_Spawn(pos, vel, radius, color, lifetime, false);
    }
}

void Particle_UpdateAll(float dt)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!g.particles[i].active) continue;

        Particle *p = &g.particles[i];
        p->pos.x += p->vel.x * dt;
        p->pos.y += p->vel.y * dt;
        if (p->gravity)
            p->vel.y += 200.0f * dt;
        p->vel.x *= 0.98f;
        p->vel.y *= 0.98f;
        p->lifetime--;

        if (p->lifetime <= 0)
            p->active = false;
    }
}

void Particle_DrawAll(void)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!g.particles[i].active) continue;

        Particle *p = &g.particles[i];
        float alpha = (float)p->lifetime / p->maxLifetime;
        Color col = p->color;
        col.a = (unsigned char)(col.a * alpha);
        DrawCircleV(p->pos, p->radius * alpha, col);
    }
}

void Starfield_Init(void)
{
    for (int i = 0; i < MAX_STARS; i++)
    {
        g.stars[i].pos.x = (float)(rand() % SCREEN_WIDTH);
        g.stars[i].pos.y = (float)(rand() % SCREEN_HEIGHT);
        g.stars[i].radius = (float)(rand() % 3) + 0.5f;
        g.stars[i].brightness = (float)(rand() % 100) / 100.0f;
        g.stars[i].phase = (float)(rand() % 1000) / 100.0f;

        // Star colors
        int c = rand() % 4;
        switch (c)
        {
            case 0: g.stars[i].color = (Color){200, 220, 255, 255}; break; // Blue-white
            case 1: g.stars[i].color = (Color){255, 255, 200, 255}; break; // Yellow
            case 2: g.stars[i].color = (Color){255, 200, 200, 255}; break; // Red
            case 3: g.stars[i].color = (Color){200, 255, 200, 255}; break; // Green
        }
    }
}

void Starfield_Update(float dt)
{
    for (int i = 0; i < MAX_STARS; i++)
    {
        g.stars[i].pos.x -= (g.stars[i].radius * 20.0f + 10.0f) * dt;
        g.stars[i].phase += dt;

        // Twinkle
        g.stars[i].brightness = 0.5f + 0.5f * sinf(g.stars[i].phase * 3.0f);

        // Wrap around
        if (g.stars[i].pos.x < -5)
        {
            g.stars[i].pos.x = SCREEN_WIDTH + 5;
            g.stars[i].pos.y = (float)(rand() % SCREEN_HEIGHT);
        }
    }
}

void Starfield_Draw(void)
{
    for (int i = 0; i < MAX_STARS; i++)
    {
        Color col = g.stars[i].color;
        col.a = (unsigned char)(g.stars[i].brightness * 255);
        DrawCircleV(g.stars[i].pos, g.stars[i].radius, col);
    }
}