#!/usr/bin/env python3
"""
Property-Based Tests: Multi-Region Save Data Round-Trip

Covers: Property 27 — Save Data Serialization Contract

Properties tested:
  - Prop 27: A randomly generated multi-region save state survives a
    struct.pack -> struct.unpack round-trip with every field intact.

The save structure (from include/multi_region_save.h and src/multi_region_save.c):
  - current_region  (u8,  0-2: Kanto=0, Hoenn=1, Johto=2)
  - unlock_flags    (u8,  bitmask: bit 0 = Hoenn unlocked, bit 1 = Johto unlocked)
  - e4_defeated[3]  (u8 array, one byte per region)
  - badges[24]      (u8 array, 8 badges x 3 regions)
  - fly_locations[48] (u16 array, 16 fly destinations x 3 regions)

This is a pure-Python model test that validates the serialization contract,
not the actual C binary format.

QA Task: QA-001
Minimum 100 iterations.
"""

import random
import struct
import sys

# ---------------------------------------------------------------------------
# Constants (mirroring multi_region_save.h)
# ---------------------------------------------------------------------------

NUM_REGIONS = 3
BADGES_PER_REGION = 8
TOTAL_BADGES = BADGES_PER_REGION * NUM_REGIONS        # 24
FLY_LOCATIONS_PER_REGION = 16
TOTAL_FLY_LOCATIONS = FLY_LOCATIONS_PER_REGION * NUM_REGIONS  # 48

REGION_KANTO = 0
REGION_HOENN = 1
REGION_JOHTO = 2

# Unlock flag bitmask definitions
UNLOCK_FLAG_HOENN = 0x01   # bit 0
UNLOCK_FLAG_JOHTO = 0x02   # bit 1

# struct format string:
#   B  = current_region   (u8)
#   B  = unlock_flags     (u8)
#   3B = e4_defeated[3]   (u8 x 3)
#   24B= badges[24]       (u8 x 24)
#   48H= fly_locations[48](u16 x 48)
SAVE_STRUCT_FORMAT = "<BB3B24B48H"
SAVE_STRUCT_SIZE = struct.calcsize(SAVE_STRUCT_FORMAT)


# ---------------------------------------------------------------------------
# Helper: pack a save state dict to bytes
# ---------------------------------------------------------------------------

def pack_save(state: dict) -> bytes:
    """
    Serialize a save state dict to bytes using struct.pack.

    Expected dict keys:
      current_region (int)
      unlock_flags   (int)
      e4_defeated    (list of 3 ints)
      badges         (list of 24 ints)
      fly_locations  (list of 48 ints)
    """
    args = (
        state["current_region"],
        state["unlock_flags"],
        *state["e4_defeated"],
        *state["badges"],
        *state["fly_locations"],
    )
    return struct.pack(SAVE_STRUCT_FORMAT, *args)


def unpack_save(data: bytes) -> dict:
    """
    Deserialize bytes back into a save state dict.
    """
    values = struct.unpack(SAVE_STRUCT_FORMAT, data)
    idx = 0

    current_region = values[idx]; idx += 1
    unlock_flags   = values[idx]; idx += 1
    e4_defeated    = list(values[idx:idx + NUM_REGIONS]); idx += NUM_REGIONS
    badges         = list(values[idx:idx + TOTAL_BADGES]); idx += TOTAL_BADGES
    fly_locations  = list(values[idx:idx + TOTAL_FLY_LOCATIONS])

    return {
        "current_region": current_region,
        "unlock_flags":   unlock_flags,
        "e4_defeated":    e4_defeated,
        "badges":         badges,
        "fly_locations":  fly_locations,
    }


# ---------------------------------------------------------------------------
# Helper: generate a random valid save state
# ---------------------------------------------------------------------------

