#ifndef GUARD_FULL_PARTY_EXP_H
#define GUARD_FULL_PARTY_EXP_H

#include "global.h"
#include "pokemon.h"

// Full party experience functions
void AwardExperienceToParty(u32 base_exp);
void GiveExperienceToMon(struct Pokemon *pokemon, u32 exp);

#endif // GUARD_FULL_PARTY_EXP_H
