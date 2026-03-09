#ifndef GUARD_LEVEL_SCALER_H
#define GUARD_LEVEL_SCALER_H

#include "global.h"

// Level range structure for wild Pokemon
struct LevelRange {
    u8 min_level;
    u8 max_level;
};

// Wild Pokemon level calculation
struct LevelRange GetWildPokemonLevelRange(u8 badge_count, u8 region_id, bool8 elite_four_defeated);

// Region starting level lookup
u8 GetRegionStartingLevel(u8 region_id);

// Trainer Pokemon level calculation
u8 GetGymLeaderAverageLevel(u8 badge_count, u8 region_id);
u8 GetGymLeaderAceLevel(u8 gym_leader_id);
u8 GetEliteFourAverageLevel(u8 badge_count, u8 region_id);
u8 GetEliteFourAceLevel(u8 elite_four_member_id);
u8 GetRivalAverageLevel(u8 badge_count, u8 region_id);
u8 GetRivalAceLevel(u8 rival_encounter_id);

// Level cap management
u8 GetCurrentLevelCap(void);
void UpdateLevelCap(u8 badge_count, u8 region_id);

#endif // GUARD_LEVEL_SCALER_H
