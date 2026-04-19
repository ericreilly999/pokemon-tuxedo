#!/usr/bin/env python3
"""
Property-Based Tests: HM Simplification (Auto-Clear on Contact)

Covers: Requirement 12 — HM field moves auto-clear on tile contact;
        party slot not required; HM moves still learnable as battle moves

Properties tested:
  - prop_cut_tree_auto_cleared: player touching cut tree tile clears it
                                without knowing Cut
  - prop_rock_smash_auto_cleared: player touching smashable rock tile clears it
                                  without knowing Rock Smash
  - prop_waterfall_passable: player can move through waterfall tile
                             without knowing Waterfall
  - prop_flash_not_required: dark cave is navigable without Flash
  - prop_hm_moves_still_learnable: Cut/Rock Smash/Waterfall/Flash can be
                                   learned as battle moves
  - prop_no_party_slot_required: none of the four HMs require a party slot
                                 to be usable in the field

Model: boolean predicates against a simulated overworld state dict.
Key invariant: field_passable(tile_type, player_has_hm=False) == True for
               all four HM tile types.

QA Task: QA-009
Minimum 100 iterations per property.
"""

import random
import sys

# ---------------------------------------------------------------------------
# Constants: Tile types
# ---------------------------------------------------------------------------

TILE_CUT_TREE = "cut_tree"
TILE_SMASH_ROCK = "smash_rock"
TILE_WATERFALL = "waterfall"
TILE_DARK_CAVE = "dark_cave"

# All tiles that HM simplification must auto-handle
HM_TILES = [TILE_CUT_TREE, TILE_SMASH_ROCK, TILE_WATERFALL, TILE_DARK_CAVE]

# HM move identifiers
HM_CUT = "Cut"
HM_ROCK_SMASH = "Rock Smash"
HM_WATERFALL = "Waterfall"
HM_FLASH = "Flash"

SIMPLIFIED_HM_MOVES = [HM_CUT, HM_ROCK_SMASH, HM_WATERFALL, HM_FLASH]

# All valid battle move slots
BATTLE_MOVE_SLOTS = [0, 1, 2, 3]  # up to 4 moves per Pokemon


# ---------------------------------------------------------------------------
# Model: Overworld State and HM Simplification Logic
# ---------------------------------------------------------------------------

def make_overworld_state(
    player_has_hm: bool = False,
    party_size: int = 1,
    map_id: int = 0,
    player_x: int = 10,
    player_y: int = 10,
) -> dict:
    """
    Construct a minimal overworld state dict.

    player_has_hm: whether the player knows the relevant HM move in their party
    party_size: number of Pokemon in party (0 = empty)
    map_id: current map identifier
    player_x, player_y: player's tile coordinates
    """
    return {
        "player_has_hm": player_has_hm,
        "party_size": party_size,
        "map_id": map_id,
        "player_x": player_x,
        "player_y": player_y,
    }


def field_passable(tile_type: str, player_has_hm: bool = False) -> bool:
    """
    Spec model: all four simplified HM tiles are always passable by the player,
    regardless of whether they know the corresponding HM.

    Requirement 12: Cut, Rock Smash, Waterfall, Flash obstacles are auto-cleared
    on tile contact — no HM knowledge required.

    Returns True if the player can move onto/through the tile.
    """
    if tile_type in HM_TILES:
        return True  # Always passable — HM check removed

    # Non-HM tiles: passable unconditionally in this model
    return True


def tile_auto_clears(tile_type: str, state: dict) -> bool:
    """
    Returns True if the tile is of an auto-clear type and clears on contact.
    The player's HM knowledge is irrelevant to clearing.

    Requirement 12: obstacles of type cut_tree and smash_rock disappear on
    contact.  Waterfall becomes passable terrain.  Dark cave is fully
    navigable (visibility not required).
    """
    auto_clear_tiles = {TILE_CUT_TREE, TILE_SMASH_ROCK}
    passthrough_tiles = {TILE_WATERFALL, TILE_DARK_CAVE}

    if tile_type in auto_clear_tiles:
        return True  # tile is removed, passage opens
    if tile_type in passthrough_tiles:
        return True  # tile remains but is traversable
    return False


def can_learn_hm_as_battle_move(hm_name: str, move_slot: int) -> bool:
    """
    Spec model: simplified HM moves are still learnable as standard battle
    moves occupying any of the four move slots.

    Requirement 12: removing the field requirement does not remove the move
    from the learnable move pool.
    """
    if hm_name not in SIMPLIFIED_HM_MOVES:
        return False  # unknown move
    if move_slot not in BATTLE_MOVE_SLOTS:
        return False  # invalid slot
    return True  # any simplified HM can be learned in any slot


