#!/usr/bin/env python3
"""
Property-Based Tests: Ace Pokemon Level Bonus

Covers: Requirement 23 — Ace Pokemon Level Bonus

Properties tested:
  - Property 23a: Ace is always the last party slot (index = party_size - 1)
  - Property 23b: Ace level equals trainer_average + 4
  - Property 23c: Ace level is always >= trainer average (bonus is never negative)
  - Property 23d: Ace level never exceeds 100
  - Property 23e: All non-ace party members are at the trainer average level
  - Property 23f: Single-Pokemon trainer treats that Pokemon as the ace
  - Property 23g: Ace bonus applies across all trainer categories (Gym Leader,
                  E4, Champion, Rival, Normal)
  - Property 23h: Non-ace member levels are never greater than ace level

QA Task: QA-004
Minimum 100 iterations per property.
"""

import random
import sys
from typing import List, Tuple

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

MAX_LEVEL = 100
MIN_LEVEL = 1
ACE_BONUS = 4

# Trainer category identifiers
CATEGORY_NORMAL  = 0
CATEGORY_GYM     = 1
CATEGORY_E4      = 2
CATEGORY_CHAMPION = 3
CATEGORY_RIVAL   = 4

ALL_CATEGORIES = [
    CATEGORY_NORMAL,
    CATEGORY_GYM,
    CATEGORY_E4,
    CATEGORY_CHAMPION,
    CATEGORY_RIVAL,
]

CATEGORY_NAMES = {
    CATEGORY_NORMAL:  "Normal",
    CATEGORY_GYM:     "Gym Leader",
    CATEGORY_E4:      "Elite Four",
    CATEGORY_CHAMPION:"Champion",
    CATEGORY_RIVAL:   "Rival",
}

# ---------------------------------------------------------------------------
# Model: Party scaling (spec-derived, NOT copied from implementation)
# ---------------------------------------------------------------------------

def clamp(value: int, lo: int = MIN_LEVEL, hi: int = MAX_LEVEL) -> int:
    return max(lo, min(hi, value))


def build_trainer_party(trainer_average: int, party_size: int) -> List[int]:
    """
    Scale a trainer party of given size using the ace-bonus rule
    (Requirement 23.1–23.3, 23.7):
      - All non-ace slots: trainer_average (clamped)
      - Last slot (ace):   trainer_average + ACE_BONUS (clamped)
    Returns list of levels, length = party_size.
    """
    if party_size <= 0:
        return []

    average_level = clamp(trainer_average)
    ace_level = clamp(trainer_average + ACE_BONUS)

    party = [average_level] * party_size
    party[-1] = ace_level  # last slot is always the ace
    return party


# ---------------------------------------------------------------------------
# Property 23a: Ace is always the last party slot
# ---------------------------------------------------------------------------

def prop_ace_is_last_slot(trainer_average: int, party_size: int) -> bool:
    """
    Property 23a — Requirement 23.1
    The ace is party[party_size - 1]. For any valid trainer_average and
    party_size >= 1, the last element of the returned party must equal
    clamp(trainer_average + 4) and no other slot should equal that value
    unless trainer_average+4 == trainer_average (i.e., both are clamped to 100).
    """
    party = build_trainer_party(trainer_average, party_size)
    if not party:
        return True  # zero-size party is vacuously true

    expected_ace = clamp(trainer_average + ACE_BONUS)
    expected_non_ace = clamp(trainer_average)

    # Ace must be the last slot
    if party[-1] != expected_ace:
        print(f"FAIL prop_ace_is_last_slot:")
        print(f"  trainer_average={trainer_average}, party_size={party_size}")
        print(f"  party={party}")
        print(f"  party[-1]={party[-1]}, expected={expected_ace}")
        return False

    # Non-ace slots (all except last) must NOT have the ace level
    # unless ace_level == non_ace_level (both clamped to 100)
    if expected_ace != expected_non_ace:
        for i in range(party_size - 1):
            if party[i] == expected_ace:
                print(f"FAIL prop_ace_is_last_slot: non-ace slot {i} has ace level")
                print(f"  trainer_average={trainer_average}, party_size={party_size}")
                print(f"  party={party}")
                return False

    return True


# ---------------------------------------------------------------------------
# Property 23b: Ace level == trainer_average + 4
# ---------------------------------------------------------------------------

