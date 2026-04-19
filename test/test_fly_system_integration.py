#!/usr/bin/env python3
"""
Integration Tests for Fly System Across Both Regions

Task 4.6: Test fly system across both regions

Test Scenarios:
1. Register fly locations in Kanto → Verify only Kanto locations in fly menu
2. Transition to Hoenn → Verify fly menu switches to Hoenn locations
3. Register locations in Hoenn → Verify Hoenn locations added
4. Cross-region fly: From Hoenn, fly to Kanto starting location
5. Save with fly locations → Load → Verify all locations preserved

**Validates: Requirements 12.1, 12.2, 12.4, 12.5**

Region order: Kanto → Hoenn → Johto (per ADR-004)
Sinnoh descoped per ADR-003 (3 regions total)
"""

# Region constants
REGION_KANTO = 0
REGION_HOENN = 1
REGION_JOHTO = 2
NUM_REGIONS = 3

# Fly location constants
FLY_LOCATIONS_PER_REGION = 16
MAX_FLY_LOCATIONS_TOTAL = FLY_LOCATIONS_PER_REGION * NUM_REGIONS  # 48

# Map ID ranges for each region
KANTO_MAP_START = 0
KANTO_MAP_END = 255
HOENN_MAP_START = 256
HOENN_MAP_END = 511
JOHTO_MAP_START = 512
JOHTO_MAP_END = 767

# Kanto fly location map IDs (sample cities)
MAP_PALLET_TOWN = 0
MAP_VIRIDIAN_CITY = 1
MAP_PEWTER_CITY = 2
MAP_CERULEAN_CITY = 3
MAP_VERMILION_CITY = 4
MAP_LAVENDER_TOWN = 5
MAP_CELADON_CITY = 6
MAP_FUCHSIA_CITY = 7
MAP_SAFFRON_CITY = 8
MAP_CINNABAR_ISLAND = 9
MAP_INDIGO_PLATEAU = 10

# Hoenn fly location map IDs (sample cities)
MAP_LITTLEROOT_TOWN = 256
MAP_OLDALE_TOWN = 257
MAP_PETALBURG_CITY = 258
MAP_RUSTBORO_CITY = 259
MAP_DEWFORD_TOWN = 260
MAP_SLATEPORT_CITY = 261
MAP_MAUVILLE_CITY = 262
MAP_VERDANTURF_TOWN = 263
MAP_FALLARBOR_TOWN = 264
MAP_LAVARIDGE_TOWN = 265
MAP_FORTREE_CITY = 266
MAP_LILYCOVE_CITY = 267
MAP_MOSSDEEP_CITY = 268
MAP_SOOTOPOLIS_CITY = 269
MAP_PACIFIDLOG_TOWN = 270
MAP_EVER_GRANDE_CITY = 271

# Starting fly locations for each region
REGION_STARTING_LOCATIONS = {
    REGION_KANTO: MAP_PALLET_TOWN,
    REGION_HOENN: MAP_LITTLEROOT_TOWN,
    REGION_JOHTO: 512,  # New Bark Town (placeholder)
}


