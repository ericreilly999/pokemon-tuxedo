#!/usr/bin/env python3
"""
Property Tests for Region Management System

This test file validates Properties 14-15 for the region management system:
- Property 14: Region Ticket Reward
- Property 15: Region Travel Restriction

**Validates: Requirements 1.3, 1.4**
"""

import random

# Region constants
REGION_KANTO = 0
REGION_JOHTO = 1
REGION_HOENN = 2
REGION_SINNOH = 3

# Ticket item IDs
ITEM_JOHTO_TICKET = 500
ITEM_HOENN_TICKET = 501
ITEM_SINNOH_TICKET = 502


class RegionManagementSystem:
    """Mock implementation of region management system."""
    
    def __init__(self):
        self.current_region = REGION_KANTO
        self.elite_four_defeated = {
            REGION_KANTO: False,
            REGION_JOHTO: False,
            REGION_HOENN: False,
            REGION_SINNOH: False
        }
        self.inventory = set()
    
    def get_current_region(self):
        return self.current_region
    
    def set_current_region(self, region_id):
        if 0 <= region_id <= 3:
            self.current_region = region_id
    
    def is_elite_four_defeated(self, region_id=None):
        if region_id is None:
            region_id = self.current_region
        return self.elite_four_defeated.get(region_id, False)
    
    def set_elite_four_defeated(self, region_id=None):
        if region_id is None:
            region_id = self.current_region
        self.elite_four_defeated[region_id] = True
    
    def has_item(self, item_id):
        return item_id in self.inventory
    
    def give_item(self, item_id):
        self.inventory.add(item_id)
    
    def get_region_ticket_item(self, region_id):
        """Returns the ticket item ID required for a region."""
        ticket_map = {
            REGION_JOHTO: ITEM_JOHTO_TICKET,
            REGION_HOENN: ITEM_HOENN_TICKET,
            REGION_SINNOH: ITEM_SINNOH_TICKET
        }
        return ticket_map.get(region_id)
    
    def can_travel_to_region(self, target_region):
        """Validates if player can travel to target region."""
        if target_region == self.current_region:
            return True
        
        if target_region == REGION_KANTO:
            return True
        
        if not self.is_elite_four_defeated(self.current_region):
            return False
        
        ticket_item = self.get_region_ticket_item(target_region)
        if ticket_item is None:
            return False
        
        return self.has_item(ticket_item)
    
    def award_region_ticket(self):
        """Awards the next region ticket after Elite Four defeat."""
        if not self.is_elite_four_defeated(self.current_region):
            return None
        
        next_ticket_map = {
            REGION_KANTO: ITEM_JOHTO_TICKET,
            REGION_JOHTO: ITEM_HOENN_TICKET,
            REGION_HOENN: ITEM_SINNOH_TICKET,
            REGION_SINNOH: None
        }
        
        ticket = next_ticket_map.get(self.current_region)
        if ticket is not None:
            self.give_item(ticket)
        
        return ticket


# ============================================================================
# Property 14: Region Ticket Reward
# ============================================================================

def prop_region_ticket_reward(current_region: int, elite_four_defeated: bool) -> bool:
    """
    Property 14: Region Ticket Reward
    
    When Elite Four is defeated in a region, the player receives a ticket
    to the next region in progression order (Kanto→Johto→Hoenn→Sinnoh).
    
    **Validates: Requirements 1.3, 1.4**
    """
    system = RegionManagementSystem()
    system.set_current_region(current_region)
    
    if elite_four_defeated:
        system.set_elite_four_defeated(current_region)
    
    awarded_ticket = system.award_region_ticket()
    
    if not elite_four_defeated:
        if awarded_ticket is not None:
            print(f"FAIL: Ticket awarded without Elite Four defeat")
            print(f"  current_region={current_region}, elite_four_defeated={elite_four_defeated}")
            print(f"  awarded_ticket={awarded_ticket}")
            return False
    else:
        expected_tickets = {
            REGION_KANTO: ITEM_JOHTO_TICKET,
            REGION_JOHTO: ITEM_HOENN_TICKET,
            REGION_HOENN: ITEM_SINNOH_TICKET,
            REGION_SINNOH: None
        }
        
        expected_ticket = expected_tickets[current_region]
        
        if awarded_ticket != expected_ticket:
            print(f"FAIL: Wrong ticket awarded")
            print(f"  current_region={current_region}")
            print(f"  awarded_ticket={awarded_ticket}, expected={expected_ticket}")
            return False
        
        if expected_ticket is not None and not system.has_item(expected_ticket):
            print(f"FAIL: Ticket not in inventory")
            print(f"  current_region={current_region}")
            print(f"  expected_ticket={expected_ticket}")
            return False
    
    return True


