# Implementation Plan: Full Multi-Region Support

## Overview

This implementation plan converts the multi-region design into actionable coding tasks. The plan follows 6 phases over 40 weeks, porting complete Johto, Hoenn, and Sinnoh regions from Pokemon Crossroads to FireRed format. Each task builds incrementally, with property-based tests validating correctness properties throughout development.

The implementation leverages the pokefirered decomp codebase (C language) and integrates with existing Region_Manager and Level_Scaler systems.

## Tasks

- [ ] 1. Phase 1: Format Converter Development (Weeks 1-8)
  - [x] 1.1 Implement core map data structures and parser
    - Create MapData intermediate representation struct in C
    - Implement ParseEmeraldMap() function to read Emerald map.json files
    - Implement FormatFireRedMap() function to write FireRed map.json files
    - Add error handling with detailed error messages
    - _Requirements: 5.1, 5.2, 19.1, 19.2, 19.3_

  - [x] 1.2 Write property test for map data round-trip
    - **Property 28: Map Data Round-Trip**
    - **Validates: Requirements 19.4, 19.5**

  - [x] 1.3 Implement tileset converter
    - Create TilesetData struct for intermediate representation
    - Implement ExtractEmeraldTileset() to read Crossroads tileset data
    - Implement ConvertTileset() to transform to FireRed format
    - Handle graphics, metatiles, behavior, and palette conversion
    - _Requirements: 6.1, 6.2, 6.3, 6.4_

  - [x] 1.4 Write property test for tileset conversion
    - **Property 9: Tileset Reference Validity**
    - **Validates: Requirements 6.5, 15.5**

  - [x] 1.5 Implement script command converter
    - Create Script and ScriptCommand structs
    - Implement MapScriptCommand() to map Emerald commands to FireRed
    - Implement ConvertScript() for full script conversion
    - Create mapping table for compatible commands
    - Log incompatible commands with map location
    - _Requirements: 7.1, 7.2, 7.3, 7.4, 7.5_

  - [x] 1.6 Write property test for script command conversion
    - **Property 10: Script Command Conversion**
    - **Validates: Requirements 7.2, 7.4**

  - [x] 1.7 Implement trainer data converter
    - Create TrainerData and TrainerPokemon structs
    - Implement ConvertTrainer() to transform Emerald trainers to FireRed
    - Implement MapTrainerClass() to map trainer classes
    - Preserve trainer rewards, AI flags, and party data
    - _Requirements: 8.1, 8.2, 8.3, 8.4, 8.5, 8.6_

  - [x] 1.8 Write property test for trainer reward preservation
    - **Property 11: Trainer Reward Preservation**
    - **Validates: Requirements 8.4**

  - [x] 1.9 Implement wild encounter converter
    - Create WildEncounterData and EncounterSlot structs
    - Implement ConvertEncounters() to transform encounter tables
    - Handle grass, water, fishing, and rock smash encounters
    - Preserve encounter rates and level ranges
    - _Requirements: 9.1, 9.2, 9.3, 9.4, 9.5_

  - [x] 1.10 Write property tests for encounter conversion
    - **Property 12: Encounter Rate Preservation**
    - **Property 13: Pokemon Species Preservation**
    - **Validates: Requirements 9.3, 9.4**

  - [x] 1.11 Implement validation engine
    - Create validation functions for map headers
    - Implement ValidateMapRoundTrip() for conversion verification
    - Validate warp destinations reference valid maps
    - Validate connection integrity
    - Validate tileset and script references
    - _Requirements: 15.1, 15.2, 15.3, 15.4, 15.5, 15.6_

  - [x] 1.12 Write property tests for validation
    - **Property 5: Map Dimension Preservation**
    - **Property 6: Map Connection Preservation**
    - **Property 7: Warp Point Preservation**
    - **Property 25: Map Header Validation**
    - **Property 26: Script Reference Validation**
    - **Validates: Requirements 5.4, 5.5, 5.6, 15.1, 15.2, 15.3, 15.4**

  - [x] 1.13 Implement error logging system
    - Create ConversionError enum and ConversionResult struct
    - Implement LogConversionError() with detailed context
    - Implement GenerateConversionReport() for summary output
    - Implement GetConversionStats() for statistics tracking
    - _Requirements: 5.7, 6.6, 7.5, 8.6, 15.6, 17.5, 18.6_

  - [x] 1.14 Write property test for error logging
    - **Property 8: Conversion Error Logging**
    - **Validates: Requirements 5.7, 6.6, 7.5, 8.6, 15.6, 17.5, 18.6**

  - [x] 1.15 Checkpoint - Ensure all format converter tests pass
    - Ensure all tests pass, ask the user if questions arise.

