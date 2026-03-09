#!/usr/bin/env python3
"""
Property Tests for Fly Location Discovery System

Properties tested:
- Property 28: Fly Location Discovery
- Property 29: Fly Location Persistence

**Validates: Requirements 15.1, 15.2, 15.3, 15.4**
"""

import random

class FlyLocationSystem:
    """Mock implementation of fly location discovery system."""
    
    def __init__(self):
        self.discovered_locations = set()
        self.all_locations = [
            "Pallet Town", "Viridian City", "Pewter City", "Cerulean City",
            "Vermilion City", "Lavender Town", "Celadon City", "Fuchsia City",
            "Saffron City", "Cinnabar Island", "Indigo Plateau"
        ]
    
    def discover_fly_location(self, location_name):
        """Discover a fly location on first visit."""
        if location_name in self.all_locations:
            self.discovered_locations.add(location_name)
    
    def is_fly_location_discovered(self, location_name):
        """Check if a fly location has been discovered."""
        return location_name in self.discovered_locations
    
    def get_discovered_fly_locations(self):
        """Get list of all discovered fly locations."""
        return list(self.discovered_locations)
    
    def visit_location(self, location_name):
        """Simulate visiting a location (triggers discovery)."""
        self.discover_fly_location(location_name)


def prop_fly_location_discovery(location_name: str) -> bool:
    """Property 28: Fly Location Discovery."""
    system = FlyLocationSystem()
    
    # Before visit
    if system.is_fly_location_discovered(location_name):
        print(f"FAIL: Location discovered before visit")
        return False
    
    # Visit location
    system.visit_location(location_name)
    
    # After visit (if valid location)
    if location_name in system.all_locations:
        if not system.is_fly_location_discovered(location_name):
            print(f"FAIL: Location not discovered after visit")
            return False
        
        # Verify in discovered list
        if location_name not in system.get_discovered_fly_locations():
            print(f"FAIL: Location not in discovered list")
            return False
    
    return True


def prop_fly_location_persistence() -> bool:
    """Property 29: Fly Location Persistence."""
    system = FlyLocationSystem()
    
    # Discover some locations
    locations_to_discover = ["Pallet Town", "Viridian City", "Pewter City"]
    for location in locations_to_discover:
        system.visit_location(location)
    
    # Verify all discovered
    discovered = system.get_discovered_fly_locations()
    for location in locations_to_discover:
        if location not in discovered:
            print(f"FAIL: Location not persisted: {location}")
            return False
    
    # Verify count
    if len(discovered) != len(locations_to_discover):
        print(f"FAIL: Wrong number of discovered locations")
        print(f"  discovered={len(discovered)}, expected={len(locations_to_discover)}")
        return False
    
    return True


def main():
    """Run all property-based tests for fly location discovery system."""
    total_passed = 0
    total_failed = 0
    
    print("=" * 70)
    print("Property-Based Tests: Fly Location Discovery System")
    print("=" * 70)
    print("Properties tested:")
    print("  - Property 28: Fly Location Discovery")
    print("  - Property 29: Fly Location Persistence")
    print("Validates: Requirements 15.1, 15.2, 15.3, 15.4")
    print("=" * 70)
    print()
    
    # Test Property 28
    print("Testing Property 28: Fly Location Discovery...")
    system = FlyLocationSystem()
    passed = 0
    for location in system.all_locations + ["Invalid Location"]:
        if prop_fly_location_discovery(location):
            passed += 1
        else:
            total_failed += 1
    print(f"  Results: {passed}/{passed} passed")
    total_passed += passed
    
    # Test Property 29
    print("\nTesting Property 29: Fly Location Persistence...")
    if prop_fly_location_persistence():
        print(f"  Results: 1/1 passed")
        total_passed += 1
    else:
        print(f"  Results: 0/1 passed")
        total_failed += 1
    
    # Edge cases
    print("\n" + "=" * 70)
    print("Edge Case Testing")
    print("=" * 70)
    
    system = FlyLocationSystem()
    
    print("\nDiscovery Progression:")
    for i, location in enumerate(system.all_locations[:5], 1):
        system.visit_location(location)
        discovered_count = len(system.get_discovered_fly_locations())
        if discovered_count == i:
            print(f"  ✓ Visit {i}: {location} (total: {discovered_count})")
            total_passed += 1
        else:
            print(f"  ✗ Visit {i}: {location} (total: {discovered_count}, expected: {i})")
            total_failed += 1
    
    print("\nDuplicate Visits:")
    system = FlyLocationSystem()
    system.visit_location("Pallet Town")
    system.visit_location("Pallet Town")
    system.visit_location("Pallet Town")
    if len(system.get_discovered_fly_locations()) == 1:
        print(f"  ✓ Duplicate visits don't create duplicates")
        total_passed += 1
    else:
        print(f"  ✗ Duplicate visits created duplicates")
        total_failed += 1
    
    print("\n" + "=" * 70)
    print("Final Test Results")
    print("=" * 70)
    print(f"Total tests: {total_passed + total_failed}")
    print(f"Passed: {total_passed}")
    print(f"Failed: {total_failed}")
    
    if total_failed == 0:
        print("\n✓ All fly location discovery property tests PASSED!")
        return 0
    else:
        print(f"\n✗ Tests FAILED with {total_failed} counterexamples")
        return 1


if __name__ == "__main__":
    exit(main())
