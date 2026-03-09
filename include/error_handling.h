#ifndef GUARD_ERROR_HANDLING_H
#define GUARD_ERROR_HANDLING_H

#include "global.h"
#include "level_scaler.h"

// Level calculation safety
u8 ClampLevel(u8 level);
bool8 ValidateBadgeCount(u8 badge_count);
bool8 ValidateRegionId(u8 region_id);
struct LevelRange SafeGetWildPokemonLevelRange(u8 badge_count, u8 region_id, bool8 elite_four_defeated);
u8 SafeGetGymLeaderAverageLevel(u8 badge_count, u8 region_id);
u8 SafeGetGymLeaderAceLevel(u8 badge_count);
u8 SafeGetEliteFourAverageLevel(u8 badge_count, u8 region_id);
u8 SafeGetEliteFourAceLevel(u8 badge_count);
u8 SafeGetRivalAverageLevel(u8 badge_count, u8 region_id);
u8 SafeGetRivalAceLevel(u8 badge_count);

// Save data corruption handling
bool8 ValidateSaveDataChecksum(void);
void RecoverFromCorruptedSave(void);
bool8 CreateBackupSave(void);
bool8 RestoreFromBackup(void);

// Battle system error handling
bool8 ValidateExpGain(u32 exp);
bool8 CanPokemonGainExp(struct Pokemon *pokemon);
void SafeAwardExperienceToParty(u32 base_exp);

// Storage system error handling
bool8 ValidateStorageAccess(void);
bool8 ValidatePokemonTransfer(struct Pokemon *pokemon);

// Region travel error handling
bool8 ValidateRegionTravel(u8 target_region);

// Error logging
void LogError(const char *system, const char *message);

// Comprehensive validation
bool8 ValidateGameState(void);

#endif // GUARD_ERROR_HANDLING_H
