# Implementation Plan: Full Multi-Region Support

## Overview

This implementation plan converts the multi-region design into actionable coding tasks. The plan has been restructured based on strategic decisions documented in [decisions.md](decisions.md).

**Key Changes (March 2026):**
- MVP scope: Kanto + Hoenn (2 regions)
- Sinnoh descoped from initial release (see ADR-003)
- Region unlock sequence: Kanto → Hoenn → Johto (see ADR-004)
- Cross-region infrastructure prioritized (see ADR-005)

The implementation leverages the pokefirered decomp codebase (C language) and integrates with existing Region_Manager and Level_Scaler systems.

## Source Data Status

| Region | Source | Status |
|--------|--------|--------|
| Kanto | pokefirered base | ✅ Available (base game) |
| Hoenn | Pokemon Crossroads | ✅ Converted (104 maps) |
| Johto | Pokemon CrystalDust | 🔄 Pending integration (see ADR-002) |
| Sinnoh | None | ❌ Descoped (see ADR-003) |

## Tasks

- [x] 1. Phase 1: Format Converter Development (COMPLETE)
  - [x] 1.1 Implement core map data structures and parser
  - [x] 1.2 Write property test for map data round-trip
  - [x] 1.3 Implement tileset converter
  - [x] 1.4 Write property test for tileset conversion
  - [x] 1.5 Implement script command converter
  - [x] 1.6 Write property test for script command conversion
  - [x] 1.7 Implement trainer data converter
  - [x] 1.8 Write property test for trainer reward preservation
  - [x] 1.9 Implement wild encounter converter
  - [x] 1.10 Write property tests for encounter conversion
  - [x] 1.11 Implement validation engine
  - [x] 1.12 Write property tests for validation
  - [x] 1.13 Implement error logging system
  - [x] 1.14 Write property test for error logging
  - [x] 1.15 Checkpoint - All format converter tests pass

- [x] 2. Phase 2: Hoenn Region Port (COMPLETE)
  - [x] 2.1 Locate Hoenn map data in Crossroads
  - [x] 2.2 Convert Hoenn routes (Routes 101-134) - 34 maps
  - [x] 2.3 Convert Hoenn cities (16 cities)
  - [x] 2.4 Convert Hoenn caves and dungeons - 54 maps
  - [x] 2.5 Write property test for Hoenn map completeness
  - [x] 2.6 Convert Hoenn tilesets
  - [x] 2.7 Convert Hoenn event scripts
  - [x] 2.8 Convert Hoenn trainer data
  - [x] 2.9 Convert Hoenn wild encounters
  - [x] 2.10 Integrate Hoenn maps into build system
  - [x] 2.11 Test Hoenn region in-game
  - [x] 2.12 Checkpoint - Hoenn region fully functional