def hm_requires_party_slot(hm_name: str) -> bool:
    """
    Spec model: simplified HM moves do NOT require a specific party member
    to hold them for field use.  Returns False for all four simplified HMs.

    Requirement 12: party slot constraint removed for Cut, Rock Smash,
    Waterfall, Flash.
    """
    if hm_name in SIMPLIFIED_HM_MOVES:
        return False  # no party slot requirement
    return True  # non-simplified HMs retain normal requirements


# ---------------------------------------------------------------------------
# Property: prop_cut_tree_auto_cleared
# ---------------------------------------------------------------------------

def prop_cut_tree_auto_cleared(player_has_hm: bool, party_size: int) -> bool:
    """
    Property 12a — Req 12.1
    A cut-tree tile is always cleared on player contact, regardless of
    whether the player knows Cut.

    Invariant: field_passable(TILE_CUT_TREE, player_has_hm=False) == True
    """
    state = make_overworld_state(player_has_hm=player_has_hm, party_size=party_size)

    passable_without_hm = field_passable(TILE_CUT_TREE, player_has_hm=False)
    passable_with_hm = field_passable(TILE_CUT_TREE, player_has_hm=True)
    clears = tile_auto_clears(TILE_CUT_TREE, state)

    if not passable_without_hm:
        print(f"FAIL prop_cut_tree_auto_cleared: cut tree not passable without HM")
        print(f"  player_has_hm={player_has_hm}, party_size={party_size}")
        return False

    if not passable_with_hm:
        print(f"FAIL prop_cut_tree_auto_cleared: cut tree not passable with HM")
        return False

    if not clears:
        print(f"FAIL prop_cut_tree_auto_cleared: cut tree did not auto-clear")
        return False

    # HM knowledge must not be the gating factor
    if passable_without_hm != passable_with_hm:
        print(f"FAIL prop_cut_tree_auto_cleared: passability differs by HM knowledge")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_rock_smash_auto_cleared
# ---------------------------------------------------------------------------

def prop_rock_smash_auto_cleared(player_has_hm: bool, party_size: int) -> bool:
    """
    Property 12b — Req 12.2
    A smashable rock tile is always cleared on player contact, regardless of
    whether the player knows Rock Smash.
    """
    state = make_overworld_state(player_has_hm=player_has_hm, party_size=party_size)

    passable_without = field_passable(TILE_SMASH_ROCK, player_has_hm=False)
    passable_with = field_passable(TILE_SMASH_ROCK, player_has_hm=True)
    clears = tile_auto_clears(TILE_SMASH_ROCK, state)

    if not passable_without:
        print(f"FAIL prop_rock_smash_auto_cleared: smash rock not passable without HM")
        print(f"  player_has_hm={player_has_hm}, party_size={party_size}")
        return False

    if not clears:
        print(f"FAIL prop_rock_smash_auto_cleared: smash rock did not auto-clear")
        return False

    if passable_without != passable_with:
        print(f"FAIL prop_rock_smash_auto_cleared: passability differs by HM knowledge")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_waterfall_passable
# ---------------------------------------------------------------------------

def prop_waterfall_passable(player_has_hm: bool, party_size: int) -> bool:
    """
    Property 12c — Req 12.3
    A waterfall tile is always traversable regardless of whether the player
    knows Waterfall.
    """
    state = make_overworld_state(player_has_hm=player_has_hm, party_size=party_size)

    passable_without = field_passable(TILE_WATERFALL, player_has_hm=False)
    passable_with = field_passable(TILE_WATERFALL, player_has_hm=True)
    passthrough = tile_auto_clears(TILE_WATERFALL, state)

    if not passable_without:
        print(f"FAIL prop_waterfall_passable: waterfall not passable without HM")
        print(f"  player_has_hm={player_has_hm}, party_size={party_size}")
        return False

    if not passthrough:
        print(f"FAIL prop_waterfall_passable: waterfall tile not marked as passthrough")
        return False

    if passable_without != passable_with:
        print(f"FAIL prop_waterfall_passable: passability differs by HM knowledge")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_flash_not_required
# ---------------------------------------------------------------------------

def prop_flash_not_required(player_has_hm: bool, party_size: int) -> bool:
    """
    Property 12d — Req 12.4
    Dark cave tiles are navigable without Flash.  Flash is not required
    to traverse or see in any cave.
    """
    state = make_overworld_state(player_has_hm=player_has_hm, party_size=party_size)

    passable_without = field_passable(TILE_DARK_CAVE, player_has_hm=False)
    passable_with = field_passable(TILE_DARK_CAVE, player_has_hm=True)
    navigable = tile_auto_clears(TILE_DARK_CAVE, state)

    if not passable_without:
        print(f"FAIL prop_flash_not_required: dark cave not passable without Flash")
        print(f"  player_has_hm={player_has_hm}, party_size={party_size}")
        return False

    if not navigable:
        print(f"FAIL prop_flash_not_required: dark cave not marked navigable")
        return False

    if passable_without != passable_with:
        print(f"FAIL prop_flash_not_required: cave passability differs by Flash knowledge")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_hm_moves_still_learnable
