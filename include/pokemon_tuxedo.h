#ifndef GUARD_POKEMON_TUXEDO_H
#define GUARD_POKEMON_TUXEDO_H

#include "global.h"

// Region enumeration for Pokemon Tuxedo
enum Region {
    REGION_KANTO = 0,
    REGION_JOHTO = 1,
    REGION_HOENN = 2,
    REGION_SINNOH = 3
};

// Pokemon Tuxedo save data structure is defined in global.h

// Initialization function
void InitPokemonTuxedoData(void);

#endif // GUARD_POKEMON_TUXEDO_H
