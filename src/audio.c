#include "game.h"

static Sound shootSnd;
static Sound hitSnd;
static Sound explosionSnd;
static Sound powerupSnd;
static Sound bossWarningSnd;
static Sound levelUpSnd;
static Sound gameOverSnd;
static Sound specialSnd;
static bool audioReady = false;

static Sound GenerateTone(float frequency, float duration, float volume)
{
    int sampleRate = 22050;
    int sampleCount = (int)(sampleRate * duration);
    if (sampleCount <= 0) sampleCount = 1;

    unsigned char *data = (unsigned char *)malloc(sampleCount * sizeof(unsigned char));
    if (!data)
    {
        Sound s = {0};
        return s;
    }

    for (int i = 0; i < sampleCount; i++)
    {
        float t = (float)i / sampleRate;
        float sample = sinf(2.0f * PI * frequency * t);
        float envelope = 1.0f;
        float fadeLen = duration * 0.1f;
        if (t < fadeLen)
            envelope = t / fadeLen;
        else if (t > duration - fadeLen)
            envelope = (duration - t) / fadeLen;
        data[i] = (unsigned char)((sample * envelope * volume + 1.0f) * 127.0f);
    }

    Wave wave = {
        .data = data,
        .frameCount = (unsigned int)sampleCount,
        .sampleRate = sampleRate,
        .sampleSize = 8,
        .channels = 1
    };

    Sound s = LoadSoundFromWave(wave);
    free(data);
    return s;
}

void Audio_Init(void)
{
    if (!IsAudioDeviceReady())
        return;

    shootSnd = GenerateTone(880.0f, 0.08f, 0.3f);
    hitSnd = GenerateTone(220.0f, 0.15f, 0.4f);
    explosionSnd = GenerateTone(100.0f, 0.3f, 0.5f);
    powerupSnd = GenerateTone(660.0f, 0.2f, 0.3f);
    bossWarningSnd = GenerateTone(150.0f, 0.5f, 0.4f);
    levelUpSnd = GenerateTone(440.0f, 0.3f, 0.3f);
    gameOverSnd = GenerateTone(80.0f, 0.8f, 0.5f);
    specialSnd = GenerateTone(1200.0f, 0.3f, 0.4f);
    audioReady = true;
}

void Audio_PlayShoot(void)
{
    if (audioReady) PlaySound(shootSnd);
}

void Audio_PlayHit(void)
{
    if (audioReady) PlaySound(hitSnd);
}

void Audio_PlayExplosion(void)
{
    if (audioReady) PlaySound(explosionSnd);
}

void Audio_PlayPowerUp(void)
{
    if (audioReady) PlaySound(powerupSnd);
}

void Audio_PlayBossWarning(void)
{
    if (audioReady) PlaySound(bossWarningSnd);
}

void Audio_PlayLevelUp(void)
{
    if (audioReady) PlaySound(levelUpSnd);
}

void Audio_PlayGameOver(void)
{
    if (audioReady) PlaySound(gameOverSnd);
}

void Audio_PlaySpecial(void)
{
    if (audioReady) PlaySound(specialSnd);
}

void Audio_Shutdown(void)
{
    if (audioReady)
    {
        UnloadSound(shootSnd);
        UnloadSound(hitSnd);
        UnloadSound(explosionSnd);
        UnloadSound(powerupSnd);
        UnloadSound(bossWarningSnd);
        UnloadSound(levelUpSnd);
        UnloadSound(gameOverSnd);
        UnloadSound(specialSnd);
        audioReady = false;
    }
}