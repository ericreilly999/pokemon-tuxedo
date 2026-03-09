#!/usr/bin/env python3
"""
Verification script for Pokemon Tuxedo Level Scaler property tests.

This script implements the same property test logic as test_level_scaler.c
to verify the test design is correct before running the actual C tests.
"""

import random
from typing import Tuple

# Region starting levels (from requirements)
REGION_STARTING_LEVELS = {
    0: 2,    # REGION_KANTO
    1: 60,   # REGION_JOHTO
    2: 110,  # REGION_HOENN
    3: 160   # REGION_SINNOH
}

def get_region_starting_level(region_id: int) -> int:
    """Get the starting level for a given region."""
    return REGION_STARTING_LEVELS.get(region_id, 2)

def get_wild_pokemon_level_range(badge_count: int, region_id: int, elite_four_defeated: bool) -> Tuple[int, int]:
    """
    Calculate wild Pokemon level range based on badge count and region.
    
    This implements the same logic as the C function GetWildPokemonLevelRange.
    
    Requirements:
    - 2.1: Minimum level = Badge_Count × 5
    - 2.2: Maximum level = (Badge_Count × 5) + 10
    - 2.3: When Badge_Count is 0, levels should be 2-10
    - 2.6: Account for region starting level
    - 2.7: Add 10 levels if Elite Four defeated in that region
    """
    region_starting_level = get_region_starting_level(region_id)
    
    # Special case: badge_count == 0 gives levels 2-10 (Requirement 2.3)
    if badge_count == 0:
        min_level = 2
        max_level = 10
    else:
        # General formula (Requirements 2.1, 2.2, 2.6):
        # min = (badge_count * 5) + region_starting_level
        # max = min + 10
        min_level = (badge_count * 5) + region_starting_level
        max_level = min_level + 10
    
    # Apply Elite Four bonus if defeated (Requirement 2.7)
    if elite_four_defeated:
        min_level += 10
        max_level += 10
    
    # Clamp to valid Pokemon level range (1-100)
    min_level = max(1, min(100, min_level))
    max_level = max(1, min(100, max_level))
    
    return (min_level, max_level)

def prop_wild_pokemon_level_range(badge_count: int, region_id: int, elite_four_defeated: bool) -> bool:
    """
    Property 1: Wild Pokemon Level Range Calculation
    
    For any badge count and region, the wild Pokemon level range should be calculated as:
    - minimum = (badge_count × 5) + region_starting_level
    - maximum = minimum + 10
    - Special case: when badge_count is 0, levels should be 2-10
    
    Validates: Requirements 2.1, 2.2, 2.3
    """
    # Get the level range from the function under test
    min_level, max_level = get_wild_pokemon_level_range(badge_count, region_id, elite_four_defeated)
    
    # Get region starting level
    region_starting_level = get_region_starting_level(region_id)
    
    # Calculate expected values
    if badge_count == 0:
        expected_min = 2
        expected_max = 10
    else:
        expected_min = (badge_count * 5) + region_starting_level
        expected_max = expected_min + 10
    
    # Apply Elite Four bonus if defeated (+10 levels)
    if elite_four_defeated:
        expected_min += 10
        expected_max += 10
    
    # Clamp expected values to valid range (1-100) for comparison
    expected_min = max(1, min(100, expected_min))
    expected_max = max(1, min(100, expected_max))
    
    # Verify the calculated range matches expected values
    if min_level != expected_min:
        print(f"FAIL: badge={badge_count}, region={region_id}, e4={elite_four_defeated} - "
              f"Expected min_level={expected_min}, got {min_level}")
        return False
    
    if max_level != expected_max:
        print(f"FAIL: badge={badge_count}, region={region_id}, e4={elite_four_defeated} - "
              f"Expected max_level={expected_max}, got {max_level}")
        return False
    
    # Verify that max is always min + 10 (unless clamped or badge_count == 0)
    # Special case: badge_count == 0 has a fixed range of 2-10 (8 level spread)
    if badge_count == 0:
        # For badge_count == 0, the range is always 2-10 before E4 bonus
        # After E4 bonus, it becomes 12-20
        expected_spread = 8
        if max_level != min_level + expected_spread:
            print(f"FAIL: badge={badge_count}, region={region_id}, e4={elite_four_defeated} - "
                  f"For badge_count=0, max_level should be min_level + 8, got min={min_level}, max={max_level}")
            return False
    elif max_level != min_level + 10 and max_level != 100:
        print(f"FAIL: badge={badge_count}, region={region_id}, e4={elite_four_defeated} - "
              f"max_level should be min_level + 10, got min={min_level}, max={max_level}")
        return False
    
    # Verify levels are within valid Pokemon level range (1-100)
    if not (1 <= min_level <= 100):
        print(f"FAIL: badge={badge_count}, region={region_id}, e4={elite_four_defeated} - "
              f"min_level out of valid range (1-100): {min_level}")
        return False
    
    if not (1 <= max_level <= 100):
        print(f"FAIL: badge={badge_count}, region={region_id}, e4={elite_four_defeated} - "
              f"max_level out of valid range (1-100): {max_level}")
        return False
    
    return True

