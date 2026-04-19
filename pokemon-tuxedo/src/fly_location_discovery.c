#include "global.h"
#include "fly_location_discovery.h"
#include "region_manager.h"
#include "event_data.h"

/*
 * Fly Location Discovery System - Multi-Region Support
 *
 * Requirements:
 * - 12.1: Register fly locations when visiting cities with fly points
 * - 12.2: Track fly locations per region (Kanto, Hoenn, Johto)
 * - 12.4: Fly menu shows only current region locations
 * - 12.5: Cross-region flying only to starting locations of unlocked regions
 *
 * Data Layout:
 * - flyLocations[48]: 16 locations per region x 3 regions
 *   - Index 0-15:  Kanto fly locations
 *   - Index 16-31: Hoenn fly locations
 *   - Index 32-47: Johto fly locations
 * - flyLocationCount[3]: Count of registered locations per region
 */

/* Fly location storage (16 per region x 3 regions = 48 total).
   Stored in EWRAM for save data integration. */
EWRAM_DATA static u16 sFlyLocations[MAX_FLY_LOCATIONS_TOTAL] = {0};
EWRAM_DATA static u8 sFlyLocationCount[NUM_REGIONS] = {0};

/* Static buffer for returning fly locations */
static u16 sCurrentRegionFlyLocationsBuffer[FLY_LOCATIONS_PER_REGION];

/* Starting fly locations for each region (first city/town) */
static const u16 sRegionStartingLocations[NUM_REGIONS] = {
    0,   /* REGION_KANTO: Pallet Town (map_id 0, placeholder) */
    16,  /* REGION_HOENN: Littleroot Town (map_id 16, placeholder) */
    32,  /* REGION_JOHTO: New Bark Town (map_id 32, placeholder) */
};

/**
 * Get the base index for a region's fly locations in the array.
 */
static u8 GetRegionBaseIndex(u8 region_id)
{
    if (region_id >= NUM_REGIONS)
        return 0;
    return region_id * FLY_LOCATIONS_PER_REGION;
}

/**
 * Determine which region a map_id belongs to.
 * This uses map_id ranges to determine region.
 * Actual implementation should use map header data.
 */
static u8 GetRegionForMapId(u16 map_id)
{
    /* Placeholder implementation - should be replaced with actual map header lookup.
       For now, use simple ranges:
         0-255: Kanto maps
         256-511: Hoenn maps
         512-767: Johto maps */
    if (map_id < 256)
        return REGION_KANTO;
    else if (map_id < 512)
        return REGION_HOENN;
    else
        return REGION_JOHTO;
}

/**
 * Initialize fly location data for a new game.
 */
void InitFlyLocationData(void)
{
    u8 i;

    for (i = 0; i < MAX_FLY_LOCATIONS_TOTAL; i++)
        sFlyLocations[i] = 0;

    for (i = 0; i < NUM_REGIONS; i++)
        sFlyLocationCount[i] = 0;
}

/**
 * Clear all fly location data.
 */
void ClearFlyLocationData(void)
{
    InitFlyLocationData();
}

/**
 * Register a fly location for the current region.
 * Validates: Requirements 12.1, 12.2
 *
 * @param map_id The map ID to register as a fly location
 */
void RegisterFlyLocation(u16 map_id)
{
    u8 current_region;

    current_region = GetCurrentRegion();
    RegisterFlyLocationForRegion(map_id, current_region);
}

/**
 * Register a fly location for a specific region.
 *
 * @param map_id The map ID to register
 * @param region_id The region to register the location for
 */
void RegisterFlyLocationForRegion(u16 map_id, u8 region_id)
{
    u8 base_index;
    u8 count;

    /* Validate region */
    if (region_id >= NUM_REGIONS)
        return;

    /* Check if already at max capacity for this region */
    count = sFlyLocationCount[region_id];
    if (count >= FLY_LOCATIONS_PER_REGION)
        return;

    /* Check if already registered */
    if (IsFlyLocationRegisteredForRegion(map_id, region_id))
        return;

    /* Register the location */
    base_index = GetRegionBaseIndex(region_id);
    sFlyLocations[base_index + count] = map_id;
    sFlyLocationCount[region_id] = count + 1;
}

/**
 * Get fly locations for the current region.
 * Validates: Requirements 12.4
 *
 * @param out_count Pointer to store the count of locations
 * @return Pointer to array of fly location map IDs
 */
u16* GetCurrentRegionFlyLocations(u8 *out_count)
{
    u8 current_region;
    u8 base_index;
    u8 count;
    u8 i;

    current_region = GetCurrentRegion();
    base_index = GetRegionBaseIndex(current_region);
    count = sFlyLocationCount[current_region];

    /* Copy to static buffer */
    for (i = 0; i < count && i < FLY_LOCATIONS_PER_REGION; i++)
        sCurrentRegionFlyLocationsBuffer[i] = sFlyLocations[base_index + i];

    if (out_count != NULL)
        *out_count = count;

    return sCurrentRegionFlyLocationsBuffer;
}

/**
 * Check if a fly location is registered (in any region).
 * Validates: Requirements 12.2
 *
 * @param map_id The map ID to check
 * @return TRUE if registered, FALSE otherwise
 */
