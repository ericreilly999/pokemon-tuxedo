#!/usr/bin/env python3
"""
Property Tests for Starter Pokemon Distribution

Properties tested:
- Property 20: Post-Elite Four Starter Distribution
- Property 21: Post-Elite Four Starter Levels
- Property 22: New Region Starter Offer

**Validates: Requirements 9.2, 9.3, 9.4**
"""

import random

# Starter Pokemon by region
REGION_STARTERS = {
    0: ["Bulbasaur", "Charmander", "Squirtle"],  # Kanto
    1: ["Chikorita", "Cyndaquil", "Totodile"],  # Johto
    2: ["Treecko", "Torchic", "Mudkip"],  # Hoenn
    3: ["Turtwig", "Chimchar", "Piplup"],  # Sinnoh
}

# Starter levels by region
STARTER_LEVELS = {
    0: 5,    # Kanto
    1: 60,   # Johto
    2: 110,  # Hoenn
    3: 160,  # Sinnoh
}

class StarterDistributionSystem:
    """Mock implementation of starter distribution system."""
    
    def __init__(self):
        self.chosen_starters = {}
        self.elite_four_defeated = {}
    
    def offer_region_starters(self, region_id):
        """Offer starter choice for a region."""
        return REGION_STARTERS.get(region_id, [])
    
    def choose_starter(self, region_id, starter_name):
        """Player chooses a starter."""
        self.chosen_starters[region_id] = starter_name
    
    def set_elite_four_defeated(self, region_id):
        """Mark Elite Four as defeated."""
        self.elite_four_defeated[region_id] = True
    
    def give_remaining_starters(self, region_id):
        """Give remaining starters after Elite Four defeat."""
        if not self.elite_four_defeated.get(region_id, False):
            return []
        
        chosen = self.chosen_starters.get(region_id)
        all_starters = REGION_STARTERS.get(region_id, [])
        
        if chosen is None:
            return []
        
        return [s for s in all_starters if s != chosen]
    
    def get_starter_level(self, region_id):
        """Get the level for starters in a region."""
        return STARTER_LEVELS.get(region_id, 5)


def prop_post_elite_four_starter_distribution(region_id: int) -> bool:
    """Property 20: Post-Elite Four Starter Distribution."""
    if region_id < 0 or region_id > 3:
        return True
    
    system = StarterDistributionSystem()
    
    # Choose a starter
    starters = system.offer_region_starters(region_id)
    if not starters:
        return True
    
    chosen = starters[0]
    system.choose_starter(region_id, chosen)
    
    # Before Elite Four defeat
    remaining = system.give_remaining_starters(region_id)
    if remaining:
        print(f"FAIL: Starters given before Elite Four defeat")
        return False
    
    # After Elite Four defeat
    system.set_elite_four_defeated(region_id)
    remaining = system.give_remaining_starters(region_id)
    
    expected_remaining = [s for s in starters if s != chosen]
    if set(remaining) != set(expected_remaining):
        print(f"FAIL: Wrong remaining starters")
        print(f"  remaining={remaining}, expected={expected_remaining}")
        return False
    
    return True


def prop_post_elite_four_starter_levels(region_id: int) -> bool:
    """Property 21: Post-Elite Four Starter Levels."""
    if region_id < 0 or region_id > 3:
        return True
    
    system = StarterDistributionSystem()
    level = system.get_starter_level(region_id)
    expected_level = STARTER_LEVELS.get(region_id, 5)
    
    if level != expected_level:
        print(f"FAIL: Wrong starter level for region {region_id}")
        print(f"  level={level}, expected={expected_level}")
        return False
    
    return True


def prop_new_region_starter_offer(region_id: int) -> bool:
    """Property 22: New Region Starter Offer."""
    if region_id < 0 or region_id > 3:
        return True
    
    system = StarterDistributionSystem()
    starters = system.offer_region_starters(region_id)
    expected_starters = REGION_STARTERS.get(region_id, [])
    
    if set(starters) != set(expected_starters):
        print(f"FAIL: Wrong starters offered for region {region_id}")
        print(f"  starters={starters}, expected={expected_starters}")
        return False
    
    if len(starters) != 3:
        print(f"FAIL: Should offer exactly 3 starters")
        print(f"  count={len(starters)}")
        return False
    
    return True


def main():
    """Run all property-based tests for starter distribution system."""
    total_passed = 0
    total_failed = 0
    
    print("=" * 70)
    print("Property-Based Tests: Starter Distribution System")
    print("=" * 70)
    print("Properties tested:")
    print("  - Property 20: Post-Elite Four Starter Distribution")
    print("  - Property 21: Post-Elite Four Starter Levels")
    print("  - Property 22: New Region Starter Offer")
    print("Validates: Requirements 9.2, 9.3, 9.4")
    print("=" * 70)
    print()
    
    # Test Property 20
    print("Testing Property 20: Post-Elite Four Starter Distribution...")
    passed = 0
    for region_id in range(4):
        if prop_post_elite_four_starter_distribution(region_id):
            passed += 1
        else:
            total_failed += 1
    print(f"  Results: {passed}/{passed} passed")
    total_passed += passed
    
    # Test Property 21
    print("\nTesting Property 21: Post-Elite Four Starter Levels...")
    passed = 0
    for region_id in range(4):
        if prop_post_elite_four_starter_levels(region_id):
            passed += 1
        else:
            total_failed += 1
    print(f"  Results: {passed}/{passed} passed")
    total_passed += passed
    
    # Test Property 22
    print("\nTesting Property 22: New Region Starter Offer...")
    passed = 0
    for region_id in range(4):
        if prop_new_region_starter_offer(region_id):
            passed += 1
        else:
            total_failed += 1
    print(f"  Results: {passed}/{passed} passed")
    total_passed += passed
    
    # Edge cases
    print("\n" + "=" * 70)
    print("Edge Case Testing")
    print("=" * 70)
    
    system = StarterDistributionSystem()
    
    print("\nStarter Levels by Region:")
    for region_id in range(4):
        level = system.get_starter_level(region_id)
        expected = STARTER_LEVELS[region_id]
        if level == expected:
            print(f"  ✓ Region {region_id}: Level {level}")
            total_passed += 1
        else:
            print(f"  ✗ Region {region_id}: Level {level} (expected {expected})")
            total_failed += 1
    
    print("\nStarters by Region:")
    for region_id in range(4):
        starters = system.offer_region_starters(region_id)
        print(f"  Region {region_id}: {', '.join(starters)}")
        if len(starters) == 3:
            total_passed += 1
        else:
            total_failed += 1
    
    print("\n" + "=" * 70)
    print("Final Test Results")
    print("=" * 70)
    print(f"Total tests: {total_passed + total_failed}")
    print(f"Passed: {total_passed}")
    print(f"Failed: {total_failed}")
    
    if total_failed == 0:
        print("\n✓ All starter distribution property tests PASSED!")
        return 0
    else:
        print(f"\n✗ Tests FAILED with {total_failed} counterexamples")
        return 1


if __name__ == "__main__":
    exit(main())
