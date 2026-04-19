#!/usr/bin/env python3
"""
Property-Based Tests: Trade Evolution Alternatives

Covers: Requirement 13 — Trade-evolved Pokemon evolve at level 37 instead of
        requiring an actual trade; item-conditioned trade evolutions still need
        the item at level 37

Trade-evo species (complete list per spec):
  Kadabra -> Alakazam
  Machoke -> Machamp
  Graveler -> Golem
  Haunter -> Gengar
  Poliwhirl -> Politoed  (requires King's Rock)
  Slowpoke -> Slowking   (requires King's Rock)
  Onix -> Steelix        (requires Metal Coat)
  Scyther -> Scizor      (requires Metal Coat)
  Seadra -> Kingdra      (requires Dragon Scale)
  Porygon -> Porygon2    (requires Up-Grade)
  Clamperl -> Huntail    (requires DeepSeaTooth)
  Clamperl -> Gorebyss   (requires DeepSeaScale)

Properties tested:
  - prop_trade_evo_species_evolve_at_37: level_up(species, level=37) triggers evolution
  - prop_no_early_evolution: level_up(species, level=36) does NOT trigger trade-evo evolution
  - prop_normal_evolutions_unchanged: non-trade-evo species (Bulbasaur at 16) are unaffected
  - prop_item_trade_evos_need_item_at_37: item-conditioned species need item at level 37
  - prop_evolution_produces_correct_species: each trade-evo produces the correct evolved form

QA Task: QA-010
Minimum 100 iterations per property.
"""

import random
import sys
from typing import Optional

# ---------------------------------------------------------------------------
# Constants: Trade evolution data
# ---------------------------------------------------------------------------

# Species IDs (numeric, matching GBA constants)
# Using canonical National Dex numbers for clarity
SPECIES_KADABRA = 64
SPECIES_ALAKAZAM = 65
SPECIES_MACHOKE = 67
SPECIES_MACHAMP = 68
SPECIES_GRAVELER = 75
SPECIES_GOLEM = 76
SPECIES_HAUNTER = 93
SPECIES_GENGAR = 94
SPECIES_POLIWHIRL = 61
SPECIES_POLITOED = 186
SPECIES_SLOWPOKE = 79
SPECIES_SLOWKING = 199
SPECIES_ONIX = 95
SPECIES_STEELIX = 208
SPECIES_SCYTHER = 123
SPECIES_SCIZOR = 212
SPECIES_SEADRA = 117
SPECIES_KINGDRA = 230
SPECIES_PORYGON = 137
SPECIES_PORYGON2 = 233
SPECIES_CLAMPERL = 366
SPECIES_HUNTAIL = 367
SPECIES_GOREBYSS = 368

# Item IDs
ITEM_NONE = 0
ITEM_KINGS_ROCK = 222
ITEM_METAL_COAT = 223
ITEM_DRAGON_SCALE = 235
ITEM_UP_GRADE = 252
ITEM_DEEP_SEA_TOOTH = 253
ITEM_DEEP_SEA_SCALE = 254

# Evolution trigger level (from Req 13)
TRADE_EVO_LEVEL = 37
PRE_EVO_LEVEL = TRADE_EVO_LEVEL - 1  # 36 — must NOT trigger

# Normal evolution examples (non-trade-evo, must remain unaffected)
NORMAL_EVOS = {
    1: (16, None, 2),    # Bulbasaur -> Ivysaur at 16, no item
    4: (16, None, 5),    # Charmander -> Charmeleon at 16
    7: (16, None, 8),    # Squirtle -> Wartortle at 16
    129: (20, None, 130),  # Magikarp -> Gyarados at 20
}

# ---------------------------------------------------------------------------
# Trade evolution table
# ---------------------------------------------------------------------------

# Entry: species_id -> (evolved_species_id, required_item_id | None)
TRADE_EVO_TABLE: dict = {
    SPECIES_KADABRA:  (SPECIES_ALAKAZAM, None),
    SPECIES_MACHOKE:  (SPECIES_MACHAMP, None),
    SPECIES_GRAVELER: (SPECIES_GOLEM, None),
    SPECIES_HAUNTER:  (SPECIES_GENGAR, None),
    SPECIES_POLIWHIRL:(SPECIES_POLITOED, ITEM_KINGS_ROCK),
    SPECIES_SLOWPOKE: (SPECIES_SLOWKING, ITEM_KINGS_ROCK),
    SPECIES_ONIX:     (SPECIES_STEELIX, ITEM_METAL_COAT),
    SPECIES_SCYTHER:  (SPECIES_SCIZOR, ITEM_METAL_COAT),
    SPECIES_SEADRA:   (SPECIES_KINGDRA, ITEM_DRAGON_SCALE),
    SPECIES_PORYGON:  (SPECIES_PORYGON2, ITEM_UP_GRADE),
    SPECIES_CLAMPERL: (SPECIES_HUNTAIL, ITEM_DEEP_SEA_TOOTH),
    # Gorebyss variant: same pre-evo, different item
    # We model it as a separate entry keyed by (species, item) — handled in check_evolution
}

