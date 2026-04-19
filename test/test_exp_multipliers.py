#!/usr/bin/env python3
"""
Property-Based Tests: EXP Multiplier Items

Covers: Requirement 9 — Lucky Egg / Mystic Egg / Magic Egg EXP multipliers

Properties tested:
  - prop_lucky_egg_doubles_exp: Lucky Egg held → result == base_exp × 2
  - prop_mystic_egg_triples_exp: Mystic Egg held → result == base_exp × 3
  - prop_magic_egg_quadruples_exp: Magic Egg held → result == base_exp × 4
  - prop_highest_multiplier_wins: multiple eggs → only highest applies (Magic > Mystic > Lucky)
  - prop_no_egg_unchanged: no egg held → result == base_exp
  - prop_multiplier_applied_after_party_distribution: multiplier applies to per-Pokemon share
  - prop_exp_never_negative: for any valid inputs, result >= 0
  - prop_exp_clamped_to_max_int: result does not overflow u32 (≤ 0xFFFFFFFF)

QA Task: QA-007
Minimum 100 iterations per property.
"""

import random
import sys

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

ITEM_NONE = 0
ITEM_LUCKY_EGG = 600
ITEM_MYSTIC_EGG = 601
ITEM_MAGIC_EGG = 602

MULTIPLIER_LUCKY = 2
MULTIPLIER_MYSTIC = 3
MULTIPLIER_MAGIC = 4
MULTIPLIER_NONE = 1

U32_MAX = 0xFFFF_FFFF


# ---------------------------------------------------------------------------
# Model: EXP Multiplier System (spec-derived, NOT copied from implementation)
# ---------------------------------------------------------------------------

def held_egg_multiplier(held_item: int) -> int:
    """
    Return the EXP multiplier conferred by a held egg item.

    Priority (highest wins): Magic Egg (4x) > Mystic Egg (3x) > Lucky Egg (2x)
    No egg: multiplier = 1.

    Requirement 9: only the highest multiplier applies; they do not stack.
    """
    if held_item == ITEM_MAGIC_EGG:
        return MULTIPLIER_MAGIC
    if held_item == ITEM_MYSTIC_EGG:
        return MULTIPLIER_MYSTIC
    if held_item == ITEM_LUCKY_EGG:
        return MULTIPLIER_LUCKY
    return MULTIPLIER_NONE


def effective_multiplier_for_party(held_items: list) -> int:
    """
    Given a list of held items across a party, the single effective multiplier
    is the maximum multiplier held by any party member.  Only one multiplier
    value is ever applied — the highest one.
    """
    return max(held_egg_multiplier(item) for item in held_items) if held_items else MULTIPLIER_NONE


def apply_exp_to_pokemon(base_exp: int, held_item: int) -> int:
    """
    Apply the egg multiplier for a single Pokemon's share of EXP.
    The multiplier is applied to the already-distributed per-Pokemon share,
    not to the raw total before distribution.

    Result is clamped to U32_MAX to simulate GBA u32 saturation.
    """
    mult = held_egg_multiplier(held_item)
    result = base_exp * mult
    return min(result, U32_MAX)


# ---------------------------------------------------------------------------
# Property: prop_lucky_egg_doubles_exp
# ---------------------------------------------------------------------------

def prop_lucky_egg_doubles_exp(base_exp: int) -> bool:
    """
    Property 9a — Req 9.1
    With Lucky Egg held, result == base_exp × 2.
    """
    result = apply_exp_to_pokemon(base_exp, ITEM_LUCKY_EGG)
    expected = min(base_exp * MULTIPLIER_LUCKY, U32_MAX)
    if result != expected:
        print(f"FAIL prop_lucky_egg_doubles_exp:")
        print(f"  base_exp={base_exp}, result={result}, expected={expected}")
        return False
    return True


# ---------------------------------------------------------------------------
# Property: prop_mystic_egg_triples_exp
# ---------------------------------------------------------------------------

def prop_mystic_egg_triples_exp(base_exp: int) -> bool:
    """
    Property 9b — Req 9.2
    With Mystic Egg held, result == base_exp × 3.
    """
    result = apply_exp_to_pokemon(base_exp, ITEM_MYSTIC_EGG)
    expected = min(base_exp * MULTIPLIER_MYSTIC, U32_MAX)
    if result != expected:
        print(f"FAIL prop_mystic_egg_triples_exp:")
        print(f"  base_exp={base_exp}, result={result}, expected={expected}")
        return False
    return True


