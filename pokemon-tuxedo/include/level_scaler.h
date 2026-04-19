#ifndef GUARD_LEVEL_SCALER_H
#define GUARD_LEVEL_SCALER_H

#include "global.h"

// Level range structure for wild Pokemon
struct LevelRange {
    u8 min_level;
    u8 max_level;
};

// Trainer type constants for GetTrainerAverageLevelForRegion
#define TRAINER_TYPE_NORMAL     0
#define TRAINER_TYPE_GYM_LEADER 1
#define TRAINER_TYPE_ELITE_FOUR 2
#define TRAINER_TYPE_CHAMPION   3
#define TRAINER_TYPE_RIVAL      4

// Wild Pokemon level calculation (legacy - uses current region badge count)
struct LevelRange GetWildPokemonLevelRange(u8 badge_count, u8 region_id, bool8 elite_four_defeated);

// Cross-region wild Pokemon level calculation
// Uses total badge count and considers progression through Kanto → Hoenn → Johto
struct LevelRange GetWildPokemonLevelRangeForRegion(u8 badge_count, u8 region_id, bool8 elite_four_defeated);

// Region starting level lookup
u8 GetRegionStartingLevel(u8 region_id);

// Trainer Pokemon level calculation (legacy)
u8 GetGymLeaderAverageLevel(u8 badge_count, u8 region_id);
u8 GetGymLeaderAceLevel(u8 gym_leader_id);
u8 GetEliteFourAverageLevel(u8 badge_count, u8 region_id);
u8 GetEliteFourAceLevel(u8 elite_four_member_id);
u8 GetRivalAverageLevel(u8 badge_count, u8 region_id);
u8 GetRivalAceLevel(u8 rival_encounter_id);

// Cross-region trainer level calculation
// Calculates trainer average level based on badge count, region, and trainer type
// trainer_type: TRAINER_TYPE_NORMAL, TRAINER_TYPE_GYM_LEADER, TRAINER_TYPE_ELITE_FOUR, 
//               TRAINER_TYPE_CHAMPION, TRAINER_TYPE_RIVAL
u8 GetTrainerAverageLevelForRegion(u8 badge_count, u8 region_id, u8 trainer_type);

// Level cap management
/* renamed from GetCurrentLevelCap to avoid collision with pokeemerald caps.c */
u8 GetTuxedoLevelCap(void);
void UpdateLevelCap(u8 badge_count, u8 region_id);

// Cross-region level cap calculation
// Returns the level cap based on total badges across all regions and current region
// Level cap increases with progression: Kanto → Hoenn → Johto
u8 GetLevelCapForProgression(u8 total_badges, u8 current_region);

#endif // GUARD_LEVEL_SCALER_H