- [ ] 2. Phase 2: Johto Region Port (Weeks 9-16)
  - [x] 2.1 Set up Crossroads source integration
    - Clone Pokemon Crossroads repository
    - Locate Johto map data in directory structure
    - Document Johto map file locations
    - _Requirements: 17.1, 17.2_

  - [x] 2.2 Convert Johto routes (Routes 29-46)
    - Run format converter on all 18 Johto route maps
    - Validate converted route maps
    - Fix any conversion errors
    - Generate conversion report for routes
    - _Requirements: 2.1, 5.2, 5.4, 5.5, 5.6_

  - [ ] 2.3 Convert Johto cities (10 cities)
    - Convert New Bark Town, Cherrygrove City, Violet City
    - Convert Azalea Town, Goldenrod City, Ecruteak City
    - Convert Olivine City, Cianwood City, Mahogany Town, Blackthorn City
    - Validate all city maps
    - _Requirements: 2.2, 5.2_

  - [ ] 2.4 Convert Johto caves and dungeons (12 locations)
    - Convert Sprout Tower, Ruins of Alph, Union Cave, Slowpoke Well
    - Convert Ilex Forest, Burned Tower, Tin Tower, Whirl Islands
    - Convert Mt Mortar, Ice Path, Dark Cave, Dragon's Den
    - Validate all dungeon maps
    - _Requirements: 2.3, 5.2_

  - [ ] 2.5 Write property test for Johto map completeness
    - **Property 4: Region Map Completeness**
    - **Validates: Requirements 2.1, 2.2, 2.3**

  - [ ] 2.6 Convert Johto tilesets
    - Extract all Johto-specific tilesets from Crossroads
    - Convert tileset graphics to FireRed format
    - Convert tileset behavior data
    - Validate tileset references in Johto maps
    - _Requirements: 6.1, 6.2, 6.3, 6.4, 6.5_

  - [ ] 2.7 Convert Johto event scripts
    - Extract event scripts from all Johto maps
    - Convert script commands to FireRed format
    - Update script references to FireRed constants
    - Test critical scripts (gym leaders, Elite Four)
    - _Requirements: 7.1, 7.2, 7.3, 7.4, 2.8_

  - [ ] 2.8 Convert Johto trainer data
    - Extract all Johto trainer data from Crossroads
    - Convert 8 Johto Gym Leaders with complete teams
    - Convert 4 Johto Elite Four members with complete teams
    - Convert 1 Johto Champion with complete team
    - Convert all route and dungeon trainers
    - _Requirements: 8.1, 8.2, 8.3, 2.4, 2.5, 2.6_

  - [ ] 2.9 Convert Johto wild encounters
    - Extract wild encounter tables for all Johto maps
    - Convert encounter data to FireRed format
    - Ensure Johto-appropriate Pokemon distribution
    - Validate encounter rates and level ranges
    - _Requirements: 9.1, 9.2, 9.3, 9.4, 2.7_

  - [ ] 2.10 Write property test for regional Pokemon exclusivity
    - **Property 29: Regional Pokemon Exclusivity**
    - **Validates: Requirements 20.1, 20.2, 20.3, 20.4**

  - [ ] 2.11 Integrate Johto maps into build system
    - Add Johto map files to Makefile
    - Update map registry with Johto map IDs
    - Compile Johto data into ROM
    - Verify ROM size remains within limits
    - _Requirements: 18.1, 18.6_

  - [ ] 2.12 Test Johto region in-game
    - Load ROM and warp to Johto starting location
    - Verify map rendering and tileset display
    - Test map connections and warps
    - Test NPC interactions and scripts
    - Test wild encounters
    - _Requirements: 2.1, 2.2, 2.3, 2.7, 2.8_

  - [ ] 2.13 Checkpoint - Ensure Johto region is fully functional
    - Ensure all tests pass, ask the user if questions arise.