- [x] 3. Phase 3: Region Management Integration (MVP CRITICAL)
  - [x] 3.1 Extend Region_Manager for multi-region support
    - Add BadgeData struct with 3 region badge arrays (24 badges total)
    - Add RegionState struct with unlock flags and Elite Four tracking
    - Implement GetRegionBadgeCount() for per-region badge counting
    - Implement GetTotalBadgeCount() for cross-region badge counting
    - Implement HasBadge() and AwardBadge() for badge management
    - Implement IsRegionUnlocked() and UnlockRegion() for region access
    - _Requirements: 14.1, 14.2, 14.3, 14.5, 14.6 (modified for 3 regions)_
    - _Decision: ADR-003 (Sinnoh descoped)_

  - [x] 3.2 Write property tests for badge tracking
    - **Property 23: Badge Tracking Separation**
    - **Property 24: Badge Storage Capacity** (24 badges)
    - **Validates: Requirements 14.1, 14.2, 14.3, 14.5, 14.6**

  - [x] 3.3 Implement region unlock logic
    - Add Elite Four defeat detection for Kanto and Hoenn
    - Implement: Kanto E4 defeat → Hoenn unlock
    - Implement: Hoenn E4 defeat → Johto unlock (when available)
    - Award region tickets on unlock
    - Prevent access to locked regions
    - _Requirements: 1.1, 1.2, 1.4, 1.5_
    - _Decision: ADR-004 (reordered sequence)_

  - [x] 3.4 Write property tests for region unlocking
    - **Property 1: Elite Four Progression Unlocks Next Region**
    - **Property 2: Region Unlock Awards Ticket**
    - **Property 3: Locked Region Access Prevention**
    - **Validates: Requirements 1.1, 1.2, 1.4, 1.5**

  - [x] 3.5 Implement region ticket system
    - Define ITEM_HOENN_TICKET, ITEM_JOHTO_TICKET
    - Implement ItemUseOutOfBattle_HoennTicket() function
    - Implement ItemUseOutOfBattle_JohtoTicket() function (stub for now)
    - Add ticket items to item database
    - _Requirements: 10.1, 10.5_

  - [x] 3.6 Write property tests for region tickets
    - **Property 14: Region Ticket Warp Functionality**
    - **Property 18: Locked Region Ticket Rejection**
    - **Validates: Requirements 10.1, 10.5**

  - [x] 3.7 Extend Level_Scaler for cross-region scaling
    - Implement GetWildPokemonLevelRangeForRegion() with region parameter
    - Implement GetTrainerAverageLevelForRegion() with region parameter
    - Implement GetLevelCapForProgression() with total badge count
    - Update scaling formulas for Kanto → Hoenn → Johto progression
    - _Requirements: 11.1, 11.3, 11.4, 11.7_
    - _Decision: ADR-004 (new progression order)_

  - [x] 3.8 Write property test for level scaling
    - **Property 19: Level Scaling Application**
    - **Validates: Requirements 11.1, 11.3, 11.4, 11.7**

  - [x] 3.9 Implement fly location registration per region
    - Add flyLocations[48] array to save data (16 per region × 3 regions)
    - Add flyLocationCount[3] array to save data
    - Implement RegisterFlyLocation() with region awareness
    - Implement GetCurrentRegionFlyLocations() to filter by region
    - Implement IsFlyLocationRegistered() with region check
    - _Requirements: 12.1, 12.2, 12.4, 12.5_

  - [x] 3.10 Write property tests for fly system
    - **Property 20: Fly Location Registration**
    - **Property 21: Fly Menu Region Filtering**
    - **Validates: Requirements 12.1, 12.2, 12.4, 12.5**

  - [x] 3.11 Implement region transition system
    - Implement UseRegionTicket() to handle ticket usage
    - Implement GetRegionStartLocation() to get starting warp
    - Implement CanTransitionToRegion() to validate transitions
    - Implement TransitionToRegion() to execute region change
    - Preserve party and inventory during transitions
    - Update Region_Manager state on transition
    - _Requirements: 10.1, 10.2, 10.3, 10.4, 10.5_

  - [x] 3.12 Write property tests for region transitions
    - **Property 15: Party Preservation During Region Transition**
    - **Property 16: Inventory Preservation During Region Transition**
    - **Property 17: Region State Update During Transition**
    - **Validates: Requirements 10.2, 10.3, 10.4**

  - [x] 3.13 Extend save data structures
    - Add currentRegion field to SaveBlock1
    - Add hoennUnlocked, johtoUnlocked flags
    - Add eliteFourDefeated[3] array
    - Add badges[24] array for all region badges
    - Add flyLocations[48] and flyLocationCount[3] arrays
    - _Requirements: 16.1, 16.2, 16.3, 16.4, 16.5_

  - [x] 3.14 Implement save/load for multi-region data
    - Update save functions to write multi-region data
    - Update load functions to read multi-region data
    - Implement save data validation
    - Test backward compatibility with existing saves
    - _Requirements: 16.1, 16.2, 16.3, 16.4, 16.5_

  - [x] 3.15 Write property test for save data round-trip
    - **Property 27: Save Data Round-Trip**
    - **Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5**

  - [x] 3.16 Checkpoint - All region management systems work
    - Verify Kanto → Hoenn transition works
    - Verify badge tracking across regions
    - Verify level scaling applies correctly
    - Verify save/load preserves all state

