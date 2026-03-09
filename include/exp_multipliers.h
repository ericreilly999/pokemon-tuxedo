#ifndef GUARD_EXP_MULTIPLIERS_H
#define GUARD_EXP_MULTIPLIERS_H

#include "global.h"
#include "constants/items.h"

// Multiplier item IDs are now defined in constants/items.h
// ITEM_LUCKY_EGG, ITEM_MYSTIC_EGG, ITEM_MAGIC_EGG

// Region IDs (from region_manager.h)
#define REGION_KANTO 0
#define REGION_JOHTO 1
#define REGION_HOENN 2
#define REGION_SINNOH 3

// EXP multiplier functions
u8 GetActiveExpMultiplier(void);
u32 ApplyExpMultiplier(u32 base_exp);
u32 SafeApplyExpMultiplier(u32 base_exp, u16 held_item);

// Multiplier item rewards
void AwardMultiplierItemAfterEliteFour(u8 region_id);

#endif // GUARD_EXP_MULTIPLIERS_H
