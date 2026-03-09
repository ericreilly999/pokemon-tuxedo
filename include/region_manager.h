#ifndef GUARD_REGION_MANAGER_H
#define GUARD_REGION_MANAGER_H

#include "global.h"
#include "constants/items.h"

// Region IDs
#define REGION_KANTO 0
#define REGION_JOHTO 1
#define REGION_HOENN 2
#define REGION_SINNOH 3

// Region ticket item IDs are now defined in constants/items.h
// ITEM_JOHTO_TICKET, ITEM_HOENN_TICKET, ITEM_SINNOH_TICKET

// Region tracking functions
u8 GetCurrentRegion(void);
void SetCurrentRegion(u8 region_id);

// Badge tracking
u8 GetBadgeCount(void);

// Elite Four tracking
bool8 IsEliteFourDefeated(u8 region_id);
void SetEliteFourDefeated(u8 region_id);

// Region starting level lookup
u8 GetRegionStartingLevel(u8 region_id);

// Region travel validation
bool8 CanTravelToRegion(u8 target_region);
u16 GetRegionTicketItem(u8 region_id);

// Region ticket rewards
void AwardRegionTicket(void);

#endif // GUARD_REGION_MANAGER_H
