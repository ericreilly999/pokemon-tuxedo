# Pokemon Tuxedo Property-Based Tests

This directory contains property-based tests for the Pokemon Tuxedo ROM hack, following a test-driven development (TDD) approach.

## Test Framework

The tests use a custom property-based testing approach inspired by QuickCheck/theft, generating random test cases to verify universal properties of the level scaling system.

## Building and Running Tests

### Prerequisites

- A C compiler (gcc, clang, or MSVC)
- Standard C library

### On Linux/Mac:

```bash
cd firered-clone/test
make
make test
```

### On Windows with MinGW:

```bash
cd firered-clone/test
gcc -Wall -Wextra -std=c99 -I../include -o test_level_scaler.exe test_level_scaler.c ../src/level_scaler.c
./test_level_scaler.exe
```

### On Windows with MSVC:

```bash
cd firered-clone/test
cl /I..\include test_level_scaler.c ..\src\level_scaler.c
test_level_scaler.exe
```

### Using WSL (Windows Subsystem for Linux):

```bash
wsl
cd /mnt/c/dev/pokemon-tuxedo/firered-clone/test
make
make test
```

## Test Structure

### Task 2.1: Wild Pokemon Level Range Calculation

**File:** `test_level_scaler.c`

**Property 1:** For any badge count and region, the wild Pokemon level range should be calculated as:
- minimum = (badge_count × 5) + region_starting_level
- maximum = minimum + 10
- Special case: when badge_count is 0, levels should be 2-10

**Validates:** Requirements 2.1, 2.2, 2.3

The test generates 1000+ random test cases covering:
- All badge counts (0-32)
- All regions (0-3: Kanto, Johto, Hoenn, Sinnoh)
- Elite Four defeated/not defeated states
- Edge cases (badge_count=0, maximum badges, etc.)

### Task 2.2: Region Starting Level Application

**File:** `test_level_scaler.c`

**Property 3:** For any region, wild Pokemon levels should include the region's starting level as a base offset in all level calculations.

**Validates:** Requirements 2.6, 6.5

The test verifies that:
1. For badge_count > 0, the minimum level includes the region starting level
2. The region starting level is correctly applied for all four regions (Kanto: 2, Johto: 60, Hoenn: 110, Sinnoh: 160)
3. The starting level offset is maintained even with Elite Four bonuses
4. Different regions with the same badge count have appropriately different levels

The test generates 1000+ random test cases covering:
- All badge counts (1-32, excluding 0 which has special handling)
- All regions (0-3)
- Elite Four defeated/not defeated states
- Edge cases (first badge in each region, same badge count across regions, etc.)

## Expected Behavior

When the implementation is correct, all tests should pass. The test will report:
- Total number of test cases run
- Number of passed tests
- Number of failed tests
- Counterexamples for any failures (showing the specific inputs that caused the failure)

### Task 3.2: Badge Tracking Property Tests

**Files:** `test_badge_tracking.c`, `test_badge_tracking.py`

**Property 23: Badge Tracking Separation**
For any badge earned, it should be tracked separately by region, with each region maintaining its own set of 8 badges.

**Validates:** Requirements 14.1, 14.2, 14.3, 14.5

The test verifies that:
1. Badges awarded in one region don't affect other regions
2. GetRegionBadgeCount() returns correct count per region
3. HasBadge() correctly identifies badges by region and index

**Property 24: Badge Storage Capacity**
For any game state, the system should support tracking a total of 24 badges (8 per region × 3 regions).

**Validates:** Requirements 14.6

The test verifies that:
1. System can track all 24 badges simultaneously
2. GetTotalBadgeCount() returns sum across all regions
3. Each region can hold exactly 8 badges

The tests generate 100+ random test cases per property covering:
- All regions (0-2: Kanto, Hoenn, Johto)
- All badge indices (0-7)
- Random badge combinations using bitmasks
- Edge cases (no badges, all badges, single region badges, etc.)

### Building Badge Tracking Tests

**On Linux/Mac:**
```bash
gcc -Wall -Wextra -std=c99 -o test_badge_tracking test_badge_tracking.c
./test_badge_tracking
```

**On Windows with MinGW:**
```bash
gcc -Wall -Wextra -std=c99 -o test_badge_tracking.exe test_badge_tracking.c
./test_badge_tracking.exe
```

**Python version:**
```bash
python test_badge_tracking.py
```

### Task 3.4: Region Unlocking Property Tests

**File:** `test_region_unlocking.c`

**Property 1: Elite Four Progression Unlocks Next Region**
For any region with an Elite Four, defeating that region's Elite Four should unlock the next region in sequence (Kanto → Hoenn → Johto).

**Validates:** Requirements 1.1, 1.2

The test verifies that:
1. Defeating Kanto E4 unlocks Hoenn
2. Defeating Hoenn E4 unlocks Johto
3. Defeating Johto E4 does not unlock any new region (Sinnoh descoped)
4. CheckEliteFourDefeatAndUnlock() returns TRUE when a region is unlocked
5. CheckEliteFourDefeatAndUnlock() returns FALSE when called again (already defeated)

**Property 2: Region Unlock Awards Ticket**
For any region that becomes unlocked, the player should receive the corresponding region ticket item.

**Validates:** Requirements 1.4

The test verifies that:
1. UnlockRegion(REGION_HOENN) awards ITEM_HOENN_TICKET
2. UnlockRegion(REGION_JOHTO) awards ITEM_JOHTO_TICKET
3. GetRegionTicketItem() returns correct ticket for each region
4. Ticket is only awarded once (idempotent unlock)

**Property 3: Locked Region Access Prevention**
For any locked region and any warp attempt, the system should prevent the player from accessing that region.

**Validates:** Requirements 1.5

The test verifies that:
1. CanTravelToRegion() returns FALSE for locked regions
2. CanTravelToRegion() returns TRUE for unlocked regions with ticket
3. CanTravelToRegion() returns TRUE for current region
4. CanTravelToRegion() returns TRUE for Kanto (always accessible)
5. IsRegionUnlocked() correctly tracks unlock status

The tests generate 100+ random test cases per property covering:
- All regions (0-2: Kanto, Hoenn, Johto)
- Various unlock states and ticket combinations
- Edge cases (full progression, invalid regions, double defeat, etc.)

### Building Region Unlocking Tests

**On Linux/Mac:**
```bash
gcc -Wall -Wextra -std=c99 -o test_region_unlocking test_region_unlocking.c
./test_region_unlocking
```

**On Windows with MinGW:**
```bash
gcc -Wall -Wextra -std=c99 -o test_region_unlocking.exe test_region_unlocking.c
./test_region_unlocking.exe
```

## Implementation Status

- [x] Test infrastructure created
- [x] Property test for wild Pokemon level calculation written (Task 2.1)
- [x] Property test for region starting level application written (Task 2.2)
- [x] Level scaler implementation created
- [x] Tests run and validated with Python verification script
- [x] Property tests for badge tracking written (Task 3.2)
  - [x] Property 23: Badge Tracking Separation
  - [x] Property 24: Badge Storage Capacity
- [x] Property tests for region unlocking written (Task 3.4)
  - [x] Property 1: Elite Four Progression Unlocks Next Region
  - [x] Property 2: Region Unlock Awards Ticket
  - [x] Property 3: Locked Region Access Prevention
