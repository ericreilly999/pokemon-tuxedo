# Design Document: Full Multi-Region Support

## Overview

This design implements complete Johto, Hoenn, and Sinnoh regions for Pokemon Tuxedo ROM hack (pokefirered decomp). The system enables players to explore three additional complete regions after defeating the Kanto Elite Four, each with full routes, cities, caves, gym leaders, Elite Four, champions, wild encounters, trainers, items, and events.

The implementation leverages Pokemon Crossroads (Emerald-based multi-region hack) as the source for porting complete region maps to FireRed format. The design builds upon existing Region_Manager and Level_Scaler systems to provide seamless multi-region gameplay with appropriate level scaling and progression gating.

Key design goals:
- Port 300+ maps from Pokemon Crossroads (Emerald) to FireRed format
- Maintain compatibility with existing level scaling and region management systems
- Provide robust format conversion with comprehensive error handling
- Enable sequential region unlocking through Elite Four progression
- Support region-specific Pokemon distribution and item placement
- Preserve save data compatibility with extended badge and region tracking

## Architecture

### System Components

The multi-region system consists of five primary architectural layers:

1. **Format Conversion Layer** - Converts Emerald map data to FireRed format
2. **Data Integration Layer** - Integrates ported maps into FireRed build system
3. **Region Management Layer** - Extends existing Region_Manager for multi-region support
4. **Level Scaling Layer** - Extends existing Level_Scaler for cross-region scaling
5. **Progression Layer** - Manages region unlocking and ticket distribution

### Component Interaction

```
┌─────────────────────────────────────────────────────────────┐
│                    Pokemon Crossroads                        │
│                   (Emerald Source Data)                      │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────────┐
│              Format Conversion Layer                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │ Map Parser   │  │ Tileset      │  │ Script       │     │
│  │              │  │ Converter    │  │ Converter    │     │
│  └──────────────┘  └──────────────┘  └──────────────┘     │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────────┐
│              Data Integration Layer                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │ Map Registry │  │ Build System │  │ Validation   │     │
│  │              │  │ Integration  │  │ Engine       │     │
│  └──────────────┘  └──────────────┘  └──────────────┘     │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────────┐
│                  Runtime Systems                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │ Region       │  │ Level        │  │ Progression  │     │
│  │ Manager      │  │ Scaler       │  │ Manager      │     │
│  └──────────────┘  └──────────────┘  └──────────────┘     │
└─────────────────────────────────────────────────────────────┘
```

### Data Flow

1. **Extraction Phase**: Clone Pokemon Crossroads repository and locate region map data
2. **Parsing Phase**: Parse Emerald-format map headers, tilesets, scripts, and encounters
3. **Conversion Phase**: Transform Emerald structures to FireRed-compatible formats
4. **Validation Phase**: Verify map integrity, connections, warps, and references
5. **Integration Phase**: Register maps in FireRed build system and compile into ROM
6. **Runtime Phase**: Load maps dynamically based on player region and progression

## Components and Interfaces

### Format Converter Component

The Format Converter is responsible for transforming Pokemon Crossroads (Emerald) data into FireRed-compatible format.

#### Map Parser

```c
// Intermediate representation for map data
struct MapData {
    char name[32];
    u16 width;
    u16 height;
    u8 region_id;
    u16 tileset_primary;
    u16 tileset_secondary;
    u16 music;
    u8 weather;
    u8 map_type;
    bool allow_cycling;
    bool allow_running;
    bool show_map_name;
    
    struct Connection* connections;
    u8 connection_count;
    
    struct WarpEvent* warps;
    u16 warp_count;
    
    struct ObjectEvent* objects;
    u16 object_count;
    
    struct CoordEvent* coord_events;
    u16 coord_event_count;
    
    struct BgEvent* bg_events;
    u16 bg_event_count;
};

// Parse Emerald map header to intermediate format
bool ParseEmeraldMap(const char* emerald_path, struct MapData* out_data, char* error_buffer, size_t error_size);

// Format intermediate data to FireRed map.json
bool FormatFireRedMap(const struct MapData* data, const char* output_path, char* error_buffer, size_t error_size);

// Round-trip validation
bool ValidateMapRoundTrip(const struct MapData* original, const struct MapData* converted);
```

#### Tileset Converter