def random_save_state(rng: random.Random) -> dict:
    """
    Generate a random valid save state within the field domains:
      current_region : 0, 1, or 2
      unlock_flags   : 0x00 - 0x03 (bits 0-1 only)
      e4_defeated[i] : 0 or 1 (treated as boolean flag)
      badges[i]      : 0 or 1 (each badge is won/not won)
      fly_locations[i]: u16, 0x0000 - 0xFFFF (map IDs or 0 = unregistered)
    """
    current_region = rng.randint(0, NUM_REGIONS - 1)
    unlock_flags   = rng.randint(0, 0x03)   # only bits 0-1 defined
    e4_defeated    = [rng.randint(0, 1) for _ in range(NUM_REGIONS)]
    badges         = [rng.randint(0, 1) for _ in range(TOTAL_BADGES)]
    fly_locations  = [rng.randint(0, 0xFFFF) for _ in range(TOTAL_FLY_LOCATIONS)]

    return {
        "current_region": current_region,
        "unlock_flags":   unlock_flags,
        "e4_defeated":    e4_defeated,
        "badges":         badges,
        "fly_locations":  fly_locations,
    }


# ---------------------------------------------------------------------------
# Property 27: Save data survives pack -> unpack round-trip
# ---------------------------------------------------------------------------

def prop_save_data_round_trip(state: dict) -> bool:
    """
    Property 27 — Save Data Serialization Contract

    For any valid save state, packing to bytes and then unpacking must
    produce a dict that is field-by-field identical to the original.
    """
    packed = pack_save(state)
    restored = unpack_save(packed)

    # Check current_region
    if restored["current_region"] != state["current_region"]:
        print(f"FAIL prop_save_data_round_trip: current_region mismatch")
        print(f"  original:  {state['current_region']}")
        print(f"  restored:  {restored['current_region']}")
        return False

    # Check unlock_flags
    if restored["unlock_flags"] != state["unlock_flags"]:
        print(f"FAIL prop_save_data_round_trip: unlock_flags mismatch")
        print(f"  original:  {state['unlock_flags']:#04x}")
        print(f"  restored:  {restored['unlock_flags']:#04x}")
        return False

    # Check e4_defeated (element by element for clear diagnostics)
    for i in range(NUM_REGIONS):
        if restored["e4_defeated"][i] != state["e4_defeated"][i]:
            print(f"FAIL prop_save_data_round_trip: e4_defeated[{i}] mismatch")
            print(f"  original:  {state['e4_defeated'][i]}")
            print(f"  restored:  {restored['e4_defeated'][i]}")
            return False

    # Check badges (element by element)
    for i in range(TOTAL_BADGES):
        if restored["badges"][i] != state["badges"][i]:
            region = i // BADGES_PER_REGION
            badge_num = i % BADGES_PER_REGION
            print(f"FAIL prop_save_data_round_trip: badges[{i}] mismatch")
            print(f"  (region {region}, badge slot {badge_num})")
            print(f"  original:  {state['badges'][i]}")
            print(f"  restored:  {restored['badges'][i]}")
            return False

    # Check fly_locations (element by element)
    for i in range(TOTAL_FLY_LOCATIONS):
        if restored["fly_locations"][i] != state["fly_locations"][i]:
            region = i // FLY_LOCATIONS_PER_REGION
            slot = i % FLY_LOCATIONS_PER_REGION
            print(f"FAIL prop_save_data_round_trip: fly_locations[{i}] mismatch")
            print(f"  (region {region}, slot {slot})")
            print(f"  original:  {state['fly_locations'][i]:#06x}")
            print(f"  restored:  {restored['fly_locations'][i]:#06x}")
            return False

    return True


# ---------------------------------------------------------------------------
# Test runner
# ---------------------------------------------------------------------------

NUM_TRIALS = 100