- [ ] 3. Phase 3: Hoenn Region Port (Weeks 17-24)
  - [ ] 3.1 Locate Hoenn map data in Crossroads
    - Identify all Hoenn map files in Crossroads repository
    - Document Hoenn map file locations
    - _Requirements: 17.3_

  - [ ] 3.2 Convert Hoenn routes (Routes 101-134)
    - Run format converter on all 34 Hoenn route maps
    - Validate converted route maps
    - Fix any conversion errors
    - Generate conversion report for routes
    - _Requirements: 3.1, 5.2_

  - [ ] 3.3 Convert Hoenn cities (16 cities)
    - Convert Littleroot Town, Oldale Town, Petalburg City, Rustboro City
    - Convert Dewford Town, Slateport City, Mauville City, Verdanturf Town
    - Convert Fallarbor Town, Lavaridge Town, Fortree City, Lilycove City
    - Convert Mossdeep City, Sootopolis City, Pacifidlog Town, Ever Grande City
    - Validate all city maps
    - _Requirements: 3.2, 5.2_

  - [ ] 3.4 Convert Hoenn caves and dungeons (13 locations)
    - Convert Petalburg Woods, Granite Cave, Rusturf Tunnel, Meteor Falls
    - Convert Mt Chimney, Jagged Pass, Fiery Path, Mt Pyre
    - Convert Shoal Cave, Cave of Origin, Seafloor Cavern, Sky Pillar, Victory Road
    - Validate all dungeon maps
    - _Requirements: 3.3, 5.2_

  - [ ] 3.5 Write property test for Hoenn map completeness
    - **Property 4: Region Map Completeness**
    - **Validates: Requirements 3.1, 3.2, 3.3**

  - [ ] 3.6 Convert Hoenn tilesets
    - Extract all Hoenn-specific tilesets from Crossroads
    - Convert tileset graphics to FireRed format
    - Convert tileset behavior data
    - Validate tileset references in Hoenn maps
    - _Requirements: 6.1, 6.2, 6.3, 6.4, 6.5_

  - [ ] 3.7 Convert Hoenn event scripts
    - Extract event scripts from all Hoenn maps
    - Convert script commands to FireRed format
    - Update script references to FireRed constants
    - Test critical scripts (gym leaders, Elite Four)
    - _Requirements: 7.1, 7.2, 7.3, 7.4, 3.8_

  - [ ] 3.8 Convert Hoenn trainer data
    - Extract all Hoenn trainer data from Crossroads
    - Convert 8 Hoenn Gym Leaders with complete teams
    - Convert 4 Hoenn Elite Four members with complete teams
    - Convert 1 Hoenn Champion with complete team
    - Convert all route and dungeon trainers
    - _Requirements: 8.1, 8.2, 8.3, 3.4, 3.5, 3.6_

  - [ ] 3.9 Convert Hoenn wild encounters
    - Extract wild encounter tables for all Hoenn maps
    - Convert encounter data to FireRed format
    - Ensure Hoenn-appropriate Pokemon distribution
    - Validate encounter rates and level ranges
    - _Requirements: 9.1, 9.2, 9.3, 9.4, 3.7_

  - [ ] 3.10 Integrate Hoenn maps into build system
    - Add Hoenn map files to Makefile
    - Update map registry with Hoenn map IDs
    - Compile Hoenn data into ROM
    - Verify ROM size remains within limits
    - _Requirements: 18.2, 18.6_

  - [ ] 3.11 Test Hoenn region in-game
    - Load ROM and warp to Hoenn starting location
    - Verify map rendering and tileset display
    - Test map connections and warps
    - Test NPC interactions and scripts
    - Test wild encounters
    - _Requirements: 3.1, 3.2, 3.3, 3.7, 3.8_

  - [ ] 3.12 Checkpoint - Ensure Hoenn region is fully functional
    - Ensure all tests pass, ask the user if questions arise.

