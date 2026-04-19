#!/usr/bin/env python3
"""
Property-Based Tests: Starter Distribution

Covers: Requirement 14 — After each region's Elite Four, the unchosen starters
        from that region become obtainable

Starter sets (per spec):
  Kanto:  Bulbasaur (1), Charmander (4), Squirtle (7)
  Hoenn:  Treecko (252), Torchic (255), Mudkip (258)
  Johto:  Chikorita (152), Cyndaquil (155), Totodile (158)

Properties tested:
  - prop_kanto_e4_awards_unchosen_starters: after Kanto E4, the 2 unchosen
    Kanto starters become available
  - prop_chosen_starter_not_re_awarded: the chosen starter is NOT in the
    post-E4 award pool
  - prop_hoenn_e4_awards_all_hoenn_starters: after Hoenn E4, all 3 Hoenn
    starters available
  - prop_johto_e4_awards_all_johto_starters: after Johto E4, all 3 Johto
    starters available
  - prop_starters_not_available_before_e4: before E4 defeat, starters locked
  - prop_starter_pool_correct_size: Kanto post-E4 pool == 2; Hoenn/Johto == 3

QA Task: QA-011
Minimum 100 iterations per property.
"""

import random
import sys
from typing import List, Optional, Set

# ---------------------------------------------------------------------------
# Constants: Starter Pokemon
# ---------------------------------------------------------------------------

# Kanto starters (National Dex IDs)
BULBASAUR = 1
CHARMANDER = 4
SQUIRTLE = 7
KANTO_STARTERS: List[int] = [BULBASAUR, CHARMANDER, SQUIRTLE]

# Hoenn starters
TREECKO = 252
TORCHIC = 255
MUDKIP = 258
HOENN_STARTERS: List[int] = [TREECKO, TORCHIC, MUDKIP]

# Johto starters
CHIKORITA = 152
CYNDAQUIL = 155
TOTODILE = 158
JOHTO_STARTERS: List[int] = [CHIKORITA, CYNDAQUIL, TOTODILE]

# Region IDs
REGION_KANTO = 0
REGION_HOENN = 1
REGION_JOHTO = 2

REGION_STARTERS = {
    REGION_KANTO: KANTO_STARTERS,
    REGION_HOENN: HOENN_STARTERS,
    REGION_JOHTO: JOHTO_STARTERS,
}


# ---------------------------------------------------------------------------
# Model: Starter Distribution System
# ---------------------------------------------------------------------------

class StarterDistribution:
    """
    Spec model of the post-Elite Four starter distribution system.

    Requirement 14 rules:
      - Kanto E4 defeated: the 2 Kanto starters the player did NOT choose
        become available.  The chosen one is excluded.
      - Hoenn E4 defeated: ALL 3 Hoenn starters become available
        (player did not start in Hoenn, so no starter was chosen there).
      - Johto E4 defeated: ALL 3 Johto starters become available.
      - Before E4 defeat: no post-E4 starters are accessible.
    """

    def __init__(self, kanto_chosen: int):
        """
        kanto_chosen: the Kanto starter the player chose at game start.
        """
        assert kanto_chosen in KANTO_STARTERS, f"Invalid Kanto starter: {kanto_chosen}"
        self.kanto_chosen: int = kanto_chosen
        self.e4_defeated: dict = {
            REGION_KANTO: False,
            REGION_HOENN: False,
            REGION_JOHTO: False,
        }

    def defeat_e4(self, region_id: int) -> None:
        if region_id in self.e4_defeated:
            self.e4_defeated[region_id] = True

    def available_post_e4_starters(self, region_id: int) -> List[int]:
        """
        Returns the list of starters available after the given region's E4
        is defeated.  Empty list if E4 not yet defeated.
        """
        if not self.e4_defeated.get(region_id, False):
            return []

        if region_id == REGION_KANTO:
            # Only the 2 unchosen Kanto starters
            return [s for s in KANTO_STARTERS if s != self.kanto_chosen]

        # Hoenn and Johto: all 3, because player never chose one there
        return list(REGION_STARTERS.get(region_id, []))


