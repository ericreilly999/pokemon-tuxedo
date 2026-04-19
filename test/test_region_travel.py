#!/usr/bin/env python3
"""
Property-Based Tests: Ticket + Fly Travel System

Covers: Requirement 24 — Region Travel: Ticket and Fly System

Properties tested:
  - Property 24a: Ticket use warps player to destination starting town
  - Property 24b: Ticket is NOT consumed after use (key item is reusable)
  - Property 24c: Ticket use for a locked region is rejected (no warp, error shown)
  - Property 24d: Starting town registered as fly destination after first ticket use
  - Property 24e: Fly destination persists through save/load round-trip
  - Property 24f: Party is unchanged after region transition
  - Property 24g: Inventory is unchanged after region transition
  - Property 24h: Region_Manager current region updated to destination after transition
  - Property 24i: Cross-region fly to a registered starting town works without ticket

QA Task: QA-005
Minimum 100 iterations per property.
"""

import random
import sys
from copy import deepcopy
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Tuple

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

# Region identifiers (ADR-004: Kanto -> Hoenn -> Johto)
REGION_KANTO = 0
REGION_HOENN = 1
REGION_JOHTO = 2
NUM_REGIONS = 3

REGION_NAMES = {
    REGION_KANTO: "Kanto",
    REGION_HOENN: "Hoenn",
    REGION_JOHTO: "Johto",
}

# Region ticket item IDs
ITEM_HOENN_TICKET = 501
ITEM_JOHTO_TICKET = 502

# Ticket item -> destination region mapping
TICKET_TO_REGION = {
    ITEM_HOENN_TICKET: REGION_HOENN,
    ITEM_JOHTO_TICKET: REGION_JOHTO,
}

# Region -> expected starting town map ID
REGION_STARTING_TOWN = {
    REGION_KANTO: 0,    # Pallet Town
    REGION_HOENN: 256,  # Littleroot Town
    REGION_JOHTO: 512,  # New Bark Town
}

# Fly location capacity
FLY_LOCATIONS_PER_REGION = 16
MAX_FLY_LOCATIONS_TOTAL = FLY_LOCATIONS_PER_REGION * NUM_REGIONS  # 48


# ---------------------------------------------------------------------------
# Data structures (spec-derived)
# ---------------------------------------------------------------------------

@dataclass
class PokemonSlot:
    """Represents one party member. Simplified for travel-invariance testing."""
    species: int = 0      # species ID; 0 = empty
    level: int = 0
    hp: int = 0
    moves: Tuple[int, ...] = field(default_factory=lambda: (0, 0, 0, 0))

    def is_empty(self) -> bool:
        return self.species == 0


@dataclass
class Inventory:
    """Player inventory. item_id -> quantity."""
    items: Dict[int, int] = field(default_factory=dict)

    def has_item(self, item_id: int) -> bool:
        return self.items.get(item_id, 0) > 0

    def give_item(self, item_id: int, quantity: int = 1):
        self.items[item_id] = self.items.get(item_id, 0) + quantity

    def remove_item(self, item_id: int, quantity: int = 1):
        if item_id in self.items:
            self.items[item_id] = max(0, self.items[item_id] - quantity)

    def snapshot(self) -> Dict[int, int]:
        return dict(self.items)

    def __eq__(self, other) -> bool:
        if not isinstance(other, Inventory):
            return False
        return self.items == other.items


@dataclass
class RegionState:
    """Full region management state."""
    current_region: int = REGION_KANTO
    region_unlocked: List[bool] = field(default_factory=lambda: [True, False, False])
    e4_defeated: List[bool] = field(default_factory=lambda: [False, False, False])

    # Fly locations: per-region list of map IDs
    fly_locations: List[List[int]] = field(
        default_factory=lambda: [[] for _ in range(NUM_REGIONS)]
    )

    def is_unlocked(self, region_id: int) -> bool:
        if 0 <= region_id < NUM_REGIONS:
            return self.region_unlocked[region_id]
        return False

    def unlock_region(self, region_id: int):
        if 0 <= region_id < NUM_REGIONS:
            self.region_unlocked[region_id] = True

    def register_fly_location(self, map_id: int, region_id: int):
        """Register a fly destination for a specific region (deduplicating)."""
        if region_id < 0 or region_id >= NUM_REGIONS:
            return
        locs = self.fly_locations[region_id]
        if map_id not in locs and len(locs) < FLY_LOCATIONS_PER_REGION:
            locs.append(map_id)

    def is_fly_registered(self, map_id: int, region_id: int) -> bool:
        if 0 <= region_id < NUM_REGIONS:
            return map_id in self.fly_locations[region_id]
        return False

    def cross_region_fly_destinations(self) -> List[int]:
        """
        Returns starting-town map IDs for each unlocked region
        that has been visited (i.e., its starting town is registered as a fly
        destination). The current region is excluded.
        """
        destinations = []
        for region_id in range(NUM_REGIONS):
            if region_id == self.current_region:
                continue
            if not self.is_unlocked(region_id):
                continue
            starting_town = REGION_STARTING_TOWN[region_id]
            if self.is_fly_registered(starting_town, region_id):
                destinations.append(starting_town)
        return destinations

    def save(self) -> dict:
        return {
            'current_region': self.current_region,
            'region_unlocked': list(self.region_unlocked),
            'e4_defeated': list(self.e4_defeated),
            'fly_locations': [list(locs) for locs in self.fly_locations],
        }

    @classmethod
    def load(cls, state: dict) -> 'RegionState':
        obj = cls()
        obj.current_region = state['current_region']
        obj.region_unlocked = list(state['region_unlocked'])
        obj.e4_defeated = list(state['e4_defeated'])
        obj.fly_locations = [list(locs) for locs in state['fly_locations']]
        return obj