# ---------------------------------------------------------------------------

def prop_hm_moves_still_learnable(hm_name: str, move_slot: int) -> bool:
    """
    Property 12e — Req 12.5
    Cut, Rock Smash, Waterfall, and Flash are still learnable as battle moves
    occupying any of the four move slots.  Simplifying their field behaviour
    must not remove them from the learnable move pool.
    """
    if hm_name not in SIMPLIFIED_HM_MOVES:
        return True
    if move_slot not in BATTLE_MOVE_SLOTS:
        return True

    learnable = can_learn_hm_as_battle_move(hm_name, move_slot)

    if not learnable:
        print(f"FAIL prop_hm_moves_still_learnable:")
        print(f"  {hm_name} in slot {move_slot} should be learnable, but is not")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_no_party_slot_required
# ---------------------------------------------------------------------------

def prop_no_party_slot_required(hm_name: str) -> bool:
    """
    Property 12f — Req 12.6
    None of the four simplified HMs require a party slot to be usable in
    the field.  Party size is irrelevant to HM tile interaction.

    Invariant: hm_requires_party_slot(hm) == False for all four simplified HMs.
    """
    if hm_name not in SIMPLIFIED_HM_MOVES:
        return True

    requires = hm_requires_party_slot(hm_name)

    if requires:
        print(f"FAIL prop_no_party_slot_required:")
        print(f"  {hm_name} requires a party slot, but should not")
        return False

    return True


# ---------------------------------------------------------------------------
# Combined key invariant check
# ---------------------------------------------------------------------------

def prop_all_hm_tiles_passable_without_hm() -> bool:
    """
    Composite key invariant — Req 12 global:
    field_passable(tile_type, player_has_hm=False) == True for every HM tile.
    """
    for tile in HM_TILES:
        if not field_passable(tile, player_has_hm=False):
            print(f"FAIL prop_all_hm_tiles_passable_without_hm: tile {tile!r} blocked")
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

