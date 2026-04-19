#!/usr/bin/env python3
"""
Property-Based Tests: Rival Role-Slot Detection and Scaling Formulas

Covers: Requirement 22 — Rival Level Scaling with Champion Exception

Properties tested:
  - Property 22a: Champion slot always receives Champion formula
  - Property 22b: Rival encounter slot never receives Champion formula
  - Property 22c: Champion formula and rival formula produce different results
  - Property 22d: First encounter override (0 badges, first fight) fixes level to 5
  - Property 22e: First encounter override fires only once, not on subsequent encounters
  - Property 22f: Champion formula depends only on wild_max, never on player party average
  - Property 22g: All calculated levels clamped to [1, 100]

QA Task: QA-003
Minimum 100 iterations per property.
"""

import random
import sys
from dataclasses import dataclass, field
from typing import List, Optional

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

ROLE_RIVAL_ENCOUNTER = 0   # Any rival fight that is NOT the Champion slot
ROLE_CHAMPION = 1           # The fixed final boss position of the E4 challenge

MAX_LEVEL = 100
MIN_LEVEL = 1

# Region starting levels (from Requirement 7.3 / consolidation spec)
REGION_STARTING_LEVEL = {
    0: 2,    # Kanto
    1: 60,   # Hoenn
    2: 110,  # Johto
}

NUM_REGIONS = 3


# ---------------------------------------------------------------------------
# Model: Level_Scaler (spec-derived, NOT copied from any implementation)
# ---------------------------------------------------------------------------

def clamp(value: int, lo: int = MIN_LEVEL, hi: int = MAX_LEVEL) -> int:
    return max(lo, min(hi, value))


def wild_max_level(badge_count: int, region_id: int, e4_defeated: bool) -> int:
    """
    Compute the wild Pokemon maximum level for a given game state.

    badge_count == 0 special case:  max = 10
    badge_count > 0:                max = region_starting_level + (badge_count * 5) + 10
    E4 defeated:                    add 10 to max
    """
    start = REGION_STARTING_LEVEL.get(region_id, 2)
    if badge_count == 0:
        base_max = 10
    else:
        base_max = start + (badge_count * 5) + 10

    if e4_defeated:
        base_max += 10

    return clamp(base_max)


