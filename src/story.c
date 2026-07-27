#include "game.h"

void Story_Init(void)
{
    // Initialize story lines
    strcpy(g.story[0].text, "Unknown signal detected...");
    g.story[0].displayTimer = 0;
    g.story[0].shown = false;

    strcpy(g.story[1].text, "The signal is responding...");
    g.story[1].displayTimer = 0;
    g.story[1].shown = false;

    strcpy(g.story[2].text, "The aliens were waiting...");
    g.story[2].displayTimer = 0;
    g.story[2].shown = false;

    strcpy(g.story[3].text, "They have been expecting us...");
    g.story[3].displayTimer = 0;
    g.story[3].shown = false;

    strcpy(g.story[4].text, "The Star Void was created by an ancient civilization.");
    g.story[4].displayTimer = 0;
    g.story[4].shown = false;

    g.currentStoryIndex = 0;
}

void Story_Advance(void)
{
    if (g.currentStoryIndex < MAX_STORY_LINES - 1)
    {
        g.currentStoryIndex++;
        g.story[g.currentStoryIndex].shown = true;
        g.story[g.currentStoryIndex].displayTimer = 0;
    }
}

const char* Story_GetCurrentText(void)
{
    return g.story[g.currentStoryIndex].text;
}

bool Story_IsComplete(void)
{
    return g.currentStoryIndex >= MAX_STORY_LINES - 1;
}