# Requirements Document: Full Multi-Region Support

## Introduction

This document specifies requirements for implementing complete Johto, Hoenn, and Sinnoh regions in Pokemon Tuxedo ROM hack (pokefirered decomp). After defeating the Kanto Elite Four, players will unlock access to three additional complete regions, each with full routes, cities, caves, gym leaders, Elite Four, champions, wild encounters, trainers, items, and events. The implementation will leverage Pokemon Crossroads (Emerald-based multi-region hack) as the source for porting complete region maps to FireRed format.

## Glossary

- **Region_Manager**: The existing system component that tracks current region and manages region-specific data
- **Level_Scaler**: The existing system component that scales Pokemon and trainer levels based on progression
- **Map_Bank**: A collection of map data organized by region in the ROM structure
- **Tileset**: Graphics data defining terrain appearance for maps
- **Event_Script**: In-game scripting that controls NPC behavior, items, and interactions
- **Wild_Encounter_Table**: Data structure defining which Pokemon appear in specific map locations
- **Trainer_Data**: Data structure containing trainer party information, AI, and rewards
- **Region_Ticket**: Key item that enables travel to a specific region
- **Crossroads_Source**: Pokemon Crossroads repository containing Emerald-format multi-region data
- **Format_Converter**: Tool or process that converts Emerald map format to FireRed format
- **Starter_Distribution**: System that provides region-specific starter Pokemon to players
- **Elite_Four_Gate**: Progression checkpoint that must be cleared to unlock next region
- **Map_Header**: Data structure containing map metadata (dimensions, connections, events)
- **Warp_System**: Mechanism for transitioning between maps and regions
- **Fly_Location**: Designated map location accessible via HM Fly
- **Gym_Leader**: Boss trainer that awards badges upon defeat
- **Champion**: Final boss trainer of a region's Elite Four challenge

## Requirements

### Requirement 1: Region Unlock Progression

**User Story:** As a player, I want to unlock new regions after defeating each region's Elite Four, so that I can progress through all four regions sequentially.

#### Acceptance Criteria

1. WHEN the player defeats the Kanto Elite Four, THE Region_Manager SHALL unlock Johto region access
2. WHEN the player defeats the Johto Elite Four, THE Region_Manager SHALL unlock Hoenn region access
3. WHEN the player defeats the Hoenn Elite Four, THE Region_Manager SHALL unlock Sinnoh region access
4. WHEN a region is unlocked, THE Region_Manager SHALL award the corresponding Region_Ticket to the player
5. THE Region_Manager SHALL prevent access to locked regions even if the player attempts to warp there

### Requirement 2: Johto Region Implementation

**User Story:** As a player, I want to explore a complete Johto region with all original content, so that I can experience the full Johto adventure.

#### Acceptance Criteria

