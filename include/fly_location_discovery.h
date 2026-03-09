#ifndef GUARD_FLY_LOCATION_DISCOVERY_H
#define GUARD_FLY_LOCATION_DISCOVERY_H

#include "global.h"

// Fly location discovery functions
void DiscoverFlyLocation(u16 map_id);
bool8 IsFlyLocationDiscovered(u16 map_id);
void GetDiscoveredFlyLocations(u16 *locations, u8 *count);

#endif // GUARD_FLY_LOCATION_DISCOVERY_H