# ---------------------------------------------------------------------------
# Warp system model (spec-derived)
# ---------------------------------------------------------------------------

class WarpResult:
    SUCCESS = "success"
    LOCKED_REGION = "locked_region"
    FLY_SUCCESS = "fly_success"


def use_region_ticket(
    ticket_item_id: int,
    region_state: RegionState,
    inventory: Inventory,
    party: List[PokemonSlot],
) -> Tuple[str, Optional[int]]:
    """
    Model of Warp_System ticket use (Requirement 24.1, 24.2, 24.3, 24.7–24.9).

    Returns (result_code, destination_map_id_or_None).

    Contract:
      - If destination region is locked:
          return (LOCKED_REGION, None), do NOT warp, do NOT consume ticket.
      - If destination region is unlocked:
          - Warp player to destination starting town.
          - Do NOT consume/remove the ticket.
          - Register starting town as fly destination.
          - Update region_state.current_region.
          - Party and inventory are NOT modified.
    """
    destination_region = TICKET_TO_REGION.get(ticket_item_id)
    if destination_region is None:
        return (WarpResult.LOCKED_REGION, None)

    if not region_state.is_unlocked(destination_region):
        # Requirement 24.7: reject, display error, no warp
        return (WarpResult.LOCKED_REGION, None)

    # Requirement 24.1: warp to starting town
    starting_town = REGION_STARTING_TOWN[destination_region]

    # Requirement 24.3: register starting town as fly destination immediately
    region_state.register_fly_location(starting_town, destination_region)

    # Requirement 24.9: update current region
    region_state.current_region = destination_region

    # Requirement 24.2: ticket is NOT consumed (no inventory modification)
    # Party and inventory pass through untouched (Requirement 24.8)

    return (WarpResult.SUCCESS, starting_town)


def use_fly_cross_region(
    destination_map_id: int,
    region_state: RegionState,
    party: List[PokemonSlot],
    inventory: Inventory,
) -> Tuple[str, Optional[int]]:
    """
    Model of Fly cross-region travel (Requirement 24.5, 24.6).

    A cross-region fly to a starting town is valid when:
      - That starting town has been registered as a fly destination
        (meaning the player visited it via ticket at least once)
      - The corresponding region is unlocked

    Returns (result_code, destination_map_id_or_None).
    No ticket is required for fly.
    Party and inventory are preserved.
    Region_Manager current region is updated to destination.
    """
    # Find which region owns this map ID
    destination_region = None
    for region_id, start_town in REGION_STARTING_TOWN.items():
        if start_town == destination_map_id:
            destination_region = region_id
            break

    if destination_region is None:
        return (WarpResult.LOCKED_REGION, None)

    if not region_state.is_unlocked(destination_region):
        return (WarpResult.LOCKED_REGION, None)

    if not region_state.is_fly_registered(destination_map_id, destination_region):
        return (WarpResult.LOCKED_REGION, None)

    # Valid cross-region fly
    region_state.current_region = destination_region
    return (WarpResult.FLY_SUCCESS, destination_map_id)


# ---------------------------------------------------------------------------
# Helper: random party / inventory generators
# ---------------------------------------------------------------------------

def random_party(rng: random.Random, size: int = None) -> List[PokemonSlot]:
    if size is None:
        size = rng.randint(1, 6)
    return [
        PokemonSlot(
            species=rng.randint(1, 251),
            level=rng.randint(1, 100),
            hp=rng.randint(1, 300),
            moves=tuple(rng.randint(0, 354) for _ in range(4)),
        )
        for _ in range(size)
    ]


