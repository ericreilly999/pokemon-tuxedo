#include "global.h"
#include "region_manager.h"
#include "event_data.h"
#include "item.h"
#include "constants/flags.h"

// Region starting levels (from requirements)
static const u8 sRegionStartingLevels[] = {
    2,    // REGION_KANTO
    60,   // REGION_JOHTO
    110,  // REGION_HOENN
    160   // REGION_SINNOH
};

// Save data structure for region tracking
// These would be integrated into the actual save structure
EWRAM_DATA static u8 sCurrentRegion = REGION_KANTO;
EWRAM_DATA static bool8 sEliteFourDefeated[4] = {FALSE, FALSE, FALSE, FALSE};

u8 GetCurrentRegion(void)
{
    return sCurrentRegion;
}

u8 GetBadgeCount(void)
{
    u8 count = 0;
    
    // Count Kanto badges
    if (FlagGet(FLAG_BADGE01_GET)) count++;
    if (FlagGet(FLAG_BADGE02_GET)) count++;
    if (FlagGet(FLAG_BADGE03_GET)) count++;
    if (FlagGet(FLAG_BADGE04_GET)) count++;
    if (FlagGet(FLAG_BADGE05_GET)) count++;
    if (FlagGet(FLAG_BADGE06_GET)) count++;
    if (FlagGet(FLAG_BADGE07_GET)) count++;
    if (FlagGet(FLAG_BADGE08_GET)) count++;
    
    return count;
}

void SetCurrentRegion(u8 region_id)
{
    if (region_id <= REGION_SINNOH)
        sCurrentRegion = region_id;
}

bool8 IsEliteFourDefeated(u8 region_id)
{
    if (region_id > REGION_SINNOH)
        return FALSE;
    
    return sEliteFourDefeated[region_id];
}

void SetEliteFourDefeated(u8 region_id)
{
    if (region_id <= REGION_SINNOH)
        sEliteFourDefeated[region_id] = TRUE;
}

u8 GetRegionStartingLevel(u8 region_id)
{
    if (region_id > REGION_SINNOH)
        return 2;
    
    return sRegionStartingLevels[region_id];
}

u16 GetRegionTicketItem(u8 region_id)
{
    switch (region_id)
    {
    case REGION_JOHTO:
        return ITEM_JOHTO_TICKET;
    case REGION_HOENN:
        return ITEM_HOENN_TICKET;
    case REGION_SINNOH:
        return ITEM_SINNOH_TICKET;
    default:
        return ITEM_NONE;
    }
}

bool8 CanTravelToRegion(u8 target_region)
{
    u16 ticket_item;
    
    // Can always stay in current region
    if (target_region == sCurrentRegion)
        return TRUE;
    
    // Kanto is always accessible (starting region)
    if (target_region == REGION_KANTO)
        return TRUE;
    
    // Other regions require Elite Four defeat in current region
    if (!IsEliteFourDefeated(sCurrentRegion))
        return FALSE;
    
    // And possession of the target region's ticket
    ticket_item = GetRegionTicketItem(target_region);
    if (ticket_item == ITEM_NONE)
        return FALSE;
    
    return CheckBagHasItem(ticket_item, 1);
}

void AwardRegionTicket(void)
{
    u16 ticket_item;
    
    // Only award ticket if Elite Four defeated in current region
    if (!IsEliteFourDefeated(sCurrentRegion))
        return;
    
    // Determine next region ticket based on current region
    ticket_item = ITEM_NONE;
    
    switch (sCurrentRegion)
    {
    case REGION_KANTO:
        ticket_item = ITEM_JOHTO_TICKET;
        break;
    case REGION_JOHTO:
        ticket_item = ITEM_HOENN_TICKET;
        break;
    case REGION_HOENN:
        ticket_item = ITEM_SINNOH_TICKET;
        break;
    case REGION_SINNOH:
        // No next region after Sinnoh
        return;
    }
    
    // Award the ticket
    if (ticket_item != ITEM_NONE)
        AddBagItem(ticket_item, 1);
}