bool8 IsFlyLocationRegistered(u16 map_id)
{
    u8 region;

    for (region = 0; region < NUM_REGIONS; region++)
    {
        if (IsFlyLocationRegisteredForRegion(map_id, region))
            return TRUE;
    }

    return FALSE;
}

/**
 * Check if a fly location is registered for a specific region.
 *
 * @param map_id The map ID to check
 * @param region_id The region to check
 * @return TRUE if registered, FALSE otherwise
 */
bool8 IsFlyLocationRegisteredForRegion(u16 map_id, u8 region_id)
{
    u8 base_index;
    u8 count;
    u8 i;

    if (region_id >= NUM_REGIONS)
        return FALSE;

    base_index = GetRegionBaseIndex(region_id);
    count = sFlyLocationCount[region_id];

    for (i = 0; i < count; i++)
    {
        if (sFlyLocations[base_index + i] == map_id)
            return TRUE;
    }

    return FALSE;
}

/**
 * Get the count of registered fly locations for a region.
 *
 * @param region_id The region to get count for
 * @return Number of registered fly locations
 */
u8 GetFlyLocationCountForRegion(u8 region_id)
{
    if (region_id >= NUM_REGIONS)
        return 0;

    return sFlyLocationCount[region_id];
}

/**
 * Get all fly locations for a specific region.
 *
 * @param region_id The region to get locations for
 * @param locations Array to store the map IDs
 * @param count Pointer to store the count
 */
void GetFlyLocationsForRegion(u8 region_id, u16 *locations, u8 *count)
{
    u8 base_index;
    u8 loc_count;
    u8 i;

    if (region_id >= NUM_REGIONS || locations == NULL || count == NULL)
    {
        if (count != NULL)
            *count = 0;
        return;
    }

    base_index = GetRegionBaseIndex(region_id);
    loc_count = sFlyLocationCount[region_id];

    for (i = 0; i < loc_count; i++)
        locations[i] = sFlyLocations[base_index + i];

    *count = loc_count;
}

/**
 * Get cross-region fly locations (starting locations of unlocked regions).
 * Validates: Requirements 12.5
 *
 * @param locations Array to store the map IDs
 * @param count Pointer to store the count
 */
void GetCrossRegionFlyLocations(u16 *locations, u8 *count)
{
    u8 region;
    u8 loc_count;
    u8 current_region;

    loc_count = 0;
    current_region = GetCurrentRegion();

    if (locations == NULL || count == NULL)
    {
        if (count != NULL)
            *count = 0;
        return;
    }

    for (region = 0; region < NUM_REGIONS; region++)
    {
        /* Skip current region (those are in GetCurrentRegionFlyLocations) */
        if (region == current_region)
            continue;

        /* Only include unlocked regions */
        if (IsRegionUnlocked(region))
        {
            locations[loc_count] = GetRegionStartingFlyLocation(region);
            loc_count++;
        }
    }

    *count = loc_count;
}

/**
 * Get the starting fly location for a region.
 *
 * @param region_id The region to get starting location for
 * @return The map ID of the starting location
 */
u16 GetRegionStartingFlyLocation(u8 region_id)
{
    if (region_id >= NUM_REGIONS)
        return 0;

    return sRegionStartingLocations[region_id];
}

/**
 * Get the name of a fly location.
 *
 * @param map_id The map ID to get the name for
 * @return Pointer to the location name string
 */
const u8* GetFlyLocationName(u16 map_id)
{
    /* Placeholder - should return actual map name from map header.
       For now, return a generic string. */
    static const u8 sUnknownLocation[] = _("Unknown");
    return sUnknownLocation;
}

/* ============================================================================
   Legacy functions for backward compatibility
   ============================================================================ */

/**
 * Legacy function: Discover a fly location (maps to RegisterFlyLocation).
 */
void DiscoverFlyLocation(u16 map_id)
{
    RegisterFlyLocation(map_id);
}

/**
 * Legacy function: Check if fly location is discovered.
 */
bool8 IsFlyLocationDiscovered(u16 map_id)
{
    return IsFlyLocationRegistered(map_id);
}

/**
 * Return a pointer to the raw fly location array (used by multi_region_save.c).
 */
u16* GetFlyLocationArray(void)
{
    return sFlyLocations;
}

/**
 * Return a pointer to the fly location count array (used by multi_region_save.c).
 */
u8* GetFlyLocationCountArray(void)
{
    return sFlyLocationCount;
}

/**
 * Legacy function: Get all discovered fly locations.
 */
void GetDiscoveredFlyLocations(u16 *locations, u8 *count)
{
    u8 region;
    u8 total_count;
    u8 region_count;
    u8 base_index;
    u8 i;

    total_count = 0;

    if (locations == NULL || count == NULL)
    {
        if (count != NULL)
            *count = 0;
        return;
    }

    /* Collect all fly locations from all regions */
    for (region = 0; region < NUM_REGIONS; region++)
    {
        base_index = GetRegionBaseIndex(region);
        region_count = sFlyLocationCount[region];

        for (i = 0; i < region_count && total_count < MAX_FLY_LOCATIONS_TOTAL; i++)
        {
            locations[total_count] = sFlyLocations[base_index + i];
            total_count++;
        }
    }

    *count = total_count;
}