# ============================================================================
# Property 15: Region Travel Restriction
# ============================================================================

def prop_region_travel_restriction(
    current_region: int, target_region: int, elite_four_defeated: bool, has_ticket: bool
) -> bool:
    """
    Property 15: Region Travel Restriction
    
    Travel to a new region requires:
    1. Elite Four defeat in current region
    2. Possession of the target region's ticket
    
    Exception: Kanto is always accessible (starting region)
    
    **Validates: Requirements 1.3, 1.4**
    """
    system = RegionManagementSystem()
    system.set_current_region(current_region)
    
    if elite_four_defeated:
        system.set_elite_four_defeated(current_region)
    
    if has_ticket and target_region != REGION_KANTO:
        ticket = system.get_region_ticket_item(target_region)
        if ticket is not None:
            system.give_item(ticket)
    
    can_travel = system.can_travel_to_region(target_region)
    
    # Verify travel rules
    if target_region == current_region:
        if not can_travel:
            print(f"FAIL: Cannot stay in current region")
            print(f"  current_region={current_region}, target_region={target_region}")
            return False
    elif target_region == REGION_KANTO:
        if not can_travel:
            print(f"FAIL: Cannot travel to Kanto (always accessible)")
            print(f"  current_region={current_region}")
            return False
    else:
        required_ticket = system.get_region_ticket_item(target_region)
        
        if elite_four_defeated and has_ticket:
            if not can_travel:
                print(f"FAIL: Cannot travel with Elite Four defeat + ticket")
                print(f"  current_region={current_region}, target_region={target_region}")
                print(f"  elite_four_defeated={elite_four_defeated}, has_ticket={has_ticket}")
                return False
        else:
            if can_travel:
                print(f"FAIL: Can travel without Elite Four defeat or ticket")
                print(f"  current_region={current_region}, target_region={target_region}")
                print(f"  elite_four_defeated={elite_four_defeated}, has_ticket={has_ticket}")
                return False
    
    return True


# ============================================================================
# Main Test Runner
# ============================================================================