class FlyLocationSystem:
    """Implementation of fly location system for testing."""
    
    def __init__(self):
        self.fly_locations = [[0] * FLY_LOCATIONS_PER_REGION for _ in range(NUM_REGIONS)]
        self.fly_location_count = [0] * NUM_REGIONS
        self.current_region = REGION_KANTO
        self.region_unlocked = [True, False, False]  # Kanto always unlocked
    
    def reset(self):
        """Reset all state."""
        self.fly_locations = [[0] * FLY_LOCATIONS_PER_REGION for _ in range(NUM_REGIONS)]
        self.fly_location_count = [0] * NUM_REGIONS
        self.current_region = REGION_KANTO
        self.region_unlocked = [True, False, False]
    
    def get_region_for_map_id(self, map_id):
        """Determine which region a map_id belongs to."""
        if map_id < 256:
            return REGION_KANTO
        elif map_id < 512:
            return REGION_HOENN
        else:
            return REGION_JOHTO
    
    def get_current_region(self):
        return self.current_region
    
    def set_current_region(self, region_id):
        if 0 <= region_id < NUM_REGIONS:
            self.current_region = region_id
    
    def is_region_unlocked(self, region_id):
        if 0 <= region_id < NUM_REGIONS:
            return self.region_unlocked[region_id]
        return False
    
    def set_region_unlocked(self, region_id, unlocked):
        if 0 <= region_id < NUM_REGIONS:
            self.region_unlocked[region_id] = unlocked

    def is_fly_location_registered_for_region(self, map_id, region_id):
        """Check if a fly location is registered for a specific region."""
        if region_id >= NUM_REGIONS:
            return False
        count = self.fly_location_count[region_id]
        for i in range(count):
            if self.fly_locations[region_id][i] == map_id:
                return True
        return False
    
    def register_fly_location_for_region(self, map_id, region_id):
        """Register a fly location for a specific region."""
        if region_id >= NUM_REGIONS:
            return
        count = self.fly_location_count[region_id]
        if count >= FLY_LOCATIONS_PER_REGION:
            return
        if self.is_fly_location_registered_for_region(map_id, region_id):
            return
        self.fly_locations[region_id][count] = map_id
        self.fly_location_count[region_id] = count + 1
    
    def register_fly_location(self, map_id):
        """Register a fly location for the current region."""
        self.register_fly_location_for_region(map_id, self.current_region)
    
    def is_fly_location_registered(self, map_id):
        """Check if a fly location is registered in any region."""
        for region in range(NUM_REGIONS):
            if self.is_fly_location_registered_for_region(map_id, region):
                return True
        return False
    
    def get_current_region_fly_locations(self):
        """Get fly locations for the current region."""
        count = self.fly_location_count[self.current_region]
        return self.fly_locations[self.current_region][:count]
    
    def get_fly_location_count_for_region(self, region_id):
        """Get the count of registered fly locations for a region."""
        if region_id >= NUM_REGIONS:
            return 0
        return self.fly_location_count[region_id]
    
    def get_fly_locations_for_region(self, region_id):
        """Get all fly locations for a specific region."""
        if region_id >= NUM_REGIONS:
            return []
        count = self.fly_location_count[region_id]
        return self.fly_locations[region_id][:count]
    
    def get_region_starting_fly_location(self, region_id):
        """Get the starting fly location for a region."""
        return REGION_STARTING_LOCATIONS.get(region_id, 0)
    
    def get_cross_region_fly_locations(self):
        """Get cross-region fly locations (starting locations of unlocked regions)."""
        locations = []
        for region in range(NUM_REGIONS):
            if region == self.current_region:
                continue
            if self.is_region_unlocked(region):
                locations.append(self.get_region_starting_fly_location(region))
        return locations

    def save_state(self):
        """Save current state to a dictionary."""
        return {
            'fly_locations': [list(locs) for locs in self.fly_locations],
            'fly_location_count': list(self.fly_location_count),
            'current_region': self.current_region,
            'region_unlocked': list(self.region_unlocked),
        }
    
    def load_state(self, state):
        """Load state from a dictionary."""
        self.fly_locations = [list(locs) for locs in state['fly_locations']]
        self.fly_location_count = list(state['fly_location_count'])
        self.current_region = state['current_region']
        self.region_unlocked = list(state['region_unlocked'])