- [ ] 4. Phase 4: Sinnoh Region Port (Weeks 25-32)
  - [ ] 4.1 Locate Sinnoh map data in Crossroads
    - Identify all Sinnoh map files in Crossroads repository
    - Document Sinnoh map file locations
    - _Requirements: 17.4_

  - [ ] 4.2 Convert Sinnoh routes (Routes 201-230)
    - Run format converter on all 30 Sinnoh route maps
    - Validate converted route maps
    - Fix any conversion errors
    - Generate conversion report for routes
    - _Requirements: 4.1, 5.2_

  - [ ] 4.3 Convert Sinnoh cities (15 cities)
    - Convert Twinleaf Town, Sandgem Town, Jubilife City, Oreburgh City
    - Convert Floaroma Town, Eterna City, Hearthome City, Solaceon Town
    - Convert Veilstone City, Pastoria City, Celestic Town, Canalave City
    - Convert Snowpoint City, Sunyshore City, Pokemon League
    - Validate all city maps
    - _Requirements: 4.2, 5.2_

  - [ ] 4.4 Convert Sinnoh caves and dungeons (14 locations)
    - Convert Oreburgh Gate, Ravaged Path, Eterna Forest, Old Chateau
    - Convert Wayward Cave, Mt Coronet, Iron Island
    - Convert Lake Verity, Lake Valor, Lake Acuity
    - Convert Victory Road, Stark Mountain, Turnback Cave, Sendoff Spring
    - Validate all dungeon maps
    - _Requirements: 4.3, 5.2_

  - [ ] 4.5 Write property test for Sinnoh map completeness
    - **Property 4: Region Map Completeness**
    - **Validates: Requirements 4.1, 4.2, 4.3**

  - [ ] 4.6 Convert Sinnoh tilesets
    - Extract all Sinnoh-specific tilesets from Crossroads
    - Convert tileset graphics to FireRed format
    - Convert tileset behavior data
    - Validate tileset references in Sinnoh maps
    - _Requirements: 6.1, 6.2, 6.3, 6.4, 6.5_

  - [ ] 4.7 Convert Sinnoh event scripts
    - Extract event scripts from all Sinnoh maps
    - Convert script commands to FireRed format
    - Update script references to FireRed constants
    - Test critical scripts (gym leaders, Elite Four)
    - _Requirements: 7.1, 7.2, 7.3, 7.4, 4.8_

  - [ ] 4.8 Convert Sinnoh trainer data
    - Extract all Sinnoh trainer data from Crossroads
    - Convert 8 Sinnoh Gym Leaders with complete teams
    - Convert 4 Sinnoh Elite Four members with complete teams
    - Convert 1 Sinnoh Champion with complete team
    - Convert all route and dungeon trainers
    - _Requirements: 8.1, 8.2, 8.3, 4.4, 4.5, 4.6_

  - [ ] 4.9 Convert Sinnoh wild encounters
    - Extract wild encounter tables for all Sinnoh maps
    - Convert encounter data to FireRed format
    - Ensure Sinnoh-appropriate Pokemon distribution
    - Validate encounter rates and level ranges
    - _Requirements: 9.1, 9.2, 9.3, 9.4, 4.7_

  - [ ] 4.10 Integrate Sinnoh maps into build system
    - Add Sinnoh map files to Makefile
    - Update map registry with Sinnoh map IDs
    - Compile Sinnoh data into ROM
    - Verify ROM size remains within limits
    - _Requirements: 18.3, 18.6_

  - [ ] 4.11 Test Sinnoh region in-game
    - Load ROM and warp to Sinnoh starting location
    - Verify map rendering and tileset display
    - Test map connections and warps
    - Test NPC interactions and scripts
    - Test wild encounters
    - _Requirements: 4.1, 4.2, 4.3, 4.7, 4.8_

  - [ ] 4.12 Checkpoint - Ensure Sinnoh region is fully functional
    - Ensure all tests pass, ask the user if questions arise.

