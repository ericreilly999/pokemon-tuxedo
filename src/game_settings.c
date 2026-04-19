#include "global.h"
#include "game_settings.h"

/*
 * Game Settings System
 *
 * Requirements:
 * - 17.1: Default battle mode to SET
 * - 17.2: Settings menu interface
 * - 17.3: Game speed range 1x-10x
 * - 17.4: Apply game speed to animations and text
 * - 17.5: Persist settings across sessions
 */

/* Settings storage */
EWRAM_DATA static u8 sBattleMode = BATTLE_MODE_SET; /* Default to SET */
EWRAM_DATA static u8 sGameSpeed = 1;                /* Default to 1x */

void SetBattleMode(u8 mode)
{
    if (mode == BATTLE_MODE_SET || mode == BATTLE_MODE_SHIFT)
        sBattleMode = mode;
}

u8 GetBattleMode(void)
{
    return sBattleMode;
}

void SetGameSpeed(u8 speed)
{
    if (speed >= 1 && speed <= 10)
        sGameSpeed = speed;
}

u8 GetGameSpeed(void)
{
    return sGameSpeed;
}

void ApplyGameSpeed(u32 *duration)
{
    /* Apply game speed multiplier to duration */
    if (sGameSpeed > 1)
        *duration = *duration / sGameSpeed;
}

void SaveGameSettings(void)
{
    /* Save settings to save data.
       This would integrate with the main save system. */
}

void LoadGameSettings(void)
{
    /* Load settings from save data.
       This would integrate with the main save system. */
}