# Clamperl has two possible evolutions depending on item
CLAMPERL_ALT = (SPECIES_GOREBYSS, ITEM_DEEP_SEA_SCALE)


# ---------------------------------------------------------------------------
# Model: check_evolution
# ---------------------------------------------------------------------------

def check_evolution(
    species_id: int,
    level: int,
    held_item: int = ITEM_NONE,
) -> Optional[int]:
    """
    Spec model of the evolution check function.

    Returns the evolved species_id if evolution triggers, or None if not.

    Trade-evo logic (Req 13):
      - If species is a trade-evo species AND level >= 37:
          - If required_item is None: evolves unconditionally
          - If required_item is set: evolves only if held_item == required_item
      - If level < 37: never triggers (even with correct item)
      - Non-trade-evo species: use normal evolution table (not modelled here —
        callers pass normal-evo species to verify they are unaffected)

    Normal evolution (simplified model — species not in TRADE_EVO_TABLE):
      - Check NORMAL_EVOS; evolve at specified level regardless of item.
    """
    # Clamperl: Gorebyss variant check first
    if species_id == SPECIES_CLAMPERL:
        if level >= TRADE_EVO_LEVEL:
            if held_item == ITEM_DEEP_SEA_SCALE:
                return SPECIES_GOREBYSS
            if held_item == ITEM_DEEP_SEA_TOOTH:
                return SPECIES_HUNTAIL
        return None

    if species_id in TRADE_EVO_TABLE:
        evolved_id, required_item = TRADE_EVO_TABLE[species_id]
        if level >= TRADE_EVO_LEVEL:
            if required_item is None or held_item == required_item:
                return evolved_id
        return None  # level < 37, or wrong item

    if species_id in NORMAL_EVOS:
        evo_level, evo_item, evolved_id = NORMAL_EVOS[species_id]
        if level >= evo_level and (evo_item is None or held_item == evo_item):
            return evolved_id
        return None

    return None  # unknown species or unevolved at this level


# ---------------------------------------------------------------------------
# Property: prop_trade_evo_species_evolve_at_37
# ---------------------------------------------------------------------------

