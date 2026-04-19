#!/usr/bin/env python3
"""
Property-Based Tests for Badge Tracking System (Python Implementation)

Tests Property 23: Badge Tracking Separation
Tests Property 24: Badge Storage Capacity

**Validates: Requirements 14.1, 14.2, 14.3, 14.5, 14.6**

Uses minimum 100 iterations per property.
Region order: Kanto → Hoenn → Johto (per ADR-004)
Sinnoh descoped per ADR-003 (3 regions, 24 badges total)
"""

import random
import sys
from dataclasses import dataclass
from typing import List, Dict

# Region constants
REGION_KANTO = 0
REGION_HOENN = 1
REGION_JOHTO = 2
NUM_REGIONS = 3
BADGES_PER_REGION = 8
TOTAL_BADGES = NUM_REGIONS * BADGES_PER_REGION  # 24 badges

REGION_NAMES = {
    REGION_KANTO: "Kanto",
    REGION_HOENN: "Hoenn",
    REGION_JOHTO: "Johto"
}


@dataclass
class BadgeData:
    """Badge tracking structure for multi-region support"""
    kanto_badges: List[bool]
    hoenn_badges: List[bool]
    johto_badges: List[bool]
    
    def __init__(self):
        self.kanto_badges = [False] * BADGES_PER_REGION
        self.hoenn_badges = [False] * BADGES_PER_REGION
        self.johto_badges = [False] * BADGES_PER_REGION


@dataclass
class RegionState:
    """Region state tracking structure"""
    current_region: int
    hoenn_unlocked: bool
    johto_unlocked: bool
    elite_four_defeated: List[bool]
    badges: BadgeData
    
    def __init__(self):
        self.current_region = REGION_KANTO
        self.hoenn_unlocked = False
        self.johto_unlocked = False
        self.elite_four_defeated = [False] * NUM_REGIONS
        self.badges = BadgeData()


# Global region state for testing
region_state = RegionState()


def init_region_state():
    """Initialize region state for a new game."""
    global region_state
    region_state = RegionState()


def has_badge(region_id: int, badge_index: int) -> bool:
    """Check if a specific badge has been earned."""
    if badge_index >= BADGES_PER_REGION:
        return False
    
    if region_id == REGION_KANTO:
        return region_state.badges.kanto_badges[badge_index]
    elif region_id == REGION_HOENN:
        return region_state.badges.hoenn_badges[badge_index]
    elif region_id == REGION_JOHTO:
        return region_state.badges.johto_badges[badge_index]
    return False


def award_badge(region_id: int, badge_index: int):
    """Award a badge to the player."""
    if badge_index >= BADGES_PER_REGION:
        return
    
    if region_id == REGION_KANTO:
        region_state.badges.kanto_badges[badge_index] = True
    elif region_id == REGION_HOENN:
        region_state.badges.hoenn_badges[badge_index] = True
    elif region_id == REGION_JOHTO:
        region_state.badges.johto_badges[badge_index] = True


def get_region_badge_count(region_id: int) -> int:
    """Get badge count for a specific region."""
    if region_id == REGION_KANTO:
        return sum(region_state.badges.kanto_badges)
    elif region_id == REGION_HOENN:
        return sum(region_state.badges.hoenn_badges)
    elif region_id == REGION_JOHTO:
        return sum(region_state.badges.johto_badges)
    return 0


def get_total_badge_count() -> int:
    """Get total badge count across all regions."""
    return (get_region_badge_count(REGION_KANTO) +
            get_region_badge_count(REGION_HOENN) +
            get_region_badge_count(REGION_JOHTO))


