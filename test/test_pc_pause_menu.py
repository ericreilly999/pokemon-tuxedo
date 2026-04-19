#!/usr/bin/env python3
"""
Property-Based Tests: PC from Pause Menu

Covers: Requirement 15 — Pokemon Storage System accessible from the field
        pause menu at any time outside of battle

Properties tested:
  - prop_pc_option_in_pause_menu: pause menu contains a "PC" option
  - prop_pc_opens_storage_system: selecting PC option opens Pokemon Storage System
  - prop_player_position_unchanged_after_pc: exiting PC returns player to exact
    same map coordinates
  - prop_pc_available_in_field: PC option accessible when player is in the field
  - prop_pc_not_available_in_battle: PC option is not accessible during battle

QA Task: QA-012
Minimum 100 iterations per property.
"""

import random
import sys
from dataclasses import dataclass
from typing import List, Optional, Tuple

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

PC_MENU_OPTION = "PC"

# Pause menu option identifiers (order reflects spec menu layout)
PAUSE_MENU_OPTIONS = [
    "POKEMON",
    "POKEDEX",
    "BAG",
    "PC",        # Req 15: always present in field
    "TRAINER",
    "SAVE",
    "OPTIONS",
    "EXIT",
]

STORAGE_SYSTEM_SCREEN = "POKEMON_STORAGE_SYSTEM"

CONTEXT_FIELD = "field"
CONTEXT_BATTLE = "battle"
CONTEXT_CUTSCENE = "cutscene"


# ---------------------------------------------------------------------------
# Model: Game State and Menu System
# ---------------------------------------------------------------------------

@dataclass
class PlayerPosition:
    map_id: int
    x: int
    y: int

    def __eq__(self, other) -> bool:
        return (
            isinstance(other, PlayerPosition)
            and self.map_id == other.map_id
            and self.x == other.x
            and self.y == other.y
        )


class PauseMenuSystem:
    """
    Spec model of the pause menu with PC storage access (Requirement 15).

    The PC option is always present in the field pause menu.
    It is never present during battle.
    Opening and closing the PC must not alter player position.
    """

    def __init__(self, context: str, player_position: PlayerPosition):
        self.context: str = context
        self.player_position: PlayerPosition = PlayerPosition(
            player_position.map_id,
            player_position.x,
            player_position.y,
        )
        self._saved_position: Optional[PlayerPosition] = None
        self.current_screen: Optional[str] = None

    def get_available_menu_options(self) -> List[str]:
        """
        Return the list of pause menu options available in the current context.

        Requirement 15.1: PC option is present in field context.
        Requirement 15.4: PC option is absent in battle context.
        """
        if self.context == CONTEXT_BATTLE:
            # Battle pause menu never includes PC
            return [opt for opt in PAUSE_MENU_OPTIONS if opt != PC_MENU_OPTION]
        if self.context in (CONTEXT_FIELD, CONTEXT_CUTSCENE):
            if self.context == CONTEXT_FIELD:
                return list(PAUSE_MENU_OPTIONS)  # PC is present
            # cutscene: menu may be disabled entirely — model as no options
            return []
        return list(PAUSE_MENU_OPTIONS)

    def select_menu_option(self, option: str) -> bool:
        """
        Attempt to select a menu option.  Returns True if the option was
        available and selected.  Saves player position before opening PC.
        """
        available = self.get_available_menu_options()
        if option not in available:
            return False

        if option == PC_MENU_OPTION:
            # Save position snapshot before entering PC
            self._saved_position = PlayerPosition(
                self.player_position.map_id,
                self.player_position.x,
                self.player_position.y,
            )
            self.current_screen = STORAGE_SYSTEM_SCREEN

        return True

    def exit_pc(self) -> PlayerPosition:
        """
        Exit the PC storage screen and return player to the saved position.

        Requirement 15.3: player position must be identical after exit.
        """
        if self._saved_position is None:
            return self.player_position

        # Restore player to exactly where they were
        self.player_position = PlayerPosition(
            self._saved_position.map_id,
            self._saved_position.x,
            self._saved_position.y,
        )
        self.current_screen = None
        return self.player_position