- [ ] 5. Phase 5: Region Management Integration (Weeks 33-36)
  - [ ] 5.1 Extend Region_Manager for multi-region support
    - Add BadgeData struct with 4 region badge arrays
    - Add RegionState struct with unlock flags and Elite Four tracking
    - Implement GetRegionBadgeCount() for per-region badge counting
    - Implement GetTotalBadgeCount() for cross-region badge counting
    - Implement HasBadge() and AwardBadge() for badge management
    - Implement IsRegionUnlocked() and UnlockRegion() for region access
    - _Requirements: 14.1, 14.2, 14.3, 14.4, 14.5, 14.6_

  - [ ] 5.2 Write property tests for badge tracking
    - **Property 23: Badge Tracking Separation**
    - **Property 24: Badge Storage Capacity**
    - **Validates: Requirements 14.1, 14.2, 14.3, 14.4, 14.5, 14.6**

  - [ ] 5.3 Implement region unlock logic
    - Add Elite Four defeat detection for each region
    - Implement region unlock on Elite Four victory
    - Award region tickets on unlock
    - Prevent access to locked regions
    - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.5_

  - [ ] 5.4 Write property tests for region unlocking
    - **Property 1: Elite Four Progression Unlocks Next Region**
    - **Property 2: Region Unlock Awards Ticket**
    - **Property 3: Locked Region Access Prevention**
    - **Validates: Requirements 1.1, 1.2, 1.3, 1.4, 1.5**

  - [ ] 5.5 Implement region ticket system
    - Define ITEM_JOHTO_TICKET, ITEM_HOENN_TICKET, ITEM_SINNOH_TICKET
    - Implement ItemUseOutOfBattle_JohtoTicket() function
    - Implement ItemUseOutOfBattle_HoennTicket() function
    - Implement ItemUseOutOfBattle_SinnohTicket() function
    - Add ticket items to item database
    - _Requirements: 10.1, 10.5_

  - [ ] 5.6 Write property tests for region tickets
    - **Property 14: Region Ticket Warp Functionality**
    - **Property 18: Locked Region Ticket Rejection**
    - **Validates: Requirements 10.1, 10.5**

  - [ ] 5.7 Extend Level_Scaler for cross-region scaling
    - Implement GetWildPokemonLevelRangeForRegion() with region parameter
    - Implement GetTrainerAverageLevelForRegion() with region parameter
    - Implement GetLevelCapForProgression() with total badge count
    - Update scaling formulas to account for multi-region progression
    - _Requirements: 11.1, 11.2, 11.3, 11.4, 11.5, 11.6, 11.7_

  - [ ] 5.8 Write property test for level scaling
    - **Property 19: Level Scaling Application**
    - **Validates: Requirements 11.1, 11.2, 11.3, 11.4, 11.5, 11.6, 11.7**

  - [ ] 5.9 Implement fly location registration per region
    - Add flyLocations[64] array to save data (16 per region)
    - Add flyLocationCount[4] array to save data
    - Implement RegisterFlyLocation() with region awareness
    - Implement GetCurrentRegionFlyLocations() to filter by region
    - Implement IsFlyLocationRegistered() with region check
    - _Requirements: 12.1, 12.2, 12.3, 12.4, 12.5_

  - [ ] 5.10 Write property tests for fly system
    - **Property 20: Fly Location Registration**
    - **Property 21: Fly Menu Region Filtering**
    - **Validates: Requirements 12.1, 12.2, 12.3, 12.4, 12.5**

  - [ ] 5.11 Implement region transition system
    - Implement UseRegionTicket() to handle ticket usage
    - Implement GetRegionStartLocation() to get starting warp
    - Implement CanTransitionToRegion() to validate transitions
    - Implement TransitionToRegion() to execute region change
    - Preserve party and inventory during transitions
    - Update Region_Manager state on transition
    - _Requirements: 10.1, 10.2, 10.3, 10.4, 10.5_

  - [ ] 5.12 Write property tests for region transitions
    - **Property 15: Party Preservation During Region Transition**
    - **Property 16: Inventory Preservation During Region Transition**
    - **Property 17: Region State Update During Transition**
    - **Validates: Requirements 10.2, 10.3, 10.4**

  - [ ] 5.13 Extend save data structures
    - Add currentRegion field to SaveBlock1
    - Add johtoUnlocked, hoennUnlocked, sinnohUnlocked flags
    - Add eliteFourDefeated[4] array
    - Add badges[32] array for all region badges
    - Add flyLocations[64] and flyLocationCount[4] arrays
    - _Requirements: 16.1, 16.2, 16.3, 16.4, 16.5_

  - [ ] 5.14 Implement save/load for multi-region data
    - Update save functions to write multi-region data
    - Update load functions to read multi-region data
    - Implement save data validation
    - Test backward compatibility with existing saves
    - _Requirements: 16.1, 16.2, 16.3, 16.4, 16.5_

  - [ ] 5.15 Write property test for save data round-trip
    - **Property 27: Save Data Round-Trip**
    - **Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5**

  - [ ] 5.16 Checkpoint - Ensure all region management systems work
    - Ensure all tests pass, ask the user if questions arise.