```c
struct TilesetData {
    char name[32];
    u8* graphics;
    size_t graphics_size;
    u16* metatiles;
    size_t metatile_count;
    u8* behavior;
    size_t behavior_size;
    u16* palettes;
    u8 palette_count;
};

// Convert Emerald tileset to FireRed format
bool ConvertTileset(const struct TilesetData* emerald_tileset, 
                    struct TilesetData* firered_tileset,
                    char* error_buffer, size_t error_size);

// Extract tileset from Crossroads
bool ExtractEmeraldTileset(const char* crossroads_path, const char* tileset_name,
                           struct TilesetData* out_data, char* error_buffer, size_t error_size);
```

#### Script Converter

```c
struct ScriptCommand {
    u16 command_id;
    u8* parameters;
    size_t param_count;
};

struct Script {
    char name[64];
    struct ScriptCommand* commands;
    size_t command_count;
};

// Convert Emerald script commands to FireRed equivalents
bool ConvertScript(const struct Script* emerald_script,
                   struct Script* firered_script,
                   char* error_buffer, size_t error_size);

// Map Emerald command to FireRed command
u16 MapScriptCommand(u16 emerald_command, bool* has_equivalent);
```

#### Trainer Data Converter

```c
struct TrainerPokemon {
    u16 species;
    u8 level;
    u16 held_item;
    u16 moves[4];
    u8 iv;
    u8 ev_hp;
    u8 ev_atk;
    u8 ev_def;
    u8 ev_speed;
    u8 ev_spatk;
    u8 ev_spdef;
};

struct TrainerData {
    char name[32];
    u8 trainer_class;
    u8 gender;
    u8 music;
    u16 sprite;
    char intro_text[256];
    char defeat_text[256];
    u16 reward;
    u8 ai_flags;
    struct TrainerPokemon* party;
    u8 party_size;
};

// Convert Emerald trainer to FireRed format
bool ConvertTrainer(const struct TrainerData* emerald_trainer,
                    struct TrainerData* firered_trainer,
                    char* error_buffer, size_t error_size);

// Map Emerald trainer class to FireRed equivalent
u8 MapTrainerClass(u8 emerald_class, bool* is_exact_match);
```

#### Wild Encounter Converter

```c
struct EncounterSlot {
    u16 species;
    u8 min_level;
    u8 max_level;
};

struct WildEncounterData {
    u8 encounter_rate;
    struct EncounterSlot grass_encounters[12];
    struct EncounterSlot water_encounters[5];
    struct EncounterSlot fishing_old[5];
    struct EncounterSlot fishing_good[5];
    struct EncounterSlot fishing_super[5];
    struct EncounterSlot rock_smash[5];
};

// Convert Emerald encounters to FireRed format
bool ConvertEncounters(const struct WildEncounterData* emerald_encounters,
                       struct WildEncounterData* firered_encounters,
                       char* error_buffer, size_t error_size);
```

### Region Manager Extensions

Extend existing Region_Manager to support additional regions and badge tracking.

```c
// Extended badge tracking (32 badges total)
struct BadgeData {
    bool kanto_badges[8];
    bool johto_badges[8];
    bool hoenn_badges[8];
    bool sinnoh_badges[8];
};

// Extended region state
struct RegionState {
    u8 current_region;
    bool johto_unlocked;
    bool hoenn_unlocked;
    bool sinnoh_unlocked;
    bool elite_four_defeated[4];  // One per region
    struct BadgeData badges;
};

// Get badge count for specific region
u8 GetRegionBadgeCount(u8 region_id);

// Get total badge count across all regions
u8 GetTotalBadgeCount(void);

// Check if specific badge is obtained
bool HasBadge(u8 region_id, u8 badge_index);

// Award badge for specific region
void AwardBadge(u8 region_id, u8 badge_index);

// Check region unlock status
bool IsRegionUnlocked(u8 region_id);

// Unlock region and award ticket
void UnlockRegion(u8 region_id);
```

### Level Scaler Extensions

Extend existing Level_Scaler to handle cross-region scaling.

```c
// Calculate wild Pokemon levels for any region
struct LevelRange GetWildPokemonLevelRangeForRegion(u8 badge_count, u8 region_id, bool elite_four_defeated);

// Calculate trainer levels for any region
u8 GetTrainerAverageLevelForRegion(u8 badge_count, u8 region_id, u8 trainer_type);

// Get level cap for current progression
u8 GetLevelCapForProgression(u8 total_badges, u8 current_region);
```

### Warp System Extensions

```c
// Region transition via ticket
bool UseRegionTicket(u16 ticket_item);

// Get starting location for region
struct WarpData GetRegionStartLocation(u8 region_id);

// Validate region transition
bool CanTransitionToRegion(u8 target_region);

// Execute region transition
void TransitionToRegion(u8 target_region);
```