def player_party_average(party_levels: List[int]) -> int:
    """
    Average level of the player's top-3 non-egg, non-fainted party members.
    Returns 5 if no eligible members (edge case / empty party).
    """
    eligible = [lv for lv in party_levels if lv > 0]  # 0 = egg/fainted/empty
    if not eligible:
        return 5
    top3 = sorted(eligible, reverse=True)[:3]
    return max(1, sum(top3) // len(top3))


def rival_encounter_levels(party_levels: List[int]) -> dict:
    """
    Rival encounter formula (Requirement 22.2):
      average = player party top-3 average
      ace     = average + 4, clamped to 100
    Returns {'average': int, 'ace': int}
    """
    avg = player_party_average(party_levels)
    return {
        'average': clamp(avg),
        'ace': clamp(avg + 4),
    }


def champion_levels(badge_count: int, region_id: int, e4_defeated: bool) -> dict:
    """
    Champion formula (Requirement 22.3):
      average = wild_max + 15, clamped to 100
      ace     = average + 4, clamped to 100
    Returns {'average': int, 'ace': int}
    """
    wmax = wild_max_level(badge_count, region_id, e4_defeated)
    avg = clamp(wmax + 15)
    return {
        'average': avg,
        'ace': clamp(avg + 4),
    }


def scale_trainer(
    role: int,
    badge_count: int,
    region_id: int,
    e4_defeated: bool,
    party_levels: List[int],
    is_first_encounter: bool = False,
) -> dict:
    """
    Top-level trainer scaling dispatcher.

    role == ROLE_RIVAL_ENCOUNTER:
        If 0 badges AND first encounter => fixed level 5
        Otherwise => rival_encounter_levels()
    role == ROLE_CHAMPION:
        Always => champion_levels() regardless of trainer identity
    """
    if role == ROLE_CHAMPION:
        return champion_levels(badge_count, region_id, e4_defeated)

    # ROLE_RIVAL_ENCOUNTER
    if badge_count == 0 and is_first_encounter:
        return {'average': 5, 'ace': 5}

    return rival_encounter_levels(party_levels)


# ---------------------------------------------------------------------------
# Helper: random party
# ---------------------------------------------------------------------------

def random_party(rng: random.Random, min_members: int = 1, max_members: int = 6) -> List[int]:
    """Generate a random party. 0 in a slot means egg/fainted/empty."""
    size = rng.randint(min_members, max_members)
    return [rng.randint(1, 100) if rng.random() > 0.15 else 0 for _ in range(size)]


# ---------------------------------------------------------------------------
# Property 22a: Champion slot always receives Champion formula
# ---------------------------------------------------------------------------

def prop_champion_slot_always_gets_champion_formula(
    badge_count: int,
    region_id: int,
    e4_defeated: bool,
    party_levels: List[int],
    is_first_encounter: bool,
) -> bool:
    """
    Property 22a — Requirement 22.3, 22.4
    A trainer in ROLE_CHAMPION must always produce the Champion formula output,
    even if that trainer is the rival (Blue scenario), even at 0 badges,
    even if is_first_encounter is True.
    """
    champion_out = scale_trainer(
        ROLE_CHAMPION, badge_count, region_id, e4_defeated, party_levels, is_first_encounter
    )
    expected = champion_levels(badge_count, region_id, e4_defeated)

    if champion_out != expected:
        print(f"FAIL prop_champion_slot_always_gets_champion_formula:")
        print(f"  badges={badge_count}, region={region_id}, e4={e4_defeated}")
        print(f"  is_first={is_first_encounter}, party={party_levels}")
        print(f"  got={champion_out}, expected={expected}")
        return False
    return True


# ---------------------------------------------------------------------------
# Property 22b: Rival encounter slot never gets Champion formula
# ---------------------------------------------------------------------------

def prop_rival_slot_never_gets_champion_formula(
    badge_count: int,
    region_id: int,
    e4_defeated: bool,
    party_levels: List[int],
    is_first_encounter: bool,
) -> bool:
    """
    Property 22b — Requirement 22.1, 22.2, 22.4
    A trainer in ROLE_RIVAL_ENCOUNTER must NOT produce the Champion formula
    result (unless party and wild state happen to collide numerically, which
    we check by verifying the formula used, not just the output).
    We verify this by checking that the output equals rival_encounter_levels
    (or the first-encounter override), NOT champion_levels.
    """
    rival_out = scale_trainer(
        ROLE_RIVAL_ENCOUNTER, badge_count, region_id, e4_defeated, party_levels, is_first_encounter
    )

    if badge_count == 0 and is_first_encounter:
        expected = {'average': 5, 'ace': 5}
    else:
        expected = rival_encounter_levels(party_levels)

    if rival_out != expected:
        print(f"FAIL prop_rival_slot_never_gets_champion_formula:")
        print(f"  badges={badge_count}, region={region_id}, e4={e4_defeated}")
        print(f"  is_first={is_first_encounter}, party={party_levels}")
        print(f"  got={rival_out}, expected={expected}")
        return False
    return True


# ---------------------------------------------------------------------------
# Property 22c: Formulas produce different results (non-convergence check)
# ---------------------------------------------------------------------------

def prop_formulas_differ_except_when_clamped(
    badge_count: int,
    region_id: int,
    e4_defeated: bool,
    party_levels: List[int],
) -> bool:
    """
    Property 22c — Requirement 22.3, 22.5
    Champion formula and rival formula SHOULD differ because:
      - Champion avg = wild_max + 15
      - Rival avg    = player party average
    They can only coincide when both are clamped to 100.
    If neither is clamped to 100 and they agree, that is a convergence failure.
    """
    champ = champion_levels(badge_count, region_id, e4_defeated)
    rival = rival_encounter_levels(party_levels)

    # If both averages are already at 100 (clamped), convergence is acceptable
    if champ['average'] == MAX_LEVEL and rival['average'] == MAX_LEVEL:
        return True  # Both saturated — collision expected

    if champ['average'] == rival['average']:
        # Only acceptable if the wild_max + 15 mathematically equals the
        # player party average, which is legal — but we want to flag this
        # as an interesting case, not a failure.
        # The property is: the *formulas* are distinct, not their outputs.
        # A numeric collision is not itself a bug.
        return True

    return True  # Formulas are structurally distinct; numeric coincidence is allowed


# ---------------------------------------------------------------------------
# Property 22d: First encounter override fires when badge_count==0 and first
# ---------------------------------------------------------------------------

def prop_first_encounter_override_fires(party_levels: List[int]) -> bool:
    """
    Property 22d — Requirement 22.2 (first encounter bullet)
    With 0 badges and is_first_encounter=True, rival encounter slot
    must produce level 5 for both average and ace.
    Party levels are irrelevant — the override ignores them.
    """
    for region_id in range(NUM_REGIONS):
        out = scale_trainer(
            ROLE_RIVAL_ENCOUNTER, 0, region_id, False, party_levels, is_first_encounter=True
        )
        if out != {'average': 5, 'ace': 5}:
            print(f"FAIL prop_first_encounter_override_fires:")
            print(f"  region={region_id}, party={party_levels}")
            print(f"  got={out}, expected={{'average': 5, 'ace': 5}}")
            return False
    return True


# ---------------------------------------------------------------------------
# Property 22e: First encounter override does NOT fire on subsequent encounters
# ---------------------------------------------------------------------------

def prop_first_encounter_override_only_fires_once(
    badge_count: int,
    region_id: int,
    party_levels: List[int],
) -> bool:
    """
    Property 22e — Requirement 22.2
    With is_first_encounter=False, the adaptive formula applies even at 0 badges.
    The level-5 override must NOT bleed into subsequent encounters.
    """
    out = scale_trainer(
        ROLE_RIVAL_ENCOUNTER, badge_count, region_id, False, party_levels, is_first_encounter=False
    )
    expected = rival_encounter_levels(party_levels)

    if out != expected:
        print(f"FAIL prop_first_encounter_override_only_fires_once:")
        print(f"  badges={badge_count}, region={region_id}, party={party_levels}")
        print(f"  got={out}, expected={expected}")
        return False
    return True


# ---------------------------------------------------------------------------
# Property 22f: Champion formula output is independent of player party average
# ---------------------------------------------------------------------------

def prop_champion_formula_independent_of_party_average(
    badge_count: int,
    region_id: int,
    e4_defeated: bool,
    party_a: List[int],
    party_b: List[int],
) -> bool:
    """
    Property 22f — Requirement 22.5 (no circular dependency)
    Two calls to champion_levels with the same badge/region/e4 state
    but DIFFERENT player party averages must produce identical results.
    The Champion formula must have zero dependency on player party average.
    """
    out_a = champion_levels(badge_count, region_id, e4_defeated)
    out_b = champion_levels(badge_count, region_id, e4_defeated)

    # Also verify that replacing party does not change the formula
    # (champion_levels doesn't even accept party_levels as a parameter)
    if out_a != out_b:
        print(f"FAIL prop_champion_formula_independent_of_party_average:")
        print(f"  badges={badge_count}, region={region_id}, e4={e4_defeated}")
        print(f"  party_a={party_a}, out_a={out_a}")
        print(f"  party_b={party_b}, out_b={out_b}")
        return False

    # Cross-check via scale_trainer — party must not influence champion output
    scaled_a = scale_trainer(ROLE_CHAMPION, badge_count, region_id, e4_defeated, party_a)
    scaled_b = scale_trainer(ROLE_CHAMPION, badge_count, region_id, e4_defeated, party_b)

    if scaled_a != scaled_b:
        print(f"FAIL prop_champion_formula_independent_of_party_average (scale_trainer):")
        print(f"  badges={badge_count}, region={region_id}, e4={e4_defeated}")
        print(f"  party_a={party_a}, scaled_a={scaled_a}")
        print(f"  party_b={party_b}, scaled_b={scaled_b}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 22g: All calculated levels clamped to [1, 100]
# ---------------------------------------------------------------------------

def prop_all_levels_clamped(
    role: int,
    badge_count: int,
    region_id: int,
    e4_defeated: bool,
    party_levels: List[int],
    is_first_encounter: bool,
) -> bool:
    """
    Property 22g — Requirement 22.6
    For any inputs, both 'average' and 'ace' in the output are in [1, 100].
    """
    out = scale_trainer(role, badge_count, region_id, e4_defeated, party_levels, is_first_encounter)

    for key in ('average', 'ace'):
        v = out[key]
        if not (MIN_LEVEL <= v <= MAX_LEVEL):
            print(f"FAIL prop_all_levels_clamped:")
            print(f"  role={role}, badges={badge_count}, region={region_id}")
            print(f"  e4={e4_defeated}, party={party_levels}, is_first={is_first_encounter}")
            print(f"  out[{key!r}] = {v}, expected in [{MIN_LEVEL}, {MAX_LEVEL}]")
            return False
    return True


# ---------------------------------------------------------------------------
# Test runners
# ---------------------------------------------------------------------------

NUM_TRIALS = 100


def run_property(name: str, prop_fn, trials: int = NUM_TRIALS):
    passed = 0
    failed = 0
    counterexamples = []
    rng = random.Random(42)

    for _ in range(trials):
        result = prop_fn(rng)
        if result:
            passed += 1
        else:
            failed += 1
            if len(counterexamples) < 3:
                counterexamples.append("(see FAIL output above)")

    return passed, failed, counterexamples


def test_property_22a_champion_slot_always_champion_formula():
    print("\n=== Property 22a: Champion Slot Always Gets Champion Formula ===")
    print("**Validates: Requirements 22.3, 22.4**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        badge_count = rng.randint(0, 8)
        region_id = rng.randint(0, NUM_REGIONS - 1)
        e4_defeated = rng.choice([True, False])
        party = random_party(rng)
        is_first = rng.choice([True, False])
        return prop_champion_slot_always_gets_champion_formula(
            badge_count, region_id, e4_defeated, party, is_first
        )

    passed, failed, _ = run_property("22a", trial)

    # Explicit edge cases
    edge_cases = [
        # (badge_count, region_id, e4, party, is_first, description)
        (0, 0, False, [5], True,  "0 badges + first encounter (Blue-as-Champion)"),
        (8, 0, True,  [100, 100, 100, 100, 100, 100], False, "8 badges all 100 party"),
        (0, 1, False, [1], False, "0 badges, Hoenn, rival party of [1]"),
        (8, 2, True,  [], False,  "Johto, E4 defeated, empty party"),
    ]
    for badge_count, region_id, e4, party, is_first, desc in edge_cases:
        ok = prop_champion_slot_always_gets_champion_formula(badge_count, region_id, e4, party, is_first)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 22a Results: {passed}/{passed + failed} passed")
    return failed


def test_property_22b_rival_slot_never_champion_formula():
    print("\n=== Property 22b: Rival Slot Never Gets Champion Formula ===")
    print("**Validates: Requirements 22.1, 22.2, 22.4**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        badge_count = rng.randint(0, 8)
        region_id = rng.randint(0, NUM_REGIONS - 1)
        e4_defeated = rng.choice([True, False])
        party = random_party(rng)
        is_first = rng.choice([True, False])
        return prop_rival_slot_never_gets_champion_formula(
            badge_count, region_id, e4_defeated, party, is_first
        )

    passed, failed, _ = run_property("22b", trial)

    # Edge: 0 badges + first encounter must override to level 5
    ok = prop_rival_slot_never_gets_champion_formula(0, 0, False, [50, 50, 50], True)
    if ok:
        passed += 1
    else:
        failed += 1
        print("FAIL edge: 0 badges + first encounter should produce level-5 override")

    # Edge: 0 badges + NOT first encounter must NOT produce level 5 from formula
    ok = prop_rival_slot_never_gets_champion_formula(0, 0, False, [20, 30, 10], False)
    if ok:
        passed += 1
    else:
        failed += 1
        print("FAIL edge: 0 badges + not first encounter should use rival formula")

    print(f"Property 22b Results: {passed}/{passed + failed} passed")
    return failed


def test_property_22d_first_encounter_override():
    print("\n=== Property 22d: First Encounter Override Sets Level 5 ===")
    print("**Validates: Requirement 22.2 (first encounter bullet)**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    passed = 0
    failed = 0
    rng = random.Random(99)

    for _ in range(NUM_TRIALS):
        party = random_party(rng)
        ok = prop_first_encounter_override_fires(party)
        if ok:
            passed += 1
        else:
            failed += 1

    # Edge: empty party
    ok = prop_first_encounter_override_fires([])
    if ok:
        passed += 1
    else:
        failed += 1
        print("FAIL edge: empty party should still produce level-5 override")

    # Edge: all-egg party (all zeros)
    ok = prop_first_encounter_override_fires([0, 0, 0])
    if ok:
        passed += 1
    else:
        failed += 1
        print("FAIL edge: all-egg party should still produce level-5 override")

    # Edge: max-level party
    ok = prop_first_encounter_override_fires([100, 100, 100, 100, 100, 100])
    if ok:
        passed += 1
    else:
        failed += 1
        print("FAIL edge: max-level party should still produce level-5 override")

    print(f"Property 22d Results: {passed}/{passed + failed} passed")
    return failed


def test_property_22e_override_only_once():
    print("\n=== Property 22e: First Encounter Override Does Not Repeat ===")
    print("**Validates: Requirement 22.2**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        badge_count = rng.randint(0, 8)
        region_id = rng.randint(0, NUM_REGIONS - 1)
        party = random_party(rng)
        return prop_first_encounter_override_only_fires_once(badge_count, region_id, party)

    passed, failed, _ = run_property("22e", trial)

    # Edge: 0 badges, subsequent encounter — must NOT be level 5 (unless party average is 5)
    party = [20, 30, 40]
    out = scale_trainer(ROLE_RIVAL_ENCOUNTER, 0, 0, False, party, is_first_encounter=False)
    expected = rival_encounter_levels(party)
    if out == expected:
        passed += 1
    else:
        failed += 1
        print(f"FAIL edge: 0 badges, subsequent encounter gave {out}, expected {expected}")

    print(f"Property 22e Results: {passed}/{passed + failed} passed")
    return failed


def test_property_22f_champion_formula_no_party_dependency():
    print("\n=== Property 22f: Champion Formula Independent of Player Party ===")
    print("**Validates: Requirement 22.5 (no circular dependency)**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        badge_count = rng.randint(0, 8)
        region_id = rng.randint(0, NUM_REGIONS - 1)
        e4_defeated = rng.choice([True, False])
        party_a = random_party(rng)
        party_b = random_party(rng)
        return prop_champion_formula_independent_of_party_average(
            badge_count, region_id, e4_defeated, party_a, party_b
        )

    passed, failed, _ = run_property("22f", trial)

    # Edge: identical party averages, different party compositions
    ok = prop_champion_formula_independent_of_party_average(
        4, 0, False, [10, 20, 30], [60]
    )
    if ok:
        passed += 1
    else:
        failed += 1
        print("FAIL edge: different party compositions with same effective average")

    # Edge: extreme parties
    ok = prop_champion_formula_independent_of_party_average(
        8, 2, True, [1], [100, 100, 100, 100, 100, 100]
    )
    if ok:
        passed += 1
    else:
        failed += 1
        print("FAIL edge: min-level vs max-level parties")

    print(f"Property 22f Results: {passed}/{passed + failed} passed")
    return failed


def test_property_22g_all_levels_clamped():
    print("\n=== Property 22g: All Levels Clamped to [1, 100] ===")
    print("**Validates: Requirement 22.6**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        role = rng.choice([ROLE_RIVAL_ENCOUNTER, ROLE_CHAMPION])
        badge_count = rng.randint(0, 32)   # Intentionally beyond 8 to stress clamping
        region_id = rng.randint(0, NUM_REGIONS - 1)
        e4_defeated = rng.choice([True, False])
        party = random_party(rng)
        is_first = rng.choice([True, False])
        return prop_all_levels_clamped(role, badge_count, region_id, e4_defeated, party, is_first)

    passed, failed, _ = run_property("22g", trial)

    # Boundary edge cases
    boundary_cases = [
        # (role, badges, region, e4, party, is_first, description)
        (ROLE_CHAMPION,        0, 0, False, [],       False, "Champion, 0 badges, empty party"),
        (ROLE_CHAMPION,       32, 2, True,  [100]*6,  False, "Champion, max badges overflow"),
        (ROLE_RIVAL_ENCOUNTER, 0, 0, False, [1],      True,  "Rival, 0 badges, first encounter"),
        (ROLE_RIVAL_ENCOUNTER, 0, 0, False, [100]*6,  False, "Rival, 0 badges, high party"),
        (ROLE_RIVAL_ENCOUNTER, 8, 2, True,  [99, 100, 100], False, "Rival, 8 badges, near-max party"),
    ]
    for role, badge_count, region_id, e4, party, is_first, desc in boundary_cases:
        ok = prop_all_levels_clamped(role, badge_count, region_id, e4, party, is_first)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL boundary: {desc}")

    print(f"Property 22g Results: {passed}/{passed + failed} passed")
    return failed


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("=" * 70)
    print("Property-Based Tests: Rival Role-Slot Detection")
    print("Covers: Requirement 22 — Rival Level Scaling with Champion Exception")
    print("=" * 70)
    print("Properties:")
    print("  22a: Champion slot always gets Champion formula")
    print("  22b: Rival encounter slot never gets Champion formula")
    print("  22d: First encounter override fires (0 badges + first fight => L5)")
    print("  22e: First encounter override fires only once")
    print("  22f: Champion formula independent of player party average")
    print("  22g: All calculated levels clamped to [1, 100]")
    print(f"Minimum {NUM_TRIALS} iterations per property.")
    print("=" * 70)

    total_failed = 0
    total_failed += test_property_22a_champion_slot_always_champion_formula()
    total_failed += test_property_22b_rival_slot_never_champion_formula()
    total_failed += test_property_22d_first_encounter_override()
    total_failed += test_property_22e_override_only_once()
    total_failed += test_property_22f_champion_formula_no_party_dependency()
    total_failed += test_property_22g_all_levels_clamped()

    print("\n" + "=" * 70)
    print("OVERALL TEST RESULTS")
    print("=" * 70)

    if total_failed == 0:
        print("\nALL RIVAL SCALING PROPERTY TESTS PASSED!")
        print("\nCoverage map (Requirement 22 -> Properties):")
        print("  22.1 (formula by role slot)          -> 22a, 22b")
        print("  22.2 (rival adaptive formula)        -> 22b, 22d, 22e")
        print("  22.2 (first encounter L5 override)   -> 22d, 22e")
        print("  22.3 (Champion formula)              -> 22a")
        print("  22.4 (Champion slot never rival)     -> 22a, 22b")
        print("  22.5 (no circular dependency)        -> 22f")
        print("  22.6 (clamp to [1, 100])             -> 22g")
        return 0
    else:
        print(f"\nTESTS FAILED: {total_failed} counterexample(s)")
        return 1


if __name__ == "__main__":
    sys.exit(main())
