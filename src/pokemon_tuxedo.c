#include "global.h"
#include "pokemon_tuxedo.h"
#include "load_save.h"

// Initialize Pokemon Tuxedo save data to default values
void InitPokemonTuxedoData(void)
{
    struct PokemonTuxedoSaveData *data = &gSaveBlock1Ptr->pokemonTuxedo;
    u16 i;
    
    // Initialize region and badge tracking
    data->current_region = REGION_KANTO;
    data->badge_count = 0;
    
    // Initialize Elite Four defeat flags
    for (i = 0; i < 4; i++)
    {
        data->elite_four_defeated[i] = FALSE;
    }
    
    // Initialize level cap (starting level cap)
    data->current_level_cap = 100;
    
    // Initialize EXP multiplier (1 = no multiplier)
    data->active_exp_multiplier = 1;
    
    // Initialize fly locations (all undiscovered)
    for (i = 0; i < 256; i++)
    {
        data->fly_locations_discovered[i] = FALSE;
    }
    
    // Initialize region badges (0 badges per region)
    for (i = 0; i < 4; i++)
    {
        data->region_badges[i] = 0;
    }
    
    // Initialize game settings
    data->battle_mode = 1; // Default to SET mode (1)
    data->game_speed = 1;  // Default to 1x speed
}