### Fly System Extensions

```c
// Register fly location for current region
void RegisterFlyLocation(u16 map_id);

// Get fly locations for current region
u16* GetCurrentRegionFlyLocations(u8* out_count);

// Check if fly location is registered
bool IsFlyLocationRegistered(u16 map_id);

// Get fly location name
const char* GetFlyLocationName(u16 map_id);
```

## Data Models

### Map Data Structure (FireRed Format)

```json
{
  "id": "MAP_JOHTO_NEW_BARK_TOWN",
  "name": "NewBarkTown",
  "layout": "LAYOUT_JOHTO_NEW_BARK_TOWN",
  "music": "MUS_NEW_BARK",
  "region_map_section": "MAPSEC_NEW_BARK_TOWN",
  "requires_flash": false,
  "weather": "WEATHER_SUNNY",
  "map_type": "MAP_TYPE_TOWN",
  "allow_cycling": true,
  "allow_escaping": false,
  "allow_running": true,
  "show_map_name": true,
  "floor_number": 0,
  "battle_scene": "MAP_BATTLE_SCENE_NORMAL",
  "region_id": 1,
  "connections": [],
  "object_events": [],
  "warp_events": [],
  "coord_events": [],
  "bg_events": []
}
```

### Tileset Data Structure

```c
struct Tileset {
    bool8 isCompressed;
    bool8 isSecondary;
    const u32 *tiles;
    const u16 (*palettes)[16];
    const u16 *metatiles;
    const u16 *metatileAttributes;
    TilesetCB callback;
};
```

### Save Data Extensions

```c
// Extended save structure for multi-region support
struct SaveBlock1 {
    // ... existing fields ...
    
    // Multi-region extensions
    u8 currentRegion;
    bool8 johtoUnlocked;
    bool8 hoennUnlocked;
    bool8 sinnohUnlocked;
    bool8 eliteFourDefeated[4];
    bool8 badges[32];  // 8 per region
    u16 flyLocations[64];  // 16 per region
    u8 flyLocationCount[4];  // Count per region
};
```

### Region Ticket Items

```c
// Item definitions for region tickets
#define ITEM_JOHTO_TICKET  500
#define ITEM_HOENN_TICKET  501
#define ITEM_SINNOH_TICKET 502

// Item data
const struct Item gItems[] = {
    // ... existing items ...
    [ITEM_JOHTO_TICKET] = {
        .name = _("JOHTO TICKET"),
        .itemId = ITEM_JOHTO_TICKET,
        .price = 0,
        .description = COMPOUND_STRING("A ticket for travel\nto the Johto region."),
        .importance = 1,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .fieldUseFunc = ItemUseOutOfBattle_JohtoTicket,
    },
    // ... similar for HOENN_TICKET and SINNOH_TICKET ...
};
```


### Map Conversion Algorithm

The map conversion process follows a multi-stage pipeline:

```
Stage 1: Extraction
├── Clone Pokemon Crossroads repository
├── Locate region map directories
└── Identify map files by region

Stage 2: Parsing
├── Parse Emerald map.json format
├── Extract tileset references
├── Parse event scripts
├── Parse trainer data
└── Parse wild encounter tables

Stage 3: Transformation
├── Convert map header fields
├── Map tileset IDs to FireRed equivalents
├── Convert script commands
├── Map trainer classes
└── Convert encounter structures

Stage 4: Validation
├── Verify map dimensions
├── Validate warp destinations
├── Check connection integrity
├── Verify tileset references
└── Validate script references

Stage 5: Output
├── Generate FireRed map.json
├── Generate header.inc
├── Generate events.inc
├── Generate scripts.inc
└── Update map registry
```

### Format Conversion Mappings

#### Map Type Mapping

| Emerald Type | FireRed Type | Notes |
|--------------|--------------|-------|
| MAP_TYPE_TOWN | MAP_TYPE_TOWN | Direct mapping |
| MAP_TYPE_CITY | MAP_TYPE_CITY | Direct mapping |
| MAP_TYPE_ROUTE | MAP_TYPE_ROUTE | Direct mapping |
| MAP_TYPE_UNDERGROUND | MAP_TYPE_UNDERGROUND | Direct mapping |
| MAP_TYPE_UNDERWATER | MAP_TYPE_UNDERWATER | May need special handling |
| MAP_TYPE_OCEAN_ROUTE | MAP_TYPE_ROUTE | Map to standard route |
| MAP_TYPE_INDOOR | MAP_TYPE_INDOOR | Direct mapping |
| MAP_TYPE_SECRET_BASE | MAP_TYPE_INDOOR | Map to indoor type |

