# Requirements Document

## Introduction

Pokemon Tuxedo is a Pokemon FireRed ROM hack featuring multi-region progression, dynamic level scaling, and quality-of-life improvements. The project currently exists across two codebases: the root directory (pokefirered-based, containing all custom Tuxedo game systems and 5426+ property-based tests) and the `pokemon-crossroads/` subdirectory (pokeemerald-based fork containing 104 converted Hoenn maps, tilesets, scripts, trainer data, and encounter data).

This consolidation effort merges both codebases into a single unified `pokemon-tuxedo/` directory, back-ports all custom Tuxedo game systems into it, integrates the Hoenn map data, cleans up obsolete files, and ensures the build system produces a working ROM. All existing game features must be preserved and all tests must continue to pass.

### Project Background

**For complete project context, see:**
- `README.md` - Project overview, features, roadmap, and statistics
- `CHECKPOINT_PHASE_4_MVP.md` - Current MVP status (Kanto + Hoenn complete)
- `.kiro/specs/pokemon-tuxedo-full-regions/decisions.md` - Architecture Decision Records (ADR-001 through ADR-005)
- `.kiro/specs/pokemon-tuxedo-full-regions/tasks.md` - Original multi-region implementation plan

**Current Project State:**
- Phases 1-4 complete (MVP ready with Kanto + Hoenn)
- All 5426+ property-based tests passing
- Sinnoh descoped (ADR-003), Johto pending (Phase 5)
- Region progression: Kanto → Hoenn → Johto (ADR-004)

### Directory Structure Overview

**Root Directory (pokefirered-based):**
```
root/
├── src/                    # Custom Tuxedo game systems (9 C files)
│   ├── region_manager.c    # Multi-region progression and badge tracking
│   ├── level_scaler.c      # Dynamic level scaling by region and badges
│   ├── fly_location_discovery.c  # Region-aware fly system
│   ├── multi_region_save.c # Extended save data for multi-region state
│   ├── item_use.c          # Region ticket handlers
│   ├── pokemon_tuxedo.c    # Main integration and initialization
│   ├── exp_multipliers.c   # Lucky/Mystic/Magic Egg multipliers
│   ├── full_party_exp.c    # Full party experience distribution
│   └── game_settings.c     # Battle mode and game speed settings
├── include/                # Header files for all custom systems
├── test/                   # 5426+ property-based tests (21 test files)
│   ├── test_region_*.c     # Region management tests
│   ├── test_level_scaling.c
│   ├── test_fly_system.c
│   └── ...
├── tools/map_converter/    # PowerShell scripts for map conversion
│   ├── integrate_hoenn_maps.ps1
│   ├── copy_hoenn_*.ps1
│   └── batch_convert_*.ps1
├── .kiro/specs/            # Specification documents
│   └── pokemon-tuxedo-full-regions/
│       ├── requirements.md  # Original multi-region requirements
│       ├── design.md        # Technical design
│       ├── tasks.md         # Implementation plan (Phases 1-6)
│       └── decisions.md     # Architecture Decision Records
├── CHECKPOINT_*.md         # Phase completion documentation
└── README.md               # Project overview and features
```

**pokemon-crossroads/ Subdirectory (pokeemerald-based):**
```
pokemon-crossroads/
├── src/                    # pokeemerald source code
│   └── data/               # Hoenn trainer and encounter data
│       ├── hoenn_trainers.party
│       └── hoenn_wild_encounters.json
├── include/                # pokeemerald headers
├── data/
│   ├── maps/               # 104 Hoenn maps (Routes 101-134, cities, dungeons)
│   ├── layouts/            # Hoenn map layouts
│   ├── tilesets/           # Hoenn tilesets (primary and secondary)
│   └── scripts/            # Hoenn event scripts
├── build.ps1               # Build script for Windows
├── Makefile                # Build system
└── config.mk               # Build configuration
```

### Key Concepts

**Multi-Region System:**
- 3 regions total: Kanto (base game), Hoenn (104 maps converted), Johto (pending Phase 5)
- Each region has 8 badges, 8 Gym Leaders, 4 Elite Four members, 1 Champion
- Region unlock sequence: Defeat Kanto E4 → Hoenn unlocks → Defeat Hoenn E4 → Johto unlocks
- Region tickets (Hoenn Ticket, Johto Ticket) awarded on unlock for fast travel