def test_register_kanto_locations_only_kanto_in_menu():
    """
    Integration Test 1: Register Kanto Locations → Verify Only Kanto in Menu
    **Validates: Requirements 12.1, 12.2, 12.4**
    """
    print("\n=== Integration Test 1: Register Kanto Locations ===")
    print("**Validates: Requirements 12.1, 12.2, 12.4**\n")
    
    passed = 0
    failed = 0
    system = FlyLocationSystem()
    system.reset()
    system.set_current_region(REGION_KANTO)
    
    # Step 1: Verify initial state - no fly locations
    print("Step 1: Verify initial state (no fly locations)...")
    fly_locations = system.get_current_region_fly_locations()
    if len(fly_locations) != 0:
        print(f"  FAIL: Should have 0 fly locations initially, got {len(fly_locations)}")
        failed += 1
    else:
        print("  PASS: No fly locations initially")
        passed += 1
    
    # Step 2: Register Kanto fly locations
    print("Step 2: Register Kanto fly locations...")
    system.register_fly_location(MAP_PALLET_TOWN)
    system.register_fly_location(MAP_VIRIDIAN_CITY)
    system.register_fly_location(MAP_PEWTER_CITY)
    system.register_fly_location(MAP_CERULEAN_CITY)
    system.register_fly_location(MAP_VERMILION_CITY)
    
    fly_locations = system.get_current_region_fly_locations()
    if len(fly_locations) != 5:
        print(f"  FAIL: Should have 5 Kanto fly locations, got {len(fly_locations)}")
        failed += 1
    else:
        print("  PASS: Registered 5 Kanto fly locations")
        passed += 1
    
    # Step 3: Verify all locations are Kanto maps
    print("Step 3: Verify all locations are Kanto maps...")
    all_kanto = True
    for loc in fly_locations:
        region = system.get_region_for_map_id(loc)
        if region != REGION_KANTO:
            print(f"  FAIL: Location {loc} is not Kanto")
            all_kanto = False
    if all_kanto:
        print("  PASS: All fly locations are Kanto maps")
        passed += 1
    else:
        failed += 1

    # Step 4: Verify specific locations are registered
    print("Step 4: Verify specific locations are registered...")
    locations_correct = True
    if not system.is_fly_location_registered(MAP_PALLET_TOWN):
        print("  FAIL: Pallet Town should be registered")
        locations_correct = False
    if not system.is_fly_location_registered(MAP_VIRIDIAN_CITY):
        print("  FAIL: Viridian City should be registered")
        locations_correct = False
    if not system.is_fly_location_registered(MAP_PEWTER_CITY):
        print("  FAIL: Pewter City should be registered")
        locations_correct = False
    if locations_correct:
        print("  PASS: All specific locations are registered")
        passed += 1
    else:
        failed += 1
    
    # Step 5: Verify Hoenn locations are NOT in fly menu
    print("Step 5: Verify Hoenn locations are NOT in fly menu...")
    no_hoenn = True
    for loc in fly_locations:
        if HOENN_MAP_START <= loc <= HOENN_MAP_END:
            print(f"  FAIL: Hoenn location {loc} found in Kanto fly menu")
            no_hoenn = False
    if no_hoenn:
        print("  PASS: No Hoenn locations in Kanto fly menu")
        passed += 1
    else:
        failed += 1
    
    print(f"\n--- Test 1 Results ---")
    print(f"Passed: {passed}, Failed: {failed}")
    return failed


def test_transition_to_hoenn_fly_menu_switches():
    """
    Integration Test 2: Transition to Hoenn → Fly Menu Switches
    **Validates: Requirements 12.4**
    """
    print("\n=== Integration Test 2: Transition to Hoenn → Fly Menu Switches ===")
    print("**Validates: Requirements 12.4**\n")
    
    passed = 0
    failed = 0
    system = FlyLocationSystem()
    system.reset()
    system.set_current_region(REGION_KANTO)
    system.set_region_unlocked(REGION_HOENN, True)
    
    # Register Kanto locations
    system.register_fly_location(MAP_PALLET_TOWN)
    system.register_fly_location(MAP_VIRIDIAN_CITY)
    system.register_fly_location(MAP_PEWTER_CITY)
    
    # Step 1: Verify Kanto fly menu has 3 locations
    print("Step 1: Verify Kanto fly menu has 3 locations...")
    fly_locations = system.get_current_region_fly_locations()
    if len(fly_locations) != 3:
        print(f"  FAIL: Kanto should have 3 fly locations, got {len(fly_locations)}")
        failed += 1
    else:
        print("  PASS: Kanto has 3 fly locations")
        passed += 1
    
    # Step 2: Transition to Hoenn
    print("Step 2: Transition to Hoenn...")
    system.set_current_region(REGION_HOENN)
    if system.get_current_region() != REGION_HOENN:
        print("  FAIL: Should be in Hoenn")
        failed += 1
    else:
        print("  PASS: Now in Hoenn")
        passed += 1
    
    # Step 3: Verify fly menu is now empty (no Hoenn locations registered yet)
    print("Step 3: Verify fly menu is empty (no Hoenn locations yet)...")
    fly_locations = system.get_current_region_fly_locations()
    if len(fly_locations) != 0:
        print(f"  FAIL: Hoenn should have 0 fly locations, got {len(fly_locations)}")
        failed += 1
    else:
        print("  PASS: Hoenn fly menu is empty")
        passed += 1

    # Step 4: Verify Kanto locations are still stored (just not shown)
    print("Step 4: Verify Kanto locations are still stored...")
    kanto_count = system.get_fly_location_count_for_region(REGION_KANTO)
    if kanto_count != 3:
        print(f"  FAIL: Kanto should still have 3 stored locations, got {kanto_count}")
        failed += 1
    else:
        print("  PASS: Kanto still has 3 stored locations")
        passed += 1
    
    # Step 5: Register Hoenn locations
    print("Step 5: Register Hoenn locations...")
    system.register_fly_location(MAP_LITTLEROOT_TOWN)
    system.register_fly_location(MAP_OLDALE_TOWN)
    system.register_fly_location(MAP_PETALBURG_CITY)
    system.register_fly_location(MAP_RUSTBORO_CITY)
    
    fly_locations = system.get_current_region_fly_locations()
    if len(fly_locations) != 4:
        print(f"  FAIL: Hoenn should have 4 fly locations, got {len(fly_locations)}")
        failed += 1
    else:
        print("  PASS: Hoenn now has 4 fly locations")
        passed += 1
    
    # Step 6: Verify fly menu shows only Hoenn locations
    print("Step 6: Verify fly menu shows only Hoenn locations...")
    all_hoenn = True
    for loc in fly_locations:
        region = system.get_region_for_map_id(loc)
        if region != REGION_HOENN:
            print(f"  FAIL: Location {loc} is not Hoenn")
            all_hoenn = False
    if all_hoenn:
        print("  PASS: All fly locations are Hoenn maps")
        passed += 1
    else:
        failed += 1
    
    print(f"\n--- Test 2 Results ---")
    print(f"Passed: {passed}, Failed: {failed}")
    return failed


