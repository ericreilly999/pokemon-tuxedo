#!/usr/bin/env python3
"""
Property-Based Tests: Game Settings (Battle Mode and Game Speed)

Covers: Requirement 11 — In-game settings menu with battle mode and speed controls

Properties tested:
  - prop_default_battle_mode_is_set: on new game, battle_mode == SET (not SHIFT)
  - prop_battle_mode_toggles: SET → SHIFT → SET round-trips correctly
  - prop_game_speed_range: valid values 1–10 accepted; invalid values rejected
  - prop_speed_1_is_normal: game_speed == 1 applies no duration change (divisor == 1)
  - prop_speed_n_divides_duration: for speed N (2–10), duration == base_duration // N
  - prop_settings_persist: save then load produces identical battle_mode and game_speed

QA Task: QA-008
Minimum 100 iterations per property.
"""

import random
import sys

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

BATTLE_MODE_SET = "SET"
BATTLE_MODE_SHIFT = "SHIFT"
VALID_BATTLE_MODES = (BATTLE_MODE_SET, BATTLE_MODE_SHIFT)

SPEED_MIN = 1
SPEED_MAX = 10

INVALID_SPEEDS = [0, 11, -1, 255, SPEED_MAX + 1, SPEED_MIN - 1]


# ---------------------------------------------------------------------------
# Model: Game Settings (spec-derived, NOT copied from implementation)
# ---------------------------------------------------------------------------

class GameSettings:
    """
    Spec model of the in-game settings system (Requirement 11).

    battle_mode: SET (default) or SHIFT — controls whether player is prompted
                 to swap Pokemon after opponent's faints.
    game_speed:  integer 1–10 — divides animation/text durations.
                 1 = normal speed (no division).  Higher = faster.
    """

    def __init__(self):
        self.battle_mode: str = BATTLE_MODE_SET   # Req 11.1: default is SET
        self.game_speed: int = SPEED_MIN           # Req 11.3: default is 1
        self._saved_battle_mode: str = BATTLE_MODE_SET
        self._saved_game_speed: int = SPEED_MIN

    def set_battle_mode(self, mode: str) -> bool:
        """Accept only SET or SHIFT.  Return True if valid, False if rejected."""
        if mode in VALID_BATTLE_MODES:
            self.battle_mode = mode
            return True
        return False

    def toggle_battle_mode(self) -> str:
        """Toggle between SET and SHIFT.  Return the new value."""
        if self.battle_mode == BATTLE_MODE_SET:
            self.battle_mode = BATTLE_MODE_SHIFT
        else:
            self.battle_mode = BATTLE_MODE_SET
        return self.battle_mode

    def set_game_speed(self, speed: int) -> bool:
        """Accept 1–10 only.  Return True if valid, False if rejected."""
        if SPEED_MIN <= speed <= SPEED_MAX:
            self.game_speed = speed
            return True
        return False

    def apply_speed_to_duration(self, base_duration: int) -> int:
        """
        Apply the current game speed as an integer divisor.
        Result = base_duration // game_speed  (integer division, matching GBA behaviour).
        """
        return base_duration // self.game_speed

    def save(self) -> None:
        self._saved_battle_mode = self.battle_mode
        self._saved_game_speed = self.game_speed

    def load(self) -> None:
        self.battle_mode = self._saved_battle_mode
        self.game_speed = self._saved_game_speed


# ---------------------------------------------------------------------------
# Property: prop_default_battle_mode_is_set
# ---------------------------------------------------------------------------

def prop_default_battle_mode_is_set() -> bool:
    """
    Property 11a — Req 11.1
    A freshly constructed settings object must default to SET, not SHIFT.
    The default must hold regardless of any prior settings object state.
    """
    settings = GameSettings()
    if settings.battle_mode != BATTLE_MODE_SET:
        print(f"FAIL prop_default_battle_mode_is_set:")
        print(f"  default battle_mode={settings.battle_mode!r}, expected={BATTLE_MODE_SET!r}")
        return False
    # Confirm SHIFT is not the default
    if settings.battle_mode == BATTLE_MODE_SHIFT:
        print(f"FAIL prop_default_battle_mode_is_set: default is SHIFT, must be SET")
        return False
    return True


# ---------------------------------------------------------------------------
# Property: prop_battle_mode_toggles
# ---------------------------------------------------------------------------

