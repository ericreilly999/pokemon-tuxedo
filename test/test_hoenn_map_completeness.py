#!/usr/bin/env python3
"""
Deterministic Test: Hoenn Map Completeness (Req 17.4)

Asserts that exactly 104 Hoenn maps exist in data/maps/, each represented
as a directory containing a map.json file.

The canonical list of 104 Hoenn maps is read from data/maps/map_groups.json
using the keys gMapGroup_HoennTownsAndRoutes (50 maps) and
gMapGroup_HoennDungeons (54 maps).

QA Task: QA-001
"""

import json
import os
import sys

# ---------------------------------------------------------------------------
# Locate repo root relative to this test file
# ---------------------------------------------------------------------------

# This file is at <repo_root>/test/test_hoenn_map_completeness.py
_TEST_DIR  = os.path.dirname(os.path.abspath(__file__))
_REPO_ROOT = os.path.dirname(_TEST_DIR)

MAP_GROUPS_JSON = os.path.join(_REPO_ROOT, "data", "maps", "map_groups.json")
MAPS_DIR        = os.path.join(_REPO_ROOT, "data", "maps")

EXPECTED_TOTAL = 104


# ---------------------------------------------------------------------------
# Helper: load the canonical Hoenn map list from map_groups.json
# ---------------------------------------------------------------------------

def load_hoenn_map_names() -> list:
    """
    Read data/maps/map_groups.json and return the combined sorted list of
    Hoenn map directory names from:
      - gMapGroup_HoennTownsAndRoutes
      - gMapGroup_HoennDungeons
    """
    with open(MAP_GROUPS_JSON, "r", encoding="utf-8") as fh:
        data = json.load(fh)

    towns_and_routes = data.get("gMapGroup_HoennTownsAndRoutes", [])
    dungeons         = data.get("gMapGroup_HoennDungeons", [])

    all_hoenn = towns_and_routes + dungeons
    return all_hoenn


# ---------------------------------------------------------------------------
# Test
# ---------------------------------------------------------------------------

def test_hoenn_map_completeness_104_maps():
    """
    Req 17.4 — Exactly 104 Hoenn map directories must exist in data/maps/,
    each containing a map.json file.

    Failure output lists every missing map so the build error is actionable.
    """
    print("\n=== Hoenn Map Completeness (Req 17.4) ===")
    print(f"map_groups.json: {MAP_GROUPS_JSON}")
    print(f"maps directory:  {MAPS_DIR}")
    print(f"Expected maps:   {EXPECTED_TOTAL}")

    # --- Step 1: Load canonical list ---
    assert os.path.isfile(MAP_GROUPS_JSON), (
        f"map_groups.json not found at {MAP_GROUPS_JSON}. "
        "Cannot determine canonical Hoenn map list."
    )

    expected_maps = load_hoenn_map_names()

    assert len(expected_maps) == EXPECTED_TOTAL, (
        f"map_groups.json lists {len(expected_maps)} Hoenn maps but "
        f"expected exactly {EXPECTED_TOTAL}. "
        f"Check gMapGroup_HoennTownsAndRoutes and gMapGroup_HoennDungeons."
    )

    print(f"Loaded {len(expected_maps)} expected map names from map_groups.json.")

    # --- Step 2: Verify each expected map directory + map.json exist ---
    missing_dirs   = []   # directory itself absent
    missing_jsons  = []   # directory present but map.json absent

    for map_name in expected_maps:
        map_dir  = os.path.join(MAPS_DIR, map_name)
        map_json = os.path.join(map_dir, "map.json")

        if not os.path.isdir(map_dir):
            missing_dirs.append(map_name)
        elif not os.path.isfile(map_json):
            missing_jsons.append(map_name)

    # --- Step 3: Report and assert ---
    if missing_dirs:
        print(f"\nMISSING DIRECTORIES ({len(missing_dirs)}):")
        for name in sorted(missing_dirs):
            print(f"  data/maps/{name}/")

    if missing_jsons:
        print(f"\nDIRECTORIES MISSING map.json ({len(missing_jsons)}):")
        for name in sorted(missing_jsons):
            print(f"  data/maps/{name}/map.json")

    total_missing = len(missing_dirs) + len(missing_jsons)

    if total_missing == 0:
        print(f"\nAll {EXPECTED_TOTAL} Hoenn maps verified. PASS")
    else:
        print(f"\n{total_missing} problem(s) found.")

    assert total_missing == 0, (
        f"{len(missing_dirs)} missing director(ies) and "
        f"{len(missing_jsons)} director(ies) missing map.json. "
        f"See output above for details."
    )


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("=" * 70)
    print("Hoenn Map Completeness Test")
    print("Covers: Requirement 17.4 — 104 Hoenn maps in data/maps/")
    print("=" * 70)

    try:
        test_hoenn_map_completeness_104_maps()
        print("\nHOENN MAP COMPLETENESS TEST PASSED!")
        return 0
    except AssertionError as e:
        print(f"\nTEST FAILED: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