def test_register_hoenn_locations():
    """
    Integration Test 3: Register Hoenn Locations → Verify Added
    **Validates: Requirements 12.1, 12.2**
    """
    print("\n=== Integration Test 3: Register Hoenn Locations ===")
    print("**Validates: Requirements 12.1, 12.2**\n")
    
    passed = 0
    failed = 0
    system = FlyLocationSystem()
    system.reset()
    system.set_current_region(REGION_HOENN)
    system.set_region_unlocked(REGION_HOENN, True)
    
    # Step 1: Register all Hoenn cities
    print("Step 1: Register all Hoenn cities...")
    hoenn_cities = [
        MAP_LITTLEROOT_TOWN, MAP_OLDALE_TOWN, MAP_PETALBURG_CITY, MAP_RUSTBORO_CITY,
        MAP_DEWFORD_TOWN, MAP_SLATEPORT_CITY, MAP_MAUVILLE_CITY, MAP_VERDANTURF_TOWN,
        MAP_FALLARBOR_TOWN, MAP_LAVARIDGE_TOWN, MAP_FORTREE_CITY, MAP_LILYCOVE_CITY,
        MAP_MOSSDEEP_CITY, MAP_SOOTOPOLIS_CITY, MAP_PACIFIDLOG_TOWN, MAP_EVER_GRANDE_CITY
    ]
    for city in hoenn_cities:
        system.register_fly_location(city)
    
    fly_locations = system.get_current_region_fly_locations()
    if len(fly_locations) != 16:
        print(f"  FAIL: Should have 16 Hoenn fly locations, got {len(fly_locations)}")
        failed += 1
    else:
        print("  PASS: Registered all 16 Hoenn fly locations")
        passed += 1

    # Step 2: Verify specific Hoenn locations
    print("Step 2: Verify specific Hoenn locations...")
    locations_correct = True
    if not system.is_fly_location_registered_for_region(MAP_LITTLEROOT_TOWN, REGION_HOENN):
        print("  FAIL: Littleroot Town should be registered")
        locations_correct = False
    if not system.is_fly_location_registered_for_region(MAP_RUSTBORO_CITY, REGION_HOENN):
        print("  FAIL: Rustboro City should be registered")
        locations_correct = False
    if not system.is_fly_location_registered_for_region(MAP_EVER_GRANDE_CITY, REGION_HOENN):
        print("  FAIL: Ever Grande City should be registered")
        locations_correct = False
    if locations_correct:
        print("  PASS: All specific Hoenn locations are registered")
        passed += 1
    else:
        failed += 1
    
    # Step 3: Verify max capacity enforcement
    print("Step 3: Verify max capacity enforcement (16 per region)...")
    system.register_fly_location(272)  # Extra map ID
    fly_count = system.get_fly_location_count_for_region(REGION_HOENN)
    if fly_count != 16:
        print(f"  FAIL: Should still have 16 locations (max), got {fly_count}")
        failed += 1
    else:
        print("  PASS: Max capacity enforced at 16 locations")
        passed += 1
    
    # Step 4: Verify Kanto is unaffected
    print("Step 4: Verify Kanto is unaffected...")
    kanto_count = system.get_fly_location_count_for_region(REGION_KANTO)
    if kanto_count != 0:
        print(f"  FAIL: Kanto should have 0 locations, got {kanto_count}")
        failed += 1
    else:
        print("  PASS: Kanto is unaffected (0 locations)")
        passed += 1
    
    print(f"\n--- Test 3 Results ---")
    print(f"Passed: {passed}, Failed: {failed}")
    return failed