def prop_region_starting_level_applied(badge_count: int, region_id: int, elite_four_defeated: bool) -> bool:
    """
    Property 3: Region Starting Level Applied to Calculations
    
    For any region, wild Pokemon levels should include the region's starting level
    as a base offset in all level calculations.
    
    This property verifies that:
    1. For badge_count > 0, the minimum level includes the region starting level
    2. The region starting level is correctly applied for all four regions
    3. The starting level offset is maintained even with Elite Four bonuses
    
    Validates: Requirements 2.6, 6.5
    """
    # Skip badge_count == 0 as it has special handling (fixed 2-10 range)
    if badge_count == 0:
        return True
    
    # Get the level range from the function under test
    min_level, max_level = get_wild_pokemon_level_range(badge_count, region_id, elite_four_defeated)
    
    # Get region starting level
    region_starting_level = get_region_starting_level(region_id)
    
    # Calculate the base level without region offset (badge_count * 5)
    base_level = badge_count * 5
    
    # Calculate expected minimum level with region starting level applied
    expected_min = base_level + region_starting_level
    
    # Apply Elite Four bonus if defeated
    if elite_four_defeated:
        expected_min += 10
    
    # Clamp to valid range for comparison
    expected_min = max(1, min(100, expected_min))
    
    # Verify that the region starting level is included in the calculation
    if min_level != expected_min:
        print(f"FAIL: Region starting level not applied correctly")
        print(f"  badge={badge_count}, region={region_id}, e4={elite_four_defeated}")
        print(f"  Region starting level: {region_starting_level}")
        print(f"  Base level (badge*5): {base_level}")
        print(f"  Expected min (base + region_start + e4_bonus): {expected_min}")
        print(f"  Actual min: {min_level}")
        return False
    
    # Verify that different regions with the same badge count have different levels
    # (unless clamped to 100)
    if region_id > 0 and min_level < 100:
        # Calculate what the level would be in Kanto (region 0) with same badges
        kanto_starting_level = get_region_starting_level(0)
        kanto_expected_min = base_level + kanto_starting_level
        if elite_four_defeated:
            kanto_expected_min += 10
        kanto_expected_min = max(1, min(100, kanto_expected_min))
        
        # If we're in a later region, levels should be higher than Kanto
        # (unless both are clamped to 100)
        if kanto_expected_min < 100 and min_level <= kanto_expected_min:
            print(f"FAIL: Region starting level offset not creating higher levels in later regions")
            print(f"  badge={badge_count}, region={region_id}, e4={elite_four_defeated}")
            print(f"  Current region min: {min_level}")
            print(f"  Kanto equivalent min: {kanto_expected_min}")
            print(f"  Region starting levels: Kanto={kanto_starting_level}, Current={region_starting_level}")
            return False
    
    return True