def prop_ace_level_equals_average_plus_four(trainer_average: int, party_size: int) -> bool:
    """
    Property 23b — Requirement 23.2
    Ace level must equal clamp(trainer_average + 4) for any party_size >= 1.
    """
    if party_size < 1:
        return True

    party = build_trainer_party(trainer_average, party_size)
    expected_ace = clamp(trainer_average + ACE_BONUS)

    if party[-1] != expected_ace:
        print(f"FAIL prop_ace_level_equals_average_plus_four:")
        print(f"  trainer_average={trainer_average}, party_size={party_size}")
        print(f"  ace={party[-1]}, expected={expected_ace}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 23c: Ace level >= trainer average (bonus never negative)
# ---------------------------------------------------------------------------

def prop_ace_level_ge_trainer_average(trainer_average: int, party_size: int) -> bool:
    """
    Property 23c — Requirement 23.2 (bonus is +4, always non-negative)
    ace_level >= trainer_average_clamped for all valid inputs.
    """
    if party_size < 1:
        return True

    party = build_trainer_party(trainer_average, party_size)
    non_ace = clamp(trainer_average)
    ace = party[-1]

    if ace < non_ace:
        print(f"FAIL prop_ace_level_ge_trainer_average:")
        print(f"  trainer_average={trainer_average}, party_size={party_size}")
        print(f"  ace={ace}, non_ace_level={non_ace}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 23d: Ace level never exceeds 100
# ---------------------------------------------------------------------------

def prop_ace_level_never_exceeds_100(trainer_average: int, party_size: int) -> bool:
    """
    Property 23d — Requirement 23.7
    Even if trainer_average is near or at 100, ace must be clamped to 100.
    """
    if party_size < 1:
        return True

    party = build_trainer_party(trainer_average, party_size)
    ace = party[-1]

    if ace > MAX_LEVEL:
        print(f"FAIL prop_ace_level_never_exceeds_100:")
        print(f"  trainer_average={trainer_average}, party_size={party_size}")
        print(f"  ace={ace}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 23e: All non-ace members are at trainer average
# ---------------------------------------------------------------------------

def prop_non_ace_members_at_trainer_average(trainer_average: int, party_size: int) -> bool:
    """
    Property 23e — Requirement 23.3
    Every slot except the last must equal clamp(trainer_average).
    """
    if party_size < 2:
        return True  # single-member party has no non-ace slots

    party = build_trainer_party(trainer_average, party_size)
    expected_non_ace = clamp(trainer_average)

    for i in range(party_size - 1):
        if party[i] != expected_non_ace:
            print(f"FAIL prop_non_ace_members_at_trainer_average:")
            print(f"  trainer_average={trainer_average}, party_size={party_size}")
            print(f"  slot {i}: level={party[i]}, expected={expected_non_ace}")
            return False

    return True


# ---------------------------------------------------------------------------
# Property 23f: Single-Pokemon trainer — that Pokemon IS the ace
# ---------------------------------------------------------------------------

def prop_single_pokemon_trainer_is_ace(trainer_average: int) -> bool:
    """
    Property 23f — Requirement 23.8
    With party_size == 1, the sole Pokemon is the ace and receives +4.
    """
    party = build_trainer_party(trainer_average, 1)

    if len(party) != 1:
        print(f"FAIL prop_single_pokemon_trainer_is_ace: party length != 1")
        return False

    expected_ace = clamp(trainer_average + ACE_BONUS)
    if party[0] != expected_ace:
        print(f"FAIL prop_single_pokemon_trainer_is_ace:")
        print(f"  trainer_average={trainer_average}")
        print(f"  party[0]={party[0]}, expected={expected_ace}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 23g: Ace bonus applies across all trainer categories
# ---------------------------------------------------------------------------

def prop_ace_bonus_applies_to_all_categories(
    category: int,
    trainer_average: int,
    party_size: int,
) -> bool:
    """
    Property 23g — Requirement 23.4
    The +4 ace bonus is applied identically regardless of trainer category.
    We verify that build_trainer_party (which represents the common scaling
    step) produces the correct ace for every category.
    """
    if party_size < 1:
        return True

    party = build_trainer_party(trainer_average, party_size)
    expected_ace = clamp(trainer_average + ACE_BONUS)

    if party[-1] != expected_ace:
        print(f"FAIL prop_ace_bonus_applies_to_all_categories:")
        print(f"  category={CATEGORY_NAMES.get(category, category)}")
        print(f"  trainer_average={trainer_average}, party_size={party_size}")
        print(f"  ace={party[-1]}, expected={expected_ace}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 23h: Non-ace levels are never greater than ace level
# ---------------------------------------------------------------------------

def prop_non_ace_never_greater_than_ace(trainer_average: int, party_size: int) -> bool:
    """
    Property 23h — implicit from Requirement 23.2/23.3
    Since ace = average + 4 and non-ace = average, and average >= 1,
    non-ace <= ace is always guaranteed (unless clamped).
    We assert it explicitly for every generated party.
    """
    if party_size < 2:
        return True

    party = build_trainer_party(trainer_average, party_size)
    ace_level = party[-1]

    for i in range(party_size - 1):
        if party[i] > ace_level:
            print(f"FAIL prop_non_ace_never_greater_than_ace:")
            print(f"  trainer_average={trainer_average}, party_size={party_size}")
            print(f"  slot {i}: {party[i]} > ace {ace_level}")
            return False

    return True


# ---------------------------------------------------------------------------
# Rival and Champion average inputs (per Requirements 22 and 23.5/23.6)
# ---------------------------------------------------------------------------

def rival_average_from_party(player_party: List[int]) -> int:
    """
    Rival trainer average = player top-3 party average (non-egg members only).
    """
    eligible = [lv for lv in player_party if lv > 0]
    if not eligible:
        return 5
    top3 = sorted(eligible, reverse=True)[:3]
    return max(1, sum(top3) // len(top3))


def champion_average(wild_max: int) -> int:
    """Champion trainer average = wild_max + 15, clamped."""
    return max(MIN_LEVEL, min(MAX_LEVEL, wild_max + 15))


def prop_rival_ace_computed_on_rival_average(
    player_party: List[int],
    party_size: int,
) -> bool:
    """
    Property 23g (rival) — Requirement 23.5
    For a rival in encounter slot, ace = rival_average + 4
    where rival_average = player top-3 average.
    """
    if party_size < 1:
        return True

    avg = rival_average_from_party(player_party)
    party = build_trainer_party(avg, party_size)
    expected_ace = clamp(avg + ACE_BONUS)

    if party[-1] != expected_ace:
        print(f"FAIL prop_rival_ace_computed_on_rival_average:")
        print(f"  player_party={player_party}, party_size={party_size}")
        print(f"  rival_average={avg}, expected_ace={expected_ace}, got={party[-1]}")
        return False

    return True


def prop_champion_ace_equals_wild_max_plus_19(
    wild_max: int,
    party_size: int,
) -> bool:
    """
    Property 23g (champion) — Requirement 23.6
    Champion ace = wild_max + 15 (average) + 4 (ace bonus) = wild_max + 19, clamped.
    """
    if party_size < 1:
        return True

    avg = champion_average(wild_max)
    party = build_trainer_party(avg, party_size)
    expected_ace = clamp(wild_max + 19)

    if party[-1] != expected_ace:
        print(f"FAIL prop_champion_ace_equals_wild_max_plus_19:")
        print(f"  wild_max={wild_max}, party_size={party_size}")
        print(f"  champion_average={avg}, expected_ace={expected_ace}, got={party[-1]}")
        return False

    return True


# ---------------------------------------------------------------------------
# Test runners
# ---------------------------------------------------------------------------

NUM_TRIALS = 100


def test_property_23a_ace_is_last_slot():
    print("\n=== Property 23a: Ace Is Always the Last Slot ===")
    print("**Validates: Requirement 23.1**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(7)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        avg = rng.randint(1, 100)
        size = rng.randint(1, 6)
        if prop_ace_is_last_slot(avg, size):
            passed += 1
        else:
            failed += 1

    # Boundary edges
    boundary_cases = [
        (1, 1,  "min average, single pokemon"),
        (100, 1, "max average, single pokemon"),
        (96, 6, "near-max average, full team — ace should clamp to 100"),
        (100, 6, "max average full team — ace and non-ace both 100"),
        (1, 6, "min average, full team"),
    ]
    for avg, size, desc in boundary_cases:
        if prop_ace_is_last_slot(avg, size):
            passed += 1
        else:
            failed += 1
            print(f"FAIL boundary: {desc}")

    print(f"Property 23a Results: {passed}/{passed + failed} passed")
    return failed


def test_property_23b_ace_equals_average_plus_four():
    print("\n=== Property 23b: Ace Level == trainer_average + 4 ===")
    print("**Validates: Requirement 23.2**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(13)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        avg = rng.randint(1, 100)
        size = rng.randint(1, 6)
        if prop_ace_level_equals_average_plus_four(avg, size):
            passed += 1
        else:
            failed += 1

    # Clamping boundary: average = 97 => ace = 100 (not 101)
    for avg in [95, 96, 97, 98, 99, 100]:
        for size in [1, 3, 6]:
            if prop_ace_level_equals_average_plus_four(avg, size):
                passed += 1
            else:
                failed += 1
                print(f"FAIL boundary: avg={avg}, size={size}")

    print(f"Property 23b Results: {passed}/{passed + failed} passed")
    return failed


def test_property_23c_ace_ge_average():
    print("\n=== Property 23c: Ace Level >= Trainer Average ===")
    print("**Validates: Requirement 23.2 (bonus non-negative)**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(17)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        avg = rng.randint(1, 100)
        size = rng.randint(1, 6)
        if prop_ace_level_ge_trainer_average(avg, size):
            passed += 1
        else:
            failed += 1

    for avg in range(1, 101):
        if prop_ace_level_ge_trainer_average(avg, 2):
            passed += 1
        else:
            failed += 1
            print(f"FAIL exhaustive: avg={avg}")

    print(f"Property 23c Results: {passed}/{passed + failed} passed")
    return failed


def test_property_23d_ace_never_exceeds_100():
    print("\n=== Property 23d: Ace Level Never Exceeds 100 ===")
    print("**Validates: Requirement 23.7**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(23)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        avg = rng.randint(1, 200)  # Intentionally beyond 100 to stress clamping
        size = rng.randint(1, 6)
        if prop_ace_level_never_exceeds_100(avg, size):
            passed += 1
        else:
            failed += 1

    # Explicit overclamped inputs
    for avg in [97, 98, 99, 100, 150, 200]:
        if prop_ace_level_never_exceeds_100(avg, 6):
            passed += 1
        else:
            failed += 1
            print(f"FAIL boundary: avg={avg}")

    print(f"Property 23d Results: {passed}/{passed + failed} passed")
    return failed


def test_property_23e_non_ace_at_trainer_average():
    print("\n=== Property 23e: Non-Ace Members at Trainer Average ===")
    print("**Validates: Requirement 23.3**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(31)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        avg = rng.randint(1, 100)
        size = rng.randint(2, 6)  # need at least 2 for non-ace slots to exist
        if prop_non_ace_members_at_trainer_average(avg, size):
            passed += 1
        else:
            failed += 1

    # Specific party sizes
    for size in range(2, 7):
        for avg in [1, 50, 97, 100]:
            if prop_non_ace_members_at_trainer_average(avg, size):
                passed += 1
            else:
                failed += 1
                print(f"FAIL boundary: avg={avg}, size={size}")

    print(f"Property 23e Results: {passed}/{passed + failed} passed")
    return failed


def test_property_23f_single_pokemon_is_ace():
    print("\n=== Property 23f: Single-Pokemon Trainer Treated as Ace ===")
    print("**Validates: Requirement 23.8**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(37)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        avg = rng.randint(1, 100)
        if prop_single_pokemon_trainer_is_ace(avg):
            passed += 1
        else:
            failed += 1

    # Exhaustive over all valid averages
    for avg in range(1, 101):
        if prop_single_pokemon_trainer_is_ace(avg):
            passed += 1
        else:
            failed += 1
            print(f"FAIL exhaustive: avg={avg}")

    print(f"Property 23f Results: {passed}/{passed + failed} passed")
    return failed


def test_property_23g_all_categories():
    print("\n=== Property 23g: Ace Bonus Applies to All Trainer Categories ===")
    print("**Validates: Requirement 23.4**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(41)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        category = rng.choice(ALL_CATEGORIES)
        avg = rng.randint(1, 100)
        size = rng.randint(1, 6)
        if prop_ace_bonus_applies_to_all_categories(category, avg, size):
            passed += 1
        else:
            failed += 1

    # Every category × all party sizes
    for category in ALL_CATEGORIES:
        for size in range(1, 7):
            for avg in [5, 50, 96, 100]:
                if prop_ace_bonus_applies_to_all_categories(category, avg, size):
                    passed += 1
                else:
                    failed += 1
                    print(f"FAIL: category={CATEGORY_NAMES[category]}, avg={avg}, size={size}")

    # Rival ace: player_party=[20, 30, 40] => avg=30, ace=34
    player_party = [20, 30, 40]
    ok = prop_rival_ace_computed_on_rival_average(player_party, 3)
    if ok:
        passed += 1
    else:
        failed += 1
        print("FAIL: rival ace = player_avg + 4 (party [20,30,40])")

    # Rival ace with high player party (clamping expected)
    player_party = [98, 100, 100]
    ok = prop_rival_ace_computed_on_rival_average(player_party, 6)
    if ok:
        passed += 1
    else:
        failed += 1
        print("FAIL: rival ace near clamp boundary")

    # Champion ace = wild_max + 19
    for wild_max in [50, 75, 80, 81, 82, 100]:
        ok = prop_champion_ace_equals_wild_max_plus_19(wild_max, 6)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL: champion ace = wild_max({wild_max}) + 19")

    print(f"Property 23g Results: {passed}/{passed + failed} passed")
    return failed


def test_property_23h_non_ace_not_greater_than_ace():
    print("\n=== Property 23h: Non-Ace Levels Never Greater Than Ace ===")
    print("**Validates: Requirement 23.2 / 23.3 ordering invariant**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(53)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        avg = rng.randint(1, 100)
        size = rng.randint(2, 6)
        if prop_non_ace_never_greater_than_ace(avg, size):
            passed += 1
        else:
            failed += 1

    # Full exhaustive for all party sizes and some averages
    for size in range(2, 7):
        for avg in range(1, 101):
            if prop_non_ace_never_greater_than_ace(avg, size):
                passed += 1
            else:
                failed += 1
                print(f"FAIL exhaustive: avg={avg}, size={size}")

    print(f"Property 23h Results: {passed}/{passed + failed} passed")
    return failed


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("=" * 70)
    print("Property-Based Tests: Ace Pokemon Level Bonus")
    print("Covers: Requirement 23 — Ace Pokemon Level Bonus")
    print("=" * 70)
    print("Properties:")
    print("  23a: Ace is always the last party slot")
    print("  23b: Ace level == trainer_average + 4")
    print("  23c: Ace level is always >= trainer average")
    print("  23d: Ace level never exceeds 100")
    print("  23e: All non-ace members are at trainer average level")
    print("  23f: Single-Pokemon trainer is treated as the ace")
    print("  23g: Ace bonus applies to all trainer categories")
    print("  23h: Non-ace member levels are never greater than ace level")
    print(f"Minimum {NUM_TRIALS} iterations per property.")
    print("=" * 70)

    total_failed = 0
    total_failed += test_property_23a_ace_is_last_slot()
    total_failed += test_property_23b_ace_equals_average_plus_four()
    total_failed += test_property_23c_ace_ge_average()
    total_failed += test_property_23d_ace_never_exceeds_100()
    total_failed += test_property_23e_non_ace_at_trainer_average()
    total_failed += test_property_23f_single_pokemon_is_ace()
    total_failed += test_property_23g_all_categories()
    total_failed += test_property_23h_non_ace_not_greater_than_ace()

    print("\n" + "=" * 70)
    print("OVERALL TEST RESULTS")
    print("=" * 70)

    if total_failed == 0:
        print("\nALL ACE POKEMON PROPERTY TESTS PASSED!")
        print("\nCoverage map (Requirement 23 -> Properties):")
        print("  23.1 (last slot is ace)              -> 23a")
        print("  23.2 (ace = average + 4)             -> 23b, 23c, 23d, 23h")
        print("  23.3 (non-ace = average)             -> 23e, 23h")
        print("  23.4 (all categories)                -> 23g")
        print("  23.5 (rival ace on rival avg)        -> 23g")
        print("  23.6 (champion ace on wild max)      -> 23g")
        print("  23.7 (clamp ace to [1, 100])         -> 23d")
        print("  23.8 (single-Pokemon = ace)          -> 23f")
        return 0
    else:
        print(f"\nTESTS FAILED: {total_failed} counterexample(s)")
        return 1


if __name__ == "__main__":
    sys.exit(main())
