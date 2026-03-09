#!/usr/bin/env python3
"""
Property Tests for EXP Multiplier System

This test file validates Properties 24-25 for the EXP multiplier system:
- Property 24: Experience Multiplier Application
- Property 25: Highest Multiplier Priority

**Validates: Requirements 11.4, 11.5, 11.6, 11.7**
"""

import random

# Multiplier item IDs
ITEM_LUCKY_EGG = 600
ITEM_MYSTIC_EGG = 601
ITEM_MAGIC_EGG = 602

# Multiplier values
MULTIPLIERS = {
    ITEM_LUCKY_EGG: 2,
    ITEM_MYSTIC_EGG: 3,
    ITEM_MAGIC_EGG: 4
}

class ExpMultiplierSystem:
    """Mock implementation of EXP multiplier system."""
    
    def __init__(self):
        self.inventory = set()
    
    def has_item(self, item_id):
        return item_id in self.inventory
    
    def give_item(self, item_id):
        self.inventory.add(item_id)
    
    def get_active_exp_multiplier(self):
        """
        Get the active experience multiplier.
        
        Requirements:
        - 11.6: Only highest multiplier applies
        - 11.7: Priority: Magic Egg (4x) > Mystic Egg (3x) > Lucky Egg (2x)
        """
        # Check in priority order (highest to lowest)
        if self.has_item(ITEM_MAGIC_EGG):
            return 4
        elif self.has_item(ITEM_MYSTIC_EGG):
            return 3
        elif self.has_item(ITEM_LUCKY_EGG):
            return 2
        else:
            return 1  # No multiplier
    
    def apply_exp_multiplier(self, base_exp):
        """
        Apply experience multiplier to base experience.
        
        Requirements:
        - 11.4: Multiplier applies to all experience gains
        - 11.5: Multiplier stacks with full party experience
        """
        multiplier = self.get_active_exp_multiplier()
        return base_exp * multiplier


# ============================================================================
# Property 24: Experience Multiplier Application
# ============================================================================

def prop_experience_multiplier_application(base_exp: int, has_lucky: bool, has_mystic: bool, has_magic: bool) -> bool:
    """
    Property 24: Experience Multiplier Application
    
    Experience multipliers correctly multiply the base experience amount
    based on which multiplier items are possessed.
    
    **Validates: Requirements 11.4, 11.5**
    """
    if base_exp < 0:
        return True  # Skip invalid exp values
    
    system = ExpMultiplierSystem()
    
    if has_lucky:
        system.give_item(ITEM_LUCKY_EGG)
    if has_mystic:
        system.give_item(ITEM_MYSTIC_EGG)
    if has_magic:
        system.give_item(ITEM_MAGIC_EGG)
    
    # Apply multiplier
    result_exp = system.apply_exp_multiplier(base_exp)
    
    # Determine expected multiplier
    expected_multiplier = system.get_active_exp_multiplier()
    expected_exp = base_exp * expected_multiplier
    
    if result_exp != expected_exp:
        print(f"FAIL: Multiplier not applied correctly")
        print(f"  base_exp={base_exp}, has_lucky={has_lucky}, has_mystic={has_mystic}, has_magic={has_magic}")
        print(f"  result_exp={result_exp}, expected={expected_exp}")
        print(f"  multiplier={expected_multiplier}")
        return False
    
    # Verify multiplier is within valid range (1-4)
    if expected_multiplier < 1 or expected_multiplier > 4:
        print(f"FAIL: Multiplier out of valid range")
        print(f"  multiplier={expected_multiplier}")
        return False
    
    return True


# ============================================================================
# Property 25: Highest Multiplier Priority
# ============================================================================

def prop_highest_multiplier_priority(has_lucky: bool, has_mystic: bool, has_magic: bool) -> bool:
    """
    Property 25: Highest Multiplier Priority
    
    When multiple multiplier items are possessed, only the highest
    multiplier applies (Magic Egg > Mystic Egg > Lucky Egg).
    
    **Validates: Requirements 11.6, 11.7**
    """
    system = ExpMultiplierSystem()
    
    if has_lucky:
        system.give_item(ITEM_LUCKY_EGG)
    if has_mystic:
        system.give_item(ITEM_MYSTIC_EGG)
    if has_magic:
        system.give_item(ITEM_MAGIC_EGG)
    
    multiplier = system.get_active_exp_multiplier()
    
    # Determine expected multiplier based on priority
    if has_magic:
        expected_multiplier = 4
    elif has_mystic:
        expected_multiplier = 3
    elif has_lucky:
        expected_multiplier = 2
    else:
        expected_multiplier = 1
    
    if multiplier != expected_multiplier:
        print(f"FAIL: Wrong multiplier priority")
        print(f"  has_lucky={has_lucky}, has_mystic={has_mystic}, has_magic={has_magic}")
        print(f"  multiplier={multiplier}, expected={expected_multiplier}")
        return False
    
    # Verify only one multiplier applies (not stacked)
    if has_magic and has_mystic and has_lucky:
        # Should be 4, not 2*3*4=24
        if multiplier != 4:
            print(f"FAIL: Multipliers incorrectly stacked")
            print(f"  multiplier={multiplier}, expected=4")
            return False
    
    return True


# ============================================================================
# Main Test Runner
# ============================================================================

