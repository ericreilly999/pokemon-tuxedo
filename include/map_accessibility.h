#ifndef GUARD_MAP_ACCESSIBILITY_H
#define GUARD_MAP_ACCESSIBILITY_H

#include "global.h"

// Map accessibility functions
void UnlockRegionMaps(u8 region_id);
bool8 IsMapAccessible(u16 map_id);
bool8 CanChallengeGymsInAnyOrder(u8 region_id);

#endif // GUARD_MAP_ACCESSIBILITY_H