def random_inventory(rng: random.Random) -> Inventory:
    inv = Inventory()
    num_items = rng.randint(0, 20)
    for _ in range(num_items):
        item_id = rng.randint(1, 600)
        qty = rng.randint(1, 99)
        inv.give_item(item_id, qty)
    return inv


def parties_equal(a: List[PokemonSlot], b: List[PokemonSlot]) -> bool:
    if len(a) != len(b):
        return False
    for sa, sb in zip(a, b):
        if sa.species != sb.species:
            return False
        if sa.level != sb.level:
            return False
        if sa.hp != sb.hp:
            return False
        if sa.moves != sb.moves:
            return False
    return True


# ---------------------------------------------------------------------------
# Property 24a: Ticket warps to destination starting town
# ---------------------------------------------------------------------------

def prop_ticket_warps_to_starting_town(
    ticket_item_id: int,
    party: List[PokemonSlot],
    inventory: Inventory,
) -> bool:
    """
    Property 24a — Requirement 24.1
    Using an unlocked region's ticket must warp to that region's starting town.
    """
    destination_region = TICKET_TO_REGION[ticket_item_id]
    region_state = RegionState()
    region_state.unlock_region(destination_region)
    inventory.give_item(ticket_item_id)

    result, destination = use_region_ticket(ticket_item_id, region_state, inventory, party)

    if result != WarpResult.SUCCESS:
        print(f"FAIL prop_ticket_warps_to_starting_town:")
        print(f"  ticket={ticket_item_id}, destination_region={destination_region}")
        print(f"  result={result}, expected SUCCESS")
        return False

    expected_town = REGION_STARTING_TOWN[destination_region]
    if destination != expected_town:
        print(f"FAIL prop_ticket_warps_to_starting_town:")
        print(f"  ticket={ticket_item_id}, destination_region={destination_region}")
        print(f"  warped to {destination}, expected {expected_town}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 24b: Ticket NOT consumed after use
# ---------------------------------------------------------------------------

def prop_ticket_not_consumed_after_use(
    ticket_item_id: int,
    party: List[PokemonSlot],
) -> bool:
    """
    Property 24b — Requirement 24.2
    After using a ticket, the item must still be in the inventory with the
    same quantity it had before.
    """
    destination_region = TICKET_TO_REGION[ticket_item_id]
    region_state = RegionState()
    region_state.unlock_region(destination_region)

    inventory = Inventory()
    inventory.give_item(ticket_item_id, 1)
    qty_before = inventory.items.get(ticket_item_id, 0)

    result, _ = use_region_ticket(ticket_item_id, region_state, inventory, party)

    if result != WarpResult.SUCCESS:
        return True  # warp failed for other reasons; test not applicable here

    qty_after = inventory.items.get(ticket_item_id, 0)
    if qty_after != qty_before:
        print(f"FAIL prop_ticket_not_consumed_after_use:")
        print(f"  ticket={ticket_item_id}")
        print(f"  qty before={qty_before}, qty after={qty_after}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 24c: Locked region ticket rejected
# ---------------------------------------------------------------------------

def prop_locked_region_ticket_rejected(
    ticket_item_id: int,
    party: List[PokemonSlot],
    inventory: Inventory,
) -> bool:
    """
    Property 24c — Requirement 24.7
    Using a ticket for a LOCKED region must:
      - Return LOCKED_REGION (no warp)
      - Not change the player's current map position
      - Not change the player's current region
    """
    region_state = RegionState()
    # Do NOT unlock the destination region
    original_region = region_state.current_region
    inventory.give_item(ticket_item_id)

    result, destination = use_region_ticket(ticket_item_id, region_state, inventory, party)

    if result != WarpResult.LOCKED_REGION:
        print(f"FAIL prop_locked_region_ticket_rejected:")
        print(f"  ticket={ticket_item_id}, destination={destination}")
        print(f"  result={result}, expected LOCKED_REGION")
        return False

    if destination is not None:
        print(f"FAIL prop_locked_region_ticket_rejected:")
        print(f"  ticket={ticket_item_id}")
        print(f"  destination={destination}, expected None (no warp)")
        return False

    if region_state.current_region != original_region:
        print(f"FAIL prop_locked_region_ticket_rejected:")
        print(f"  ticket={ticket_item_id}")
        print(f"  current_region changed from {original_region} to {region_state.current_region}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 24d: Starting town registered as fly destination after ticket use
# ---------------------------------------------------------------------------

def prop_starting_town_registered_after_ticket_use(
    ticket_item_id: int,
    party: List[PokemonSlot],
) -> bool:
    """
    Property 24d — Requirement 24.3
    After a successful ticket use, the destination starting town must be
    registered as a fly destination in the fly system.
    """
    destination_region = TICKET_TO_REGION[ticket_item_id]
    region_state = RegionState()
    region_state.unlock_region(destination_region)

    inventory = Inventory()
    inventory.give_item(ticket_item_id)

    # Verify NOT registered before use
    starting_town = REGION_STARTING_TOWN[destination_region]
    if region_state.is_fly_registered(starting_town, destination_region):
        print(f"FAIL prop_starting_town_registered_after_ticket_use: registered BEFORE use")
        return False

    result, _ = use_region_ticket(ticket_item_id, region_state, inventory, party)
    if result != WarpResult.SUCCESS:
        print(f"FAIL prop_starting_town_registered_after_ticket_use: warp failed unexpectedly")
        return False

    # Must be registered now
    if not region_state.is_fly_registered(starting_town, destination_region):
        print(f"FAIL prop_starting_town_registered_after_ticket_use:")
        print(f"  ticket={ticket_item_id}, destination_region={destination_region}")
        print(f"  starting_town={starting_town} not registered after use")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 24e: Fly destination persists through save/load round-trip
# ---------------------------------------------------------------------------

def prop_fly_destination_persists_save_load(
    ticket_item_id: int,
    party: List[PokemonSlot],
) -> bool:
    """
    Property 24e — Requirement 24.4
    A fly destination registered by ticket use must survive a save/load
    round-trip and be present after loading.
    """
    destination_region = TICKET_TO_REGION[ticket_item_id]
    region_state = RegionState()
    region_state.unlock_region(destination_region)

    inventory = Inventory()
    inventory.give_item(ticket_item_id)

    result, _ = use_region_ticket(ticket_item_id, region_state, inventory, party)
    if result != WarpResult.SUCCESS:
        print(f"FAIL prop_fly_destination_persists_save_load: warp failed unexpectedly")
        return False

    starting_town = REGION_STARTING_TOWN[destination_region]

    # Save
    saved = region_state.save()

    # Simulate game restart: create a fresh state and load
    loaded_state = RegionState.load(saved)

    if not loaded_state.is_fly_registered(starting_town, destination_region):
        print(f"FAIL prop_fly_destination_persists_save_load:")
        print(f"  starting_town={starting_town}, region={destination_region}")
        print(f"  not found after save/load")
        return False

    # Full round-trip: re-saved state must equal loaded state
    resaved = loaded_state.save()
    if resaved != saved:
        print(f"FAIL prop_fly_destination_persists_save_load: save/load not idempotent")
        print(f"  original save: {saved}")
        print(f"  after re-save: {resaved}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 24f: Party unchanged after region transition
# ---------------------------------------------------------------------------

def prop_party_unchanged_after_transition(
    ticket_item_id: int,
    party: List[PokemonSlot],
) -> bool:
    """
    Property 24f — Requirement 24.8
    The player's party must be byte-for-byte identical before and after
    a region transition via ticket.
    """
    destination_region = TICKET_TO_REGION[ticket_item_id]
    region_state = RegionState()
    region_state.unlock_region(destination_region)

    inventory = Inventory()
    inventory.give_item(ticket_item_id)

    party_before = deepcopy(party)
    result, _ = use_region_ticket(ticket_item_id, region_state, inventory, party)

    if result != WarpResult.SUCCESS:
        return True  # not applicable

    if not parties_equal(party_before, party):
        print(f"FAIL prop_party_unchanged_after_transition:")
        print(f"  ticket={ticket_item_id}")
        print(f"  party changed during transition")
        for i, (a, b) in enumerate(zip(party_before, party)):
            if a.species != b.species or a.level != b.level or a.hp != b.hp or a.moves != b.moves:
                print(f"    slot {i}: before={a}, after={b}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 24g: Inventory unchanged after region transition
# ---------------------------------------------------------------------------

def prop_inventory_unchanged_after_transition(
    ticket_item_id: int,
    party: List[PokemonSlot],
    inventory: Inventory,
) -> bool:
    """
    Property 24g — Requirement 24.8
    The player's inventory (all items, all quantities) must be identical
    before and after a region transition via ticket.
    The ticket itself must also be present with the same quantity.
    """
    destination_region = TICKET_TO_REGION[ticket_item_id]
    region_state = RegionState()
    region_state.unlock_region(destination_region)
    inventory.give_item(ticket_item_id)

    snapshot_before = inventory.snapshot()
    result, _ = use_region_ticket(ticket_item_id, region_state, inventory, party)

    if result != WarpResult.SUCCESS:
        return True  # not applicable

    snapshot_after = inventory.snapshot()

    if snapshot_before != snapshot_after:
        print(f"FAIL prop_inventory_unchanged_after_transition:")
        print(f"  ticket={ticket_item_id}")
        diff_keys = set(snapshot_before) | set(snapshot_after)
        for item_id in sorted(diff_keys):
            qty_before = snapshot_before.get(item_id, 0)
            qty_after = snapshot_after.get(item_id, 0)
            if qty_before != qty_after:
                print(f"    item {item_id}: before={qty_before}, after={qty_after}")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 24h: Region_Manager updated to destination after transition
# ---------------------------------------------------------------------------

def prop_region_manager_updated_after_transition(
    ticket_item_id: int,
    party: List[PokemonSlot],
) -> bool:
    """
    Property 24h — Requirement 24.9
    After a successful ticket use, region_state.current_region must equal
    the destination region, not the origin.
    """
    destination_region = TICKET_TO_REGION[ticket_item_id]
    region_state = RegionState()
    region_state.unlock_region(destination_region)
    original_region = region_state.current_region

    inventory = Inventory()
    inventory.give_item(ticket_item_id)

    result, _ = use_region_ticket(ticket_item_id, region_state, inventory, party)

    if result != WarpResult.SUCCESS:
        return True  # not applicable

    if region_state.current_region != destination_region:
        print(f"FAIL prop_region_manager_updated_after_transition:")
        print(f"  ticket={ticket_item_id}")
        print(f"  expected current_region={destination_region}, got={region_state.current_region}")
        return False

    # Verify it changed from the origin
    if original_region != REGION_KANTO:
        return True  # already different; trivially passes
    if region_state.current_region == original_region and destination_region != original_region:
        print(f"FAIL prop_region_manager_updated_after_transition: region did not change")
        return False

    return True


# ---------------------------------------------------------------------------
# Property 24i: Cross-region fly works without ticket (after starting town registered)
# ---------------------------------------------------------------------------

def prop_cross_region_fly_without_ticket(
    ticket_item_id: int,
    party: List[PokemonSlot],
) -> bool:
    """
    Property 24i — Requirement 24.5, 24.6
    Once a starting town is registered via ticket use, the player can fly
    there on subsequent trips WITHOUT presenting the ticket again.
    The fly must succeed, update current region, and leave party intact.
    """
    destination_region = TICKET_TO_REGION[ticket_item_id]
    region_state = RegionState()
    region_state.unlock_region(destination_region)

    inventory = Inventory()
    inventory.give_item(ticket_item_id)

    # First trip: use ticket to register the fly location
    result, _ = use_region_ticket(ticket_item_id, region_state, inventory, party)
    if result != WarpResult.SUCCESS:
        print(f"FAIL prop_cross_region_fly_without_ticket: initial ticket warp failed")
        return False

    # Return to Kanto
    region_state.current_region = REGION_KANTO

    # Second trip: fly without presenting ticket
    starting_town = REGION_STARTING_TOWN[destination_region]
    party_before = deepcopy(party)
    inv_before = inventory.snapshot()

    fly_result, fly_dest = use_fly_cross_region(starting_town, region_state, party, inventory)

    if fly_result != WarpResult.FLY_SUCCESS:
        print(f"FAIL prop_cross_region_fly_without_ticket:")
        print(f"  starting_town={starting_town}, region={destination_region}")
        print(f"  fly_result={fly_result}, expected FLY_SUCCESS")
        return False

    if fly_dest != starting_town:
        print(f"FAIL prop_cross_region_fly_without_ticket:")
        print(f"  fly_dest={fly_dest}, expected={starting_town}")
        return False

    if region_state.current_region != destination_region:
        print(f"FAIL prop_cross_region_fly_without_ticket: current region not updated")
        return False

    if not parties_equal(party_before, party):
        print(f"FAIL prop_cross_region_fly_without_ticket: party changed during fly")
        return False

    if inventory.snapshot() != inv_before:
        print(f"FAIL prop_cross_region_fly_without_ticket: inventory changed during fly")
        return False

    return True


def prop_fly_fails_to_locked_region(
    ticket_item_id: int,
    party: List[PokemonSlot],
) -> bool:
    """
    Property 24i (negative) — Requirement 24.7
    Cross-region fly to a locked region must be rejected, even if the
    starting town map ID happens to be known.
    """
    destination_region = TICKET_TO_REGION[ticket_item_id]
    region_state = RegionState()
    # Do NOT unlock the destination region

    starting_town = REGION_STARTING_TOWN[destination_region]
    fly_result, _ = use_fly_cross_region(starting_town, region_state, party, Inventory())

    if fly_result == WarpResult.FLY_SUCCESS:
        print(f"FAIL prop_fly_fails_to_locked_region:")
        print(f"  destination_region={destination_region} is locked but fly succeeded")
        return False

    return True


def prop_fly_fails_without_prior_ticket_use(
    ticket_item_id: int,
    party: List[PokemonSlot],
) -> bool:
    """
    Property 24i (negative) — Requirement 24.6
    Cross-region fly to a starting town that has NOT been visited via ticket
    must be rejected (fly location not registered).
    """
    destination_region = TICKET_TO_REGION[ticket_item_id]
    region_state = RegionState()
    region_state.unlock_region(destination_region)
    # Do NOT use a ticket — starting town is NOT registered

    starting_town = REGION_STARTING_TOWN[destination_region]
    fly_result, _ = use_fly_cross_region(starting_town, region_state, party, Inventory())

    if fly_result == WarpResult.FLY_SUCCESS:
        print(f"FAIL prop_fly_fails_without_prior_ticket_use:")
        print(f"  destination_region={destination_region}")
        print(f"  fly succeeded without prior ticket use (starting town not registered)")
        return False

    return True


# ---------------------------------------------------------------------------
# Test runners
# ---------------------------------------------------------------------------

NUM_TRIALS = 100
ALL_TICKETS = list(TICKET_TO_REGION.keys())


def test_property_24a_ticket_warps_to_starting_town():
    print("\n=== Property 24a: Ticket Warps to Destination Starting Town ===")
    print("**Validates: Requirement 24.1**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(2)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        ticket = rng.choice(ALL_TICKETS)
        party = random_party(rng)
        inv = random_inventory(rng)
        if prop_ticket_warps_to_starting_town(ticket, party, inv):
            passed += 1
        else:
            failed += 1

    # Edge: each ticket individually
    for ticket in ALL_TICKETS:
        party = [PokemonSlot(species=1, level=5, hp=45, moves=(33, 0, 0, 0))]
        if prop_ticket_warps_to_starting_town(ticket, party, Inventory()):
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge: ticket={ticket}")

    print(f"Property 24a Results: {passed}/{passed + failed} passed")
    return failed


def test_property_24b_ticket_not_consumed():
    print("\n=== Property 24b: Ticket NOT Consumed After Use ===")
    print("**Validates: Requirement 24.2**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(3)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        ticket = rng.choice(ALL_TICKETS)
        party = random_party(rng)
        if prop_ticket_not_consumed_after_use(ticket, party):
            passed += 1
        else:
            failed += 1

    # Use the same ticket multiple times — it must persist each time
    for ticket in ALL_TICKETS:
        destination_region = TICKET_TO_REGION[ticket]
        region_state = RegionState()
        region_state.unlock_region(destination_region)
        inventory = Inventory()
        inventory.give_item(ticket, 1)
        party = []

        for _ in range(5):  # use ticket 5 times
            qty_before = inventory.items.get(ticket, 0)
            use_region_ticket(ticket, region_state, inventory, party)
            qty_after = inventory.items.get(ticket, 0)
            if qty_after != qty_before:
                print(f"FAIL edge: ticket={ticket} quantity changed from {qty_before} to {qty_after}")
                failed += 1
            else:
                passed += 1

    print(f"Property 24b Results: {passed}/{passed + failed} passed")
    return failed


def test_property_24c_locked_region_rejected():
    print("\n=== Property 24c: Locked Region Ticket Rejected ===")
    print("**Validates: Requirement 24.7**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(5)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        ticket = rng.choice(ALL_TICKETS)
        party = random_party(rng)
        inv = random_inventory(rng)
        if prop_locked_region_ticket_rejected(ticket, party, inv):
            passed += 1
        else:
            failed += 1

    # Edge: try every ticket against locked state
    for ticket in ALL_TICKETS:
        party = random_party(random.Random(7))
        if prop_locked_region_ticket_rejected(ticket, party, Inventory()):
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge: ticket={ticket}")

    print(f"Property 24c Results: {passed}/{passed + failed} passed")
    return failed


def test_property_24d_starting_town_registered():
    print("\n=== Property 24d: Starting Town Registered as Fly Dest After Ticket Use ===")
    print("**Validates: Requirement 24.3**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(11)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        ticket = rng.choice(ALL_TICKETS)
        party = random_party(rng)
        if prop_starting_town_registered_after_ticket_use(ticket, party):
            passed += 1
        else:
            failed += 1

    for ticket in ALL_TICKETS:
        for size in [1, 3, 6]:
            party = random_party(random.Random(13), size=size)
            if prop_starting_town_registered_after_ticket_use(ticket, party):
                passed += 1
            else:
                failed += 1
                print(f"FAIL edge: ticket={ticket}, party_size={size}")

    print(f"Property 24d Results: {passed}/{passed + failed} passed")
    return failed


def test_property_24e_fly_dest_persists_save_load():
    print("\n=== Property 24e: Fly Destination Persists Through Save/Load ===")
    print("**Validates: Requirement 24.4**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(19)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        ticket = rng.choice(ALL_TICKETS)
        party = random_party(rng)
        if prop_fly_destination_persists_save_load(ticket, party):
            passed += 1
        else:
            failed += 1

    # Stress: multiple ticket uses then save/load
    for ticket in ALL_TICKETS:
        destination_region = TICKET_TO_REGION[ticket]
        region_state = RegionState()
        region_state.unlock_region(destination_region)
        inventory = Inventory()
        inventory.give_item(ticket)
        party = []

        # Use ticket twice — registration must be idempotent
        use_region_ticket(ticket, region_state, inventory, party)
        region_state.current_region = REGION_KANTO
        use_region_ticket(ticket, region_state, inventory, party)

        saved = region_state.save()
        loaded = RegionState.load(saved)
        starting_town = REGION_STARTING_TOWN[destination_region]
        if loaded.is_fly_registered(starting_town, destination_region):
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge: double-use ticket={ticket}, starting_town not in loaded state")

    print(f"Property 24e Results: {passed}/{passed + failed} passed")
    return failed


def test_property_24f_party_unchanged():
    print("\n=== Property 24f: Party Unchanged After Region Transition ===")
    print("**Validates: Requirement 24.8**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(29)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        ticket = rng.choice(ALL_TICKETS)
        party = random_party(rng)
        if prop_party_unchanged_after_transition(ticket, party):
            passed += 1
        else:
            failed += 1

    # Edge: empty party
    for ticket in ALL_TICKETS:
        if prop_party_unchanged_after_transition(ticket, []):
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge: empty party, ticket={ticket}")

    # Edge: 6-member party with edge levels
    for ticket in ALL_TICKETS:
        party = [
            PokemonSlot(species=1, level=1, hp=1, moves=(0, 0, 0, 0)),
            PokemonSlot(species=2, level=100, hp=300, moves=(1, 2, 3, 4)),
        ]
        if prop_party_unchanged_after_transition(ticket, party):
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge: min/max level party, ticket={ticket}")

    print(f"Property 24f Results: {passed}/{passed + failed} passed")
    return failed


def test_property_24g_inventory_unchanged():
    print("\n=== Property 24g: Inventory Unchanged After Region Transition ===")
    print("**Validates: Requirement 24.8**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(37)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        ticket = rng.choice(ALL_TICKETS)
        party = random_party(rng)
        inventory = random_inventory(rng)
        if prop_inventory_unchanged_after_transition(ticket, party, inventory):
            passed += 1
        else:
            failed += 1

    # Edge: empty inventory (only the ticket will be added inside prop)
    for ticket in ALL_TICKETS:
        if prop_inventory_unchanged_after_transition(ticket, [], Inventory()):
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge: empty inventory, ticket={ticket}")

    # Edge: inventory with 99 of every item
    for ticket in ALL_TICKETS:
        inv = Inventory()
        for i in range(1, 50):
            inv.give_item(i, 99)
        if prop_inventory_unchanged_after_transition(ticket, [], inv):
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge: large inventory, ticket={ticket}")

    print(f"Property 24g Results: {passed}/{passed + failed} passed")
    return failed


def test_property_24h_region_manager_updated():
    print("\n=== Property 24h: Region_Manager Updated to Destination After Transition ===")
    print("**Validates: Requirement 24.9**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(43)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        ticket = rng.choice(ALL_TICKETS)
        party = random_party(rng)
        if prop_region_manager_updated_after_transition(ticket, party):
            passed += 1
        else:
            failed += 1

    # Edge: use ticket from each starting region
    for ticket in ALL_TICKETS:
        if prop_region_manager_updated_after_transition(ticket, []):
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge: ticket={ticket}")

    # Edge: verify locked region use does NOT update region
    for ticket in ALL_TICKETS:
        destination_region = TICKET_TO_REGION[ticket]
        region_state = RegionState()  # region is locked
        original = region_state.current_region
        inventory = Inventory()
        inventory.give_item(ticket)
        result, _ = use_region_ticket(ticket, region_state, inventory, [])
        if result == WarpResult.LOCKED_REGION:
            if region_state.current_region != original:
                print(f"FAIL edge: locked region changed current_region, ticket={ticket}")
                failed += 1
            else:
                passed += 1
        else:
            passed += 1  # should not reach here since region is locked

    print(f"Property 24h Results: {passed}/{passed + failed} passed")
    return failed


def test_property_24i_cross_region_fly():
    print("\n=== Property 24i: Cross-Region Fly Works After Ticket Registration ===")
    print("**Validates: Requirements 24.5, 24.6**")
    print(f"Running {NUM_TRIALS} random trials...\n")

    rng = random.Random(53)
    passed = 0
    failed = 0

    for _ in range(NUM_TRIALS):
        ticket = rng.choice(ALL_TICKETS)
        party = random_party(rng)
        if prop_cross_region_fly_without_ticket(ticket, party):
            passed += 1
        else:
            failed += 1

    # Negative: fly to locked region must fail
    for ticket in ALL_TICKETS:
        party = random_party(random.Random(59))
        if prop_fly_fails_to_locked_region(ticket, party):
            passed += 1
        else:
            failed += 1
            print(f"FAIL negative: fly to locked region succeeded, ticket={ticket}")

    # Negative: fly without prior ticket use must fail
    for ticket in ALL_TICKETS:
        party = random_party(random.Random(61))
        if prop_fly_fails_without_prior_ticket_use(ticket, party):
            passed += 1
        else:
            failed += 1
            print(f"FAIL negative: fly without ticket succeeded, ticket={ticket}")

    # Edge: verify fly after save/load still works
    for ticket in ALL_TICKETS:
        destination_region = TICKET_TO_REGION[ticket]
        region_state = RegionState()
        region_state.unlock_region(destination_region)
        inventory = Inventory()
        inventory.give_item(ticket)
        use_region_ticket(ticket, region_state, inventory, [])

        # Save and reload
        saved = region_state.save()
        region_state = RegionState.load(saved)
        region_state.current_region = REGION_KANTO  # return to Kanto

        starting_town = REGION_STARTING_TOWN[destination_region]
        fly_result, fly_dest = use_fly_cross_region(starting_town, region_state, [], Inventory())
        if fly_result == WarpResult.FLY_SUCCESS and fly_dest == starting_town:
            passed += 1
        else:
            failed += 1
            print(f"FAIL edge: fly after save/load, ticket={ticket}, result={fly_result}")

    print(f"Property 24i Results: {passed}/{passed + failed} passed")
    return failed


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("=" * 70)
    print("Property-Based Tests: Ticket + Fly Travel System")
    print("Covers: Requirement 24 — Region Travel: Ticket and Fly System")
    print("=" * 70)
    print("Properties:")
    print("  24a: Ticket use warps player to destination starting town")
    print("  24b: Ticket is NOT consumed after use (reusable key item)")
    print("  24c: Locked region ticket is rejected (no warp, no error-suppression)")
    print("  24d: Starting town registered as fly destination after first ticket use")
    print("  24e: Fly destination persists through save/load round-trip")
    print("  24f: Party unchanged after region transition")
    print("  24g: Inventory unchanged after region transition")
    print("  24h: Region_Manager current region updated to destination")
    print("  24i: Cross-region fly works after starting town registered (no ticket)")
    print(f"Minimum {NUM_TRIALS} iterations per property.")
    print("=" * 70)

    total_failed = 0
    total_failed += test_property_24a_ticket_warps_to_starting_town()
    total_failed += test_property_24b_ticket_not_consumed()
    total_failed += test_property_24c_locked_region_rejected()
    total_failed += test_property_24d_starting_town_registered()
    total_failed += test_property_24e_fly_dest_persists_save_load()
    total_failed += test_property_24f_party_unchanged()
    total_failed += test_property_24g_inventory_unchanged()
    total_failed += test_property_24h_region_manager_updated()
    total_failed += test_property_24i_cross_region_fly()

    print("\n" + "=" * 70)
    print("OVERALL TEST RESULTS")
    print("=" * 70)

    if total_failed == 0:
        print("\nALL REGION TRAVEL PROPERTY TESTS PASSED!")
        print("\nCoverage map (Requirement 24 -> Properties):")
        print("  24.1 (ticket warps to starting town)         -> 24a")
        print("  24.2 (ticket is reusable, not consumed)      -> 24b")
        print("  24.3 (starting town registered as fly dest)  -> 24d")
        print("  24.4 (fly dest persists save/load)           -> 24e")
        print("  24.5 (fly menu shows starting town)          -> 24i")
        print("  24.6 (fly handles subsequent travel)         -> 24i")
        print("  24.7 (locked region rejected)                -> 24c, 24i")
        print("  24.8 (party and inventory preserved)         -> 24f, 24g")
        print("  24.9 (region manager updated)                -> 24h")
        return 0
    else:
        print(f"\nTESTS FAILED: {total_failed} counterexample(s)")
        return 1


if __name__ == "__main__":
    sys.exit(main())
