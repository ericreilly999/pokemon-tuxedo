#ifndef GUARD_POKEMON_TUXEDO_H
#define GUARD_POKEMON_TUXEDO_H

#include "global.h"

// Region enumeration is defined in region_manager.h
// REGION_KANTO = 0, REGION_HOENN = 1, REGION_JOHTO = 2
// Note: Sinnoh is descoped per ADR-003
// Sequence: Kanto → Hoenn → Johto per ADR-004

// Pokemon Tuxedo save data structure is defined in global.h

// Initialization function
void InitPokemonTuxedoData(void);

#endif // GUARD_POKEMON_TUXEDO_H