#### Weather Mapping

| Emerald Weather | FireRed Weather | Notes |
|-----------------|-----------------|-------|
| WEATHER_NONE | WEATHER_SUNNY | Default |
| WEATHER_SUNNY | WEATHER_SUNNY | Direct mapping |
| WEATHER_RAIN | WEATHER_RAIN | Direct mapping |
| WEATHER_SNOW | WEATHER_SNOW | Direct mapping |
| WEATHER_THUNDERSTORM | WEATHER_RAIN | Downgrade to rain |
| WEATHER_FOG | WEATHER_SUNNY | No fog in FireRed |
| WEATHER_ASH | WEATHER_SUNNY | No ash in FireRed |
| WEATHER_SANDSTORM | WEATHER_SUNNY | No sandstorm weather |
| WEATHER_DROUGHT | WEATHER_SUNNY | Map to sunny |
| WEATHER_UNDERWATER | WEATHER_SUNNY | Special case |

#### Script Command Mapping

Key script command conversions:

```c
// Emerald -> FireRed command mappings
const u16 gScriptCommandMap[] = {
    [EMERALD_CMD_MSGBOX] = FIRERED_CMD_MSGBOX,
    [EMERALD_CMD_GIVEITEM] = FIRERED_CMD_GIVEITEM,
    [EMERALD_CMD_GIVEMON] = FIRERED_CMD_GIVEMON,
    [EMERALD_CMD_SETFLAG] = FIRERED_CMD_SETFLAG,
    [EMERALD_CMD_CLEARFLAG] = FIRERED_CMD_CLEARFLAG,
    [EMERALD_CMD_CHECKFLAG] = FIRERED_CMD_CHECKFLAG,
    [EMERALD_CMD_WARP] = FIRERED_CMD_WARP,
    [EMERALD_CMD_PLAYBGM] = FIRERED_CMD_PLAYBGM,
    [EMERALD_CMD_FADESCREEN] = FIRERED_CMD_FADESCREEN,
    // ... additional mappings ...
};

// Commands with no direct equivalent
const u16 gIncompatibleCommands[] = {
    EMERALD_CMD_SETWEATHER,  // Limited weather support
    EMERALD_CMD_DOWEATHER,   // Limited weather support
    EMERALD_CMD_SETMAPTILE,  // Different implementation
    // ... additional incompatible commands ...
};
```

## Error Handling

### Conversion Error Types

```c
enum ConversionError {
    CONV_ERROR_NONE = 0,
    CONV_ERROR_FILE_NOT_FOUND,
    CONV_ERROR_PARSE_FAILED,
    CONV_ERROR_INVALID_FORMAT,
    CONV_ERROR_MISSING_TILESET,
    CONV_ERROR_INVALID_WARP,
    CONV_ERROR_INVALID_CONNECTION,
    CONV_ERROR_SCRIPT_INCOMPATIBLE,
    CONV_ERROR_TRAINER_CLASS_MISSING,
    CONV_ERROR_VALIDATION_FAILED,
    CONV_ERROR_OUTPUT_FAILED,
};

struct ConversionResult {
    enum ConversionError error;
    char error_message[256];
    char map_name[64];
    u32 line_number;  // For parse errors
};
```

### Error Handling Strategy

1. **Graceful Degradation**: When encountering incompatible features, log warnings and use safe defaults
2. **Detailed Logging**: Record all conversion issues with map names, line numbers, and specific problems
3. **Validation Gates**: Prevent invalid data from entering the build system
4. **Recovery Mechanisms**: Provide fallback options for missing resources

### Error Reporting

```c
// Log conversion error with context
void LogConversionError(const char* map_name, enum ConversionError error, const char* details);

// Generate conversion report
void GenerateConversionReport(const char* output_path);

// Check if map conversion succeeded
bool IsMapConversionSuccessful(const char* map_name);

// Get conversion statistics
struct ConversionStats {
    u32 maps_attempted;
    u32 maps_succeeded;
    u32 maps_failed;
    u32 warnings_generated;
    u32 tilesets_converted;
    u32 scripts_converted;
    u32 trainers_converted;
};

void GetConversionStats(struct ConversionStats* out_stats);
```

### Runtime Error Handling