def prop_battle_mode_toggles(initial_mode: str, num_toggles: int) -> bool:
    """
    Property 11b — Req 11.2
    Toggling battle_mode is an involution: every two toggles returns to the
    original mode.  After n toggles from a known start, the mode is fully
    deterministic based on parity.
    """
    settings = GameSettings()
    settings.set_battle_mode(initial_mode)

    current = initial_mode
    for i in range(num_toggles):
        new_mode = settings.toggle_battle_mode()
        expected = BATTLE_MODE_SHIFT if current == BATTLE_MODE_SET else BATTLE_MODE_SET
        if new_mode != expected:
            print(f"FAIL prop_battle_mode_toggles:")
            print(f"  toggle #{i+1}: from={current!r}, got={new_mode!r}, expected={expected!r}")
            return False
        current = new_mode

    # Round-trip: even number of toggles must restore initial
    settings2 = GameSettings()
    settings2.set_battle_mode(initial_mode)
    for _ in range(10):
        settings2.toggle_battle_mode()
        settings2.toggle_battle_mode()
    if settings2.battle_mode != initial_mode:
        print(f"FAIL prop_battle_mode_toggles (round-trip):")
        print(f"  after 10 double-toggles: got={settings2.battle_mode!r}, expected={initial_mode!r}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_game_speed_range
# ---------------------------------------------------------------------------

def prop_game_speed_range(speed: int) -> bool:
    """
    Property 11c — Req 11.3
    Valid speeds (1–10) are accepted; invalid speeds (0, 11, -1, 255, etc.) are rejected.
    After any rejected set, game_speed remains unchanged at its previous valid value.
    """
    settings = GameSettings()
    prev_speed = settings.game_speed  # should be 1

    accepted = settings.set_game_speed(speed)

    if SPEED_MIN <= speed <= SPEED_MAX:
        if not accepted:
            print(f"FAIL prop_game_speed_range: valid speed {speed} was rejected")
            return False
        if settings.game_speed != speed:
            print(f"FAIL prop_game_speed_range: valid speed {speed} not stored, got {settings.game_speed}")
            return False
    else:
        if accepted:
            print(f"FAIL prop_game_speed_range: invalid speed {speed} was accepted")
            return False
        # Speed must remain unchanged after rejection
        if settings.game_speed != prev_speed:
            print(f"FAIL prop_game_speed_range: speed changed after rejection of {speed}")
            print(f"  prev={prev_speed}, now={settings.game_speed}")
            return False

    # Invariant: stored speed is always in [SPEED_MIN, SPEED_MAX]
    if not (SPEED_MIN <= settings.game_speed <= SPEED_MAX):
        print(f"FAIL prop_game_speed_range: stored speed {settings.game_speed} out of bounds")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_speed_1_is_normal
# ---------------------------------------------------------------------------

def prop_speed_1_is_normal(base_duration: int) -> bool:
    """
    Property 11d — Req 11.4
    At game_speed == 1 (normal), applying speed to a duration leaves it
    unchanged (divisor == 1, so base_duration // 1 == base_duration).
    """
    if base_duration < 0:
        return True  # invalid input, skip

    settings = GameSettings()
    # Default speed is 1, but set it explicitly
    settings.set_game_speed(1)

    result = settings.apply_speed_to_duration(base_duration)
    if result != base_duration:
        print(f"FAIL prop_speed_1_is_normal:")
        print(f"  base_duration={base_duration}, result={result}, expected={base_duration}")
        return False
    return True


# ---------------------------------------------------------------------------
# Property: prop_speed_n_divides_duration
# ---------------------------------------------------------------------------

def prop_speed_n_divides_duration(base_duration: int, speed: int) -> bool:
    """
    Property 11e — Req 11.5
    For speed N in [2, 10], result == base_duration // N (integer floor division).
    Faster speed == shorter duration.
    """
    if speed < 2 or speed > SPEED_MAX:
        return True  # out-of-scope for this property
    if base_duration < 0:
        return True

    settings = GameSettings()
    settings.set_game_speed(speed)

    result = settings.apply_speed_to_duration(base_duration)
    expected = base_duration // speed

    if result != expected:
        print(f"FAIL prop_speed_n_divides_duration:")
        print(f"  base_duration={base_duration}, speed={speed}")
        print(f"  result={result}, expected={expected}")
        return False

    # Faster speed must produce duration <= slower speed duration
    settings_slow = GameSettings()
    settings_slow.set_game_speed(speed - 1)
    result_slow = settings_slow.apply_speed_to_duration(base_duration)
    if result > result_slow:
        print(f"FAIL prop_speed_n_divides_duration (monotonicity):")
        print(f"  speed {speed} produced longer duration than speed {speed - 1}")
        print(f"  {result} > {result_slow}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property: prop_settings_persist
# ---------------------------------------------------------------------------

def prop_settings_persist(battle_mode: str, game_speed: int) -> bool:
    """
    Property 11f — Req 11.6
    Save then load must restore exactly the same battle_mode and game_speed.
    Mutating settings after save must not affect what load restores.
    """
    if battle_mode not in VALID_BATTLE_MODES:
        return True
    if not (SPEED_MIN <= game_speed <= SPEED_MAX):
        return True

    settings = GameSettings()
    settings.set_battle_mode(battle_mode)
    settings.set_game_speed(game_speed)
    settings.save()

    # Mutate to different values
    opposite_mode = BATTLE_MODE_SHIFT if battle_mode == BATTLE_MODE_SET else BATTLE_MODE_SET
    opposite_speed = (game_speed % SPEED_MAX) + 1  # rotate within valid range

    settings.set_battle_mode(opposite_mode)
    settings.set_game_speed(opposite_speed)

    # Load must restore saved values
    settings.load()

    if settings.battle_mode != battle_mode:
        print(f"FAIL prop_settings_persist (battle_mode):")
        print(f"  saved={battle_mode!r}, after load={settings.battle_mode!r}")
        return False

    if settings.game_speed != game_speed:
        print(f"FAIL prop_settings_persist (game_speed):")
        print(f"  saved={game_speed}, after load={settings.game_speed}")
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

def test_property_11a_default_battle_mode_is_set():
    print("\n=== Property 11a: Default Battle Mode Is SET ===")
    print("**Validates: Requirement 11.1**")
    print(f"Running {NUM_TRIALS} independent construction checks...\n")

    passed = 0
    failed = 0
    # Instantiate a fresh settings object in each trial
    for _ in range(NUM_TRIALS):
        ok = prop_default_battle_mode_is_set()
        if ok:
            passed += 1
        else:
            failed += 1

    print(f"Property 11a Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_default_battle_mode_is_set"


def test_property_11b_battle_mode_toggles():
    print("\n=== Property 11b: Battle Mode Toggle Round-Trips ===")
    print("**Validates: Requirement 11.2**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        initial = rng.choice(list(VALID_BATTLE_MODES))
        num_toggles = rng.randint(1, 20)
        return prop_battle_mode_toggles(initial, num_toggles)

    passed, failed, _ = run_property("11b", trial)

    # Explicit edge cases
    edge_cases = [
        (BATTLE_MODE_SET, 1, "SET toggle once -> SHIFT"),
        (BATTLE_MODE_SHIFT, 1, "SHIFT toggle once -> SET"),
        (BATTLE_MODE_SET, 2, "SET double-toggle -> SET"),
        (BATTLE_MODE_SHIFT, 100, "SHIFT 100 toggles -> SHIFT (even)"),
        (BATTLE_MODE_SET, 101, "SET 101 toggles -> SHIFT (odd)"),
    ]
    for initial, n, desc in edge_cases:
        ok = prop_battle_mode_toggles(initial, n)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 11b Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_battle_mode_toggles"


def test_property_11c_game_speed_range():
    print("\n=== Property 11c: Game Speed Range 1-10 ===")
    print("**Validates: Requirement 11.3**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        # Mix valid and invalid speeds
        if rng.random() < 0.5:
            speed = rng.randint(SPEED_MIN, SPEED_MAX)
        else:
            speed = rng.choice(INVALID_SPEEDS + [rng.randint(-100, 0), rng.randint(11, 300)])
        return prop_game_speed_range(speed)

    passed, failed, _ = run_property("11c", trial)

    # Exhaustive boundary coverage
    boundary_cases = [
        (SPEED_MIN,      True,  "minimum valid speed"),
        (SPEED_MAX,      True,  "maximum valid speed"),
        (SPEED_MIN - 1,  False, "one below minimum (0)"),
        (SPEED_MAX + 1,  False, "one above maximum (11)"),
        (-1,             False, "negative speed"),
        (255,            False, "255 (byte overflow candidate)"),
        (0,              False, "zero speed"),
    ]
    for speed, should_accept, desc in boundary_cases:
        settings = GameSettings()
        accepted = settings.set_game_speed(speed)
        if accepted == should_accept:
            passed += 1
        else:
            failed += 1
            print(f"FAIL boundary: {desc} — accepted={accepted}, should_accept={should_accept}")

    print(f"Property 11c Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_game_speed_range"


def test_property_11d_speed_1_is_normal():
    print("\n=== Property 11d: Speed 1 Applies No Duration Change ===")
    print("**Validates: Requirement 11.4**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        base_duration = rng.randint(0, 100_000)
        return prop_speed_1_is_normal(base_duration)

    passed, failed, _ = run_property("11d", trial)

    edge_cases = [
        (0, "zero duration at speed 1"),
        (1, "duration 1 at speed 1"),
        (1000, "duration 1000 at speed 1"),
        (0xFFFF, "GBA 16-bit max at speed 1"),
    ]
    for base_duration, desc in edge_cases:
        ok = prop_speed_1_is_normal(base_duration)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 11d Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_speed_1_is_normal"


def test_property_11e_speed_n_divides_duration():
    print("\n=== Property 11e: Speed N Divides Duration by N ===")
    print("**Validates: Requirement 11.5**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        base_duration = rng.randint(0, 10_000)
        speed = rng.randint(2, SPEED_MAX)
        return prop_speed_n_divides_duration(base_duration, speed)

    passed, failed, _ = run_property("11e", trial)

    edge_cases = [
        # (base_duration, speed, description)
        (100,  2,  "100 / 2 = 50"),
        (100,  3,  "100 / 3 = 33 (floor div)"),
        (100,  10, "100 / 10 = 10"),
        (1,    10, "1 / 10 = 0 (floor div)"),
        (0,    5,  "0 / 5 = 0"),
        (999,  7,  "999 / 7 = 142 (floor div)"),
        (1000, 10, "1000 / 10 = 100 (exact)"),
    ]
    for base_duration, speed, desc in edge_cases:
        ok = prop_speed_n_divides_duration(base_duration, speed)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 11e Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_speed_n_divides_duration"


def test_property_11f_settings_persist():
    print("\n=== Property 11f: Settings Persist Across Save/Load ===")
    print("**Validates: Requirement 11.6**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    def trial(rng):
        battle_mode = rng.choice(list(VALID_BATTLE_MODES))
        game_speed = rng.randint(SPEED_MIN, SPEED_MAX)
        return prop_settings_persist(battle_mode, game_speed)

    passed, failed, _ = run_property("11f", trial)

    edge_cases = [
        # (battle_mode, game_speed, description)
        (BATTLE_MODE_SET,   1,         "default values round-trip"),
        (BATTLE_MODE_SHIFT, 10,        "max speed with SHIFT mode"),
        (BATTLE_MODE_SET,   10,        "max speed with SET mode"),
        (BATTLE_MODE_SHIFT, SPEED_MIN, "min speed with SHIFT mode"),
        (BATTLE_MODE_SET,   5,         "mid speed with SET mode"),
    ]
    for battle_mode, game_speed, desc in edge_cases:
        ok = prop_settings_persist(battle_mode, game_speed)
        if ok:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge case: {desc}")

    print(f"Property 11f Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} failure(s) in prop_settings_persist"


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("=" * 70)
    print("Property-Based Tests: Game Settings")
    print("Covers: Requirement 11 — Battle mode and game speed settings")
    print("=" * 70)
    print("Properties:")
    print("  11a: Default battle_mode == SET on new game")
    print("  11b: Battle mode toggles correctly (SET <-> SHIFT round-trips)")
    print("  11c: Game speed valid range 1-10; invalid values rejected")
    print("  11d: Speed 1 applies no duration change")
    print("  11e: Speed N divides duration by N (floor)")
    print("  11f: Settings persist across save/load cycle")
    print(f"Minimum {NUM_TRIALS} iterations per property.")
    print("=" * 70)

    total_failed = 0

    tests = [
        test_property_11a_default_battle_mode_is_set,
        test_property_11b_battle_mode_toggles,
        test_property_11c_game_speed_range,
        test_property_11d_speed_1_is_normal,
        test_property_11e_speed_n_divides_duration,
        test_property_11f_settings_persist,
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
        print("\nALL GAME SETTINGS PROPERTY TESTS PASSED!")
        print("\nCoverage map (Requirement 11 -> Properties):")
        print("  11.1 (default battle mode SET)         -> 11a")
        print("  11.2 (battle mode toggle)              -> 11b")
        print("  11.3 (speed range 1-10)                -> 11c")
        print("  11.4 (speed 1 = normal)                -> 11d")
        print("  11.5 (speed N divides duration)        -> 11e")
        print("  11.6 (settings persist)                -> 11f")
        return 0
    else:
        print(f"\nTESTS FAILED: {total_failed} property group(s) failed")
        return 1


if __name__ == "__main__":
    sys.exit(main())