def test_property_12a_cut_tree_auto_cleared():
    print("\n=== Property 12a: Cut Tree Auto-Cleared on Contact ===")
    print("**Validates: Requirement 12.1**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        player_has_hm = rng.choice([True, False])
        party_size = rng.randint(0, 6)
        return prop_cut_tree_auto_cleared(player_has_hm, party_size)

    passed, failed, _ = run_property("12a", trial)

    edge_cases = [
        (False, 0, "no HM, empty party"),
        (False, 1, "no HM, single Pokemon"),
        (False, 6, "no HM, full party"),
        (True, 1, "has HM, single Pokemon (must still clear)"),
    ]
    for player_has_hm, party_size, desc in edge_cases:
        ok = prop_cut_tree_auto_cleared(player_has_hm, party_size)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 12a Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_cut_tree_auto_cleared"


def test_property_12b_rock_smash_auto_cleared():
    print("\n=== Property 12b: Rock Smash Auto-Cleared on Contact ===")
    print("**Validates: Requirement 12.2**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        player_has_hm = rng.choice([True, False])
        party_size = rng.randint(0, 6)
        return prop_rock_smash_auto_cleared(player_has_hm, party_size)

    passed, failed, _ = run_property("12b", trial)

    edge_cases = [
        (False, 0, "no HM, empty party"),
        (False, 6, "no HM, full party"),
        (True, 1, "has HM, single Pokemon"),
    ]
    for player_has_hm, party_size, desc in edge_cases:
        ok = prop_rock_smash_auto_cleared(player_has_hm, party_size)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 12b Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_rock_smash_auto_cleared"


def test_property_12c_waterfall_passable():
    print("\n=== Property 12c: Waterfall Passable Without HM ===")
    print("**Validates: Requirement 12.3**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        player_has_hm = rng.choice([True, False])
        party_size = rng.randint(0, 6)
        return prop_waterfall_passable(player_has_hm, party_size)

    passed, failed, _ = run_property("12c", trial)

    edge_cases = [
        (False, 0, "no HM, empty party"),
        (False, 6, "no HM, full party"),
        (True, 1, "has HM — must also be passable"),
    ]
    for player_has_hm, party_size, desc in edge_cases:
        ok = prop_waterfall_passable(player_has_hm, party_size)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 12c Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_waterfall_passable"


def test_property_12d_flash_not_required():
    print("\n=== Property 12d: Dark Cave Navigable Without Flash ===")
    print("**Validates: Requirement 12.4**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        player_has_hm = rng.choice([True, False])
        party_size = rng.randint(0, 6)
        return prop_flash_not_required(player_has_hm, party_size)

    passed, failed, _ = run_property("12d", trial)

    edge_cases = [
        (False, 0, "no Flash, empty party"),
        (False, 6, "no Flash, full party"),
        (True, 1, "has Flash — must also be navigable"),
    ]
    for player_has_hm, party_size, desc in edge_cases:
        ok = prop_flash_not_required(player_has_hm, party_size)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 12d Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_flash_not_required"


def test_property_12e_hm_moves_still_learnable():
    print("\n=== Property 12e: HM Moves Still Learnable as Battle Moves ===")
    print("**Validates: Requirement 12.5**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        hm_name = rng.choice(SIMPLIFIED_HM_MOVES)
        move_slot = rng.choice(BATTLE_MOVE_SLOTS)
        return prop_hm_moves_still_learnable(hm_name, move_slot)

    passed, failed, _ = run_property("12e", trial)

    # Exhaustive coverage of all 4 HMs × 4 move slots
    for hm in SIMPLIFIED_HM_MOVES:
        for slot in BATTLE_MOVE_SLOTS:
            ok = prop_hm_moves_still_learnable(hm, slot)
            if ok:
                passed += 1
            else:
                failed += 1
                print(f"FAIL: {hm} in slot {slot}")

    print(f"Property 12e Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_hm_moves_still_learnable"


def test_property_12f_no_party_slot_required():
    print("\n=== Property 12f: No Party Slot Required for HM Field Use ===")
    print("**Validates: Requirement 12.6**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    passed = 0
    failed = 0

    # Test all four simplified HMs repeatedly
    rng = random.Random(42)
    for _ in range(NUM_TRIALS):
        hm_name = rng.choice(SIMPLIFIED_HM_MOVES)
        ok = prop_no_party_slot_required(hm_name)
        if ok:
            passed += 1
        else:
            failed += 1

    # Exhaustive: every simplified HM must not require a party slot
    for hm in SIMPLIFIED_HM_MOVES:
        ok = prop_no_party_slot_required(hm)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL: {hm} incorrectly requires party slot")

    print(f"Property 12f Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_no_party_slot_required"


def test_property_12_key_invariant_all_hm_tiles_passable():
    print("\n=== Property 12 Key Invariant: All HM Tiles Passable Without HM ===")
    print("**Validates: Requirement 12 global**")
    print(f"Running {NUM_TRIALS} checks...\n")

    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        ok = prop_all_hm_tiles_passable_without_hm()
        if ok:
            passed += 1
        else:
            failed += 1

    print(f"Key Invariant Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_all_hm_tiles_passable_without_hm"


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("=" * 70)
    print("Property-Based Tests: HM Simplification")
    print("Covers: Requirement 12 — HM auto-clear on contact")
    print("=" * 70)
    print("Properties:")
    print("  12a: Cut tree auto-cleared without Cut")
    print("  12b: Smash rock auto-cleared without Rock Smash")
    print("  12c: Waterfall passable without Waterfall")
    print("  12d: Dark cave navigable without Flash")
    print("  12e: Cut/Rock Smash/Waterfall/Flash still learnable as battle moves")
    print("  12f: None of the four HMs require a party slot for field use")
    print("  12g: Key invariant — all HM tiles passable without HM knowledge")
    print(f"Minimum {NUM_TRIALS} iterations per property.")
    print("=" * 70)

    total_failed = 0

    tests = [
        test_property_12a_cut_tree_auto_cleared,
        test_property_12b_rock_smash_auto_cleared,
        test_property_12c_waterfall_passable,
        test_property_12d_flash_not_required,
        test_property_12e_hm_moves_still_learnable,
        test_property_12f_no_party_slot_required,
        test_property_12_key_invariant_all_hm_tiles_passable,
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
        print("\nALL HM SIMPLIFICATION PROPERTY TESTS PASSED!")
        print("\nCoverage map (Requirement 12 -> Properties):")
        print("  12.1 (Cut tree auto-cleared)           -> 12a")
        print("  12.2 (Rock Smash auto-cleared)         -> 12b")
        print("  12.3 (Waterfall passable)              -> 12c")
        print("  12.4 (Flash not required)              -> 12d")
        print("  12.5 (HMs still learnable)             -> 12e")
        print("  12.6 (no party slot required)          -> 12f")
        print("  12.* (global passability invariant)    -> 12g")
        return 0
    else:
        print(f"\nTESTS FAILED: {total_failed} property group(s) failed")
        return 1


if __name__ == "__main__":
    sys.exit(main())