# ---------------------------------------------------------------------------
# Property: prop_pc_option_in_pause_menu
# ---------------------------------------------------------------------------

def prop_pc_option_in_pause_menu(map_id: int, x: int, y: int) -> bool:
    """
    Property 15a — Req 15.1
    When the player opens the pause menu from the field, the "PC" option
    is present in the returned option list.

    This holds regardless of map, coordinates, or time of day.
    """
    pos = PlayerPosition(map_id, x, y)
    menu = PauseMenuSystem(CONTEXT_FIELD, pos)
    options = menu.get_available_menu_options()

    if PC_MENU_OPTION not in options:
        print(f"FAIL prop_pc_option_in_pause_menu:")
        print(f"  map_id={map_id}, x={x}, y={y}")
        print(f"  options={options}")
        print(f"  'PC' not found in pause menu options")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_pc_opens_storage_system
# ---------------------------------------------------------------------------

def prop_pc_opens_storage_system(map_id: int, x: int, y: int) -> bool:
    """
    Property 15b — Req 15.2
    Selecting the PC option from the field pause menu must open the
    Pokemon Storage System (not any other screen).
    """
    pos = PlayerPosition(map_id, x, y)
    menu = PauseMenuSystem(CONTEXT_FIELD, pos)

    selected = menu.select_menu_option(PC_MENU_OPTION)
    if not selected:
        print(f"FAIL prop_pc_opens_storage_system: PC option rejected")
        print(f"  map_id={map_id}, x={x}, y={y}")
        return False

    if menu.current_screen != STORAGE_SYSTEM_SCREEN:
        print(f"FAIL prop_pc_opens_storage_system:")
        print(f"  current_screen={menu.current_screen!r}, expected={STORAGE_SYSTEM_SCREEN!r}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_player_position_unchanged_after_pc
# ---------------------------------------------------------------------------

def prop_player_position_unchanged_after_pc(
    map_id: int, x: int, y: int
) -> bool:
    """
    Property 15c — Req 15.3
    After opening the PC and then exiting it, the player's map_id, x, and y
    must be exactly the same as before opening.

    No warp, no drift, no coordinate reset.
    """
    original_pos = PlayerPosition(map_id, x, y)
    menu = PauseMenuSystem(CONTEXT_FIELD, original_pos)

    menu.select_menu_option(PC_MENU_OPTION)

    # Simulate player doing things inside PC (position should not change)
    returned_pos = menu.exit_pc()

    if returned_pos != original_pos:
        print(f"FAIL prop_player_position_unchanged_after_pc:")
        print(f"  original: map={map_id}, x={x}, y={y}")
        print(f"  returned: map={returned_pos.map_id}, x={returned_pos.x}, y={returned_pos.y}")
        return False

    # Verify via menu.player_position as well
    if menu.player_position != original_pos:
        print(f"FAIL prop_player_position_unchanged_after_pc (stored position):")
        print(f"  stored: map={menu.player_position.map_id}, x={menu.player_position.x}, y={menu.player_position.y}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_pc_available_in_field
# ---------------------------------------------------------------------------

def prop_pc_available_in_field(map_id: int, x: int, y: int) -> bool:
    """
    Property 15d — Req 15.4
    The PC option must be available (selectable) when the player is in the
    field context, on any map, at any coordinates.
    """
    pos = PlayerPosition(map_id, x, y)
    menu = PauseMenuSystem(CONTEXT_FIELD, pos)

    available = menu.get_available_menu_options()
    if PC_MENU_OPTION not in available:
        print(f"FAIL prop_pc_available_in_field:")
        print(f"  context=field, map={map_id}, x={x}, y={y}")
        print(f"  PC not in available options: {available}")
        return False

    # Must also be selectable
    menu2 = PauseMenuSystem(CONTEXT_FIELD, pos)
    selected = menu2.select_menu_option(PC_MENU_OPTION)
    if not selected:
        print(f"FAIL prop_pc_available_in_field: PC option not selectable")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_pc_not_available_in_battle
# ---------------------------------------------------------------------------

def prop_pc_not_available_in_battle(map_id: int, x: int, y: int) -> bool:
    """
    Property 15e — Req 15.5
    The PC option must NOT be present in the pause menu during battle.
    Selecting PC during battle must be rejected.
    """
    pos = PlayerPosition(map_id, x, y)
    menu = PauseMenuSystem(CONTEXT_BATTLE, pos)

    available = menu.get_available_menu_options()
    if PC_MENU_OPTION in available:
        print(f"FAIL prop_pc_not_available_in_battle:")
        print(f"  context=battle, options={available}")
        print(f"  'PC' must not appear during battle")
        return False

    # Attempting to select PC during battle must be rejected
    selected = menu.select_menu_option(PC_MENU_OPTION)
    if selected:
        print(f"FAIL prop_pc_not_available_in_battle: PC was selectable during battle")
        return False

    return True


# ---------------------------------------------------------------------------
# Test runners
# ---------------------------------------------------------------------------

NUM_TRIALS = 100

# Map ID range to stress: 0–511 (GBA map table size typical max)
MAP_ID_MIN = 0
MAP_ID_MAX = 511
COORD_MIN = 0
COORD_MAX = 127  # Typical GBA overworld tile grid


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


def random_position(rng: random.Random) -> Tuple[int, int, int]:
    return (
        rng.randint(MAP_ID_MIN, MAP_ID_MAX),
        rng.randint(COORD_MIN, COORD_MAX),
        rng.randint(COORD_MIN, COORD_MAX),
    )


# ---------------------------------------------------------------------------
# pytest-compatible test functions
# ---------------------------------------------------------------------------

def test_property_15a_pc_option_in_pause_menu():
    print("\n=== Property 15a: PC Option Present in Pause Menu ===")
    print("**Validates: Requirement 15.1**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        map_id, x, y = random_position(rng)
        return prop_pc_option_in_pause_menu(map_id, x, y)

    passed, failed, _ = run_property("15a", trial)

    edge_cases = [
        (0, 0, 0, "origin of first map"),
        (MAP_ID_MAX, COORD_MAX, COORD_MAX, "max map + max coords"),
        (0, 0, COORD_MAX, "bottom edge of first map"),
        (MAP_ID_MAX, 0, 0, "top-left of last map"),
        (1, 10, 10, "typical starting position"),
    ]
    for map_id, x, y, desc in edge_cases:
        ok = prop_pc_option_in_pause_menu(map_id, x, y)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 15a Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_pc_option_in_pause_menu"


def test_property_15b_pc_opens_storage_system():
    print("\n=== Property 15b: PC Opens Storage System ===")
    print("**Validates: Requirement 15.2**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        map_id, x, y = random_position(rng)
        return prop_pc_opens_storage_system(map_id, x, y)

    passed, failed, _ = run_property("15b", trial)

    edge_cases = [
        (0, 0, 0, "origin of map 0"),
        (100, 50, 50, "mid-map position"),
        (MAP_ID_MAX, COORD_MAX, COORD_MAX, "far corner"),
    ]
    for map_id, x, y, desc in edge_cases:
        ok = prop_pc_opens_storage_system(map_id, x, y)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 15b Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_pc_opens_storage_system"


def test_property_15c_player_position_unchanged_after_pc():
    print("\n=== Property 15c: Player Position Unchanged After PC ===")
    print("**Validates: Requirement 15.3**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        map_id, x, y = random_position(rng)
        return prop_player_position_unchanged_after_pc(map_id, x, y)

    passed, failed, _ = run_property("15c", trial)

    edge_cases = [
        # Boundary coordinates
        (0, 0, 0, "coordinate origin"),
        (0, COORD_MAX, COORD_MAX, "max coordinates"),
        (MAP_ID_MAX, 1, 1, "near-origin on last map"),
        # Same x different y
        (5, 20, 0, "zero y coordinate"),
        (5, 0, 20, "zero x coordinate"),
        # Map boundaries
        (MAP_ID_MIN, COORD_MIN, COORD_MIN, "all minimums"),
        (MAP_ID_MAX, COORD_MAX, COORD_MAX, "all maximums"),
    ]
    for map_id, x, y, desc in edge_cases:
        ok = prop_player_position_unchanged_after_pc(map_id, x, y)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 15c Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_player_position_unchanged_after_pc"


def test_property_15d_pc_available_in_field():
    print("\n=== Property 15d: PC Available in Field Context ===")
    print("**Validates: Requirement 15.4**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        map_id, x, y = random_position(rng)
        return prop_pc_available_in_field(map_id, x, y)

    passed, failed, _ = run_property("15d", trial)

    edge_cases = [
        (0, 0, 0, "map 0 origin"),
        (1, 5, 5, "Pallet Town typical"),
        (MAP_ID_MAX, 0, 0, "last map, top-left"),
    ]
    for map_id, x, y, desc in edge_cases:
        ok = prop_pc_available_in_field(map_id, x, y)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 15d Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_pc_available_in_field"


def test_property_15e_pc_not_available_in_battle():
    print("\n=== Property 15e: PC Not Available During Battle ===")
    print("**Validates: Requirement 15.5**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        map_id, x, y = random_position(rng)
        return prop_pc_not_available_in_battle(map_id, x, y)

    passed, failed, _ = run_property("15e", trial)

    edge_cases = [
        (0, 0, 0, "battle at map 0 origin"),
        (1, 10, 10, "typical battle encounter location"),
        (MAP_ID_MAX, COORD_MAX, COORD_MAX, "battle at far corner"),
    ]
    for map_id, x, y, desc in edge_cases:
        ok = prop_pc_not_available_in_battle(map_id, x, y)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    # Cross-check: field vs. battle distinction is sharp
    rng = random.Random(99)
    for _ in range(50):
        map_id, x, y = random_position(rng)
        pos = PlayerPosition(map_id, x, y)

        field_menu = PauseMenuSystem(CONTEXT_FIELD, pos)
        battle_menu = PauseMenuSystem(CONTEXT_BATTLE, pos)

        field_has_pc = PC_MENU_OPTION in field_menu.get_available_menu_options()
        battle_has_pc = PC_MENU_OPTION in battle_menu.get_available_menu_options()

        if not field_has_pc or battle_has_pc:
            failed += 1
            print(f"FAIL cross-check: field_has_pc={field_has_pc}, battle_has_pc={battle_has_pc}")
            print(f"  map={map_id}, x={x}, y={y}")
        else:
            passed += 1

    print(f"Property 15e Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_pc_not_available_in_battle"


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("=" * 70)
    print("Property-Based Tests: PC from Pause Menu")
    print("Covers: Requirement 15 — Pokemon Storage accessible from pause menu")
    print("=" * 70)
    print("Properties:")
    print("  15a: Pause menu contains 'PC' option in field context")
    print("  15b: Selecting PC opens Pokemon Storage System screen")
    print("  15c: Player position (map, x, y) unchanged after exiting PC")
    print("  15d: PC option is accessible and selectable in the field")
    print("  15e: PC option absent and unselectable during battle")
    print(f"Minimum {NUM_TRIALS} iterations per property.")
    print("=" * 70)

    total_failed = 0

    tests = [
        test_property_15a_pc_option_in_pause_menu,
        test_property_15b_pc_opens_storage_system,
        test_property_15c_player_position_unchanged_after_pc,
        test_property_15d_pc_available_in_field,
        test_property_15e_pc_not_available_in_battle,
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
        print("\nALL PC PAUSE MENU PROPERTY TESTS PASSED!")
        print("\nCoverage map (Requirement 15 -> Properties):")
        print("  15.1 (PC option in pause menu)         -> 15a")
        print("  15.2 (PC opens storage system)         -> 15b")
        print("  15.3 (position unchanged after PC)     -> 15c")
        print("  15.4 (PC available in field)           -> 15d")
        print("  15.5 (PC absent in battle)             -> 15e")
        return 0
    else:
        print(f"\nTESTS FAILED: {total_failed} property group(s) failed")
        return 1


if __name__ == "__main__":
    sys.exit(main())
