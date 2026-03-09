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

// Pokemon Tuxedo save data structure
struct PokemonTuxedoSaveData {
    u8 current_region;                    // 0-3 for Kanto/Johto/Hoenn/Sinnoh
    u8 badge_count;                       // Total badges across all regions (0-32)
    bool8 elite_four_defeated[4];         // One flag per region
    u8 current_level_cap;                 // Maximum level Pokemon can reach
    u8 active_exp_multiplier;             // 1=none, 2=Lucky, 3=Mystic, 4=Magic
    bool8 fly_locations_discovered[256];  // Bitfield for discovered fly locations
    u8 region_badges[4];                  // Badges per region (0-8 each)
    u8 battle_mode;                       // 0=shift, 1=set
    u8 game_speed;                        // 1-10 (speed multiplier)
};

// Initialization function
void InitPokemonTuxedoData(void);

#endif // GUARD_POKEMON_TUXEDO_H