```c
// Handle missing map data at runtime
void HandleMissingMap(u16 map_id);

// Handle invalid warp destination
void HandleInvalidWarp(u16 source_map, u16 dest_map);

// Handle missing tileset
void HandleMissingTileset(u16 tileset_id);

// Fallback to safe state
void FallbackToSafeMap(void);
```

## Testing Strategy

### Unit Testing

Unit tests focus on specific conversion functions and data structure transformations:

1. **Map Parser Tests**
   - Test parsing valid Emerald map.json files
   - Test handling malformed JSON
   - Test extraction of all map fields
   - Test error reporting for invalid data

2. **Format Converter Tests**
   - Test map type conversions
   - Test weather conversions
   - Test script command mappings
   - Test trainer class mappings
   - Test encounter table conversions

3. **Validation Tests**
   - Test warp destination validation
   - Test connection integrity checks
   - Test tileset reference validation
   - Test script reference validation

4. **Region Manager Tests**
   - Test badge tracking for all regions
   - Test region unlock logic
   - Test Elite Four defeat tracking
   - Test ticket awarding

5. **Level Scaler Tests**
   - Test level calculations for each region
   - Test cross-region scaling
   - Test Elite Four bonus application

### Property-Based Testing

Property tests verify universal behaviors across all inputs. The testing library used will be **Theft** (C property-based testing library).

Configuration:
- Minimum 100 iterations per property test
- Each test tagged with feature name and property number
- Tests reference design document properties

### Integration Testing

Integration tests verify end-to-end workflows:

1. **Map Conversion Pipeline**
   - Test complete conversion of sample maps
   - Verify output file generation
   - Validate build system integration

2. **Region Transition**
   - Test region unlocking after Elite Four defeat
   - Test ticket usage and region warping
   - Test party and inventory preservation

3. **Level Scaling**
   - Test wild Pokemon levels across regions
   - Test trainer levels across regions
   - Test level cap enforcement

4. **Save/Load**
   - Test saving with multi-region data
   - Test loading and state restoration
   - Test backward compatibility

### Manual Testing Checklist

1. Complete Kanto Elite Four and verify Johto unlock
2. Travel to Johto and verify map rendering
3. Battle wild Pokemon and verify level scaling
4. Battle trainers and verify level scaling
5. Collect Johto badges and verify tracking
6. Complete Johto Elite Four and verify Hoenn unlock
7. Repeat for Hoenn and Sinnoh regions
8. Test Fly system in each region
9. Test save/load at various progression points
10. Verify all 32 badges are trackable


## Correctness Properties

A property is a characteristic or behavior that should hold true across all valid executions of a system-essentially, a formal statement about what the system should do. Properties serve as the bridge between human-readable specifications and machine-verifiable correctness guarantees.

### Property Reflection

After analyzing all acceptance criteria, several properties can be consolidated:
- Properties 1.1, 1.2, 1.3 can be combined into a single property about Elite Four defeat unlocking the next region
- Properties 2.1-2.9, 3.1-3.9, 4.1-4.9 can be combined into properties about map completeness per region
- Properties 11.1-11.6 can be combined into properties about level scaling application across all regions
- Properties 12.1-12.3 can be combined into a single property about fly location registration
- Properties 14.1-14.4 can be combined into a property about badge tracking separation
- Properties 20.1-20.4 can be combined into a property about regional Pokemon exclusivity

### Property 1: Elite Four Progression Unlocks Next Region

*For any* region with an Elite Four, defeating that region's Elite Four should unlock the next region in sequence (Kanto → Johto → Hoenn → Sinnoh).

**Validates: Requirements 1.1, 1.2, 1.3**

### Property 2: Region Unlock Awards Ticket

*For any* region that becomes unlocked, the player should receive the corresponding region ticket item.

**Validates: Requirements 1.4**

### Property 3: Locked Region Access Prevention

*For any* locked region and any warp attempt, the system should prevent the player from accessing that region.

**Validates: Requirements 1.5**

### Property 4: Region Map Completeness

*For any* region (Johto, Hoenn, Sinnoh), all required maps (routes, cities, caves, dungeons) from the original game should be present in the Map_Bank.

**Validates: Requirements 2.1-2.9, 3.1-3.9, 4.1-4.9**

### Property 5: Map Dimension Preservation

*For any* map converted from Emerald to FireRed format, the map dimensions (width and height) should remain unchanged.

**Validates: Requirements 5.4**

### Property 6: Map Connection Preservation

*For any* map with connections, all connection data should be preserved during format conversion with valid destination maps.

**Validates: Requirements 5.5, 15.2**

### Property 7: Warp Point Preservation

