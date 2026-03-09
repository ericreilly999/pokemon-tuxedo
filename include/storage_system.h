#ifndef GUARD_STORAGE_SYSTEM_H
#define GUARD_STORAGE_SYSTEM_H

#include "global.h"

// Storage system accessibility
bool8 CanAccessStorageSystem(void);

// Pause menu PC access
void OpenStorageSystemFromMenu(void);
void AddStorageSystemToPauseMenu(void);

#endif // GUARD_STORAGE_SYSTEM_H