def test_cross_region_fly_to_kanto():
    """
    Integration Test 4: Cross-Region Fly to Kanto Starting Location
    **Validates: Requirements 12.5**
    """
    print("\n=== Integration Test 4: Cross-Region Fly to Kanto Starting Location ===")
    print("**Validates: Requirements 12.5**\n")
    
    passed = 0
    failed = 0
    system = FlyLocationSystem()
    system.reset()
    system.set_region_unlocked(REGION_KANTO, True)
    system.set_region_unlocked(REGION_HOENN, True)
    
    # Register some locations in both regions
    system.set_current_region(REGION_KANTO)
    system.register_fly_location(MAP_PALLET_TOWN)
    system.register_fly_location(MAP_VIRIDIAN_CITY)
    system.register_fly_location(MAP_PEWTER_CITY)
    
    system.set_current_region(REGION_HOENN)
    system.register_fly_location(MAP_LITTLEROOT_TOWN)
    system.register_fly_location(MAP_OLDALE_TOWN)
    system.register_fly_location(MAP_PETALBURG_CITY)
    
    # Step 1: Get cross-region fly locations from Hoenn
    print("Step 1: Get cross-region fly locations from Hoenn...")
    cross_locations = system.get_cross_region_fly_locations()
    
    if len(cross_locations) != 1:
        print(f"  FAIL: Should have 1 cross-region location (Kanto), got {len(cross_locations)}")
        failed += 1
    else:
        print("  PASS: Have 1 cross-region location")
        passed += 1

    # Step 2: Verify cross-region location is Kanto starting location
    print("Step 2: Verify cross-region location is Kanto starting location...")
    kanto_start = system.get_region_starting_fly_location(REGION_KANTO)
    if len(cross_locations) > 0 and cross_locations[0] != kanto_start:
        print(f"  FAIL: Cross-region location should be {kanto_start} (Pallet Town), got {cross_locations[0]}")
        failed += 1
    elif len(cross_locations) > 0:
        print(f"  PASS: Cross-region location is Pallet Town ({kanto_start})")
        passed += 1
    
    # Step 3: Verify cross-region does NOT include other Kanto locations
    print("Step 3: Verify cross-region does NOT include other Kanto locations...")
    only_starting = True
    for loc in cross_locations:
        if loc != kanto_start:
            print(f"  FAIL: Cross-region should only have starting location, found {loc}")
            only_starting = False
    if only_starting:
        print("  PASS: Cross-region only includes starting location")
        passed += 1
    else:
        failed += 1
    
    # Step 4: Verify current region (Hoenn) is NOT in cross-region list
    print("Step 4: Verify current region (Hoenn) is NOT in cross-region list...")
    hoenn_start = system.get_region_starting_fly_location(REGION_HOENN)
    hoenn_excluded = True
    for loc in cross_locations:
        if loc == hoenn_start:
            print("  FAIL: Hoenn starting location should not be in cross-region list")
            hoenn_excluded = False
    if hoenn_excluded:
        print("  PASS: Hoenn is excluded from cross-region list")
        passed += 1
    else:
        failed += 1
    
    # Step 5: Test with Johto also unlocked
    print("Step 5: Test with Johto also unlocked...")
    system.set_region_unlocked(REGION_JOHTO, True)
    cross_locations = system.get_cross_region_fly_locations()
    
    if len(cross_locations) != 2:
        print(f"  FAIL: Should have 2 cross-region locations (Kanto, Johto), got {len(cross_locations)}")
        failed += 1
    else:
        print("  PASS: Have 2 cross-region locations")
        passed += 1
    
    # Step 6: Verify locked regions are excluded
    print("Step 6: Verify locked regions are excluded...")
    system.set_region_unlocked(REGION_JOHTO, False)
    cross_locations = system.get_cross_region_fly_locations()
    
    if len(cross_locations) != 1:
        print(f"  FAIL: Should have 1 cross-region location after locking Johto, got {len(cross_locations)}")
        failed += 1
    else:
        print("  PASS: Locked Johto is excluded from cross-region list")
        passed += 1
    
    print(f"\n--- Test 4 Results ---")
    print(f"Passed: {passed}, Failed: {failed}")
    return failed


