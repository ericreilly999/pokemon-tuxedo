#include "global.h"
#include "storage_system.h"
#include "battle.h"
#include "event_data.h"
#include "menu.h"
#include "script.h"

/*
 * Storage System - Pause Menu Access
 * 
 * Requirements:
 * - 12.2: PC Storage accessible from pause menu
 * - 12.3: OpenStorageSystemFromMenu() function
 * - 12.4: CanAccessStorageSystem() validation
 * - 12.5: Block access during battles and cutscenes
 */

bool8 CanAccessStorageSystem(void)
{
    // Block access during battles
    if (gBattleTypeFlags != 0)
        return FALSE;
    
    // Block access during cutscenes
    // This would check various cutscene flags
    if (ScriptContext_IsEnabled())
        return FALSE;
    
    return TRUE;
}

void OpenStorageSystemFromMenu(void)
{
    // Validate access
    if (!CanAccessStorageSystem())
        return;
    
    // Open the storage system
    // This would call the existing PC storage system functions
    // For now, this is a stub showing the integration point
    // In actual implementation, this would call:
    // SetMainCallback2(CB2_InitPokeStorage);
}

void AddStorageSystemToPauseMenu(void)
{
    /*
     * Add "PC Storage" option to pause menu
     * 
     * This would integrate with the existing pause menu system
     * to add a new menu option that calls OpenStorageSystemFromMenu()
     */
}