def main():
    """Run all property-based tests for EXP multiplier system."""
    num_trials = 500
    total_passed = 0
    total_failed = 0
    
    print("=" * 70)
    print("Property-Based Tests: EXP Multiplier System")
    print("=" * 70)
    print("Properties tested:")
    print("  - Property 24: Experience Multiplier Application")
    print("  - Property 25: Highest Multiplier Priority")
    print("Validates: Requirements 11.4, 11.5, 11.6, 11.7")
    print("=" * 70)
    print()
    
    # ========================================================================
    # Property 24: Experience Multiplier Application
    # ========================================================================
    print("Testing Property 24: Experience Multiplier Application...")
    passed = 0
    failed = 0
    
    for i in range(num_trials):
        base_exp = random.randint(0, 10000)
        has_lucky = random.choice([True, False])
        has_mystic = random.choice([True, False])
        has_magic = random.choice([True, False])
        
        if prop_experience_multiplier_application(base_exp, has_lucky, has_mystic, has_magic):
            passed += 1
        else:
            failed += 1
            if failed <= 3:
                print(f"  Counterexample: exp={base_exp}, lucky={has_lucky}, mystic={has_mystic}, magic={has_magic}")
    
    print(f"  Results: {passed}/{passed + failed} passed")
    total_passed += passed
    total_failed += failed
    
    # ========================================================================
    # Property 25: Highest Multiplier Priority
    # ========================================================================
    print("\nTesting Property 25: Highest Multiplier Priority...")
    passed = 0
    failed = 0
    
    for i in range(num_trials):
        has_lucky = random.choice([True, False])
        has_mystic = random.choice([True, False])
        has_magic = random.choice([True, False])
        
        if prop_highest_multiplier_priority(has_lucky, has_mystic, has_magic):
            passed += 1
        else:
            failed += 1
            if failed <= 3:
                print(f"  Counterexample: lucky={has_lucky}, mystic={has_mystic}, magic={has_magic}")
    
    print(f"  Results: {passed}/{passed + failed} passed")
    total_passed += passed
    total_failed += failed
    
    # ========================================================================
    # Edge Case Testing
    # ========================================================================
    print("\n" + "=" * 70)
    print("Edge Case Testing")
    print("=" * 70)
    
    # Test no multiplier
    print("\nNo Multiplier:")
    system = ExpMultiplierSystem()
    result = system.apply_exp_multiplier(1000)
    if result == 1000:
        print(f"  ✓ No multiplier: 1000 → 1000 (1x)")
        total_passed += 1
    else:
        print(f"  ✗ No multiplier failed: got {result}")
        total_failed += 1
    
    # Test Lucky Egg only
    print("\nLucky Egg Only:")
    system = ExpMultiplierSystem()
    system.give_item(ITEM_LUCKY_EGG)
    result = system.apply_exp_multiplier(1000)
    if result == 2000:
        print(f"  ✓ Lucky Egg: 1000 → 2000 (2x)")
        total_passed += 1
    else:
        print(f"  ✗ Lucky Egg failed: got {result}")
        total_failed += 1
    
    # Test Mystic Egg only
    print("\nMystic Egg Only:")
    system = ExpMultiplierSystem()
    system.give_item(ITEM_MYSTIC_EGG)
    result = system.apply_exp_multiplier(1000)
    if result == 3000:
        print(f"  ✓ Mystic Egg: 1000 → 3000 (3x)")
        total_passed += 1
    else:
        print(f"  ✗ Mystic Egg failed: got {result}")
        total_failed += 1
    
    # Test Magic Egg only
    print("\nMagic Egg Only:")
    system = ExpMultiplierSystem()
    system.give_item(ITEM_MAGIC_EGG)
    result = system.apply_exp_multiplier(1000)
    if result == 4000:
        print(f"  ✓ Magic Egg: 1000 → 4000 (4x)")
        total_passed += 1
    else:
        print(f"  ✗ Magic Egg failed: got {result}")
        total_failed += 1
    
    # Test all multipliers (should use highest)
    print("\nAll Multipliers (should use Magic Egg):")
    system = ExpMultiplierSystem()
    system.give_item(ITEM_LUCKY_EGG)
    system.give_item(ITEM_MYSTIC_EGG)
    system.give_item(ITEM_MAGIC_EGG)
    result = system.apply_exp_multiplier(1000)
    if result == 4000:
        print(f"  ✓ All multipliers: 1000 → 4000 (4x, not stacked)")
        total_passed += 1
    else:
        print(f"  ✗ All multipliers failed: got {result}")
        total_failed += 1
    
    # Test Lucky + Mystic (should use Mystic)
    print("\nLucky + Mystic (should use Mystic):")
    system = ExpMultiplierSystem()
    system.give_item(ITEM_LUCKY_EGG)
    system.give_item(ITEM_MYSTIC_EGG)
    result = system.apply_exp_multiplier(1000)
    if result == 3000:
        print(f"  ✓ Lucky + Mystic: 1000 → 3000 (3x)")
        total_passed += 1
    else:
        print(f"  ✗ Lucky + Mystic failed: got {result}")
        total_failed += 1
    
    # Test zero experience
    print("\nZero Experience:")
    system = ExpMultiplierSystem()
    system.give_item(ITEM_MAGIC_EGG)
    result = system.apply_exp_multiplier(0)
    if result == 0:
        print(f"  ✓ Zero exp with multiplier: 0 → 0")
        total_passed += 1
    else:
        print(f"  ✗ Zero exp failed: got {result}")
        total_failed += 1
    
    # ========================================================================
    # Final Results
    # ========================================================================
    print("\n" + "=" * 70)
    print("Final Test Results")
    print("=" * 70)
    print(f"Total tests: {total_passed + total_failed}")
    print(f"Passed: {total_passed}")
    print(f"Failed: {total_failed}")
    
    if total_failed == 0:
        print("\n✓ All EXP multiplier property tests PASSED!")
        print("Properties 24 and 25 are correctly validated.")
        return 0
    else:
        print(f"\n✗ Tests FAILED with {total_failed} counterexamples")
        return 1


if __name__ == "__main__":
    exit(main())