def prop_badge_tracking_separation(region_id: int, badge_index: int) -> tuple:
    """
    Property 23: Badge Tracking Separation
    
    For any badge earned, it should be tracked separately by region,
    with each region maintaining its own set of 8 badges.
    
    **Validates: Requirements 14.1, 14.2, 14.3, 14.5**
    """
    # Reset state for clean test
    init_region_state()
    
    # Validate input bounds
    if region_id >= NUM_REGIONS or badge_index >= BADGES_PER_REGION:
        return True, None
    
    # Record initial state of all regions
    initial_counts = {
        REGION_KANTO: get_region_badge_count(REGION_KANTO),
        REGION_HOENN: get_region_badge_count(REGION_HOENN),
        REGION_JOHTO: get_region_badge_count(REGION_JOHTO)
    }
    
    # Award the badge
    award_badge(region_id, badge_index)
    
    # Verify the badge was awarded
    if not has_badge(region_id, badge_index):
        return False, f"Badge not awarded - region={region_id}, badge={badge_index}"
    
    # Verify the target region's badge count increased by 1
    new_target_count = get_region_badge_count(region_id)
    expected_target_count = initial_counts[region_id] + 1
    
    if new_target_count != expected_target_count:
        return False, f"Target region badge count incorrect - region={region_id}, expected={expected_target_count}, got={new_target_count}"
    
    # CRITICAL: Verify other regions were NOT affected
    for other_region in range(NUM_REGIONS):
        if other_region != region_id:
            new_count = get_region_badge_count(other_region)
            if new_count != initial_counts[other_region]:
                return False, f"{REGION_NAMES[other_region]} badges affected by award to region {region_id} - was={initial_counts[other_region]}, now={new_count}"
    
    # Verify HasBadge returns false for same badge index in other regions
    for other_region in range(NUM_REGIONS):
        if other_region != region_id:
            if has_badge(other_region, badge_index):
                return False, f"HasBadge incorrectly returns true for region={other_region}, badge={badge_index} (badge was awarded to region={region_id})"
    
    return True, None


def prop_badge_storage_capacity(kanto_mask: int, hoenn_mask: int, johto_mask: int) -> tuple:
    """
    Property 24: Badge Storage Capacity
    
    For any game state, the system should support tracking a total of 
    24 badges (8 per region × 3 regions).
    
    **Validates: Requirements 14.6**
    """
    # Reset state for clean test
    init_region_state()
    
    expected_kanto = 0
    expected_hoenn = 0
    expected_johto = 0
    
    # Award Kanto badges based on bitmask
    for i in range(BADGES_PER_REGION):
        if kanto_mask & (1 << i):
            award_badge(REGION_KANTO, i)
            expected_kanto += 1
    
    # Award Hoenn badges based on bitmask
    for i in range(BADGES_PER_REGION):
        if hoenn_mask & (1 << i):
            award_badge(REGION_HOENN, i)
            expected_hoenn += 1
    
    # Award Johto badges based on bitmask
    for i in range(BADGES_PER_REGION):
        if johto_mask & (1 << i):
            award_badge(REGION_JOHTO, i)
            expected_johto += 1
    
    # Verify per-region counts
    actual_kanto = get_region_badge_count(REGION_KANTO)
    actual_hoenn = get_region_badge_count(REGION_HOENN)
    actual_johto = get_region_badge_count(REGION_JOHTO)
    
    if actual_kanto != expected_kanto:
        return False, f"Kanto badge count mismatch - expected={expected_kanto}, got={actual_kanto} (mask=0x{kanto_mask:02X})"
    
    if actual_hoenn != expected_hoenn:
        return False, f"Hoenn badge count mismatch - expected={expected_hoenn}, got={actual_hoenn} (mask=0x{hoenn_mask:02X})"
    
    if actual_johto != expected_johto:
        return False, f"Johto badge count mismatch - expected={expected_johto}, got={actual_johto} (mask=0x{johto_mask:02X})"
    
    # Verify total count equals sum of all regions
    expected_total = expected_kanto + expected_hoenn + expected_johto
    actual_total = get_total_badge_count()
    
    if actual_total != expected_total:
        return False, f"Total badge count mismatch - expected={expected_total}, got={actual_total}"
    
    # Verify total is within valid range (0-24)
    if actual_total > TOTAL_BADGES:
        return False, f"Total badge count exceeds maximum - got={actual_total}, max={TOTAL_BADGES}"
    
    # Verify each region count is within valid range (0-8)
    if actual_kanto > BADGES_PER_REGION or actual_hoenn > BADGES_PER_REGION or actual_johto > BADGES_PER_REGION:
        return False, f"Region badge count exceeds maximum of {BADGES_PER_REGION}"
    
    return True, None