# ---------------------------------------------------------------------------
# Property: prop_kanto_e4_awards_unchosen_starters
# ---------------------------------------------------------------------------

def prop_kanto_e4_awards_unchosen_starters(chosen_starter: int) -> bool:
    """
    Property 14a — Req 14.1
    After defeating the Kanto E4, the 2 Kanto starters the player did NOT
    choose become available.
    """
    dist = StarterDistribution(chosen_starter)
    dist.defeat_e4(REGION_KANTO)

    pool = dist.available_post_e4_starters(REGION_KANTO)
    expected = [s for s in KANTO_STARTERS if s != chosen_starter]

    if set(pool) != set(expected):
        print(f"FAIL prop_kanto_e4_awards_unchosen_starters:")
        print(f"  chosen={chosen_starter}, pool={pool}, expected={expected}")
        return False

    # Chosen starter must NOT be in the pool
    if chosen_starter in pool:
        print(f"FAIL prop_kanto_e4_awards_unchosen_starters: chosen starter in pool")
        print(f"  chosen={chosen_starter}, pool={pool}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_chosen_starter_not_re_awarded
# ---------------------------------------------------------------------------

def prop_chosen_starter_not_re_awarded(chosen_starter: int) -> bool:
    """
    Property 14b — Req 14.2
    The Kanto starter the player originally chose is never in the post-E4
    award pool, regardless of which E4s have been defeated.
    """
    dist = StarterDistribution(chosen_starter)

    # Defeat all E4s
    for region_id in (REGION_KANTO, REGION_HOENN, REGION_JOHTO):
        dist.defeat_e4(region_id)

    kanto_pool = dist.available_post_e4_starters(REGION_KANTO)

    if chosen_starter in kanto_pool:
        print(f"FAIL prop_chosen_starter_not_re_awarded:")
        print(f"  chosen={chosen_starter} appears in Kanto post-E4 pool={kanto_pool}")
        return False

    # Hoenn and Johto pools should not contain Kanto starters at all
    for region_id in (REGION_HOENN, REGION_JOHTO):
        pool = dist.available_post_e4_starters(region_id)
        for kanto_starter in KANTO_STARTERS:
            if kanto_starter in pool:
                print(f"FAIL prop_chosen_starter_not_re_awarded:")
                print(f"  Kanto starter {kanto_starter} found in region {region_id} pool={pool}")
                return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_hoenn_e4_awards_all_hoenn_starters
# ---------------------------------------------------------------------------

def prop_hoenn_e4_awards_all_hoenn_starters(chosen_starter: int) -> bool:
    """
    Property 14c — Req 14.3
    After defeating the Hoenn E4, all three Hoenn starters (Treecko, Torchic,
    Mudkip) become available, regardless of which Kanto starter was chosen.
    """
    dist = StarterDistribution(chosen_starter)
    dist.defeat_e4(REGION_HOENN)

    pool = dist.available_post_e4_starters(REGION_HOENN)

    if set(pool) != set(HOENN_STARTERS):
        print(f"FAIL prop_hoenn_e4_awards_all_hoenn_starters:")
        print(f"  chosen_kanto={chosen_starter}, pool={pool}, expected={HOENN_STARTERS}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_johto_e4_awards_all_johto_starters
# ---------------------------------------------------------------------------

def prop_johto_e4_awards_all_johto_starters(chosen_starter: int) -> bool:
    """
    Property 14d — Req 14.4
    After defeating the Johto E4, all three Johto starters (Chikorita,
    Cyndaquil, Totodile) become available.
    """
    dist = StarterDistribution(chosen_starter)
    dist.defeat_e4(REGION_JOHTO)

    pool = dist.available_post_e4_starters(REGION_JOHTO)

    if set(pool) != set(JOHTO_STARTERS):
        print(f"FAIL prop_johto_e4_awards_all_johto_starters:")
        print(f"  chosen_kanto={chosen_starter}, pool={pool}, expected={JOHTO_STARTERS}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_starters_not_available_before_e4
# ---------------------------------------------------------------------------

def prop_starters_not_available_before_e4(
    chosen_starter: int,
    region_id: int,
) -> bool:
    """
    Property 14e — Req 14.5
    Before the corresponding E4 is defeated, no post-E4 starters are
    accessible for that region.
    """
    if region_id not in REGION_STARTERS:
        return True

    dist = StarterDistribution(chosen_starter)
    # E4 NOT defeated for this region
    pool = dist.available_post_e4_starters(region_id)

    if pool:
        print(f"FAIL prop_starters_not_available_before_e4:")
        print(f"  region={region_id}, pool={pool} — should be empty before E4 defeat")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_starter_pool_correct_size
# ---------------------------------------------------------------------------

def prop_starter_pool_correct_size(chosen_starter: int) -> bool:
    """
    Property 14f — Req 14.6
    Pool sizes after E4 defeat:
      Kanto post-E4: exactly 2 Pokemon
      Hoenn post-E4: exactly 3 Pokemon
      Johto post-E4: exactly 3 Pokemon
    """
    dist = StarterDistribution(chosen_starter)

    # Defeat all E4s
    for region_id in (REGION_KANTO, REGION_HOENN, REGION_JOHTO):
        dist.defeat_e4(region_id)

    kanto_pool = dist.available_post_e4_starters(REGION_KANTO)
    hoenn_pool = dist.available_post_e4_starters(REGION_HOENN)
    johto_pool = dist.available_post_e4_starters(REGION_JOHTO)

    if len(kanto_pool) != 2:
        print(f"FAIL prop_starter_pool_correct_size (Kanto):")
        print(f"  chosen={chosen_starter}, pool={kanto_pool}, size={len(kanto_pool)}, expected=2")
        return False

    if len(hoenn_pool) != 3:
        print(f"FAIL prop_starter_pool_correct_size (Hoenn):")
        print(f"  pool={hoenn_pool}, size={len(hoenn_pool)}, expected=3")
        return False

    if len(johto_pool) != 3:
        print(f"FAIL prop_starter_pool_correct_size (Johto):")
        print(f"  pool={johto_pool}, size={len(johto_pool)}, expected=3")
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


# ---------------------------------------------------------------------------
# pytest-compatible test functions
# ---------------------------------------------------------------------------

def test_property_14a_kanto_e4_awards_unchosen_starters():
    print("\n=== Property 14a: Kanto E4 Awards Unchosen Starters ===")
    print("**Validates: Requirement 14.1**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        chosen = rng.choice(KANTO_STARTERS)
        return prop_kanto_e4_awards_unchosen_starters(chosen)

    passed, failed, _ = run_property("14a", trial)

    # Exhaustive: every possible chosen starter
    for chosen in KANTO_STARTERS:
        ok = prop_kanto_e4_awards_unchosen_starters(chosen)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL exhaustive: chosen={chosen}")

    print(f"Property 14a Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_kanto_e4_awards_unchosen_starters"


def test_property_14b_chosen_starter_not_re_awarded():
    print("\n=== Property 14b: Chosen Starter Not Re-Awarded ===")
    print("**Validates: Requirement 14.2**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        chosen = rng.choice(KANTO_STARTERS)
        return prop_chosen_starter_not_re_awarded(chosen)

    passed, failed, _ = run_property("14b", trial)

    for chosen in KANTO_STARTERS:
        ok = prop_chosen_starter_not_re_awarded(chosen)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL exhaustive: chosen={chosen}")

    print(f"Property 14b Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_chosen_starter_not_re_awarded"


def test_property_14c_hoenn_e4_awards_all_hoenn_starters():
    print("\n=== Property 14c: Hoenn E4 Awards All Hoenn Starters ===")
    print("**Validates: Requirement 14.3**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        chosen = rng.choice(KANTO_STARTERS)
        return prop_hoenn_e4_awards_all_hoenn_starters(chosen)

    passed, failed, _ = run_property("14c", trial)

    for chosen in KANTO_STARTERS:
        ok = prop_hoenn_e4_awards_all_hoenn_starters(chosen)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL exhaustive: chosen_kanto={chosen}")

    print(f"Property 14c Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_hoenn_e4_awards_all_hoenn_starters"


def test_property_14d_johto_e4_awards_all_johto_starters():
    print("\n=== Property 14d: Johto E4 Awards All Johto Starters ===")
    print("**Validates: Requirement 14.4**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        chosen = rng.choice(KANTO_STARTERS)
        return prop_johto_e4_awards_all_johto_starters(chosen)

    passed, failed, _ = run_property("14d", trial)

    for chosen in KANTO_STARTERS:
        ok = prop_johto_e4_awards_all_johto_starters(chosen)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL exhaustive: chosen_kanto={chosen}")

    print(f"Property 14d Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_johto_e4_awards_all_johto_starters"


def test_property_14e_starters_not_available_before_e4():
    print("\n=== Property 14e: Starters Locked Before E4 Defeat ===")
    print("**Validates: Requirement 14.5**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        chosen = rng.choice(KANTO_STARTERS)
        region_id = rng.choice([REGION_KANTO, REGION_HOENN, REGION_JOHTO])
        return prop_starters_not_available_before_e4(chosen, region_id)

    passed, failed, _ = run_property("14e", trial)

    # Exhaustive: every chosen × every region before E4
    for chosen in KANTO_STARTERS:
        for region_id in (REGION_KANTO, REGION_HOENN, REGION_JOHTO):
            ok = prop_starters_not_available_before_e4(chosen, region_id)
            if ok:
                passed += 1
            else:
                failed += 1
                print(f"FAIL exhaustive: chosen={chosen}, region={region_id}")

    print(f"Property 14e Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_starters_not_available_before_e4"


def test_property_14f_starter_pool_correct_size():
    print("\n=== Property 14f: Starter Pool Sizes Correct ===")
    print("**Validates: Requirement 14.6**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        chosen = rng.choice(KANTO_STARTERS)
        return prop_starter_pool_correct_size(chosen)

    passed, failed, _ = run_property("14f", trial)

    for chosen in KANTO_STARTERS:
        ok = prop_starter_pool_correct_size(chosen)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL exhaustive: chosen={chosen}")

    print(f"Property 14f Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_starter_pool_correct_size"


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("=" * 70)
    print("Property-Based Tests: Starter Distribution")
    print("Covers: Requirement 14 — Post-E4 starter award pools")
    print("=" * 70)
    print("Properties:")
    print("  14a: Kanto E4 awards the 2 unchosen Kanto starters")
    print("  14b: Chosen starter is never re-awarded")
    print("  14c: Hoenn E4 awards all 3 Hoenn starters")
    print("  14d: Johto E4 awards all 3 Johto starters")
    print("  14e: Starters are locked before the relevant E4 is defeated")
    print("  14f: Pool sizes: Kanto=2, Hoenn=3, Johto=3")
    print(f"Minimum {NUM_TRIALS} iterations per property.")
    print("=" * 70)

    total_failed = 0

    tests = [
        test_property_14a_kanto_e4_awards_unchosen_starters,
        test_property_14b_chosen_starter_not_re_awarded,
        test_property_14c_hoenn_e4_awards_all_hoenn_starters,
        test_property_14d_johto_e4_awards_all_johto_starters,
        test_property_14e_starters_not_available_before_e4,
        test_property_14f_starter_pool_correct_size,
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
        print("\nALL STARTER DISTRIBUTION PROPERTY TESTS PASSED!")
        print("\nCoverage map (Requirement 14 -> Properties):")
        print("  14.1 (Kanto E4 -> unchosen starters)  -> 14a")
        print("  14.2 (chosen not re-awarded)           -> 14b")
        print("  14.3 (Hoenn E4 -> all Hoenn starters)  -> 14c")
        print("  14.4 (Johto E4 -> all Johto starters)  -> 14d")
        print("  14.5 (locked before E4)                -> 14e")
        print("  14.6 (pool sizes: Kanto=2, others=3)   -> 14f")
        return 0
    else:
        print(f"\nTESTS FAILED: {total_failed} property group(s) failed")
        return 1


if __name__ == "__main__":
    sys.exit(main())
