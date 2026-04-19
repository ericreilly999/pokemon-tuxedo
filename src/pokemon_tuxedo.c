#include "global.h"
#include "pokemon_tuxedo.h"
#include "load_save.h"
#include "region_manager.h"

/* Initialize Pokemon Tuxedo save data to default values.
   Updated for multi-region support (Task 3.13).
   Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5 */
void InitPokemonTuxedoData(void)
{
    struct PokemonTuxedoSaveData *data = &gSaveBlock1Ptr->pokemonTuxedo;
    u16 i;

    /* Initialize legacy fields */
    data->current_level_cap = 100;       /* Starting level cap */
    data->active_exp_multiplier = 1;     /* 1 = no multiplier */
    data->battle_mode = 1;               /* Default to SET mode (1) */
    data->game_speed = 1;                /* Default to 1x speed */

    /* Initialize multi-region fields (Requirements 16.1-16.5) */

    /* Region tracking (Requirement 16.1) */
    data->currentRegion = REGION_KANTO;  /* Start in Kanto */

    /* Region unlock flags (Requirement 16.2).
       Kanto is always unlocked (starting region).
       Sinnoh descoped per ADR-003. */
    data->hoennUnlocked = FALSE;
    data->johtoUnlocked = FALSE;

    /* Elite Four defeat tracking (Requirement 16.2).
       3 regions: Kanto, Hoenn, Johto (ADR-003, ADR-004). */
    for (i = 0; i < 3; i++)
    {
        data->eliteFourDefeated[i] = FALSE;
    }

    /* Badge tracking (Requirement 16.3).
       24 badges total: 8 per region x 3 regions. */
    for (i = 0; i < 24; i++)
    {
        data->badges[i] = FALSE;
    }

    /* Fly location tracking (Requirement 16.4).
       48 fly locations: 16 per region x 3 regions. */
    for (i = 0; i < 48; i++)
    {
        data->flyLocations[i] = 0; /* 0 = no location registered */
    }

    /* Fly location counts per region (Requirement 16.4) */
    for (i = 0; i < 3; i++)
    {
        data->flyLocationCount[i] = 0;
    }
}