- [x] 4. Phase 4: MVP Testing (Kanto + Hoenn)
  - [x] 4.1 Run all property-based tests
    - Execute property tests with Theft library
    - Verify minimum 100 iterations per test
    - Document any failures with counterexamples
    - Fix any issues discovered by property tests
    - _Requirements: All applicable requirements_

  - [x] 4.2 Run integration tests for region progression
    - Test complete Kanto Elite Four and Hoenn unlock
    - Test Hoenn ticket usage and region warping
    - Test party and inventory preservation across transitions
    - _Requirements: 1.1, 1.4, 10.1, 10.2, 10.3_

  - [x] 4.3 Run integration tests for level scaling
    - Test wild Pokemon levels in Kanto and Hoenn at various progressions
    - Test trainer levels in Kanto and Hoenn at various progressions
    - Test level cap enforcement across both regions
    - _Requirements: 11.1, 11.3, 11.4, 11.7_

  - [x] 4.4 Run integration tests for save/load
    - Test saving in each region and loading correctly
    - Test saving with various badge counts and loading
    - Test saving with various fly locations and loading
    - _Requirements: 16.1, 16.2, 16.3, 16.4, 16.5_

  - [x] 4.5 Perform manual testing of Hoenn region
    - Complete full Hoenn playthrough from Littleroot Town
    - Battle all 8 Hoenn Gym Leaders
    - Collect all Hoenn badges
    - Battle Hoenn Elite Four and Champion
    - Test all major Hoenn events and scripts
    - _Requirements: 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9_

  - [x] 4.6 Test fly system across both regions
    - Register fly locations in Kanto and Hoenn
    - Verify fly menu shows only current region locations
    - Test cross-region flying to starting locations
    - Verify fly location persistence after save/load
    - _Requirements: 12.1, 12.2, 12.4, 12.5_

  - [x] 4.7 Test regional item distribution
    - Verify Hoenn-specific items appear only in Hoenn
    - Test TM distribution across both regions
    - _Requirements: 13.2, 13.4_

  - [x] 4.8 Test regional Pokemon distribution
    - Verify Hoenn-exclusive Pokemon appear only in Hoenn
    - Verify cross-regional Pokemon appear in appropriate regions
    - _Requirements: 20.2, 20.5_

  - [x] 4.9 Fix identified bugs and issues
    - Address all bugs found during testing
    - Re-run affected tests after fixes
    - Document all bug fixes and resolutions

  - [x] 4.10 Checkpoint - MVP (Kanto + Hoenn) ready for release
    - All tests pass
    - Region transition works smoothly
    - No critical bugs remaining