**Level Scaling:**
- Wild Pokemon: `region_starting_level + (badges × 5)` to `region_starting_level + (badges × 5) + 10`
- Region starting levels: Kanto (2), Hoenn (60), Johto (110)
- Trainers scale above wild Pokemon: Gym Leaders (+5), Elite Four (+10), Champion (+15)
- Elite Four bonus: +10 levels when defeated in current region

**Save Data:**
- Extended to store: current region, unlock flags, 24 badges, 48 fly locations, Elite Four status
- Backward compatible with pre-multi-region saves
- Round-trip property: `load(save(state)) == state`

**Testing:**
- 5426+ property-based tests using Theft library (minimum 100 iterations per property)
- 14 named properties covering region progression, level scaling, fly system, save data
- Integration tests for region transitions, level scaling, save/load
- Manual testing checklists for Hoenn region

### Consolidation Goal

The goal is to create a single `pokemon-tuxedo/` directory that:
1. Contains the pokeemerald build system (from pokemon-crossroads)
2. Contains all 104 Hoenn maps and data (from pokemon-crossroads)
3. Contains all 9 custom Tuxedo game systems (from root src/)
4. Contains all tests and tools (from root test/ and tools/)
5. Builds a working ROM with Kanto + Hoenn playable
6. Passes all 5426+ tests
7. Is ready for Phase 5 (Johto integration from CrystalDust)

---

## Glossary