def main():
    """Run all property-based tests for region management system."""
    num_trials = 500
    total_passed = 0
    total_failed = 0
    
    print("=" * 70)
    print("Property-Based Tests: Region Management System")
    print("=" * 70)
    print("Properties tested:")
    print("  - Property 14: Region Ticket Reward")
    print("  - Property 15: Region Travel Restriction")
    print("Validates: Requirements 1.3, 1.4")
    print("=" * 70)
    print()
    
    # ========================================================================
    # Property 14: Region Ticket Reward
    # ========================================================================
    print("Testing Property 14: Region Ticket Reward...")
    passed = 0
    failed = 0
    
    for i in range(num_trials):
        current_region = random.randint(0, 3)
        elite_four_defeated = random.choice([True, False])
        
        if prop_region_ticket_reward(current_region, elite_four_defeated):
            passed += 1
        else:
            failed += 1
            if failed <= 3:
                print(f"  Counterexample: region={current_region}, e4_defeated={elite_four_defeated}")
    
    print(f"  Results: {passed}/{passed + failed} passed")
    total_passed += passed
    total_failed += failed
    
    # ========================================================================
    # Property 15: Region Travel Restriction
    # ========================================================================
    print("\nTesting Property 15: Region Travel Restriction...")
    passed = 0
    failed = 0
    
    for i in range(num_trials):
        current_region = random.randint(0, 3)
        target_region = random.randint(0, 3)
        elite_four_defeated = random.choice([True, False])
        has_ticket = random.choice([True, False])
        
        if prop_region_travel_restriction(current_region, target_region, elite_four_defeated, has_ticket):
            passed += 1
        else:
            failed += 1
            if failed <= 3:
                print(f"  Counterexample: current={current_region}, target={target_region}, e4={elite_four_defeated}, ticket={has_ticket}")
    
    print(f"  Results: {passed}/{passed + failed} passed")
    total_passed += passed
    total_failed += failed
    
    # ========================================================================
    # Edge Case Testing
    # ========================================================================
    print("\n" + "=" * 70)
    print("Edge Case Testing")
    print("=" * 70)
    
    # Test region progression order
    print("\nRegion Progression Order:")
    system = RegionManagementSystem()
    
    print(f"  Starting region: {system.get_current_region()} (Kanto)")
    
    system.set_elite_four_defeated(REGION_KANTO)
    ticket = system.award_region_ticket()
    print(f"  Kanto E4 defeated → Ticket: {ticket} (Johto)")
    if ticket == ITEM_JOHTO_TICKET and system.can_travel_to_region(REGION_JOHTO):
        print(f"    ✓ Can travel to Johto")
        total_passed += 1
    else:
        print(f"    ✗ Cannot travel to Johto")
        total_failed += 1
    
    system.set_current_region(REGION_JOHTO)
    system.set_elite_four_defeated(REGION_JOHTO)
    ticket = system.award_region_ticket()
    print(f"  Johto E4 defeated → Ticket: {ticket} (Hoenn)")
    if ticket == ITEM_HOENN_TICKET and system.can_travel_to_region(REGION_HOENN):
        print(f"    ✓ Can travel to Hoenn")
        total_passed += 1
    else:
        print(f"    ✗ Cannot travel to Hoenn")
        total_failed += 1
    
    system.set_current_region(REGION_HOENN)
    system.set_elite_four_defeated(REGION_HOENN)
    ticket = system.award_region_ticket()
    print(f"  Hoenn E4 defeated → Ticket: {ticket} (Sinnoh)")
    if ticket == ITEM_SINNOH_TICKET and system.can_travel_to_region(REGION_SINNOH):
        print(f"    ✓ Can travel to Sinnoh")
        total_passed += 1
    else:
        print(f"    ✗ Cannot travel to Sinnoh")
        total_failed += 1
    
    system.set_current_region(REGION_SINNOH)
    system.set_elite_four_defeated(REGION_SINNOH)
    ticket = system.award_region_ticket()
    print(f"  Sinnoh E4 defeated → Ticket: {ticket} (None)")
    if ticket is None:
        print(f"    ✓ No more regions")
        total_passed += 1
    else:
        print(f"    ✗ Unexpected ticket awarded")
        total_failed += 1
    
    # Test cannot skip regions
    print("\nCannot Skip Regions:")
    system = RegionManagementSystem()
    
    if not system.can_travel_to_region(REGION_HOENN):
        print(f"  ✓ Cannot skip to Hoenn from Kanto without progression")
        total_passed += 1
    else:
        print(f"  ✗ Can skip to Hoenn (should not be possible)")
        total_failed += 1
    
    system.set_elite_four_defeated(REGION_KANTO)
    if not system.can_travel_to_region(REGION_HOENN):
        print(f"  ✓ Cannot skip to Hoenn even with E4 defeat (need ticket)")
        total_passed += 1
    else:
        print(f"  ✗ Can skip to Hoenn with E4 defeat (should need ticket)")
        total_failed += 1
    
    system.give_item(ITEM_JOHTO_TICKET)
    if not system.can_travel_to_region(REGION_HOENN):
        print(f"  ✓ Cannot skip to Hoenn with wrong ticket")
        total_passed += 1
    else:
        print(f"  ✗ Can skip to Hoenn with wrong ticket")
        total_failed += 1
    
    system.give_item(ITEM_HOENN_TICKET)
    if system.can_travel_to_region(REGION_HOENN):
        print(f"  ✓ Can travel to Hoenn with correct ticket")
        total_passed += 1
    else:
        print(f"  ✗ Cannot travel to Hoenn with correct ticket")
        total_failed += 1
    
    # Test Kanto always accessible
    print("\nKanto Always Accessible:")
    for region in [REGION_KANTO, REGION_JOHTO, REGION_HOENN, REGION_SINNOH]:
        system = RegionManagementSystem()
        system.set_current_region(region)
        if system.can_travel_to_region(REGION_KANTO):
            print(f"  ✓ Can travel to Kanto from region {region}")
            total_passed += 1
        else:
            print(f"  ✗ Cannot travel to Kanto from region {region}")
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
        print("\n✓ All region management property tests PASSED!")
        print("Properties 14 and 15 are correctly validated.")
        return 0
    else:
        print(f"\n✗ Tests FAILED with {total_failed} counterexamples")
        return 1


if __name__ == "__main__":
    exit(main())
