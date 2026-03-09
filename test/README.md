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

## Implementation Status

- [x] Test infrastructure created
- [x] Property test for wild Pokemon level calculation written (Task 2.1)
- [x] Property test for region starting level application written (Task 2.2)
- [x] Level scaler implementation created
- [x] Tests run and validated with Python verification script