*For any* map with warp points, all warp data should be preserved during format conversion with valid destination maps.

**Validates: Requirements 5.6, 15.3**

### Property 8: Conversion Error Logging

*For any* map conversion that fails, the system should log detailed error information including the map name and specific failure reason.

**Validates: Requirements 5.7, 6.6, 7.5, 8.6, 15.6, 17.5, 18.6**

### Property 9: Tileset Reference Validity

*For any* converted map, all tileset references in the map header should point to valid, converted tilesets.

**Validates: Requirements 6.5, 15.5**

### Property 10: Script Command Conversion

*For any* Emerald script command that has a FireRed equivalent, the conversion should produce the correct FireRed command with preserved parameters.

**Validates: Requirements 7.2, 7.4**

### Property 11: Trainer Reward Preservation

*For any* trainer converted from Emerald to FireRed format, the reward amount should remain unchanged.

**Validates: Requirements 8.4**

### Property 12: Encounter Rate Preservation

*For any* wild encounter table converted from Emerald to FireRed format, the encounter rates should remain unchanged.

**Validates: Requirements 9.3**

### Property 13: Pokemon Species Preservation

*For any* wild encounter table converted from Emerald to FireRed format, all Pokemon species and their level ranges should remain unchanged.

**Validates: Requirements 9.4**

### Property 14: Region Ticket Warp Functionality

*For any* region ticket used by the player, if the region is unlocked, the player should be warped to that region's starting location.

**Validates: Requirements 10.1**

### Property 15: Party Preservation During Region Transition

*For any* region transition, the player's party should remain unchanged before and after the transition.

**Validates: Requirements 10.2**

### Property 16: Inventory Preservation During Region Transition

*For any* region transition, the player's inventory should remain unchanged before and after the transition.

**Validates: Requirements 10.3**

### Property 17: Region State Update During Transition

*For any* region transition, the Region_Manager's current region state should be updated to reflect the new region.

**Validates: Requirements 10.4**

### Property 18: Locked Region Ticket Rejection

*For any* attempt to use a region ticket for a locked region, the system should display an error message and prevent the transition.

**Validates: Requirements 10.5**

### Property 19: Level Scaling Application

*For any* wild encounter or trainer in any region (Johto, Hoenn, Sinnoh), level scaling should be applied based on player progression (badge count and Elite Four completion).

**Validates: Requirements 11.1, 11.2, 11.3, 11.4, 11.5, 11.6, 11.7**

### Property 20: Fly Location Registration

*For any* city with a designated fly location in any region, visiting that city should register the fly location for that region.

**Validates: Requirements 12.1, 12.2, 12.3**

### Property 21: Fly Menu Region Filtering

*For any* use of HM Fly, the displayed fly locations should only include locations from the current region (plus starting locations of unlocked regions).

**Validates: Requirements 12.4, 12.5**

### Property 22: Regional Item Placement

*For any* region-specific item, that item should only appear in locations within its corresponding region.

**Validates: Requirements 13.1, 13.2, 13.3, 13.4, 13.5**

### Property 23: Badge Tracking Separation

*For any* badge earned, it should be tracked separately by region, with each region maintaining its own set of 8 badges.

**Validates: Requirements 14.1, 14.2, 14.3, 14.4, 14.5**

### Property 24: Badge Storage Capacity

*For any* game state, the system should support tracking a total of 32 badges (8 per region × 4 regions).

**Validates: Requirements 14.6**

### Property 25: Map Header Validation

*For any* ported map, the map header structure should be valid according to FireRed format specifications.

**Validates: Requirements 15.1**

### Property 26: Script Reference Validation

*For any* ported map, all event script references should point to valid, existing scripts.

**Validates: Requirements 15.4**

### Property 27: Save Data Round-Trip

*For any* game state with multi-region data (current region, unlocked regions, badges, fly locations), saving and then loading should restore the exact same state.

**Validates: Requirements 16.1, 16.2, 16.3, 16.4, 16.5**

### Property 28: Map Data Round-Trip

*For any* valid map data, parsing from Emerald format, converting to FireRed format, and parsing as FireRed should produce an equivalent map structure with all metadata preserved (dimensions, tileset references, music, weather).

**Validates: Requirements 19.4, 19.5**

### Property 29: Regional Pokemon Exclusivity

*For any* region-exclusive Pokemon species, that species should only appear in wild encounters within its native region.

**Validates: Requirements 20.1, 20.2, 20.3, 20.4**

### Property 30: Cross-Regional Pokemon Distribution

