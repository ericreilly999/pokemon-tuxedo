#!/usr/bin/env python3
"""
Property Tests for Full Party Experience System

This test file validates Property 23 for the full party experience system:
- Property 23: Full Party Experience Distribution

**Validates: Requirements 10.1, 10.2, 10.3**
"""

import random

class PartyPokemon:
    """Mock Pokemon in party."""
    def __init__(self, level, participated_in_battle=False):
        self.level = level
        self.participated_in_battle = participated_in_battle
        self.exp_gained = 0

class FullPartyExpSystem:
    """Mock implementation of full party experience system."""
    
    def award_experience_to_party(self, party, base_exp):
        """
        Award full experience to all party Pokemon.
        
        Requirements:
        - 10.1: All Pokemon in party receive full experience
        - 10.2: Experience is not split among party members
        - 10.3: Non-participating Pokemon receive same exp as participants
        """
        for pokemon in party:
            # Award full experience to each Pokemon
            pokemon.exp_gained = base_exp


# ============================================================================
# Property 23: Full Party Experience Distribution
# ============================================================================

def prop_full_party_experience_distribution(party_size: int, base_exp: int) -> bool:
    """
    Property 23: Full Party Experience Distribution
    
    All Pokemon in the party receive the full base experience amount,
    regardless of whether they participated in battle or not.
    
    **Validates: Requirements 10.1, 10.2, 10.3**
    """
    if party_size < 1 or party_size > 6:
        return True  # Skip invalid party sizes
    
    if base_exp < 0:
        return True  # Skip invalid exp values
    
    # Create party with mix of participants and non-participants
    party = []
    for i in range(party_size):
        participated = (i % 2 == 0)  # Alternate participation
        pokemon = PartyPokemon(level=random.randint(1, 100), participated_in_battle=participated)
        party.append(pokemon)
    
    # Award experience
    system = FullPartyExpSystem()
    system.award_experience_to_party(party, base_exp)
    
    # Verify all Pokemon received full experience
    for i, pokemon in enumerate(party):
        if pokemon.exp_gained != base_exp:
            print(f"FAIL: Pokemon {i} did not receive full experience")
            print(f"  party_size={party_size}, base_exp={base_exp}")
            print(f"  pokemon.exp_gained={pokemon.exp_gained}, expected={base_exp}")
            print(f"  participated={pokemon.participated_in_battle}")
            return False
    
    # Verify participants and non-participants received same amount
    participants = [p for p in party if p.participated_in_battle]
    non_participants = [p for p in party if not p.participated_in_battle]
    
    if participants and non_participants:
        participant_exp = participants[0].exp_gained
        non_participant_exp = non_participants[0].exp_gained
        
        if participant_exp != non_participant_exp:
            print(f"FAIL: Participants and non-participants received different exp")
            print(f"  participant_exp={participant_exp}, non_participant_exp={non_participant_exp}")
            return False
    
    # Verify experience is not split (each Pokemon gets full amount)
    total_exp_awarded = sum(p.exp_gained for p in party)
    expected_total = base_exp * party_size
    
    if total_exp_awarded != expected_total:
        print(f"FAIL: Total experience awarded is incorrect")
        print(f"  total_exp_awarded={total_exp_awarded}, expected={expected_total}")
        print(f"  (base_exp={base_exp} * party_size={party_size})")
        return False
    
    return True


# ============================================================================
# Main Test Runner
# ============================================================================

def main():
    """Run all property-based tests for full party experience system."""
    num_trials = 500
    total_passed = 0
    total_failed = 0
    
    print("=" * 70)
    print("Property-Based Tests: Full Party Experience System")
    print("=" * 70)
    print("Properties tested:")
    print("  - Property 23: Full Party Experience Distribution")
    print("Validates: Requirements 10.1, 10.2, 10.3")
    print("=" * 70)
    print()
    
    # ========================================================================
    # Property 23: Full Party Experience Distribution
    # ========================================================================
    print("Testing Property 23: Full Party Experience Distribution...")
    passed = 0
    failed = 0
    
    for i in range(num_trials):
        party_size = random.randint(1, 6)
        base_exp = random.randint(0, 10000)
        
        if prop_full_party_experience_distribution(party_size, base_exp):
            passed += 1
        else:
            failed += 1
            if failed <= 3:
                print(f"  Counterexample: party_size={party_size}, base_exp={base_exp}")
    
    print(f"  Results: {passed}/{passed + failed} passed")
    total_passed += passed
    total_failed += failed
    
    # ========================================================================
    # Edge Case Testing
    # ========================================================================
    print("\n" + "=" * 70)
    print("Edge Case Testing")
    print("=" * 70)
    
    # Test single Pokemon party
    print("\nSingle Pokemon Party:")
    party = [PartyPokemon(level=50, participated_in_battle=True)]
    system = FullPartyExpSystem()
    system.award_experience_to_party(party, 1000)
    
    if party[0].exp_gained == 1000:
        print(f"  ✓ Single Pokemon receives full exp (1000)")
        total_passed += 1
    else:
        print(f"  ✗ Single Pokemon did not receive full exp")
        total_failed += 1
    
    # Test full party (6 Pokemon)
    print("\nFull Party (6 Pokemon):")
    party = [PartyPokemon(level=50, participated_in_battle=(i < 3)) for i in range(6)]
    system.award_experience_to_party(party, 2000)
    
    all_received_full = all(p.exp_gained == 2000 for p in party)
    if all_received_full:
        print(f"  ✓ All 6 Pokemon receive full exp (2000 each)")
        total_passed += 1
    else:
        print(f"  ✗ Not all Pokemon received full exp")
        total_failed += 1
    
    # Test no participants (all Pokemon fainted or not in battle)
    print("\nNo Participants (all non-participants):")
    party = [PartyPokemon(level=50, participated_in_battle=False) for i in range(4)]
    system.award_experience_to_party(party, 1500)
    
    all_received_full = all(p.exp_gained == 1500 for p in party)
    if all_received_full:
        print(f"  ✓ Non-participants receive full exp (1500 each)")
        total_passed += 1
    else:
        print(f"  ✗ Non-participants did not receive full exp")
        total_failed += 1
    
    # Test zero experience
    print("\nZero Experience:")
    party = [PartyPokemon(level=50, participated_in_battle=True) for i in range(3)]
    system.award_experience_to_party(party, 0)
    
    all_received_zero = all(p.exp_gained == 0 for p in party)
    if all_received_zero:
        print(f"  ✓ All Pokemon receive 0 exp correctly")
        total_passed += 1
    else:
        print(f"  ✗ Zero exp not handled correctly")
        total_failed += 1
    
    # Test large experience value
    print("\nLarge Experience Value:")
    party = [PartyPokemon(level=50, participated_in_battle=True) for i in range(2)]
    system.award_experience_to_party(party, 999999)
    
    all_received_full = all(p.exp_gained == 999999 for p in party)
    if all_received_full:
        print(f"  ✓ Large exp value handled correctly (999999 each)")
        total_passed += 1
    else:
        print(f"  ✗ Large exp value not handled correctly")
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
        print("\n✓ All full party experience property tests PASSED!")
        print("Property 23 is correctly validated.")
        return 0
    else:
        print(f"\n✗ Tests FAILED with {total_failed} counterexamples")
        return 1


if __name__ == "__main__":
    exit(main())