def main():
    """Run property-based tests with random inputs."""
    num_trials = 1000
    passed = 0
    failed = 0
    
    print("=== Property-Based Test: Wild Pokemon Level Range Calculation ===")
    print("Validates: Requirements 2.1, 2.2, 2.3")
    print(f"Running {num_trials} random test cases...\n")
    
    # Run property tests with random inputs
    for i in range(num_trials):
        badge_count = random.randint(0, 32)
        region_id = random.randint(0, 3)
        elite_four_defeated = random.choice([True, False])
        
        if prop_wild_pokemon_level_range(badge_count, region_id, elite_four_defeated):
            passed += 1
        else:
            failed += 1
            if failed <= 5:
                print(f"  Counterexample #{failed}: badge_count={badge_count}, "
                      f"region_id={region_id}, elite_four_defeated={elite_four_defeated}")
    
    # Also test specific edge cases
    print("\nTesting specific edge cases...")
    
    # Edge case 1: badge_count = 0 (should always be 2-10)
    if not prop_wild_pokemon_level_range(0, 0, False):
        print("FAIL: Edge case badge_count=0 failed")
        failed += 1
    else:
        passed += 1
    
    # Edge case 2: badge_count = 0 with Elite Four defeated
    if not prop_wild_pokemon_level_range(0, 0, True):
        print("FAIL: Edge case badge_count=0 with E4 defeated failed")
        failed += 1
    else:
        passed += 1
    
    # Edge case 3: Maximum badges (32)
    if not prop_wild_pokemon_level_range(32, 3, True):
        print("FAIL: Edge case badge_count=32 failed")
        failed += 1
    else:
        passed += 1
    
    # Edge case 4: First badge in each region
    for region in range(4):
        if not prop_wild_pokemon_level_range(1, region, False):
            print(f"FAIL: Edge case badge_count=1, region={region} failed")
            failed += 1
        else:
            passed += 1
    
    # Report results
    print("\n=== Test Results ===")
    print(f"Total tests: {passed + failed}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    property1_failed = failed
    
    if failed == 0:
        print("\n✓ Property 1 tests PASSED!")
    else:
        print(f"\n✗ Property 1 tests FAILED with {failed} counterexamples")
    
    # Run Property 3 tests
    print("\n\n=== Property-Based Test: Region Starting Level Applied to Calculations ===")
    print("Validates: Requirements 2.6, 6.5")
    print(f"Running {num_trials} random test cases...\n")
    
    passed3 = 0
    failed3 = 0
    
    # Run property tests with random inputs
    for i in range(num_trials):
        # Focus on badge_count > 0 since badge_count == 0 has special handling
        badge_count = random.randint(1, 32)
        region_id = random.randint(0, 3)
        elite_four_defeated = random.choice([True, False])
        
        if prop_region_starting_level_applied(badge_count, region_id, elite_four_defeated):
            passed3 += 1
        else:
            failed3 += 1
            if failed3 <= 5:
                print(f"  Counterexample #{failed3}: badge_count={badge_count}, "
                      f"region_id={region_id}, elite_four_defeated={elite_four_defeated}")
    
    # Test specific edge cases for each region
    print("\nTesting specific edge cases for each region...")
    
    # Edge case 1: First badge in each region (should show region offset clearly)
    for region in range(4):
        if not prop_region_starting_level_applied(1, region, False):
            print(f"FAIL: Edge case badge_count=1, region={region} failed")
            failed3 += 1
        else:
            passed3 += 1
    
    # Edge case 2: Same badge count across all regions (should show different levels)
    test_badge_count = 8  # Mid-game badge count
    for region in range(4):
        if not prop_region_starting_level_applied(test_badge_count, region, False):
            print(f"FAIL: Edge case badge_count={test_badge_count}, region={region} failed")
            failed3 += 1
        else:
            passed3 += 1
    
    # Edge case 3: Verify region starting levels are correctly defined
    print("\nVerifying region starting levels:")
    expected_starting_levels = {0: 2, 1: 60, 2: 110, 3: 160}
    for region in range(4):
        actual = get_region_starting_level(region)
        expected = expected_starting_levels[region]
        if actual != expected:
            print(f"FAIL: Region {region} starting level is {actual}, expected {expected}")
            failed3 += 1
        else:
            print(f"  Region {region} starting level: {actual} ✓")
            passed3 += 1
    
    # Edge case 4: Test with Elite Four defeated in each region
    for region in range(4):
        if not prop_region_starting_level_applied(8, region, True):
            print(f"FAIL: Edge case badge_count=8, region={region}, E4 defeated failed")
            failed3 += 1
        else:
            passed3 += 1
    
    # Report results for Property 3
    print("\n=== Property 3 Test Results ===")
    print(f"Total tests: {passed3 + failed3}")
    print(f"Passed: {passed3}")
    print(f"Failed: {failed3}")
    
    if failed3 == 0:
        print("\n✓ Property 3 tests PASSED!")
    else:
        print(f"\n✗ Property 3 tests FAILED with {failed3} counterexamples")
    
    # Overall results
    print("\n\n=== OVERALL TEST RESULTS ===")
    if property1_failed == 0 and failed3 == 0:
        print("✓ ALL PROPERTY TESTS PASSED!")
        return 0
    else:
        print("✗ SOME PROPERTY TESTS FAILED")
        if property1_failed > 0:
            print("  - Property 1 (Wild Pokemon Level Range): FAILED")
        else:
            print("  - Property 1 (Wild Pokemon Level Range): PASSED")
        if failed3 > 0:
            print("  - Property 3 (Region Starting Level): FAILED")
        else:
            print("  - Property 3 (Region Starting Level): PASSED")
        return 1

if __name__ == "__main__":
    exit(main())