*For any* Pokemon species that appears in multiple regions in the original games, that species should appear in wild encounters in all appropriate regions.

**Validates: Requirements 20.5**


## Implementation Phases

### Phase 1: Format Converter Development (Weeks 1-8)

**Objective**: Build robust format conversion tools

Tasks:
1. Implement Emerald map parser
2. Implement FireRed map formatter
3. Implement tileset converter
4. Implement script command mapper
5. Implement trainer data converter
6. Implement wild encounter converter
7. Implement validation engine
8. Implement error logging system
9. Write unit tests for all converters
10. Write property tests for round-trip conversions

**Deliverables**:
- Working format converter tool
- Comprehensive test suite
- Conversion error logging system
- Documentation for converter usage

### Phase 2: Johto Region Port (Weeks 9-16)

**Objective**: Port complete Johto region from Crossroads

Tasks:
1. Clone Pokemon Crossroads repository
2. Identify all Johto maps in Crossroads
3. Convert all Johto routes (Routes 29-46)
4. Convert all Johto cities (10 cities)
5. Convert all Johto caves and dungeons (12 locations)
6. Convert Johto tilesets
7. Convert Johto event scripts
8. Convert Johto trainer data
9. Convert Johto wild encounters
10. Validate all Johto maps
11. Integrate Johto maps into build system
12. Test Johto region in-game

**Deliverables**:
- Complete Johto region maps
- Johto tilesets
- Johto scripts and trainers
- Johto wild encounters
- Build system integration

### Phase 3: Hoenn Region Port (Weeks 17-24)

**Objective**: Port complete Hoenn region from Crossroads

Tasks:
1. Convert all Hoenn routes (Routes 101-134)
2. Convert all Hoenn cities (16 cities)
3. Convert all Hoenn caves and dungeons (13 locations)
4. Convert Hoenn tilesets
5. Convert Hoenn event scripts
6. Convert Hoenn trainer data
7. Convert Hoenn wild encounters
8. Validate all Hoenn maps
9. Integrate Hoenn maps into build system
10. Test Hoenn region in-game

**Deliverables**:
- Complete Hoenn region maps
- Hoenn tilesets
- Hoenn scripts and trainers
- Hoenn wild encounters
- Build system integration

### Phase 4: Sinnoh Region Port (Weeks 25-32)

**Objective**: Port complete Sinnoh region from Crossroads

Tasks:
1. Convert all Sinnoh routes (Routes 201-230)
2. Convert all Sinnoh cities (15 cities)
3. Convert all Sinnoh caves and dungeons (14 locations)
4. Convert Sinnoh tilesets
5. Convert Sinnoh event scripts
6. Convert Sinnoh trainer data
7. Convert Sinnoh wild encounters
8. Validate all Sinnoh maps
9. Integrate Sinnoh maps into build system
10. Test Sinnoh region in-game

**Deliverables**:
- Complete Sinnoh region maps
- Sinnoh tilesets
- Sinnoh scripts and trainers
- Sinnoh wild encounters
- Build system integration

### Phase 5: Region Management Integration (Weeks 33-36)

**Objective**: Integrate multi-region support into existing systems

Tasks:
1. Extend Region_Manager for 4 regions
2. Implement 32-badge tracking system
3. Implement region unlock logic
4. Implement region ticket system
5. Extend Level_Scaler for cross-region scaling
6. Implement fly location registration per region
7. Implement region transition system
8. Extend save data structures
9. Implement save/load for multi-region data
10. Write integration tests

**Deliverables**:
- Extended Region_Manager
- Extended Level_Scaler
- Region transition system
- Multi-region save data support
- Integration test suite

### Phase 6: Testing and Polish (Weeks 37-40)

**Objective**: Comprehensive testing and bug fixes

Tasks:
1. Run all property-based tests
2. Run all integration tests
3. Perform manual testing of all regions
4. Test progression through all 4 regions
5. Test level scaling across regions
6. Test save/load at various points
7. Fix identified bugs
8. Optimize ROM size if needed
9. Performance testing
10. Final validation

**Deliverables**:
- Bug-free multi-region system
- Complete test coverage
- Performance optimization
- Final ROM build

## Technical Constraints

### ROM Size Limitations

FireRed ROM has size constraints that must be managed:

- **Base ROM Size**: 16 MB (maximum for GBA)
- **Current Usage**: ~8 MB (Kanto + existing features)
- **Available Space**: ~8 MB for three additional regions
- **Per-Region Budget**: ~2.5 MB per region (Johto, Hoenn, Sinnoh)