def prop_trade_evo_species_evolve_at_37(species_id: int, level: int) -> bool:
    """
    Property 13a — Req 13.1
    For every no-item trade-evo species, check_evolution at level 37+
    must return a non-None evolved species.
    """
    evolved_id, required_item = TRADE_EVO_TABLE[species_id]
    if required_item is not None:
        return True  # skip item-conditioned evos (covered by 13d)

    if level < TRADE_EVO_LEVEL:
        return True  # out of scope for this property

    result = check_evolution(species_id, level, ITEM_NONE)
    if result is None:
        print(f"FAIL prop_trade_evo_species_evolve_at_37:")
        print(f"  species={species_id}, level={level}")
        print(f"  expected evolved_id={evolved_id}, got None")
        return False

    if result != evolved_id:
        print(f"FAIL prop_trade_evo_species_evolve_at_37:")
        print(f"  species={species_id}, level={level}")
        print(f"  expected={evolved_id}, got={result}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_no_early_evolution
# ---------------------------------------------------------------------------

def prop_no_early_evolution(species_id: int, level: int) -> bool:
    """
    Property 13b — Req 13.2
    For any trade-evo species, check_evolution at level < 37 must return None,
    even if the correct held item is present.
    """
    if level >= TRADE_EVO_LEVEL:
        return True  # not under test here

    if species_id == SPECIES_CLAMPERL:
        # Try both items
        for item in (ITEM_DEEP_SEA_TOOTH, ITEM_DEEP_SEA_SCALE):
            result = check_evolution(species_id, level, item)
            if result is not None:
                print(f"FAIL prop_no_early_evolution (Clamperl):")
                print(f"  level={level}, item={item}, got={result} — expected None")
                return False
        return True

    evolved_id, required_item = TRADE_EVO_TABLE[species_id]
    item_to_try = required_item if required_item is not None else ITEM_NONE

    result = check_evolution(species_id, level, item_to_try)
    if result is not None:
        print(f"FAIL prop_no_early_evolution:")
        print(f"  species={species_id}, level={level}, item={item_to_try}")
        print(f"  expected None (no early evo), got={result}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_normal_evolutions_unchanged
# ---------------------------------------------------------------------------

def prop_normal_evolutions_unchanged(species_id: int, level: int) -> bool:
    """
    Property 13c — Req 13.3
    Normal (non-trade-evo) species evolve at their canonical levels.
    The trade-evo-at-37 change must not affect them.
    """
    if species_id not in NORMAL_EVOS:
        return True

    evo_level, _, expected_evolved = NORMAL_EVOS[species_id]

    if level >= evo_level:
        result = check_evolution(species_id, level, ITEM_NONE)
        if result != expected_evolved:
            print(f"FAIL prop_normal_evolutions_unchanged:")
            print(f"  species={species_id}, level={level}")
            print(f"  expected={expected_evolved}, got={result}")
            return False
    else:
        result = check_evolution(species_id, level, ITEM_NONE)
        if result is not None:
            print(f"FAIL prop_normal_evolutions_unchanged (early evo):")
            print(f"  species={species_id}, level={level}")
            print(f"  expected None, got={result}")
            return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_item_trade_evos_need_item_at_37
# ---------------------------------------------------------------------------

def prop_item_trade_evos_need_item_at_37(species_id: int) -> bool:
    """
    Property 13d — Req 13.4
    Item-conditioned trade-evo species (e.g. Poliwhirl+King's Rock) must
    carry the required item at level 37 to evolve.  Level 37 without the
    item must return None.  Level 37 with the wrong item must return None.
    Level 37 with the correct item must return the evolved species.
    """
    if species_id == SPECIES_CLAMPERL:
        # Gorebyss path
        result_with = check_evolution(species_id, TRADE_EVO_LEVEL, ITEM_DEEP_SEA_SCALE)
        result_without = check_evolution(species_id, TRADE_EVO_LEVEL, ITEM_NONE)
        result_wrong = check_evolution(species_id, TRADE_EVO_LEVEL, ITEM_KINGS_ROCK)

        if result_with != SPECIES_GOREBYSS:
            print(f"FAIL prop_item_trade_evos_need_item_at_37 (Clamperl/Gorebyss):")
            print(f"  with correct item: got={result_with}, expected={SPECIES_GOREBYSS}")
            return False
        if result_without is not None:
            print(f"FAIL prop_item_trade_evos_need_item_at_37 (Clamperl no item):")
            print(f"  without item: got={result_without}, expected=None")
            return False
        if result_wrong is not None:
            print(f"FAIL prop_item_trade_evos_need_item_at_37 (Clamperl wrong item):")
            print(f"  with wrong item: got={result_wrong}, expected=None")
            return False
        return True

    if species_id not in TRADE_EVO_TABLE:
        return True

    evolved_id, required_item = TRADE_EVO_TABLE[species_id]
    if required_item is None:
        return True  # no-item evo; skip

    # With correct item at level 37 — must evolve
    result_correct = check_evolution(species_id, TRADE_EVO_LEVEL, required_item)
    if result_correct != evolved_id:
        print(f"FAIL prop_item_trade_evos_need_item_at_37 (correct item):")
        print(f"  species={species_id}, item={required_item}")
        print(f"  expected={evolved_id}, got={result_correct}")
        return False

    # Without item at level 37 — must NOT evolve
    result_none = check_evolution(species_id, TRADE_EVO_LEVEL, ITEM_NONE)
    if result_none is not None:
        print(f"FAIL prop_item_trade_evos_need_item_at_37 (no item):")
        print(f"  species={species_id} at level {TRADE_EVO_LEVEL} without item")
        print(f"  expected None, got={result_none}")
        return False

    # Wrong item at level 37 — must NOT evolve
    wrong_item = ITEM_DRAGON_SCALE if required_item != ITEM_DRAGON_SCALE else ITEM_METAL_COAT
    result_wrong = check_evolution(species_id, TRADE_EVO_LEVEL, wrong_item)
    if result_wrong is not None:
        print(f"FAIL prop_item_trade_evos_need_item_at_37 (wrong item):")
        print(f"  species={species_id}, wrong_item={wrong_item}")
        print(f"  expected None, got={result_wrong}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_evolution_produces_correct_species
# ---------------------------------------------------------------------------

def prop_evolution_produces_correct_species(species_id: int) -> bool:
    """
    Property 13e — Req 13.5
    Each trade-evo produces the exact, correct evolved form — no wrong
    species, no null when evolution is expected.
    """
    if species_id == SPECIES_CLAMPERL:
        r1 = check_evolution(species_id, TRADE_EVO_LEVEL, ITEM_DEEP_SEA_TOOTH)
        r2 = check_evolution(species_id, TRADE_EVO_LEVEL, ITEM_DEEP_SEA_SCALE)
        if r1 != SPECIES_HUNTAIL:
            print(f"FAIL prop_evolution_produces_correct_species (Clamperl/Huntail):")
            print(f"  expected={SPECIES_HUNTAIL}, got={r1}")
            return False
        if r2 != SPECIES_GOREBYSS:
            print(f"FAIL prop_evolution_produces_correct_species (Clamperl/Gorebyss):")
            print(f"  expected={SPECIES_GOREBYSS}, got={r2}")
            return False
        return True

    if species_id not in TRADE_EVO_TABLE:
        return True

    evolved_id, required_item = TRADE_EVO_TABLE[species_id]
    item = required_item if required_item is not None else ITEM_NONE
    result = check_evolution(species_id, TRADE_EVO_LEVEL, item)

    if result != evolved_id:
        print(f"FAIL prop_evolution_produces_correct_species:")
        print(f"  species={species_id}, item={item}")
        print(f"  expected={evolved_id}, got={result}")
        return False

    return True


# ---------------------------------------------------------------------------
# Test runners
# ---------------------------------------------------------------------------

NUM_TRIALS = 100

NO_ITEM_TRADE_EVOS = [
    s for s, (_, req) in TRADE_EVO_TABLE.items()
    if req is None and s != SPECIES_CLAMPERL
]

ITEM_TRADE_EVOS = [
    s for s, (_, req) in TRADE_EVO_TABLE.items()
    if req is not None
]

ALL_TRADE_EVO_SPECIES = list(TRADE_EVO_TABLE.keys())
ALL_NORMAL_EVO_SPECIES = list(NORMAL_EVOS.keys())


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


# ---------------------------------------------------------------------------
# pytest-compatible test functions
# ---------------------------------------------------------------------------

def test_property_13a_trade_evo_species_evolve_at_37():
    print("\n=== Property 13a: Trade-Evo Species Evolve at Level 37 ===")
    print("**Validates: Requirement 13.1**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        species_id = rng.choice(NO_ITEM_TRADE_EVOS)
        level = rng.randint(TRADE_EVO_LEVEL, 100)
        return prop_trade_evo_species_evolve_at_37(species_id, level)

    passed, failed, _ = run_property("13a", trial)

    # Exhaustive: every no-item trade-evo at exactly level 37 and at 100
    for species_id in NO_ITEM_TRADE_EVOS:
        for level in (TRADE_EVO_LEVEL, 100):
            ok = prop_trade_evo_species_evolve_at_37(species_id, level)
            if ok:
                passed += 1
            else:
                failed += 1
                print(f"FAIL exhaustive: species={species_id}, level={level}")

    print(f"Property 13a Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_trade_evo_species_evolve_at_37"


def test_property_13b_no_early_evolution():
    print("\n=== Property 13b: No Evolution Before Level 37 ===")
    print("**Validates: Requirement 13.2**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        species_id = rng.choice(ALL_TRADE_EVO_SPECIES)
        level = rng.randint(1, PRE_EVO_LEVEL)
        return prop_no_early_evolution(species_id, level)

    passed, failed, _ = run_property("13b", trial)

    # Key boundary: exactly level 36 for every species
    for species_id in ALL_TRADE_EVO_SPECIES:
        ok = prop_no_early_evolution(species_id, PRE_EVO_LEVEL)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL boundary: species={species_id}, level={PRE_EVO_LEVEL}")

    # Level 1 for every species
    for species_id in ALL_TRADE_EVO_SPECIES:
        ok = prop_no_early_evolution(species_id, 1)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL boundary: species={species_id}, level=1")

    print(f"Property 13b Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_no_early_evolution"


def test_property_13c_normal_evolutions_unchanged():
    print("\n=== Property 13c: Normal Evolutions Unaffected ===")
    print("**Validates: Requirement 13.3**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        species_id = rng.choice(ALL_NORMAL_EVO_SPECIES)
        level = rng.randint(1, 100)
        return prop_normal_evolutions_unchanged(species_id, level)

    passed, failed, _ = run_property("13c", trial)

    # Boundary at evolution level for each normal species
    for species_id, (evo_level, _, _) in NORMAL_EVOS.items():
        for level in (evo_level - 1, evo_level, evo_level + 1, 100):
            ok = prop_normal_evolutions_unchanged(species_id, level)
            if ok:
                passed += 1
            else:
                failed += 1
                print(f"FAIL boundary: species={species_id}, level={level}")

    print(f"Property 13c Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_normal_evolutions_unchanged"


def test_property_13d_item_trade_evos_need_item_at_37():
    print("\n=== Property 13d: Item-Conditioned Trade Evos Need Item at 37 ===")
    print("**Validates: Requirement 13.4**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        species_id = rng.choice(ITEM_TRADE_EVOS + [SPECIES_CLAMPERL])
        return prop_item_trade_evos_need_item_at_37(species_id)

    passed, failed, _ = run_property("13d", trial)

    # Exhaustive: every item-conditioned species + Clamperl
    for species_id in ITEM_TRADE_EVOS + [SPECIES_CLAMPERL]:
        ok = prop_item_trade_evos_need_item_at_37(species_id)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL exhaustive: species={species_id}")

    print(f"Property 13d Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_item_trade_evos_need_item_at_37"


def test_property_13e_evolution_produces_correct_species():
    print("\n=== Property 13e: Each Trade-Evo Produces Correct Species ===")
    print("**Validates: Requirement 13.5**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        species_id = rng.choice(ALL_TRADE_EVO_SPECIES)
        return prop_evolution_produces_correct_species(species_id)

    passed, failed, _ = run_property("13e", trial)

    # Exhaustive: every trade-evo species
    for species_id in ALL_TRADE_EVO_SPECIES:
        ok = prop_evolution_produces_correct_species(species_id)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL exhaustive: species={species_id}")

    # Adversarial: wrong species cross-checks
    # Kadabra must NOT produce Machamp
    r = check_evolution(SPECIES_KADABRA, TRADE_EVO_LEVEL, ITEM_NONE)
    if r == SPECIES_MACHAMP:
        failed += 1
        print(f"FAIL adversarial: Kadabra produced Machamp")
    else:
        passed += 1

    # Haunter must NOT produce Alakazam
    r = check_evolution(SPECIES_HAUNTER, TRADE_EVO_LEVEL, ITEM_NONE)
    if r == SPECIES_ALAKAZAM:
        failed += 1
        print(f"FAIL adversarial: Haunter produced Alakazam")
    else:
        passed += 1

    print(f"Property 13e Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_evolution_produces_correct_species"


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("=" * 70)
    print("Property-Based Tests: Trade Evolution Alternatives")
    print("Covers: Requirement 13 — Trade evolutions trigger at level 37")
    print("=" * 70)
    print("Properties:")
    print("  13a: Trade-evo species evolve at level 37 (no item needed for base evos)")
    print("  13b: Level 36 and below never triggers trade-evo evolution")
    print("  13c: Normal evolutions (Bulbasaur at 16 etc.) are unaffected")
    print("  13d: Item-conditioned trade evos require correct item at level 37")
    print("  13e: Each trade-evo produces the exact correct evolved species")
    print(f"Minimum {NUM_TRIALS} iterations per property.")
    print("=" * 70)

    total_failed = 0

    tests = [
        test_property_13a_trade_evo_species_evolve_at_37,
        test_property_13b_no_early_evolution,
        test_property_13c_normal_evolutions_unchanged,
        test_property_13d_item_trade_evos_need_item_at_37,
        test_property_13e_evolution_produces_correct_species,
    ]

    for test_fn in tests:
        try:
            test_fn()
        except AssertionError as e:
            total_failed += 1
            print(f"  ASSERTION: {e}")

    print("\n" + "=" * 70)
    print("OVERALL TEST RESULTS")
    print("=" * 70)

    if total_failed == 0:
        print("\nALL TRADE EVOLUTION PROPERTY TESTS PASSED!")
        print("\nCoverage map (Requirement 13 -> Properties):")
        print("  13.1 (trade evos at level 37)          -> 13a")
        print("  13.2 (no evolution before 37)          -> 13b")
        print("  13.3 (normal evos unchanged)           -> 13c")
        print("  13.4 (item evos need item at 37)       -> 13d")
        print("  13.5 (correct evolved species)         -> 13e")
        return 0
    else:
        print(f"\nTESTS FAILED: {total_failed} property group(s) failed")
        return 1


if __name__ == "__main__":
    sys.exit(main())