1. THE Map_Bank SHALL contain all Johto routes (Routes 29-46)
2. THE Map_Bank SHALL contain all Johto cities (New Bark Town, Cherrygrove City, Violet City, Azalea Town, Goldenrod City, Ecruteak City, Olivine City, Cianwood City, Mahogany Town, Blackthorn City)
3. THE Map_Bank SHALL contain all Johto caves and dungeons (Sprout Tower, Ruins of Alph, Union Cave, Slowpoke Well, Ilex Forest, Burned Tower, Tin Tower, Whirl Islands, Mt Mortar, Ice Path, Dark Cave, Dragon's Den)
4. THE Trainer_Data SHALL contain 8 Johto Gym Leaders with complete teams
5. THE Trainer_Data SHALL contain 4 Johto Elite Four members with complete teams
6. THE Trainer_Data SHALL contain 1 Johto Champion with complete team
7. THE Wild_Encounter_Table SHALL contain Johto-appropriate wild Pokemon for all Johto maps
8. THE Event_Script SHALL implement all Johto-specific events and NPC interactions
9. THE Starter_Distribution SHALL provide Chikorita, Cyndaquil, and Totodile as Johto starters

### Requirement 3: Hoenn Region Implementation

**User Story:** As a player, I want to explore a complete Hoenn region with all original content, so that I can experience the full Hoenn adventure.

#### Acceptance Criteria

1. THE Map_Bank SHALL contain all Hoenn routes (Routes 101-134)
2. THE Map_Bank SHALL contain all Hoenn cities (Littleroot Town, Oldale Town, Petalburg City, Rustboro City, Dewford Town, Slateport City, Mauville City, Verdanturf Town, Fallarbor Town, Lavaridge Town, Fortree City, Lilycove City, Mossdeep City, Sootopolis City, Pacifidlog Town, Ever Grande City)
3. THE Map_Bank SHALL contain all Hoenn caves and dungeons (Petalburg Woods, Granite Cave, Rusturf Tunnel, Meteor Falls, Mt Chimney, Jagged Pass, Fiery Path, Mt Pyre, Shoal Cave, Cave of Origin, Seafloor Cavern, Sky Pillar, Victory Road)
4. THE Trainer_Data SHALL contain 8 Hoenn Gym Leaders with complete teams
5. THE Trainer_Data SHALL contain 4 Hoenn Elite Four members with complete teams
6. THE Trainer_Data SHALL contain 1 Hoenn Champion with complete team
7. THE Wild_Encounter_Table SHALL contain Hoenn-appropriate wild Pokemon for all Hoenn maps
8. THE Event_Script SHALL implement all Hoenn-specific events and NPC interactions
9. THE Starter_Distribution SHALL provide Treecko, Torchic, and Mudkip as Hoenn starters

### Requirement 4: Sinnoh Region Implementation

**User Story:** As a player, I want to explore a complete Sinnoh region with all original content, so that I can experience the full Sinnoh adventure.

#### Acceptance Criteria

1. THE Map_Bank SHALL contain all Sinnoh routes (Routes 201-230)
2. THE Map_Bank SHALL contain all Sinnoh cities (Twinleaf Town, Sandgem Town, Jubilife City, Oreburgh City, Floaroma Town, Eterna City, Hearthome City, Solaceon Town, Veilstone City, Pastoria City, Celestic Town, Canalave City, Snowpoint City, Sunyshore City, Pokemon League)
3. THE Map_Bank SHALL contain all Sinnoh caves and dungeons (Oreburgh Gate, Ravaged Path, Eterna Forest, Old Chateau, Wayward Cave, Mt Coronet, Iron Island, Lake Verity, Lake Valor, Lake Acuity, Victory Road, Stark Mountain, Turnback Cave, Sendoff Spring)
4. THE Trainer_Data SHALL contain 8 Sinnoh Gym Leaders with complete teams
5. THE Trainer_Data SHALL contain 4 Sinnoh Elite Four members with complete teams
6. THE Trainer_Data SHALL contain 1 Sinnoh Champion with complete team
7. THE Wild_Encounter_Table SHALL contain Sinnoh-appropriate wild Pokemon for all Sinnoh maps
8. THE Event_Script SHALL implement all Sinnoh-specific events and NPC interactions
9. THE Starter_Distribution SHALL provide Turtwig, Chimchar, and Piplup as Sinnoh starters

### Requirement 5: Map Data Porting from Crossroads

**User Story:** As a developer, I want to port map data from Pokemon Crossroads, so that I can leverage proven multi-region map implementations.

#### Acceptance Criteria

1. THE Format_Converter SHALL export map data from Crossroads_Source repository
2. THE Format_Converter SHALL convert Emerald-format Map_Header structures to FireRed-format Map_Header structures
3. THE Format_Converter SHALL convert Emerald-format tileset data to FireRed-format tileset data
4. THE Format_Converter SHALL preserve map dimensions during format conversion
5. THE Format_Converter SHALL preserve map connections during format conversion
6. THE Format_Converter SHALL preserve warp points during format conversion
7. WHEN map data conversion fails, THE Format_Converter SHALL log detailed error information including map name and failure reason

### Requirement 6: Tileset Porting and Integration

**User Story:** As a developer, I want to port tilesets from Crossroads to FireRed, so that maps render correctly with appropriate graphics.

#### Acceptance Criteria

1. THE Format_Converter SHALL extract tileset graphics from Crossroads_Source
2. THE Format_Converter SHALL convert tileset graphics to FireRed-compatible format
3. THE Format_Converter SHALL extract tileset behavior data from Crossroads_Source
4. THE Format_Converter SHALL convert tileset behavior data to FireRed-compatible format
5. THE Map_Bank SHALL reference converted tilesets correctly in map headers
6. WHEN a tileset is missing required graphics, THE Format_Converter SHALL report the missing graphics by name

### Requirement 7: Event Script Porting

**User Story:** As a developer, I want to port event scripts from Crossroads to FireRed, so that NPCs, items, and interactions work correctly.

#### Acceptance Criteria

1. THE Format_Converter SHALL extract Event_Script data from Crossroads_Source maps
2. THE Format_Converter SHALL convert Emerald script commands to FireRed script commands
3. THE Format_Converter SHALL preserve script logic during conversion
4. THE Format_Converter SHALL update script references to FireRed-compatible constants
5. WHEN a script command has no FireRed equivalent, THE Format_Converter SHALL log the incompatible command and map location

### Requirement 8: Trainer Data Porting

**User Story:** As a developer, I want to port trainer data from Crossroads to FireRed, so that all trainers have correct teams and behavior.

#### Acceptance Criteria

1. THE Format_Converter SHALL extract Trainer_Data from Crossroads_Source
2. THE Format_Converter SHALL convert trainer party structures to FireRed format
3. THE Format_Converter SHALL convert trainer AI flags to FireRed format
4. THE Format_Converter SHALL preserve trainer reward amounts during conversion
5. THE Format_Converter SHALL map Emerald trainer classes to FireRed trainer classes
6. WHEN a trainer class has no FireRed equivalent, THE Format_Converter SHALL map it to the closest equivalent and log the substitution

### Requirement 9: Wild Encounter Porting

**User Story:** As a developer, I want to port wild encounter data from Crossroads to FireRed, so that appropriate Pokemon appear in each location.

#### Acceptance Criteria

1. THE Format_Converter SHALL extract Wild_Encounter_Table data from Crossroads_Source
2. THE Format_Converter SHALL convert encounter table structures to FireRed format
3. THE Format_Converter SHALL preserve encounter rates during conversion
4. THE Format_Converter SHALL preserve Pokemon species and level ranges during conversion
5. THE Format_Converter SHALL handle encounter types (grass, water, fishing, rock smash) correctly for FireRed

### Requirement 10: Region Transition System

**User Story:** As a player, I want to travel between regions using ticket items, so that I can access unlocked regions.

#### Acceptance Criteria

1. WHEN the player uses a Region_Ticket, THE Warp_System SHALL transport the player to the corresponding region's starting location
2. THE Warp_System SHALL preserve the player's party during region transitions
3. THE Warp_System SHALL preserve the player's inventory during region transitions
4. THE Warp_System SHALL update the Region_Manager current region state during transitions
5. WHEN the player attempts to use a Region_Ticket for a locked region, THE Warp_System SHALL display an error message and prevent the transition

### Requirement 11: Level Scaling Integration

**User Story:** As a player, I want Pokemon and trainers to scale appropriately across all regions, so that difficulty remains balanced throughout my journey.

#### Acceptance Criteria

1. THE Level_Scaler SHALL apply scaling to all Johto wild encounters based on player progression
2. THE Level_Scaler SHALL apply scaling to all Johto trainers based on player progression
3. THE Level_Scaler SHALL apply scaling to all Hoenn wild encounters based on player progression
4. THE Level_Scaler SHALL apply scaling to all Hoenn trainers based on player progression
5. THE Level_Scaler SHALL apply scaling to all Sinnoh wild encounters based on player progression
6. THE Level_Scaler SHALL apply scaling to all Sinnoh trainers based on player progression
7. THE Level_Scaler SHALL recognize Elite_Four_Gate completion for each region when calculating scaling

### Requirement 12: Fly Location Registration

**User Story:** As a player, I want to register Fly locations in all regions, so that I can quickly travel to visited cities.

#### Acceptance Criteria

1. WHEN the player visits a Johto city with a Fly_Location, THE Region_Manager SHALL register that location
2. WHEN the player visits a Hoenn city with a Fly_Location, THE Region_Manager SHALL register that location
3. WHEN the player visits a Sinnoh city with a Fly_Location, THE Region_Manager SHALL register that location
4. WHEN the player uses HM Fly, THE Warp_System SHALL display only Fly_Location entries for the current region
5. THE Warp_System SHALL enable cross-region flying only to the starting location of unlocked regions

### Requirement 13: Regional Item Distribution

**User Story:** As a player, I want to find region-appropriate items in each region, so that I can collect items from all four regions.

#### Acceptance Criteria

1. THE Event_Script SHALL place all Johto-specific items in appropriate Johto locations
2. THE Event_Script SHALL place all Hoenn-specific items in appropriate Hoenn locations
3. THE Event_Script SHALL place all Sinnoh-specific items in appropriate Sinnoh locations
4. THE Event_Script SHALL place TMs in region-appropriate locations matching original game distributions
5. THE Event_Script SHALL place held items on wild Pokemon matching regional distributions

### Requirement 14: Badge and Progression Tracking

**User Story:** As a player, I want to earn badges from all regions, so that I can track my progression across all four regions.

#### Acceptance Criteria

1. THE Region_Manager SHALL track Kanto badges separately from other region badges
2. THE Region_Manager SHALL track Johto badges separately from other region badges
3. THE Region_Manager SHALL track Hoenn badges separately from other region badges
4. THE Region_Manager SHALL track Sinnoh badges separately from other region badges
5. WHEN the player defeats a Gym_Leader, THE Region_Manager SHALL award the corresponding badge for that region
6. THE Region_Manager SHALL support a total of 32 badges (8 per region)

### Requirement 15: Map Validation and Testing

**User Story:** As a developer, I want to validate ported maps, so that I can ensure all maps are accessible and functional.

#### Acceptance Criteria

1. THE Format_Converter SHALL verify that all ported maps have valid Map_Header structures
2. THE Format_Converter SHALL verify that all map connections reference valid destination maps
3. THE Format_Converter SHALL verify that all warp points reference valid destination maps
4. THE Format_Converter SHALL verify that all Event_Script references are valid
5. THE Format_Converter SHALL verify that all Tileset references are valid
6. WHEN validation fails for a map, THE Format_Converter SHALL report the map name and specific validation failure

### Requirement 16: Save Data Compatibility

**User Story:** As a player, I want my save data to work correctly with multi-region support, so that I don't lose progress.

#### Acceptance Criteria

1. THE Region_Manager SHALL store current region state in save data
2. THE Region_Manager SHALL store unlocked regions state in save data
3. THE Region_Manager SHALL store all 32 badges in save data
4. THE Region_Manager SHALL store registered Fly_Location data for all regions in save data
5. WHEN loading save data, THE Region_Manager SHALL restore all region-related state correctly

### Requirement 17: Crossroads Source Integration

**User Story:** As a developer, I want to clone and access Pokemon Crossroads repository, so that I can extract map data for porting.

#### Acceptance Criteria

1. THE Format_Converter SHALL clone Crossroads_Source from the official repository
2. THE Format_Converter SHALL locate Johto map data within Crossroads_Source directory structure
3. THE Format_Converter SHALL locate Hoenn map data within Crossroads_Source directory structure
4. THE Format_Converter SHALL locate Kanto map data within Crossroads_Source directory structure
5. WHEN Crossroads_Source is unavailable, THE Format_Converter SHALL report a clear error message with repository URL

### Requirement 18: Build System Integration

**User Story:** As a developer, I want the build system to compile all ported maps, so that the ROM includes all region data.

#### Acceptance Criteria

1. THE Build_System SHALL compile all Johto map data into the ROM
2. THE Build_System SHALL compile all Hoenn map data into the ROM
3. THE Build_System SHALL compile all Sinnoh map data into the ROM
4. THE Build_System SHALL compile all ported tilesets into the ROM
5. THE Build_System SHALL compile all ported Event_Script data into the ROM
6. WHEN the ROM exceeds size limits, THE Build_System SHALL report the size overflow and affected data sections

### Requirement 19: Parser and Pretty Printer for Map Data

**User Story:** As a developer, I want to parse and format map data, so that I can convert between Emerald and FireRed formats reliably.

#### Acceptance Criteria

1. WHEN valid Emerald map data is provided, THE Format_Converter SHALL parse it into an intermediate Map_Header object
2. WHEN invalid Emerald map data is provided, THE Format_Converter SHALL return a descriptive error with line number and issue
3. THE Format_Converter SHALL format intermediate Map_Header objects into valid FireRed map data files
4. FOR ALL valid Map_Header objects, parsing Emerald format then formatting to FireRed format then parsing as FireRed SHALL produce an equivalent object (round-trip property)
5. THE Format_Converter SHALL preserve all map metadata (dimensions, tileset references, music, weather) during round-trip conversion

### Requirement 20: Regional Pokemon Availability

**User Story:** As a player, I want to catch region-exclusive Pokemon in their native regions, so that I can complete the National Pokedex.

#### Acceptance Criteria

1. THE Wild_Encounter_Table SHALL include Johto-exclusive Pokemon only in Johto region maps
2. THE Wild_Encounter_Table SHALL include Hoenn-exclusive Pokemon only in Hoenn region maps
3. THE Wild_Encounter_Table SHALL include Sinnoh-exclusive Pokemon only in Sinnoh region maps
4. THE Wild_Encounter_Table SHALL include Kanto-exclusive Pokemon only in Kanto region maps
5. THE Wild_Encounter_Table SHALL include cross-regional Pokemon in appropriate locations across multiple regions where they naturally appear