def test_prop_27_save_data_round_trip():
    """
    Property 27: multi-region save data survives pack/unpack round-trip.
    Runs 100 random trials plus targeted boundary cases.
    """
    print("\n=== Property 27: Save Data Round-Trip (struct pack/unpack) ===")
    print("**Validates: multi-region save serialization contract**")
    print(f"Save struct size: {SAVE_STRUCT_SIZE} bytes")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(27)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        state = random_save_state(rng)
        if prop_save_data_round_trip(state):
            passed += 1
        else:
            failed += 1

    # Edge case: all-zero save (fresh game state)
    zero_state = {
        "current_region": 0,
        "unlock_flags":   0,
        "e4_defeated":    [0, 0, 0],
        "badges":         [0] * TOTAL_BADGES,
        "fly_locations":  [0] * TOTAL_FLY_LOCATIONS,
    }
    if prop_save_data_round_trip(zero_state):
        passed += 1
    else:
        failed += 1
        print("FAIL edge: all-zero (fresh game) state did not survive round-trip")

    # Edge case: all-max save (fully completed game)
    max_state = {
        "current_region": NUM_REGIONS - 1,
        "unlock_flags":   0x03,
        "e4_defeated":    [1, 1, 1],
        "badges":         [1] * TOTAL_BADGES,
        "fly_locations":  [0xFFFF] * TOTAL_FLY_LOCATIONS,
    }
    if prop_save_data_round_trip(max_state):
        passed += 1
    else:
        failed += 1
        print("FAIL edge: all-max (fully completed) state did not survive round-trip")

    # Edge case: Kanto only (other regions locked, no fly locations)
    kanto_only = {
        "current_region": REGION_KANTO,
        "unlock_flags":   0x00,
        "e4_defeated":    [0, 0, 0],
        "badges":         [1, 1, 1, 1, 1, 1, 1, 1] + [0] * 16,
        "fly_locations":  [0] * TOTAL_FLY_LOCATIONS,
    }
    if prop_save_data_round_trip(kanto_only):
        passed += 1
    else:
        failed += 1
        print("FAIL edge: Kanto-only state did not survive round-trip")

    # Edge case: Hoenn unlocked, mid-game progress
    hoenn_mid = {
        "current_region": REGION_HOENN,
        "unlock_flags":   UNLOCK_FLAG_HOENN,
        "e4_defeated":    [1, 0, 0],
        "badges":         [1] * 8 + [1, 1, 1, 0, 0, 0, 0, 0] + [0] * 8,
        "fly_locations":  [256, 257, 258, 0] + [0] * 12 + [0] * 32,
    }
    if prop_save_data_round_trip(hoenn_mid):
        passed += 1
    else:
        failed += 1
        print("FAIL edge: Hoenn mid-game state did not survive round-trip")

    # Edge case: all three regions unlocked, all E4 defeated
    all_regions = {
        "current_region": REGION_JOHTO,
        "unlock_flags":   UNLOCK_FLAG_HOENN | UNLOCK_FLAG_JOHTO,
        "e4_defeated":    [1, 1, 1],
        "badges":         [1] * TOTAL_BADGES,
        "fly_locations":  list(range(TOTAL_FLY_LOCATIONS)),  # map IDs 0..47
    }
    if prop_save_data_round_trip(all_regions):
        passed += 1
    else:
        failed += 1
        print("FAIL edge: all-regions-complete state did not survive round-trip")

    # Verify idempotency: pack(unpack(pack(state))) == pack(state)
    rng2 = random.Random(271)
    for _ in range(10):
        state = random_save_state(rng2)
        packed_once = pack_save(state)
        restored = unpack_save(packed_once)
        packed_twice = pack_save(restored)
        if packed_once != packed_twice:
            print("FAIL idempotency: pack(unpack(pack(state))) != pack(state)")
            failed += 1
        else:
            passed += 1

    print(f"Property 27 Results: {passed}/{passed + failed} passed")
    assert failed == 0, f"{failed} round-trip failure(s) — see FAIL output above"


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("=" * 70)
    print("Property-Based Tests: Multi-Region Save Data Round-Trip")
    print("Covers: Property 27 — Save Serialization Contract")
    print("=" * 70)
    print(f"Struct layout: current_region(u8) + unlock_flags(u8)")
    print(f"             + e4_defeated[{NUM_REGIONS}](u8) + badges[{TOTAL_BADGES}](u8)")
    print(f"             + fly_locations[{TOTAL_FLY_LOCATIONS}](u16)")
    print(f"Total size: {SAVE_STRUCT_SIZE} bytes")
    print(f"Minimum {NUM_TRIALS} iterations per property.")
    print("=" * 70)

    try:
        test_prop_27_save_data_round_trip()
        print("\nALL SAVE ROUND-TRIP TESTS PASSED!")
        return 0
    except AssertionError as e:
        print(f"\nTESTS FAILED: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