- **Build_System**: The Makefile, build.ps1, and associated toolchain configuration that compiles the ROM.
- **Consolidation_Tool**: Any PowerShell script or manual process used to perform the codebase merge.
- **CrystalDust**: The pokeemerald-based Johto ROM hack (https://github.com/Sierraffinity/CrystalDust) designated as the Johto map source per ADR-002.
- **EXP_Multiplier_System**: The system in `src/exp_multipliers.c` that applies Lucky Egg (2x), Mystic Egg (3x), and Magic Egg (4x) multipliers to experience gains.
- **Fly_System**: The fly location discovery and region-aware fly menu system in `src/fly_location_discovery.c`.
- **Full_Party_Exp_System**: The system in `src/full_party_exp.c` that awards full (unsplit) experience to all party members.
- **Game_Settings_System**: The system in `src/game_settings.c` that manages battle mode and game speed settings.
- **Hoenn_Map_Data**: The 104 converted Hoenn maps, tilesets, scripts, trainer data, and encounter data currently in `pokemon-crossroads/data/maps/` and related directories.
- **Level_Scaler**: The dynamic level scaling system in `src/level_scaler.c` that scales wild Pokemon and trainer levels based on badge count and region.
- **Multi_Region_Save**: The save/load system in `src/multi_region_save.c` that persists multi-region state across sessions.
- **Pokemon_Crossroads**: The pokeemerald-based fork subdirectory at `pokemon-crossroads/` that contains Hoenn map data and serves as the source for consolidation.
- **Pokemon_Tuxedo**: The consolidated, unified ROM hack codebase that will be the single source of truth after consolidation.
- **Region_Manager**: The region management system in `src/region_manager.c` that tracks current region, badge counts, Elite Four status, and region unlock state.
- **Root_Codebase**: The pokefirered-based root directory containing all custom Tuxedo game systems, tests, and tools.
- **Tuxedo_Systems**: The collective set of custom source files: `region_manager.c`, `level_scaler.c`, `fly_location_discovery.c`, `multi_region_save.c`, `item_use.c`, `pokemon_tuxedo.c`, `exp_multipliers.c`, `full_party_exp.c`, `game_settings.c`, and their corresponding headers.

---

## Requirements

### Requirement 1: Codebase Rename and Directory Structure

**User Story:** As a developer, I want the pokemon-crossroads directory renamed to pokemon-tuxedo, so that the codebase name accurately reflects the project it contains.

#### Acceptance Criteria

1. THE Consolidation_Tool SHALL rename the `pokemon-crossroads/` directory to `pokemon-tuxedo/`.
2. WHEN the rename is complete, THE Pokemon_Tuxedo directory SHALL contain the full pokeemerald-based build system including `Makefile`, `build.ps1`, `config.mk`, and all associated toolchain files.
3. WHEN the rename is complete, THE Pokemon_Tuxedo directory SHALL retain all 104 Hoenn maps in `data/maps/` with their existing structure.
4. THE Pokemon_Tuxedo directory SHALL be the single authoritative build target for the ROM hack going forward.

---

### Requirement 2: Back-Port Tuxedo Game Systems

**User Story:** As a developer, I want all custom Tuxedo game systems copied from the root codebase into pokemon-tuxedo/src/ and pokemon-tuxedo/include/, so that the consolidated codebase contains all game features.

#### Acceptance Criteria

1. THE Consolidation_Tool SHALL copy the following source files from the root `src/` into `pokemon-tuxedo/src/`:
   - `region_manager.c`
   - `level_scaler.c`
   - `fly_location_discovery.c`
   - `multi_region_save.c`
   - `item_use.c`
   - `pokemon_tuxedo.c`
   - `exp_multipliers.c`
   - `full_party_exp.c`
   - `game_settings.c`
2. THE Consolidation_Tool SHALL copy the following header files from the root `include/` into `pokemon-tuxedo/include/`:
   - `region_manager.h`
   - `level_scaler.h`
   - `fly_location_discovery.h`
   - `multi_region_save.h`
   - `pokemon_tuxedo.h`
   - `exp_multipliers.h`
   - `full_party_exp.h`
   - `game_settings.h`
   - `system_integration.h`
3. WHEN a Tuxedo source file references a symbol that does not exist in the pokeemerald base, THE Consolidation_Tool SHALL document the missing symbol in a compatibility report.
4. IF a Tuxedo source file conflicts with an existing file in `pokemon-tuxedo/src/`, THEN THE Consolidation_Tool SHALL preserve the Tuxedo version and archive the original with a `.orig` suffix.

---

### Requirement 3: Hoenn Map Data Integration

**User Story:** As a developer, I want the Hoenn map data already present in pokemon-crossroads to be confirmed as integrated into the consolidated codebase, so that Hoenn is playable in the final ROM.

#### Acceptance Criteria

1. THE Pokemon_Tuxedo codebase SHALL contain all 104 Hoenn maps in `data/maps/` organized as:
   - 34 routes (Route 101–134)
   - 16 cities and towns
   - 54 caves and dungeons
2. THE Pokemon_Tuxedo codebase SHALL contain Hoenn tilesets in `data/tilesets/secondary/hoenn/` and `data/tilesets/primary/hoenn/`.
3. THE Pokemon_Tuxedo codebase SHALL contain Hoenn trainer data in `src/data/hoenn_trainers.party` covering all 8 Gym Leaders, 4 Elite Four members, 1 Champion, and route trainers.
4. THE Pokemon_Tuxedo codebase SHALL contain Hoenn wild encounter data in `src/data/hoenn_wild_encounters.json` covering all 86 maps with encounter tables.
5. THE Build_System SHALL include Hoenn map groups `gMapGroup_HoennTownsAndRoutes` and `gMapGroup_HoennDungeons` in `map_groups.json`.
6. WHEN the ROM is built, THE Build_System SHALL compile all 104 Hoenn maps without errors.

---

### Requirement 4: Build System Consolidation

**User Story:** As a developer, I want the pokemon-tuxedo build system to compile a working ROM that includes all Tuxedo game systems and Hoenn map data, so that I can produce a playable game from a single directory.

#### Acceptance Criteria

1. THE Build_System SHALL compile the consolidated `pokemon-tuxedo/` codebase using `build.ps1` or `make modern` without errors.
2. THE Build_System SHALL link all Tuxedo_Systems source files into the final ROM.
3. THE Build_System SHALL produce a `.gba` ROM file that does not exceed 16 MB.
4. WHEN `build.ps1` is executed, THE Build_System SHALL invoke devkitARM via MSYS2 bash with the correct `DEVKITPRO` and `DEVKITARM` environment variables.
5. THE Build_System SHALL rebuild the `trainerproc` tool before the main ROM build to ensure GCC 15 compatibility.
6. IF a build error occurs due to a missing Tuxedo symbol, THEN THE Build_System SHALL emit a compiler error that identifies the missing symbol by name.

---

### Requirement 5: Obsolete File Cleanup

**User Story:** As a developer, I want obsolete build status files and duplicate code removed from both locations, so that the repository is clean and unambiguous.

#### Acceptance Criteria

1. THE Consolidation_Tool SHALL delete the following obsolete files from the root directory:
   - `BUILD_GUIDE.md`
   - `BUILD_IN_PROGRESS.md`
   - `BUILD_NOW.md`
   - `BUILD_SUCCESS.md`
2. THE Consolidation_Tool SHALL delete checkpoint files from the root directory that document completed phases:
   - `CHECKPOINT_PHASE_2_HOENN.md`
   - `CHECKPOINT_PHASE_3_REGION_MANAGEMENT.md`
   - `CHECKPOINT_PHASE_4_MVP.md`
   - `CHECKPOINT_TASK_6_THROUGH_17.md`
3. WHEN cleanup is complete, THE Root_Codebase SHALL retain the `test/` directory, `tools/map_converter/` directory, `.kiro/` specs directory, and `src/` and `include/` directories as reference archives.
4. THE Consolidation_Tool SHALL NOT delete any file from `pokemon-tuxedo/` that is required by the build system.

---

### Requirement 6: Multi-Region Progression System

**User Story:** As a player, I want to progress through Kanto, then Hoenn, then Johto by defeating each region's Elite Four, so that I experience a continuous multi-region adventure.

#### Acceptance Criteria

1. THE Region_Manager SHALL initialize with Kanto as the current region and all other regions locked on new game start.
2. WHEN the player defeats the Kanto Elite Four, THE Region_Manager SHALL unlock Hoenn and award the Hoenn Ticket to the player's bag.
3. WHEN the player defeats the Hoenn Elite Four, THE Region_Manager SHALL unlock Johto and award the Johto Ticket to the player's bag.
4. WHILE a region is locked, THE Region_Manager SHALL prevent the player from using the corresponding region ticket to travel there.
5. THE Region_Manager SHALL track 24 badges independently across 3 regions (8 badges per region: Kanto, Hoenn, Johto).
6. WHEN the player uses a region ticket for an unlocked region, THE Region_Manager SHALL warp the player to that region's starting town.
7. WHEN a region transition occurs, THE Region_Manager SHALL preserve the player's party and inventory without modification.
8. THE Region_Manager SHALL update the current region state to the destination region after a successful transition.

---

### Requirement 7: Dynamic Level Scaling

**User Story:** As a player, I want wild Pokemon and trainer levels to scale with my badge count and current region, so that the game remains challenging throughout my journey.

#### Acceptance Criteria

1. WHEN the player has 0 badges in Kanto, THE Level_Scaler SHALL set wild Pokemon levels to the range 2–10.
2. WHEN the player has N badges in a region (N > 0), THE Level_Scaler SHALL set wild Pokemon minimum level to `region_starting_level + (N × 5)` and maximum level to `minimum + 10`.
3. THE Level_Scaler SHALL apply the following region starting levels:
   - Kanto: level 2
   - Hoenn: level 60
   - Johto: level 110
4. WHEN the player has defeated the Elite Four in the current region, THE Level_Scaler SHALL add 10 to both the minimum and maximum wild Pokemon levels.
5. THE Level_Scaler SHALL calculate trainer levels as follows:
   - Normal trainers: wild Pokemon maximum level
   - Gym Leaders: wild Pokemon maximum level + 5
   - Elite Four: wild Pokemon maximum level + 10
   - Champion: wild Pokemon maximum level + 15
   - Rival: wild Pokemon maximum level + 10 (level 5 for the first encounter with 0 badges)
6. THE Level_Scaler SHALL clamp all calculated levels to the valid range of 1–100.
7. THE Level_Scaler SHALL calculate the level cap as `region_starting_level + 20 + (badges_in_current_region × 5)`, clamped to 100.

---

### Requirement 8: Full Party Experience

**User Story:** As a player, I want all Pokemon in my party to receive full experience after each battle, so that I can keep my whole team leveled without grinding.

#### Acceptance Criteria

1. WHEN a battle ends with a Pokemon defeated, THE Full_Party_Exp_System SHALL award the full base experience to every non-egg Pokemon in the party.
2. THE Full_Party_Exp_System SHALL NOT split experience among party members.
3. THE Full_Party_Exp_System SHALL award the same experience to non-participating party members as to the active battler.
4. WHEN a party slot contains an egg, THE Full_Party_Exp_System SHALL skip that slot and award no experience to it.
5. WHEN a party slot is empty, THE Full_Party_Exp_System SHALL skip that slot.

---

### Requirement 9: EXP Multiplier Items

**User Story:** As a player, I want to earn EXP multiplier items by defeating Elite Fours, so that I can accelerate leveling in later regions.

#### Acceptance Criteria

1. WHEN the player defeats the Kanto Elite Four, THE EXP_Multiplier_System SHALL add one Lucky Egg (2× multiplier) to the player's bag.
2. WHEN the player defeats the Hoenn Elite Four, THE EXP_Multiplier_System SHALL add one Mystic Egg (3× multiplier) to the player's bag.
3. WHEN the player defeats the Johto Elite Four, THE EXP_Multiplier_System SHALL add one Magic Egg (4× multiplier) to the player's bag.
4. WHEN the player holds a Lucky Egg, THE EXP_Multiplier_System SHALL multiply all experience gains by 2.
5. WHEN the player holds a Mystic Egg, THE EXP_Multiplier_System SHALL multiply all experience gains by 3.
6. WHEN the player holds a Magic Egg, THE EXP_Multiplier_System SHALL multiply all experience gains by 4.
7. WHEN the player holds multiple multiplier eggs, THE EXP_Multiplier_System SHALL apply only the highest multiplier (Magic Egg > Mystic Egg > Lucky Egg).
8. THE EXP_Multiplier_System SHALL apply the multiplier after the Full_Party_Exp_System awards base experience.

---

### Requirement 10: Fly Location Discovery

**User Story:** As a player, I want fly destinations to be discovered automatically when I first visit a location, so that I can fast-travel without manually registering each city.

#### Acceptance Criteria

1. WHEN the player enters a map that has a fly point for the first time, THE Fly_System SHALL register that map as a fly destination.
2. THE Fly_System SHALL store up to 16 fly locations per region (48 total across 3 regions).
3. WHEN the player opens the fly menu, THE Fly_System SHALL display only fly locations registered in the current region.
4. WHEN the player opens the fly menu while in an unlocked non-current region, THE Fly_System SHALL also display the starting town of each other unlocked region as a cross-region fly option.
5. THE Fly_System SHALL persist all registered fly locations across save and load cycles.
6. WHEN a fly location is registered, THE Fly_System SHALL NOT register the same map ID twice for the same region.

---

### Requirement 11: Game Settings

**User Story:** As a player, I want to configure battle mode and game speed, so that I can tailor the gameplay experience to my preferences.

#### Acceptance Criteria

1. THE Game_Settings_System SHALL default the battle mode to SET on new game start.
2. THE Game_Settings_System SHALL allow the player to switch battle mode between SET and SHIFT via the settings menu.
3. THE Game_Settings_System SHALL allow the player to set game speed to any integer value from 1 to 10 inclusive.
4. WHEN game speed is set to a value N greater than 1, THE Game_Settings_System SHALL divide animation and text durations by N.
5. THE Game_Settings_System SHALL persist battle mode and game speed settings across save and load cycles.

---

### Requirement 12: HM Simplification

**User Story:** As a player, I want Cut, Rock Smash, Waterfall, and Flash removed as HM requirements, so that I do not need to dedicate party slots to HM slaves.

#### Acceptance Criteria

1. THE Pokemon_Tuxedo codebase SHALL remove Cut, Rock Smash, Waterfall, and Flash as field HM requirements.
2. WHEN the player encounters a tree that previously required Cut, THE Pokemon_Tuxedo codebase SHALL allow passage without using Cut.
3. WHEN the player encounters a rock that previously required Rock Smash, THE Pokemon_Tuxedo codebase SHALL allow passage without using Rock Smash.
4. WHEN the player encounters a waterfall that previously required Waterfall, THE Pokemon_Tuxedo codebase SHALL allow passage without using Waterfall.
5. WHEN the player enters a dark cave that previously required Flash, THE Pokemon_Tuxedo codebase SHALL not require Flash to navigate.

---

### Requirement 13: Trade Evolution Alternatives

**User Story:** As a player, I want Pokemon that normally require trading to evolve to have alternative evolution methods, so that I can complete my Pokedex without trading.

#### Acceptance Criteria

1. THE Pokemon_Tuxedo codebase SHALL allow Pokemon that normally evolve by trade to evolve at level 37 instead.
2. THE Pokemon_Tuxedo codebase SHALL provide an NPC in at least one location per region who can trigger trade evolutions for the player.
3. WHEN a Pokemon reaches level 37 and its species normally evolves by trade, THE Pokemon_Tuxedo codebase SHALL trigger the evolution sequence.

---

### Requirement 14: Starter Distribution

**User Story:** As a player, I want to receive the remaining starter Pokemon after defeating each Elite Four, so that I can build a team with all starters without trading.

#### Acceptance Criteria

1. WHEN the player defeats the Kanto Elite Four, THE Pokemon_Tuxedo codebase SHALL make the two Kanto starters not chosen at the start available for the player to receive.
2. WHEN the player defeats the Hoenn Elite Four, THE Pokemon_Tuxedo codebase SHALL make all three Hoenn starters available for the player to receive.
3. WHEN the player defeats the Johto Elite Four, THE Pokemon_Tuxedo codebase SHALL make all three Johto starters available for the player to receive.

---

### Requirement 15: PC Storage from Pause Menu

**User Story:** As a player, I want to access PC Pokemon storage from the pause menu anywhere in the field, so that I can manage my boxes without finding a PC.

#### Acceptance Criteria

1. THE Pokemon_Tuxedo codebase SHALL add a "PC" option to the pause menu accessible from the field.
2. WHEN the player selects the PC option from the pause menu, THE Pokemon_Tuxedo codebase SHALL open the Pokemon Storage System.
3. WHEN the player exits the Pokemon Storage System opened from the pause menu, THE Pokemon_Tuxedo codebase SHALL return the player to the field without changing their map position.

---

### Requirement 16: Multi-Region Save Data

**User Story:** As a player, I want my region progress, badges, and fly locations to be saved and restored correctly, so that I never lose my multi-region progress.

#### Acceptance Criteria

1. THE Multi_Region_Save SHALL store the current region (0–2) in the save data structure.
2. THE Multi_Region_Save SHALL store region unlock flags (hoennUnlocked, johtoUnlocked) and Elite Four defeated flags for all 3 regions in the save data structure.
3. THE Multi_Region_Save SHALL store all 24 badges (8 per region × 3 regions) as a flat boolean array in the save data structure.
4. THE Multi_Region_Save SHALL store all 48 fly locations (16 per region × 3 regions) and per-region fly location counts in the save data structure.
5. WHEN the player loads a save, THE Multi_Region_Save SHALL restore current region, unlock flags, Elite Four status, all badges, and all fly locations to their saved values.
6. WHEN the player loads a save that predates multi-region support, THE Multi_Region_Save SHALL initialize all multi-region fields to defaults with Kanto as the current region and all other regions locked.
7. FOR ALL valid multi-region save states S, saving then loading S SHALL produce a state equivalent to S (round-trip property).

---

### Requirement 17: Property-Based Test Suite Preservation

**User Story:** As a developer, I want all 5426+ existing property-based tests to continue passing after consolidation, so that I have confidence the game systems are correct.

#### Acceptance Criteria

1. THE Pokemon_Tuxedo codebase SHALL include the `test/` directory from the Root_Codebase containing all existing test files.
2. WHEN the test suite is executed, THE Pokemon_Tuxedo codebase SHALL pass all property-based tests with a minimum of 100 iterations per property.
3. THE Pokemon_Tuxedo codebase SHALL pass the following named properties:
   - Property 1: Elite Four Progression Unlocks Next Region
   - Property 2: Region Unlock Awards Ticket
   - Property 3: Locked Region Access Prevention
   - Property 14: Region Ticket Warp Functionality
   - Property 15: Party Preservation During Region Transition
   - Property 16: Inventory Preservation During Region Transition
   - Property 17: Region State Update During Transition
   - Property 18: Locked Region Ticket Rejection
   - Property 19: Level Scaling Application
   - Property 20: Fly Location Registration
   - Property 21: Fly Menu Region Filtering
   - Property 23: Badge Tracking Separation
   - Property 24: Badge Storage Capacity
   - Property 27: Save Data Round-Trip
4. THE Pokemon_Tuxedo codebase SHALL pass the Hoenn map completeness property test verifying all 104 Hoenn maps are present.
5. FOR ALL valid multi-region save states, THE Multi_Region_Save round-trip property SHALL hold: `load(save(state)) == state`.

---

### Requirement 18: Map Converter Tools Preservation

**User Story:** As a developer, I want the map converter PowerShell tools preserved and accessible, so that I can re-run conversions or adapt them for Johto integration.

#### Acceptance Criteria

1. THE Root_Codebase SHALL retain the `tools/map_converter/` directory containing all PowerShell conversion scripts after consolidation.
2. THE tools directory SHALL contain the following scripts:
   - `integrate_hoenn_maps.ps1`
   - `copy_hoenn_encounters.ps1`
   - `copy_hoenn_trainers.ps1`
   - `copy_hoenn_scripts.ps1`
   - `copy_hoenn_tilesets.ps1`
   - `batch_convert_hoenn.ps1`
   - `batch_convert_hoenn_cities.ps1`
   - `batch_convert_hoenn_dungeons.ps1`
3. WHEN a conversion script is executed against a valid source directory, THE Consolidation_Tool SHALL produce output files in the format expected by the Pokemon_Tuxedo build system.

---

### Requirement 19: Johto Region Integration (Phase 5)

**User Story:** As a developer, I want a clear integration path for Johto maps from CrystalDust into the consolidated codebase, so that Phase 5 can proceed without architectural rework.

#### Acceptance Criteria

1. THE Pokemon_Tuxedo codebase SHALL contain stub implementations for Johto ticket handling (`ItemUseOutOfBattle_JohtoTicket`, `ItemUseOnFieldCB_JohtoTicket`) that compile without errors.
2. THE Region_Manager SHALL define `MAP_NEW_BARK_TOWN` as `MAP_UNDEFINED` when Johto maps are not yet integrated, allowing the codebase to compile.
3. WHEN Johto maps are integrated, THE Region_Manager SHALL replace the `MAP_NEW_BARK_TOWN` placeholder with the auto-generated map constant.
4. THE Multi_Region_Save data structure SHALL reserve space for Johto badges (indices 16–23 in the badges array) and Johto fly locations (indices 32–47 in the flyLocations array) even before Johto maps are integrated.
5. WHEN the player defeats the Hoenn Elite Four and Johto is integrated, THE Region_Manager SHALL unlock Johto and award the Johto Ticket per the Kanto → Hoenn → Johto progression sequence (ADR-004).
6. THE format converter tools in `tools/map_converter/` SHALL be adaptable to process CrystalDust map data with modifications limited to path handling and flag/variable remapping.

---

### Requirement 20: ROM Size Constraint

**User Story:** As a developer, I want the final ROM to remain within the GBA hardware limit, so that the game runs on real hardware and accurate emulators.

#### Acceptance Criteria

1. THE Build_System SHALL produce a `.gba` ROM file of 16 MB (16,777,216 bytes) or less.
2. WHEN the ROM size approaches 16 MB, THE Build_System SHALL emit a warning identifying the current ROM size.
3. THE Pokemon_Tuxedo codebase SHALL reuse common tilesets across regions where possible to minimize ROM size.
4. IF the ROM size exceeds 16 MB after adding Johto maps in Phase 5, THEN THE Build_System SHALL provide a tileset compression step to reduce size.

---

### Requirement 21: Custom Characters and Narrative

**User Story:** As a player, I want to encounter the custom Pokemon Tuxedo characters throughout the game, so that the game has a distinct identity from the base FireRed experience.

#### Acceptance Criteria

1. THE Pokemon_Tuxedo codebase SHALL include sprite assets and dialogue for Professor Nana (calico cat professor) as the game's starting professor.
2. THE Pokemon_Tuxedo codebase SHALL include sprite assets and dialogue for Mister Mango as the player's rival character.
3. THE Pokemon_Tuxedo codebase SHALL include the Tuxedo Cat mascot as a recurring character or logo element in the game's title screen or intro sequence.
4. WHERE custom character sprites are not yet finalized, THE Pokemon_Tuxedo codebase SHALL use placeholder sprites that compile without errors.