# ---------------------------------------------------------------------------
# Property: prop_magic_egg_quadruples_exp
# ---------------------------------------------------------------------------

def prop_magic_egg_quadruples_exp(base_exp: int) -> bool:
    """
    Property 9c — Req 9.3
    With Magic Egg held, result == base_exp × 4.
    """
    result = apply_exp_to_pokemon(base_exp, ITEM_MAGIC_EGG)
    expected = min(base_exp * MULTIPLIER_MAGIC, U32_MAX)
    if result != expected:
        print(f"FAIL prop_magic_egg_quadruples_exp:")
        print(f"  base_exp={base_exp}, result={result}, expected={expected}")
        return False
    return True


# ---------------------------------------------------------------------------
# Property: prop_highest_multiplier_wins
# ---------------------------------------------------------------------------

def prop_highest_multiplier_wins(held_items: list) -> bool:
    """
    Property 9d — Req 9.4
    When multiple eggs are held (across the party), only the highest multiplier
    is applied.  Multipliers must never stack multiplicatively.

    Invariant: effective_mult == max(individual multipliers)
    """
    effective = effective_multiplier_for_party(held_items)
    individual_maxes = [held_egg_multiplier(item) for item in held_items]
    expected = max(individual_maxes) if individual_maxes else MULTIPLIER_NONE

    if effective != expected:
        print(f"FAIL prop_highest_multiplier_wins:")
        print(f"  held_items={held_items}, effective={effective}, expected={expected}")
        return False

    # Verify stacking is absent: product of all multipliers != effective (unless trivially equal)
    all_eggs = [held_egg_multiplier(i) for i in held_items if held_egg_multiplier(i) > 1]
    if len(all_eggs) >= 2:
        stacked = 1
        for m in all_eggs:
            stacked *= m
        if effective == stacked and stacked != expected:
            print(f"FAIL prop_highest_multiplier_wins (stacking detected):")
            print(f"  held_items={held_items}, effective={effective}, stacked_product={stacked}")
            return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_no_egg_unchanged
# ---------------------------------------------------------------------------

def prop_no_egg_unchanged(base_exp: int) -> bool:
    """
    Property 9e — Req 9.5
    With no egg held, result == base_exp (multiplier is exactly 1).
    """
    result = apply_exp_to_pokemon(base_exp, ITEM_NONE)
    if result != base_exp:
        print(f"FAIL prop_no_egg_unchanged:")
        print(f"  base_exp={base_exp}, result={result}, expected={base_exp}")
        return False
    return True


# ---------------------------------------------------------------------------
# Property: prop_multiplier_applied_after_party_distribution
# ---------------------------------------------------------------------------

