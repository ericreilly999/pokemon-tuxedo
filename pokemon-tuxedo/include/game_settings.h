#ifndef GUARD_GAME_SETTINGS_H
#define GUARD_GAME_SETTINGS_H

#include "global.h"

// Battle mode options
#define BATTLE_MODE_SET 0
#define BATTLE_MODE_SHIFT 1

// Game settings functions
void SetBattleMode(u8 mode);
u8 GetBattleMode(void);

void SetGameSpeed(u8 speed);
u8 GetGameSpeed(void);

void ApplyGameSpeed(u32 *duration);

void SaveGameSettings(void);
void LoadGameSettings(void);

#endif // GUARD_GAME_SETTINGS_H
