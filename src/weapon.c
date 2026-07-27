#include "game.h"

const char* Weapon_GetName(WeaponLevel wl)
{
    switch (wl)
    {
        case WEAPON_SINGLE: return "Basic Laser";
        case WEAPON_DOUBLE: return "Double Laser";
        case WEAPON_SPREAD: return "Plasma Spread";
        case WEAPON_BEAM:   return "Void Cannon";
        default:            return "Unknown";
    }
}

int Weapon_GetDamage(WeaponLevel wl)
{
    int base = 10;
    int bonus = g.upgrades[1].upgradeLevel * 5;
    switch (wl)
    {
        case WEAPON_SINGLE: return base + bonus;
        case WEAPON_DOUBLE: return (int)((base + bonus) * 0.8f);
        case WEAPON_SPREAD: return (int)((base + bonus) * 0.6f);
        case WEAPON_BEAM:   return (int)((base + bonus) * 1.5f);
        default:            return base + bonus;
    }
}

float Weapon_GetFireRate(WeaponLevel wl)
{
    float base = PLAYER_FIRE_RATE;
    float speedBonus = 1.0f - g.upgrades[0].upgradeLevel * 0.15f;
    if (speedBonus < 0.4f) speedBonus = 0.4f;
    switch (wl)
    {
        case WEAPON_SINGLE: return base * speedBonus;
        case WEAPON_DOUBLE: return base * 1.2f * speedBonus;
        case WEAPON_SPREAD: return base * 1.4f * speedBonus;
        case WEAPON_BEAM:   return base * 0.8f * speedBonus;
        default:            return base * speedBonus;
    }
}

void Weapon_Fire(Player *p, Bullet *bullets, float dt)
{
    if (p->fireTimer > 0) return;

    float fireRate = Weapon_GetFireRate(p->weaponLevel);
    p->fireTimer = fireRate;

    int damage = Weapon_GetDamage(p->weaponLevel);
    Vector2 baseVel = {500, 0};

    switch (p->weaponLevel)
    {
        case WEAPON_SINGLE:
        {
            Vector2 pos = {p->pos.x + PLAYER_SIZE, p->pos.y};
            Bullet_Fire(pos, baseVel, damage, BULLET_PLAYER, (Color){0, 200, 255, 255});
            Particle_SpawnTrail(pos, (Color){0, 200, 255, 100});
            Audio_PlayShoot();
            break;
        }

        case WEAPON_DOUBLE:
        {
            Vector2 pos1 = {p->pos.x + PLAYER_SIZE, p->pos.y - 6};
            Vector2 pos2 = {p->pos.x + PLAYER_SIZE, p->pos.y + 6};
            Bullet_Fire(pos1, baseVel, damage, BULLET_PLAYER, (Color){0, 200, 255, 255});
            Bullet_Fire(pos2, baseVel, damage, BULLET_PLAYER, (Color){100, 200, 255, 255});
            Particle_SpawnTrail(pos1, (Color){0, 200, 255, 100});
            Particle_SpawnTrail(pos2, (Color){100, 200, 255, 100});
            Audio_PlayShoot();
            break;
        }

        case WEAPON_SPREAD:
        {
            Vector2 vel1 = {450, -60};
            Vector2 vel2 = {500, 0};
            Vector2 vel3 = {450, 60};
            Vector2 pos = {p->pos.x + PLAYER_SIZE, p->pos.y};
            Bullet_Fire(pos, vel1, damage, BULLET_PLAYER, (Color){255, 100, 200, 255});
            Bullet_Fire(pos, vel2, damage, BULLET_PLAYER, (Color){255, 150, 200, 255});
            Bullet_Fire(pos, vel3, damage, BULLET_PLAYER, (Color){255, 100, 200, 255});
            Particle_SpawnBurst(pos, 3, (Color){255, 100, 200, 150}, 80);
            Audio_PlayShoot();
            break;
        }

        case WEAPON_BEAM:
        {
            Vector2 pos = {p->pos.x + PLAYER_SIZE, p->pos.y};
            Vector2 beamVel = {700, 0};
            Bullet_Fire(pos, beamVel, damage, BULLET_BEAM, (Color){255, 50, 50, 255});
            // Extra visual beam
            Bullet_Fire((Vector2){pos.x, pos.y - 3}, beamVel, 0, BULLET_BEAM, (Color){255, 100, 50, 150});
            Bullet_Fire((Vector2){pos.x, pos.y + 3}, beamVel, 0, BULLET_BEAM, (Color){255, 100, 50, 150});
            Particle_SpawnBurst(pos, 5, (Color){255, 50, 50, 200}, 120);
            Audio_PlayShoot();
            break;
        }
    }
}