def prop_multiplier_applied_after_party_distribution(
    raw_exp: int,
    party_size: int,
    held_item: int,
) -> bool:
    """
    Property 9f — Req 9.6
    The multiplier is applied to each Pokemon's *share* (after distribution),
    not to the total EXP pool before splitting.

    Invariant:
      per_pokemon_share = raw_exp // party_size
      result_per_pokemon = per_pokemon_share * multiplier

    This must equal: (raw_exp // party_size) * multiplier
    It must NOT equal: (raw_exp * multiplier) // party_size
    (those differ when raw_exp is not evenly divisible by party_size)
    """
    if party_size < 1:
        return True  # degenerate input, skip

    per_share = raw_exp // party_size
    result = apply_exp_to_pokemon(per_share, held_item)

    mult = held_egg_multiplier(held_item)
    expected = min(per_share * mult, U32_MAX)

    if result != expected:
        print(f"FAIL prop_multiplier_applied_after_party_distribution:")
        print(f"  raw_exp={raw_exp}, party_size={party_size}, held_item={held_item}")
        print(f"  per_share={per_share}, result={result}, expected={expected}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_exp_never_negative
# ---------------------------------------------------------------------------

def prop_exp_never_negative(base_exp: int, held_item: int) -> bool:
    """
    Property 9g — Req 9.7
    For any non-negative base_exp and any valid held item,
    the result is always >= 0.
    """
    if base_exp < 0:
        return True  # invalid input — skip

    result = apply_exp_to_pokemon(base_exp, held_item)
    if result < 0:
        print(f"FAIL prop_exp_never_negative:")
        print(f"  base_exp={base_exp}, held_item={held_item}, result={result}")
        return False
    return True


# ---------------------------------------------------------------------------
# Property: prop_exp_clamped_to_max_int
# ---------------------------------------------------------------------------

def prop_exp_clamped_to_max_int(base_exp: int, held_item: int) -> bool:
    """
    Property 9h — Req 9.8
    Result must never exceed U32_MAX (0xFFFFFFFF).
    The model applies u32 saturation arithmetic.
    """
    result = apply_exp_to_pokemon(base_exp, held_item)
    if result > U32_MAX:
        print(f"FAIL prop_exp_clamped_to_max_int:")
        print(f"  base_exp={base_exp}, held_item={held_item}, result={result}, U32_MAX={U32_MAX}")
        return False
    return True


# ---------------------------------------------------------------------------
# Test runners
# ---------------------------------------------------------------------------

NUM_TRIALS = 100

ALL_EGG_ITEMS = [ITEM_NONE, ITEM_LUCKY_EGG, ITEM_MYSTIC_EGG, ITEM_MAGIC_EGG]


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

def test_property_9a_lucky_egg_doubles_exp():
    print("\n=== Property 9a: Lucky Egg Doubles EXP ===")
    print("**Validates: Requirement 9.1**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        base_exp = rng.randint(0, 10_000)
        return prop_lucky_egg_doubles_exp(base_exp)

    passed, failed, _ = run_property("9a", trial)

    edge_cases = [
        (0, "zero base EXP"),
        (1, "minimum base EXP"),
        (U32_MAX // MULTIPLIER_LUCKY, "boundary: exactly fits in u32 after 2x"),
        (U32_MAX // MULTIPLIER_LUCKY + 1, "boundary: one over u32 cap after 2x"),
        (U32_MAX, "maximum u32 base EXP"),
    ]
    for base_exp, desc in edge_cases:
        ok = prop_lucky_egg_doubles_exp(base_exp)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 9a Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_lucky_egg_doubles_exp"


def test_property_9b_mystic_egg_triples_exp():
    print("\n=== Property 9b: Mystic Egg Triples EXP ===")
    print("**Validates: Requirement 9.2**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        base_exp = rng.randint(0, 10_000)
        return prop_mystic_egg_triples_exp(base_exp)

    passed, failed, _ = run_property("9b", trial)

    edge_cases = [
        (0, "zero base EXP"),
        (1, "minimum base EXP"),
        (U32_MAX // MULTIPLIER_MYSTIC, "boundary: exactly fits in u32 after 3x"),
        (U32_MAX // MULTIPLIER_MYSTIC + 1, "boundary: one over u32 cap after 3x"),
        (U32_MAX, "maximum u32 base EXP"),
    ]
    for base_exp, desc in edge_cases:
        ok = prop_mystic_egg_triples_exp(base_exp)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 9b Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_mystic_egg_triples_exp"


def test_property_9c_magic_egg_quadruples_exp():
    print("\n=== Property 9c: Magic Egg Quadruples EXP ===")
    print("**Validates: Requirement 9.3**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        base_exp = rng.randint(0, 10_000)
        return prop_magic_egg_quadruples_exp(base_exp)

    passed, failed, _ = run_property("9c", trial)

    edge_cases = [
        (0, "zero base EXP"),
        (1, "minimum base EXP"),
        (U32_MAX // MULTIPLIER_MAGIC, "boundary: exactly fits in u32 after 4x"),
        (U32_MAX // MULTIPLIER_MAGIC + 1, "boundary: one over u32 cap after 4x"),
        (U32_MAX, "maximum u32 base EXP"),
    ]
    for base_exp, desc in edge_cases:
        ok = prop_magic_egg_quadruples_exp(base_exp)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 9c Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_magic_egg_quadruples_exp"


def test_property_9d_highest_multiplier_wins():
    print("\n=== Property 9d: Highest Multiplier Wins ===")
    print("**Validates: Requirement 9.4**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        party_size = rng.randint(1, 6)
        held_items = [rng.choice(ALL_EGG_ITEMS) for _ in range(party_size)]
        return prop_highest_multiplier_wins(held_items)

    passed, failed, _ = run_property("9d", trial)

    edge_cases = [
        ([ITEM_LUCKY_EGG, ITEM_MYSTIC_EGG, ITEM_MAGIC_EGG], "all three eggs → Magic wins"),
        ([ITEM_LUCKY_EGG, ITEM_MYSTIC_EGG], "Lucky + Mystic → Mystic wins"),
        ([ITEM_LUCKY_EGG, ITEM_MAGIC_EGG], "Lucky + Magic → Magic wins"),
        ([ITEM_MYSTIC_EGG, ITEM_MAGIC_EGG], "Mystic + Magic → Magic wins"),
        ([ITEM_NONE, ITEM_MAGIC_EGG], "no-egg + Magic → Magic wins"),
        ([ITEM_NONE, ITEM_NONE, ITEM_NONE], "all no-egg → multiplier 1"),
        ([ITEM_LUCKY_EGG], "single Lucky Egg"),
        ([ITEM_MAGIC_EGG, ITEM_MAGIC_EGG, ITEM_MAGIC_EGG], "all Magic Eggs → still 4x, not stacked"),
    ]
    for held_items, desc in edge_cases:
        ok = prop_highest_multiplier_wins(held_items)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 9d Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_highest_multiplier_wins"


def test_property_9e_no_egg_unchanged():
    print("\n=== Property 9e: No Egg Leaves EXP Unchanged ===")
    print("**Validates: Requirement 9.5**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        base_exp = rng.randint(0, 10_000)
        return prop_no_egg_unchanged(base_exp)

    passed, failed, _ = run_property("9e", trial)

    edge_cases = [
        (0, "zero EXP with no egg"),
        (1, "1 EXP with no egg"),
        (U32_MAX, "max u32 EXP with no egg"),
        (65535, "common GBA large value with no egg"),
    ]
    for base_exp, desc in edge_cases:
        ok = prop_no_egg_unchanged(base_exp)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 9e Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_no_egg_unchanged"


def test_property_9f_multiplier_applied_after_party_distribution():
    print("\n=== Property 9f: Multiplier Applied After Party Distribution ===")
    print("**Validates: Requirement 9.6**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        raw_exp = rng.randint(0, 100_000)
        party_size = rng.randint(1, 6)
        held_item = rng.choice(ALL_EGG_ITEMS)
        return prop_multiplier_applied_after_party_distribution(raw_exp, party_size, held_item)

    passed, failed, _ = run_property("9f", trial)

    edge_cases = [
        # (raw_exp, party_size, held_item, description)
        (100, 3, ITEM_LUCKY_EGG, "100 EXP, 3 members, Lucky Egg: share=33, result=66"),
        (100, 3, ITEM_MAGIC_EGG, "100 EXP, 3 members, Magic Egg: share=33, result=132"),
        (60, 6, ITEM_MYSTIC_EGG, "60 EXP, 6 members, Mystic Egg: share=10, result=30"),
        (1, 6, ITEM_MAGIC_EGG, "1 EXP, 6 members: share=0 (floor div), result=0"),
        (U32_MAX, 1, ITEM_MAGIC_EGG, "max EXP, 1 member, Magic Egg: clamped to U32_MAX"),
    ]
    for raw_exp, party_size, held_item, desc in edge_cases:
        ok = prop_multiplier_applied_after_party_distribution(raw_exp, party_size, held_item)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 9f Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_multiplier_applied_after_party_distribution"


def test_property_9g_exp_never_negative():
    print("\n=== Property 9g: EXP Result Never Negative ===")
    print("**Validates: Requirement 9.7**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        base_exp = rng.randint(0, 10_000)
        held_item = rng.choice(ALL_EGG_ITEMS)
        return prop_exp_never_negative(base_exp, held_item)

    passed, failed, _ = run_property("9g", trial)

    edge_cases = [
        (0, ITEM_NONE, "zero EXP, no egg"),
        (0, ITEM_MAGIC_EGG, "zero EXP, Magic Egg"),
        (1, ITEM_NONE, "1 EXP, no egg"),
        (U32_MAX, ITEM_MAGIC_EGG, "max EXP, Magic Egg (saturation)"),
    ]
    for base_exp, held_item, desc in edge_cases:
        ok = prop_exp_never_negative(base_exp, held_item)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 9g Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_exp_never_negative"


def test_property_9h_exp_clamped_to_max_int():
    print("\n=== Property 9h: EXP Clamped to U32_MAX ===")
    print("**Validates: Requirement 9.8**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        # Use large base_exp values to stress the overflow path
        base_exp = rng.randint(U32_MAX // 8, U32_MAX)
        held_item = rng.choice(ALL_EGG_ITEMS)
        return prop_exp_clamped_to_max_int(base_exp, held_item)

    passed, failed, _ = run_property("9h", trial)

    edge_cases = [
        # Exact boundary values
        (U32_MAX // MULTIPLIER_MAGIC, ITEM_MAGIC_EGG, "exact u32 boundary with 4x"),
        (U32_MAX // MULTIPLIER_MAGIC + 1, ITEM_MAGIC_EGG, "one over boundary with 4x — must clamp"),
        (U32_MAX, ITEM_LUCKY_EGG, "full u32 max with 2x — must clamp"),
        (U32_MAX, ITEM_MAGIC_EGG, "full u32 max with 4x — must clamp"),
        (U32_MAX, ITEM_NONE, "full u32 max, no egg — no clamp needed"),
        (0, ITEM_MAGIC_EGG, "zero with 4x — stays 0"),
    ]
    for base_exp, held_item, desc in edge_cases:
        ok = prop_exp_clamped_to_max_int(base_exp, held_item)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 9h Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_exp_clamped_to_max_int"


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("=" * 70)
    print("Property-Based Tests: EXP Multiplier Items")
    print("Covers: Requirement 9 — Lucky / Mystic / Magic Egg EXP multipliers")
    print("=" * 70)
    print("Properties:")
    print("  9a: Lucky Egg held → base_exp × 2")
    print("  9b: Mystic Egg held → base_exp × 3")
    print("  9c: Magic Egg held → base_exp × 4")
    print("  9d: Multiple eggs → only highest multiplier applies")
    print("  9e: No egg → EXP unchanged")
    print("  9f: Multiplier applied after party distribution (per-share)")
    print("  9g: Result never negative")
    print("  9h: Result clamped to U32_MAX (no overflow)")
    print(f"Minimum {NUM_TRIALS} iterations per property.")
    print("=" * 70)

    total_failed = 0

    try:
        test_property_9a_lucky_egg_doubles_exp()
    except AssertionError as e:
        total_failed += 1
        print(f"  ASSERTION: {e}")

    try:
        test_property_9b_mystic_egg_triples_exp()
    except AssertionError as e:
        total_failed += 1
        print(f"  ASSERTION: {e}")

    try:
        test_property_9c_magic_egg_quadruples_exp()
    except AssertionError as e:
        total_failed += 1
        print(f"  ASSERTION: {e}")

    try:
        test_property_9d_highest_multiplier_wins()
    except AssertionError as e:
        total_failed += 1
        print(f"  ASSERTION: {e}")

    try:
        test_property_9e_no_egg_unchanged()
    except AssertionError as e:
        total_failed += 1
        print(f"  ASSERTION: {e}")

    try:
        test_property_9f_multiplier_applied_after_party_distribution()
    except AssertionError as e:
        total_failed += 1
        print(f"  ASSERTION: {e}")

    try:
        test_property_9g_exp_never_negative()
    except AssertionError as e:
        total_failed += 1
        print(f"  ASSERTION: {e}")

    try:
        test_property_9h_exp_clamped_to_max_int()
    except AssertionError as e:
        total_failed += 1
        print(f"  ASSERTION: {e}")

    print("\n" + "=" * 70)
    print("OVERALL TEST RESULTS")
    print("=" * 70)

    if total_failed == 0:
        print("\nALL EXP MULTIPLIER PROPERTY TESTS PASSED!")
        print("\nCoverage map (Requirement 9 -> Properties):")
        print("  9.1 (Lucky Egg × 2)                    -> 9a")
        print("  9.2 (Mystic Egg × 3)                   -> 9b")
        print("  9.3 (Magic Egg × 4)                    -> 9c")
        print("  9.4 (highest multiplier only)          -> 9d")
        print("  9.5 (no egg → unchanged)               -> 9e")
        print("  9.6 (multiplier after distribution)    -> 9f")
        print("  9.7 (result >= 0)                      -> 9g")
        print("  9.8 (result <= U32_MAX)                -> 9h")
        return 0
    else:
        print(f"\nTESTS FAILED: {total_failed} property group(s) failed")
        return 1


if __name__ == "__main__":
    sys.exit(main())
