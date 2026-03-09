#ifndef GUARD_SYSTEM_INTEGRATION_H
#define GUARD_SYSTEM_INTEGRATION_H

#include "global.h"

// Save data management
void SavePokemonTuxedoData(void);
bool8 LoadPokemonTuxedoData(void);

// Level scaling integration
void UpdateWildPokemonLevel(struct Pokemon *pokemon);
void UpdateTrainerPokemonLevels(struct Trainer *trainer);

// Experience system integration
void AwardBattleExperience(u32 base_exp);

// Event hooks
void OnBadgeAcquired(void);
void OnRegionChanged(void);
void OnEliteFourDefeated(void);
void OnMapEntered(u16 map_id);

// Initialization
void InitializePokemonTuxedoSystems(void);

// Testing
bool8 TestSystemIntegration(void);

#endif // GUARD_SYSTEM_INTEGRATION_H