- [ ] 5. Phase 5: Johto Region Port (Post-MVP)
  - [~] 5.1 Clone and analyze CrystalDust repository
    - Clone https://github.com/Sierraffinity/CrystalDust
    - Document Johto map file locations
    - Identify any converter modifications needed
    - _Decision: ADR-002 (CrystalDust as source)_

  - [~] 5.2 Adapt format converter for CrystalDust
    - Update path handling for CrystalDust structure
    - Map CrystalDust flags/variables to FireRed equivalents
    - Test converter on sample Johto maps
    - _Requirements: 5.1, 5.2_

  - [~] 5.3 Convert Johto routes (Routes 29-46)
    - Run format converter on all 18 Johto route maps
    - Validate converted route maps
    - Fix any conversion errors
    - Generate conversion report for routes
    - _Requirements: 2.1, 5.2, 5.4, 5.5, 5.6_

  - [~] 5.4 Convert Johto cities (10 cities)
    - Convert New Bark Town, Cherrygrove City, Violet City
    - Convert Azalea Town, Goldenrod City, Ecruteak City
    - Convert Olivine City, Cianwood City, Mahogany Town, Blackthorn City
    - Validate all city maps
    - _Requirements: 2.2, 5.2_

  - [~] 5.5 Convert Johto caves and dungeons (12 locations)
    - Convert Sprout Tower, Ruins of Alph, Union Cave, Slowpoke Well
    - Convert Ilex Forest, Burned Tower, Tin Tower, Whirl Islands
    - Convert Mt Mortar, Ice Path, Dark Cave, Dragon's Den
    - Validate all dungeon maps
    - _Requirements: 2.3, 5.2_

  - [~] 5.6 Write property test for Johto map completeness
    - **Property 4: Region Map Completeness**
    - **Validates: Requirements 2.1, 2.2, 2.3**

  - [~] 5.7 Convert Johto tilesets
    - Extract all Johto-specific tilesets from CrystalDust
    - Convert tileset graphics to FireRed format
    - Convert tileset behavior data
    - Validate tileset references in Johto maps
    - _Requirements: 6.1, 6.2, 6.3, 6.4, 6.5_

  - [~] 5.8 Convert Johto event scripts
    - Extract event scripts from all Johto maps
    - Convert script commands to FireRed format
    - Update script references to FireRed constants
    - Test critical scripts (gym leaders, Elite Four, Radio Tower)
    - _Requirements: 7.1, 7.2, 7.3, 7.4, 2.8_

  - [~] 5.9 Convert Johto trainer data
    - Extract all Johto trainer data from CrystalDust
    - Convert 8 Johto Gym Leaders with complete teams
    - Convert 4 Johto Elite Four members with complete teams
    - Convert 1 Johto Champion with complete team
    - Convert all route and dungeon trainers
    - _Requirements: 8.1, 8.2, 8.3, 2.4, 2.5, 2.6_

  - [~] 5.10 Convert Johto wild encounters
    - Extract wild encounter tables for all Johto maps
    - Convert encounter data to FireRed format
    - Ensure Johto-appropriate Pokemon distribution
    - Validate encounter rates and level ranges
    - _Requirements: 9.1, 9.2, 9.3, 9.4, 2.7_

  - [~] 5.11 Write property test for regional Pokemon exclusivity
    - **Property 29: Regional Pokemon Exclusivity**
    - **Validates: Requirements 20.1, 20.2, 20.3**

  - [~] 5.12 Integrate Johto maps into build system
    - Add Johto map files to Makefile
    - Update map registry with Johto map IDs
    - Compile Johto data into ROM
    - Verify ROM size remains within limits
    - _Requirements: 18.1, 18.6_

  - [~] 5.13 Extend region management for Johto
    - Enable Johto unlock after Hoenn Elite Four
    - Activate ITEM_JOHTO_TICKET functionality
    - Add Johto fly locations
    - Update level scaling for 3-region progression
    - _Requirements: 1.2, 11.1, 11.2, 12.1_

  - [~] 5.14 Test Johto region in-game
    - Load ROM and warp to Johto starting location
    - Verify map rendering and tileset display
    - Test map connections and warps
    - Test NPC interactions and scripts
    - Test wild encounters
    - _Requirements: 2.1, 2.2, 2.3, 2.7, 2.8_

  - [~] 5.15 Checkpoint - Johto region fully functional
    - Full Kanto → Hoenn → Johto progression works
    - All 24 badges trackable
    - All tests pass

- [ ] 6. Phase 6: Final Polish and Release
  - [~] 6.1 Run complete test suite
    - Execute all 30 property tests
    - Run all integration tests
    - Document any failures
    - _Requirements: All requirements_

  - [~] 6.2 Perform full 3-region playthrough
    - Complete Kanto from start to Elite Four
    - Transition to Hoenn and complete to Elite Four
    - Transition to Johto and complete to Elite Four
    - Verify all progression gates work correctly

  - [~] 6.3 Optimize ROM size if needed
    - Check current ROM size against 16 MB limit
    - Compress tileset graphics if size is tight
    - Identify and remove unused assets
    - Reuse common tilesets across regions
    - _Requirements: 18.6_

  - [~] 6.4 Performance testing and optimization
    - Profile map loading times
    - Profile level calculation performance
    - Implement caching for frequently accessed data
    - Test on real GBA hardware or accurate emulator

  - [~] 6.5 Final validation and ROM build
    - Run complete test suite one final time
    - Generate final conversion report
    - Build final ROM with all regions
    - Verify ROM boots and all regions are accessible
    - Create release documentation

  - [~] 6.6 Final checkpoint - Multi-region system complete
    - 3-region game fully functional
    - All tests pass
    - Ready for release

## Notes

- Phase execution order changed per ADR-005: Infrastructure before content
- Sinnoh removed from scope per ADR-003
- Region unlock sequence: Kanto → Hoenn → Johto per ADR-004
- Johto source: Pokemon CrystalDust per ADR-002
- All decisions documented in [decisions.md](decisions.md)
- Property tests use the Theft library with minimum 100 iterations
- ROM size must stay within 16 MB GBA limit
- All existing Tuxedo features must remain functional