- [ ] 6. Phase 6: Testing and Polish (Weeks 37-40)
  - [ ] 6.1 Run all property-based tests
    - Execute all 30 property tests with Theft library
    - Verify minimum 100 iterations per test
    - Document any failures with counterexamples
    - Fix any issues discovered by property tests
    - _Requirements: All requirements_

  - [ ] 6.2 Run integration tests for region progression
    - Test complete Kanto Elite Four and Johto unlock
    - Test complete Johto Elite Four and Hoenn unlock
    - Test complete Hoenn Elite Four and Sinnoh unlock
    - Test region ticket usage for all regions
    - Test party and inventory preservation across transitions
    - _Requirements: 1.1, 1.2, 1.3, 1.4, 10.1, 10.2, 10.3_

  - [ ] 6.3 Run integration tests for level scaling
    - Test wild Pokemon levels in all regions at various progressions
    - Test trainer levels in all regions at various progressions
    - Test level cap enforcement across all regions
    - Verify Elite Four bonus application
    - _Requirements: 11.1, 11.2, 11.3, 11.4, 11.5, 11.6, 11.7_

  - [ ] 6.4 Run integration tests for save/load
    - Test saving in each region and loading correctly
    - Test saving with various badge counts and loading
    - Test saving with various fly locations and loading
    - Test backward compatibility with pre-multi-region saves
    - _Requirements: 16.1, 16.2, 16.3, 16.4, 16.5_

  - [ ] 6.5 Perform manual testing of Johto region
    - Complete full Johto playthrough from New Bark Town
    - Battle all 8 Johto Gym Leaders
    - Collect all Johto badges
    - Battle Johto Elite Four and Champion
    - Test all major Johto events and scripts
    - _Requirements: 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9_

  - [ ] 6.6 Perform manual testing of Hoenn region
    - Complete full Hoenn playthrough from Littleroot Town
    - Battle all 8 Hoenn Gym Leaders
    - Collect all Hoenn badges
    - Battle Hoenn Elite Four and Champion
    - Test all major Hoenn events and scripts
    - _Requirements: 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9_

  - [ ] 6.7 Perform manual testing of Sinnoh region
    - Complete full Sinnoh playthrough from Twinleaf Town
    - Battle all 8 Sinnoh Gym Leaders
    - Collect all Sinnoh badges
    - Battle Sinnoh Elite Four and Champion
    - Test all major Sinnoh events and scripts
    - _Requirements: 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7, 4.8, 4.9_

  - [ ] 6.8 Test fly system across all regions
    - Register fly locations in each region
    - Verify fly menu shows only current region locations
    - Test cross-region flying to starting locations
    - Verify fly location persistence after save/load
    - _Requirements: 12.1, 12.2, 12.3, 12.4, 12.5_

  - [ ] 6.9 Test regional item distribution
    - Verify Johto-specific items appear only in Johto
    - Verify Hoenn-specific items appear only in Hoenn
    - Verify Sinnoh-specific items appear only in Sinnoh
    - Test TM distribution across all regions
    - _Requirements: 13.1, 13.2, 13.3, 13.4, 13.5_

  - [ ] 6.10 Write property test for regional item placement
    - **Property 22: Regional Item Placement**
    - **Validates: Requirements 13.1, 13.2, 13.3, 13.4, 13.5**

  - [ ] 6.11 Test regional Pokemon distribution
    - Verify Johto-exclusive Pokemon appear only in Johto
    - Verify Hoenn-exclusive Pokemon appear only in Hoenn
    - Verify Sinnoh-exclusive Pokemon appear only in Sinnoh
    - Verify cross-regional Pokemon appear in appropriate regions
    - _Requirements: 20.1, 20.2, 20.3, 20.4, 20.5_

  - [ ] 6.12 Write property test for cross-regional Pokemon
    - **Property 30: Cross-Regional Pokemon Distribution**
    - **Validates: Requirements 20.5**

  - [ ] 6.13 Fix identified bugs and issues
    - Address all bugs found during testing
    - Re-run affected tests after fixes
    - Document all bug fixes and resolutions
    - _Requirements: All requirements_

  - [ ] 6.14 Optimize ROM size if needed
    - Check current ROM size against 16 MB limit
    - Compress tileset graphics if size is tight
    - Identify and remove unused assets
    - Reuse common tilesets across regions
    - _Requirements: 18.6_

  - [ ] 6.15 Performance testing and optimization
    - Profile map loading times
    - Profile level calculation performance
    - Implement caching for frequently accessed data
    - Test on real GBA hardware or accurate emulator
    - _Requirements: All requirements_

  - [ ] 6.16 Final validation and ROM build
    - Run complete test suite one final time
    - Generate final conversion report
    - Build final ROM with all regions
    - Verify ROM boots and all regions are accessible
    - Create release documentation
    - _Requirements: All requirements_

  - [ ] 6.17 Final checkpoint - Multi-region system complete
    - Ensure all tests pass, ask the user if questions arise.

## Notes

- Tasks marked with `*` are optional property-based tests that can be skipped for faster MVP
- Each task references specific requirements for traceability
- The implementation uses C language for the pokefirered decomp codebase
- Property tests use the Theft library with minimum 100 iterations
- Checkpoints ensure incremental validation at phase boundaries
- Total estimated timeline: 40 weeks across 6 phases
- ROM size must stay within 16 MB GBA limit
- All existing Tuxedo features must remain functional