def test_property_23_badge_separation():
    """Test runner for Property 23: Badge Tracking Separation"""
    num_trials = 100  # Minimum 100 iterations per property
    passed = 0
    failed = 0
    counterexamples = []
    
    print("\n=== Property-Based Test: Badge Tracking Separation (Property 23) ===")
    print("**Validates: Requirements 14.1, 14.2, 14.3, 14.5**")
    print(f"Running {num_trials} random test cases...\n")
    
    # Run property tests with random inputs
    for _ in range(num_trials):
        region_id = random.randint(0, NUM_REGIONS - 1)
        badge_index = random.randint(0, BADGES_PER_REGION - 1)
        
        success, error = prop_badge_tracking_separation(region_id, badge_index)
        if success:
            passed += 1
        else:
            failed += 1
            if len(counterexamples) < 5:
                counterexamples.append(f"region_id={region_id}, badge_index={badge_index}: {error}")
    
    # Test specific edge cases
    print("Testing specific edge cases...")
    
    # Edge case 1: First badge in each region
    for region in range(NUM_REGIONS):
        success, error = prop_badge_tracking_separation(region, 0)
        if success:
            passed += 1
        else:
            failed += 1
            print(f"FAIL: Edge case first badge in region {region}: {error}")
    
    # Edge case 2: Last badge in each region
    for region in range(NUM_REGIONS):
        success, error = prop_badge_tracking_separation(region, BADGES_PER_REGION - 1)
        if success:
            passed += 1
        else:
            failed += 1
            print(f"FAIL: Edge case last badge in region {region}: {error}")
    
    # Edge case 3: Same badge index across all regions
    print("\nTesting same badge index across all regions...")
    init_region_state()
    award_badge(REGION_KANTO, 3)
    award_badge(REGION_HOENN, 3)
    award_badge(REGION_JOHTO, 3)
    
    if (get_region_badge_count(REGION_KANTO) == 1 and
        get_region_badge_count(REGION_HOENN) == 1 and
        get_region_badge_count(REGION_JOHTO) == 1):
        print("  Same badge index (3) in all regions: PASSED")
        passed += 1
    else:
        print("FAIL: Same badge index test - each region should have 1 badge")
        failed += 1
    
    if get_total_badge_count() == 3:
        passed += 1
    else:
        print(f"FAIL: Same badge index test - total should be 3, got {get_total_badge_count()}")
        failed += 1
    
    # Report results
    print(f"\n=== Property 23 Test Results ===")
    print(f"Total tests: {passed + failed}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if counterexamples:
        print("\nCounterexamples:")
        for ce in counterexamples:
            print(f"  {ce}")
    
    if failed == 0:
        print("\n✓ Property 23 (Badge Tracking Separation) PASSED!")
        return 0
    else:
        print(f"\n✗ Property 23 (Badge Tracking Separation) FAILED with {failed} counterexamples")
        return 1


def test_property_24_badge_capacity():
    """Test runner for Property 24: Badge Storage Capacity"""
    num_trials = 100  # Minimum 100 iterations per property
    passed = 0
    failed = 0
    counterexamples = []
    
    print("\n=== Property-Based Test: Badge Storage Capacity (Property 24) ===")
    print("**Validates: Requirements 14.6**")
    print(f"Running {num_trials} random test cases...\n")
    
    # Run property tests with random inputs
    for _ in range(num_trials):
        kanto_mask = random.randint(0, 255)
        hoenn_mask = random.randint(0, 255)
        johto_mask = random.randint(0, 255)
        
        success, error = prop_badge_storage_capacity(kanto_mask, hoenn_mask, johto_mask)
        if success:
            passed += 1
        else:
            failed += 1
            if len(counterexamples) < 5:
                counterexamples.append(f"kanto=0x{kanto_mask:02X}, hoenn=0x{hoenn_mask:02X}, johto=0x{johto_mask:02X}: {error}")
    
    # Test specific edge cases
    print("Testing specific edge cases...")
    
    # Edge case 1: No badges
    success, error = prop_badge_storage_capacity(0x00, 0x00, 0x00)
    if success:
        print("  No badges: PASSED")
        passed += 1
    else:
        print(f"FAIL: Edge case no badges: {error}")
        failed += 1
    
    # Edge case 2: All badges (24 total)
    success, error = prop_badge_storage_capacity(0xFF, 0xFF, 0xFF)
    if success:
        print("  All 24 badges: PASSED")
        passed += 1
    else:
        print(f"FAIL: Edge case all badges: {error}")
        failed += 1
    
    # Edge case 3: Only Kanto badges (8)
    success, error = prop_badge_storage_capacity(0xFF, 0x00, 0x00)
    if success:
        print("  Only Kanto badges (8): PASSED")
        passed += 1
    else:
        print(f"FAIL: Edge case only Kanto badges: {error}")
        failed += 1
    
    # Edge case 4: Only Hoenn badges (8)
    success, error = prop_badge_storage_capacity(0x00, 0xFF, 0x00)
    if success:
        print("  Only Hoenn badges (8): PASSED")
        passed += 1
    else:
        print(f"FAIL: Edge case only Hoenn badges: {error}")
        failed += 1
    
    # Edge case 5: Only Johto badges (8)
    success, error = prop_badge_storage_capacity(0x00, 0x00, 0xFF)
    if success:
        print("  Only Johto badges (8): PASSED")
        passed += 1
    else:
        print(f"FAIL: Edge case only Johto badges: {error}")
        failed += 1

    # Edge case 6: Verify maximum capacity (24 badges)
    print("\nVerifying maximum badge capacity...")
    init_region_state()
    
    # Award all 24 badges
    for region in range(NUM_REGIONS):
        for badge in range(BADGES_PER_REGION):
            award_badge(region, badge)
    
    total = get_total_badge_count()
    if total == TOTAL_BADGES:
        print(f"  Maximum capacity ({TOTAL_BADGES} badges): PASSED")
        passed += 1
    else:
        print(f"FAIL: Maximum capacity test - expected {TOTAL_BADGES} badges, got {total}")
        failed += 1
    
    # Verify each region has exactly 8 badges
    for region in range(NUM_REGIONS):
        count = get_region_badge_count(region)
        if count == BADGES_PER_REGION:
            passed += 1
        else:
            print(f"FAIL: Region {region} should have {BADGES_PER_REGION} badges, got {count}")
            failed += 1
    
    # Edge case 7: Verify HasBadge for all 24 badges
    print("\nVerifying HasBadge for all 24 badges...")
    all_badges_found = True
    for region in range(NUM_REGIONS):
        for badge in range(BADGES_PER_REGION):
            if not has_badge(region, badge):
                print(f"FAIL: HasBadge returned false for region={region}, badge={badge}")
                all_badges_found = False
                failed += 1
    if all_badges_found:
        print("  HasBadge for all 24 badges: PASSED")
        passed += 1
    
    # Edge case 8: Verify invalid badge index returns false
    print("\nVerifying invalid badge index handling...")
    if not has_badge(REGION_KANTO, BADGES_PER_REGION):
        print(f"  Invalid badge index ({BADGES_PER_REGION}) returns false: PASSED")
        passed += 1
    else:
        print(f"FAIL: HasBadge should return false for invalid badge index {BADGES_PER_REGION}")
        failed += 1
    
    # Edge case 9: Verify invalid region returns 0 count
    invalid_region_count = get_region_badge_count(NUM_REGIONS)
    if invalid_region_count == 0:
        print("  Invalid region returns 0 count: PASSED")
        passed += 1
    else:
        print(f"FAIL: GetRegionBadgeCount should return 0 for invalid region, got {invalid_region_count}")
        failed += 1
    
    # Report results
    print(f"\n=== Property 24 Test Results ===")
    print(f"Total tests: {passed + failed}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if counterexamples:
        print("\nCounterexamples:")
        for ce in counterexamples:
            print(f"  {ce}")
    
    if failed == 0:
        print("\n✓ Property 24 (Badge Storage Capacity) PASSED!")
        return 0
    else:
        print(f"\n✗ Property 24 (Badge Storage Capacity) FAILED with {failed} counterexamples")
        return 1


def main():
    """Main test runner"""
    print("╔══════════════════════════════════════════════════════════════════╗")
    print("║     Property-Based Tests for Badge Tracking System               ║")
    print("║     Task 3.2: Write property tests for badge tracking            ║")
    print("╚══════════════════════════════════════════════════════════════════╝")
    print("\nRegion configuration: Kanto → Hoenn → Johto (3 regions, 24 badges)")
    print("Sinnoh descoped per ADR-003")
    print("Minimum iterations per property: 100")
    
    property23_result = test_property_23_badge_separation()
    property24_result = test_property_24_badge_capacity()
    
    # Overall results
    print("\n\n╔══════════════════════════════════════════════════════════════════╗")
    print("║                    OVERALL TEST RESULTS                          ║")
    print("╚══════════════════════════════════════════════════════════════════╝")
    
    if property23_result == 0 and property24_result == 0:
        print("\n✓ ALL PROPERTY TESTS PASSED!\n")
        print("Property 23 (Badge Tracking Separation): PASSED")
        print("  - Validates: Requirements 14.1, 14.2, 14.3, 14.5")
        print("Property 24 (Badge Storage Capacity): PASSED")
        print("  - Validates: Requirements 14.6")
        return 0
    else:
        print("\n✗ SOME PROPERTY TESTS FAILED\n")
        if property23_result != 0:
            print("Property 23 (Badge Tracking Separation): FAILED")
        else:
            print("Property 23 (Badge Tracking Separation): PASSED")
        if property24_result != 0:
            print("Property 24 (Badge Storage Capacity): FAILED")
        else:
            print("Property 24 (Badge Storage Capacity): PASSED")
        return 1


if __name__ == "__main__":
    sys.exit(main())
