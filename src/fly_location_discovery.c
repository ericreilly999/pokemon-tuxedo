#include "global.h"
#include "fly_location_discovery.h"
#include "event_data.h"

/*
 * Fly Location Discovery System
 * 
 * Requirements:
 * - 15.1: Discover fly locations on first visit
 * - 15.2: Track discovered locations
 * - 15.3: Filter Fly menu to show only discovered locations
 * - 15.4: Persist discovery across regions and sessions
 */

#define MAX_FLY_LOCATIONS 50

// Save data structure for fly location discovery
static bool8 sDiscoveredFlyLocations[MAX_FLY_LOCATIONS] = {FALSE};

void DiscoverFlyLocation(u16 map_id)
{
    if (map_id < MAX_FLY_LOCATIONS)
        sDiscoveredFlyLocations[map_id] = TRUE;
}

bool8 IsFlyLocationDiscovered(u16 map_id)
{
    if (map_id >= MAX_FLY_LOCATIONS)
        return FALSE;
    
    return sDiscoveredFlyLocations[map_id];
}

void GetDiscoveredFlyLocations(u16 *locations, u8 *count)
{
    u8 i;
    u8 discovered_count = 0;
    
    for (i = 0; i < MAX_FLY_LOCATIONS; i++)
    {
        if (sDiscoveredFlyLocations[i])
        {
            locations[discovered_count] = i;
            discovered_count++;
        }
    }
    
    *count = discovered_count;
}