def test_fly_location_save_load_persistence():
    """
    Integration Test 5: Save/Load Fly Location Persistence
    **Validates: Requirements 12.1, 12.2, 12.4, 12.5 (persistence aspect)**
    """
    print("\n=== Integration Test 5: Save/Load Fly Location Persistence ===")
    print("**Validates: Requirements 12.1, 12.2, 12.4, 12.5 (persistence)**\n")
    
    passed = 0
    failed = 0
    system = FlyLocationSystem()
    system.reset()
    system.set_region_unlocked(REGION_KANTO, True)
    system.set_region_unlocked(REGION_HOENN, True)
    
    # Register Kanto locations
    system.set_current_region(REGION_KANTO)
    system.register_fly_location(MAP_PALLET_TOWN)
    system.register_fly_location(MAP_VIRIDIAN_CITY)
    system.register_fly_location(MAP_PEWTER_CITY)
    system.register_fly_location(MAP_CERULEAN_CITY)
    system.register_fly_location(MAP_VERMILION_CITY)
    
    # Register Hoenn locations
    system.set_current_region(REGION_HOENN)
    system.register_fly_location(MAP_LITTLEROOT_TOWN)
    system.register_fly_location(MAP_OLDALE_TOWN)
    system.register_fly_location(MAP_PETALBURG_CITY)
    system.register_fly_location(MAP_RUSTBORO_CITY)
    system.register_fly_location(MAP_DEWFORD_TOWN)
    system.register_fly_location(MAP_SLATEPORT_CITY)
    
    # Step 1: Record state before save
    print("Step 1: Record state before save...")
    kanto_locations_before = system.get_fly_locations_for_region(REGION_KANTO)
    hoenn_locations_before = system.get_fly_locations_for_region(REGION_HOENN)
    kanto_count_before = len(kanto_locations_before)
    hoenn_count_before = len(hoenn_locations_before)
    
    print(f"  Kanto: {kanto_count_before} locations, Hoenn: {hoenn_count_before} locations")
    if kanto_count_before != 5 or hoenn_count_before != 6:
        print("  FAIL: Expected 5 Kanto and 6 Hoenn locations")
        failed += 1
    else:
        print("  PASS: Correct location counts before save")
        passed += 1
    
    # Step 2: Save fly location data
    print("Step 2: Save fly location data...")
    saved_state = system.save_state()
    print("  PASS: Fly location data saved")
    passed += 1
    
    # Step 3: Clear all fly location data (simulate game restart)
    print("Step 3: Clear all fly location data (simulate restart)...")
    system.reset()
    
    cleared_kanto = system.get_fly_location_count_for_region(REGION_KANTO)
    cleared_hoenn = system.get_fly_location_count_for_region(REGION_HOENN)
    if cleared_kanto != 0 or cleared_hoenn != 0:
        print(f"  FAIL: Data should be cleared, got Kanto={cleared_kanto}, Hoenn={cleared_hoenn}")
        failed += 1
    else:
        print("  PASS: Data cleared successfully")
        passed += 1

    # Step 4: Load fly location data
    print("Step 4: Load fly location data...")
    system.load_state(saved_state)
    print("  PASS: Fly location data loaded")
    passed += 1
    
    # Step 5: Verify location counts restored
    print("Step 5: Verify location counts restored...")
    kanto_locations_after = system.get_fly_locations_for_region(REGION_KANTO)
    hoenn_locations_after = system.get_fly_locations_for_region(REGION_HOENN)
    kanto_count_after = len(kanto_locations_after)
    hoenn_count_after = len(hoenn_locations_after)
    
    if kanto_count_after != kanto_count_before:
        print(f"  FAIL: Kanto count mismatch: before={kanto_count_before}, after={kanto_count_after}")
        failed += 1
    else:
        print(f"  PASS: Kanto count restored ({kanto_count_after} locations)")
        passed += 1
    
    if hoenn_count_after != hoenn_count_before:
        print(f"  FAIL: Hoenn count mismatch: before={hoenn_count_before}, after={hoenn_count_after}")
        failed += 1
    else:
        print(f"  PASS: Hoenn count restored ({hoenn_count_after} locations)")
        passed += 1
    
    # Step 6: Verify specific locations restored
    print("Step 6: Verify specific locations restored...")
    kanto_match = kanto_locations_before == kanto_locations_after
    if kanto_match:
        print("  PASS: All Kanto locations restored correctly")
        passed += 1
    else:
        print("  FAIL: Kanto locations mismatch")
        failed += 1
    
    hoenn_match = hoenn_locations_before == hoenn_locations_after
    if hoenn_match:
        print("  PASS: All Hoenn locations restored correctly")
        passed += 1
    else:
        print("  FAIL: Hoenn locations mismatch")
        failed += 1
    
    # Step 7: Verify region unlock state restored
    print("Step 7: Verify region unlock state restored...")
    if not system.is_region_unlocked(REGION_HOENN):
        print("  FAIL: Hoenn should be unlocked after load")
        failed += 1
    else:
        print("  PASS: Hoenn unlock state restored")
        passed += 1
    
    # Step 8: Verify current region restored
    print("Step 8: Verify current region restored...")
    if system.get_current_region() != REGION_HOENN:
        print(f"  FAIL: Current region should be Hoenn, got {system.get_current_region()}")
        failed += 1
    else:
        print("  PASS: Current region restored to Hoenn")
        passed += 1
    
    print(f"\n--- Test 5 Results ---")
    print(f"Passed: {passed}, Failed: {failed}")
    return failed


