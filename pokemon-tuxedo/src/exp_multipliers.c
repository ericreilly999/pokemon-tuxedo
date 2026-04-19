#include "global.h"
#include "exp_multipliers.h"
#include "item.h"
#include "constants/items.h"
#include "constants/hold_effects.h"

/* Custom Tuxedo EXP multiplier items - not yet added to pokeemerald item tables */
/* These are defined here as stubs until pokeemerald's items.h includes them */
#ifndef ITEM_MYSTIC_EGG
#define ITEM_MYSTIC_EGG ITEM_NONE
#endif
#ifndef ITEM_MAGIC_EGG
#define ITEM_MAGIC_EGG ITEM_NONE
#endif
#ifndef HOLD_EFFECT_MYSTIC_EGG
#define HOLD_EFFECT_MYSTIC_EGG HOLD_EFFECT_LUCKY_EGG
#endif
#ifndef HOLD_EFFECT_MAGIC_EGG
#define HOLD_EFFECT_MAGIC_EGG HOLD_EFFECT_LUCKY_EGG
#endif

/* pokeemerald uses GetItemHoldEffect; pokefirered used ItemId_GetHoldEffect */
#define ItemId_GetHoldEffect(item) GetItemHoldEffect(item)

/*
 * EXP Multiplier System
 * 
 * Requirements:
 * - 11.4: Multiplier applies to all experience gains
 * - 11.5: Multiplier stacks with full party experience
 * - 11.6: Only highest multiplier applies
 * - 11.7: Priority: Magic Egg (4x) > Mystic Egg (3x) > Lucky Egg (2x)
 */

u8 GetActiveExpMultiplier(void)
{
    // Check in priority order (highest to lowest)
    if (CheckBagHasItem(ITEM_MAGIC_EGG, 1))
        return 4;
    else if (CheckBagHasItem(ITEM_MYSTIC_EGG, 1))
        return 3;
    else if (CheckBagHasItem(ITEM_LUCKY_EGG, 1))
        return 2;
    else
        return 1;  // No multiplier
}

u32 ApplyExpMultiplier(u32 base_exp)
{
    u8 multiplier = GetActiveExpMultiplier();
    return base_exp * multiplier;
}

void AwardMultiplierItemAfterEliteFour(u8 region_id)
{
    /*
     * Award multiplier items after Elite Four defeats:
     * - Kanto E4: Lucky Egg (2x)
     * - Hoenn E4: Mystic Egg (3x)
     * - Johto E4: Magic Egg (4x)
     * 
     * Note: Region order is Kanto → Hoenn → Johto per ADR-004
     * Sinnoh is descoped per ADR-003
     */
    
    switch (region_id)
    {
    case REGION_KANTO:
        AddBagItem(ITEM_LUCKY_EGG, 1);
        break;
    case REGION_HOENN:
        AddBagItem(ITEM_MYSTIC_EGG, 1);
        break;
    case REGION_JOHTO:
        AddBagItem(ITEM_MAGIC_EGG, 1);
        break;
    }
}

u32 SafeApplyExpMultiplier(u32 base_exp, u16 held_item)
{
    /*
     * Safe wrapper for applying exp multipliers based on held item
     * Used in battle_script_commands.c
     * 
     * Checks hold effect of the item and applies appropriate multiplier:
     * - HOLD_EFFECT_MYSTIC_EGG: 3x
     * - HOLD_EFFECT_MAGIC_EGG: 4x
     * - Default: 1x (no change)
     */
    
    u8 hold_effect = ItemId_GetHoldEffect(held_item);
    
    if (hold_effect == HOLD_EFFECT_MYSTIC_EGG)
        return (base_exp * 300) / 100;  // 3x multiplier
    else if (hold_effect == HOLD_EFFECT_MAGIC_EGG)
        return (base_exp * 400) / 100;  // 4x multiplier
    else
        return base_exp;  // No multiplier
}
