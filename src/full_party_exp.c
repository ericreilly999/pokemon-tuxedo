#include "global.h"
#include "pokemon.h"
#include "battle.h"

/*
 * Full Party Experience System
 * 
 * Requirements:
 * - 10.1: All Pokemon in party receive full experience
 * - 10.2: Experience is not split among party members
 * - 10.3: Non-participating Pokemon receive same exp as participants
 */

void AwardExperienceToParty(u32 base_exp)
{
    u8 i;
    struct Pokemon *pokemon;
    
    // Award full experience to all party Pokemon
    for (i = 0; i < PARTY_SIZE; i++)
    {
        pokemon = &gPlayerParty[i];
        
        // Skip empty slots
        if (GetMonData(pokemon, MON_DATA_SPECIES, NULL) == SPECIES_NONE)
            continue;
        
        // Skip eggs
        if (GetMonData(pokemon, MON_DATA_IS_EGG, NULL))
            continue;
        
        // Award full experience (not split)
        // This would integrate with the existing experience gain system
        // For now, this is a stub showing the concept
        GiveExperienceToMon(pokemon, base_exp);
    }
}

void GiveExperienceToMon(struct Pokemon *pokemon, u32 exp)
{
    u32 current_exp;
    u32 new_exp;
    
    // Get current experience
    current_exp = GetMonData(pokemon, MON_DATA_EXP, NULL);
    
    // Add new experience
    new_exp = current_exp + exp;
    
    // Cap at maximum experience (depends on growth rate)
    // This would need to check against the Pokemon's growth rate table
    // For now, simplified implementation
    
    // Set new experience
    SetMonData(pokemon, MON_DATA_EXP, &new_exp);
    
    // Calculate new level based on experience
    // This would trigger level-up animations and stat recalculation
    CalculateMonStats(pokemon);
}