Size optimization strategies:
1. Compress tileset graphics
2. Reuse common tilesets across regions
3. Optimize script data
4. Share trainer sprites where possible
5. Compress map data

### Build System Integration

The build system must be extended to handle multi-region data:

```makefile
# Map data rules for multi-region support
JOHTO_MAPS := $(wildcard data/maps/Johto_*/)
HOENN_MAPS := $(wildcard data/maps/Hoenn_*/)
SINNOH_MAPS := $(wildcard data/maps/Sinnoh_*/)

# Compile all region maps
$(BUILD_DIR)/data/maps.o: $(KANTO_MAPS) $(JOHTO_MAPS) $(HOENN_MAPS) $(SINNOH_MAPS)
	$(CC) $(CFLAGS) -c data/maps.s -o $@

# Tileset compilation
$(BUILD_DIR)/data/tilesets.o: $(TILESET_FILES)
	$(CC) $(CFLAGS) -c data/tilesets.s -o $@
```

### Performance Considerations

1. **Map Loading**: Implement lazy loading for region maps to reduce memory usage
2. **Tileset Caching**: Cache frequently used tilesets in RAM
3. **Script Execution**: Optimize script interpreter for converted scripts
4. **Level Calculation**: Cache level scaling calculations to avoid repeated computation

### Compatibility Requirements

1. **Save Data**: Maintain backward compatibility with existing saves (graceful upgrade)
2. **Existing Features**: Ensure all existing Tuxedo features continue to work
3. **Level Scaling**: Integrate seamlessly with existing Level_Scaler
4. **Region Manager**: Extend existing Region_Manager without breaking current functionality

## Risk Mitigation

### Risk 1: ROM Size Overflow

**Likelihood**: Medium  
**Impact**: High  
**Mitigation**:
- Monitor ROM size throughout development
- Implement compression for graphics and maps
- Identify and remove unused assets
- Consider splitting into multiple ROM versions if necessary

### Risk 2: Conversion Errors

**Likelihood**: High  
**Impact**: Medium  
**Mitigation**:
- Comprehensive validation at each conversion step
- Detailed error logging with map names and line numbers
- Manual review of converted maps
- Property-based testing for round-trip conversions

### Risk 3: Performance Degradation

**Likelihood**: Medium  
**Impact**: Medium  
**Mitigation**:
- Profile performance at each phase
- Implement caching strategies
- Optimize critical paths (map loading, level calculation)
- Test on real hardware (GBA or accurate emulator)

### Risk 4: Script Incompatibilities

**Likelihood**: High  
**Impact**: Medium  
**Mitigation**:
- Maintain mapping table of incompatible commands
- Implement fallback behaviors for unsupported commands
- Manual review of critical scripts (gym leaders, Elite Four)
- Test all major story events

### Risk 5: Save Data Corruption

**Likelihood**: Low  
**Impact**: High  
**Mitigation**:
- Implement save data validation
- Maintain save data version numbers
- Test save/load extensively
- Provide save data migration tools if needed

## Future Enhancements

### Post-Launch Improvements

1. **Additional Regions**: Support for more regions (Alola, Galar, etc.)
2. **Custom Maps**: Tools for creating custom multi-region content
3. **Enhanced Transitions**: Animated region transitions
4. **Region-Specific Features**: Unique mechanics per region (contests, underground, etc.)
5. **Cross-Region Trading**: Enhanced trading between regions
6. **Region Leaderboards**: Track completion times per region

### Optimization Opportunities

1. **Dynamic Map Loading**: Load maps on-demand to reduce memory usage
2. **Procedural Generation**: Generate some maps procedurally to save space
3. **Asset Streaming**: Stream assets from external storage if supported
4. **Advanced Compression**: Implement custom compression algorithms for map data

## Conclusion

This design provides a comprehensive architecture for implementing full multi-region support in Pokemon Tuxedo ROM hack. The system leverages Pokemon Crossroads as a proven source for multi-region map data, implements robust format conversion with extensive error handling, and integrates seamlessly with existing Region_Manager and Level_Scaler systems.

Key success factors:
- Robust format conversion with comprehensive validation
- Extensive property-based testing for correctness guarantees
- Careful ROM size management and optimization
- Seamless integration with existing systems
- Thorough testing at each phase

The phased implementation approach allows for incremental delivery and testing, reducing risk and enabling early feedback. The estimated timeline of 6-12 months accounts for the complexity of porting 300+ maps and ensuring quality across all regions.

