#ifndef GUARD_EXP_MULTIPLIERS_H
#define GUARD_EXP_MULTIPLIERS_H

#include "global.h"
#include "constants/items.h"
#include "region_manager.h"

// Multiplier item IDs are now defined in constants/items.h
// ITEM_LUCKY_EGG, ITEM_MYSTIC_EGG, ITEM_MAGIC_EGG

// Region IDs are now defined in region_manager.h
// REGION_KANTO, REGION_HOENN, REGION_JOHTO
// Note: REGION_SINNOH is descoped per ADR-003

/* EXP multiplier functions.
   holdEffect: the hold effect of the gaining Pokemon's held item.  Pass
   HOLD_EFFECT_NONE when not called from a per-Pokemon context. */
u8 GetActiveExpMultiplier(u8 holdEffect);
u32 ApplyExpMultiplier(u32 base_exp);
u32 SafeApplyExpMultiplier(u32 base_exp, u16 held_item);

// Multiplier item rewards
void AwardMultiplierItemAfterEliteFour(u8 region_id);

#endif // GUARD_EXP_MULTIPLIERS_H