def test_full_cross_region_fly_workflow():
    """
    Integration Test 6: Full Cross-Region Fly Workflow
    **Validates: Requirements 12.1, 12.2, 12.4, 12.5**
    """
    print("\n=== Integration Test 6: Full Cross-Region Fly Workflow ===")
    print("**Validates: Requirements 12.1, 12.2, 12.4, 12.5**\n")
    
    passed = 0
    failed = 0
    system = FlyLocationSystem()
    system.reset()
    system.set_region_unlocked(REGION_KANTO, True)
    system.set_current_region(REGION_KANTO)
    
    # Step 1: Player explores Kanto, registers fly locations
    print("Step 1: Player explores Kanto, registers fly locations...")
    system.register_fly_location(MAP_PALLET_TOWN)
    system.register_fly_location(MAP_VIRIDIAN_CITY)
    system.register_fly_location(MAP_PEWTER_CITY)
    system.register_fly_location(MAP_CERULEAN_CITY)
    
    fly_locations = system.get_current_region_fly_locations()
    if len(fly_locations) != 4:
        print(f"  FAIL: Should have 4 Kanto locations, got {len(fly_locations)}")
        failed += 1
    else:
        print("  PASS: Registered 4 Kanto fly locations")
        passed += 1
    
    # Step 2: Player defeats Kanto E4, Hoenn unlocks
    print("Step 2: Player defeats Kanto E4, Hoenn unlocks...")
    system.set_region_unlocked(REGION_HOENN, True)
    
    cross_locations = system.get_cross_region_fly_locations()
    if len(cross_locations) != 1:
        print(f"  FAIL: Should have 1 cross-region location (Hoenn), got {len(cross_locations)}")
        failed += 1
    else:
        print("  PASS: Cross-region fly to Hoenn now available")
        passed += 1
    
    # Step 3: Player uses cross-region fly to Hoenn
    print("Step 3: Player uses cross-region fly to Hoenn...")
    system.set_current_region(REGION_HOENN)
    
    fly_locations = system.get_current_region_fly_locations()
    if len(fly_locations) != 0:
        print(f"  FAIL: Hoenn should have 0 fly locations initially, got {len(fly_locations)}")
        failed += 1
    else:
        print("  PASS: Hoenn fly menu is empty initially")
        passed += 1
    
    # Step 4: Player explores Hoenn, registers fly locations
    print("Step 4: Player explores Hoenn, registers fly locations...")
    system.register_fly_location(MAP_LITTLEROOT_TOWN)
    system.register_fly_location(MAP_OLDALE_TOWN)
    system.register_fly_location(MAP_PETALBURG_CITY)
    system.register_fly_location(MAP_RUSTBORO_CITY)
    system.register_fly_location(MAP_DEWFORD_TOWN)
    
    fly_locations = system.get_current_region_fly_locations()
    if len(fly_locations) != 5:
        print(f"  FAIL: Should have 5 Hoenn locations, got {len(fly_locations)}")
        failed += 1
    else:
        print("  PASS: Registered 5 Hoenn fly locations")
        passed += 1

    # Step 5: Verify cross-region fly back to Kanto available
    print("Step 5: Verify cross-region fly back to Kanto available...")
    cross_locations = system.get_cross_region_fly_locations()
    if len(cross_locations) != 1:
        print(f"  FAIL: Should have 1 cross-region location (Kanto), got {len(cross_locations)}")
        failed += 1
    else:
        print("  PASS: Cross-region fly to Kanto available")
        passed += 1
    
    kanto_start = system.get_region_starting_fly_location(REGION_KANTO)
    if cross_locations[0] != kanto_start:
        print(f"  FAIL: Cross-region should be Pallet Town ({kanto_start}), got {cross_locations[0]}")
        failed += 1
    else:
        print("  PASS: Cross-region destination is Pallet Town")
        passed += 1
    
    # Step 6: Player returns to Kanto
    print("Step 6: Player returns to Kanto...")
    system.set_current_region(REGION_KANTO)
    
    fly_locations = system.get_current_region_fly_locations()
    if len(fly_locations) != 4:
        print(f"  FAIL: Kanto should still have 4 locations, got {len(fly_locations)}")
        failed += 1
    else:
        print("  PASS: Kanto fly locations preserved (4 locations)")
        passed += 1
    
    # Step 7: Verify both regions have correct counts
    print("Step 7: Verify both regions have correct counts...")
    kanto_count = system.get_fly_location_count_for_region(REGION_KANTO)
    hoenn_count = system.get_fly_location_count_for_region(REGION_HOENN)
    
    if kanto_count != 4 or hoenn_count != 5:
        print(f"  FAIL: Expected Kanto=4, Hoenn=5, got Kanto={kanto_count}, Hoenn={hoenn_count}")
        failed += 1
    else:
        print("  PASS: Both regions have correct fly location counts")
        passed += 1
    
    print(f"\n--- Test 6 Results ---")
    print(f"Passed: {passed}, Failed: {failed}")
    return failed


