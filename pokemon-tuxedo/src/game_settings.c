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

/* Settings storage — EWRAM_DATA cannot have non-zero initializers on GBA.
   Defaults are applied by LoadGameSettings() / InitGameSettings(). */
EWRAM_DATA static u8 sBattleMode = 0;
EWRAM_DATA static u8 sGameSpeed = 0;

/* Returns BATTLE_MODE_SET if sBattleMode has not been initialised (0 maps to SET). */
static u8 GetInitialisedBattleMode(void)
{
    return (sBattleMode == 0) ? BATTLE_MODE_SET : sBattleMode;
}

/* Returns 1x speed if sGameSpeed has not been initialised. */
static u8 GetInitialisedGameSpeed(void)
{
    return (sGameSpeed == 0) ? 1 : sGameSpeed;
}

void SetBattleMode(u8 mode)
{
    if (mode == BATTLE_MODE_SET || mode == BATTLE_MODE_SHIFT)
        sBattleMode = mode;
}

u8 GetBattleMode(void)
{
    return GetInitialisedBattleMode();
}

void SetGameSpeed(u8 speed)
{
    if (speed >= 1 && speed <= 10)
        sGameSpeed = speed;
}

u8 GetGameSpeed(void)
{
    return GetInitialisedGameSpeed();
}

void ApplyGameSpeed(u32 *duration)
{
    u8 speed = GetInitialisedGameSpeed();
    /* Apply game speed multiplier to duration */
    if (speed > 1)
        *duration = *duration / speed;
}

void SaveGameSettings(void)
{
    // Save settings to save data
    // This would integrate with the main save system
}

void LoadGameSettings(void)
{
    // Load settings from save data
    // This would integrate with the main save system
}
