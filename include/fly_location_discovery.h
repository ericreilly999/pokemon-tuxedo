#ifndef GUARD_FLY_LOCATION_DISCOVERY_H
#define GUARD_FLY_LOCATION_DISCOVERY_H

#include "global.h"
#include "region_manager.h"

// Fly locations per region (16 per region × 3 regions = 48 total)
#define FLY_LOCATIONS_PER_REGION  16
#define MAX_FLY_LOCATIONS_TOTAL   (FLY_LOCATIONS_PER_REGION * NUM_REGIONS)  // 48

// Legacy fly location discovery functions (backward compatible)
void DiscoverFlyLocation(u16 map_id);
bool8 IsFlyLocationDiscovered(u16 map_id);
void GetDiscoveredFlyLocations(u16 *locations, u8 *count);

// Region-aware fly location functions (Requirements 12.1, 12.2, 12.4, 12.5)
void RegisterFlyLocation(u16 map_id);
u16* GetCurrentRegionFlyLocations(u8 *out_count);
bool8 IsFlyLocationRegistered(u16 map_id);
const u8* GetFlyLocationName(u16 map_id);

// Region-specific fly location functions
void RegisterFlyLocationForRegion(u16 map_id, u8 region_id);
bool8 IsFlyLocationRegisteredForRegion(u16 map_id, u8 region_id);
u8 GetFlyLocationCountForRegion(u8 region_id);
void GetFlyLocationsForRegion(u8 region_id, u16 *locations, u8 *count);

// Cross-region fly support (starting locations of unlocked regions)
void GetCrossRegionFlyLocations(u16 *locations, u8 *count);
u16 GetRegionStartingFlyLocation(u8 region_id);

// Save data access
void InitFlyLocationData(void);
void ClearFlyLocationData(void);

#endif // GUARD_FLY_LOCATION_DISCOVERY_H