def main():
    """Main test runner."""
    print("╔══════════════════════════════════════════════════════════════════╗")
    print("║     Integration Tests for Fly System Across Both Regions         ║")
    print("║     Task 4.6: Test fly system across both regions                ║")
    print("╚══════════════════════════════════════════════════════════════════╝")
    print("\nRegion configuration: Kanto → Hoenn → Johto (3 regions)")
    print("Sinnoh descoped per ADR-003")
    print("Progression order per ADR-004")
    print("\nFly location storage:")
    print("  - 16 locations per region")
    print("  - 3 regions total")
    print("  - 48 total fly locations maximum")
    print("\nTest Scenarios:")
    print("  1. Register Kanto locations → Verify only Kanto in menu")
    print("  2. Transition to Hoenn → Verify fly menu switches")
    print("  3. Register Hoenn locations → Verify added")
    print("  4. Cross-region fly to Kanto starting location")
    print("  5. Save/Load fly location persistence")
    print("  6. Full cross-region fly workflow")
    
    total_failed = 0
    total_failed += test_register_kanto_locations_only_kanto_in_menu()
    total_failed += test_transition_to_hoenn_fly_menu_switches()
    total_failed += test_register_hoenn_locations()
    total_failed += test_cross_region_fly_to_kanto()
    total_failed += test_fly_location_save_load_persistence()
    total_failed += test_full_cross_region_fly_workflow()

    # Overall results
    print("\n\n╔══════════════════════════════════════════════════════════════════╗")
    print("║                    OVERALL TEST RESULTS                          ║")
    print("╚══════════════════════════════════════════════════════════════════╝")
    
    if total_failed == 0:
        print("\n✓ ALL INTEGRATION TESTS PASSED!\n")
        print("Test 1 (Kanto Registration): PASSED")
        print("  - Validates: Requirements 12.1, 12.2, 12.4")
        print("Test 2 (Region Transition): PASSED")
        print("  - Validates: Requirements 12.4")
        print("Test 3 (Hoenn Registration): PASSED")
        print("  - Validates: Requirements 12.1, 12.2")
        print("Test 4 (Cross-Region Fly): PASSED")
        print("  - Validates: Requirements 12.5")
        print("Test 5 (Save/Load Persistence): PASSED")
        print("  - Validates: Requirements 12.1, 12.2, 12.4, 12.5")
        print("Test 6 (Full Workflow): PASSED")
        print("  - Validates: Requirements 12.1, 12.2, 12.4, 12.5")
        return 0
    else:
        print(f"\n✗ SOME INTEGRATION TESTS FAILED\n")
        print(f"Total failures: {total_failed}")
        return 1


if __name__ == "__main__":
    exit(main())
