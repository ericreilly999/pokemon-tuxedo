// map_converter.h
// Format converter for Pokemon Crossroads (Emerald) to FireRed map data

#ifndef MAP_CONVERTER_H
#define MAP_CONVERTER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Maximum sizes for arrays and strings
#define MAX_NAME_LENGTH 64
#define MAX_CONNECTIONS 16
#define MAX_WARPS 128
#define MAX_OBJECTS 128
#define MAX_COORD_EVENTS 64
#define MAX_BG_EVENTS 128
#define ERROR_BUFFER_SIZE 512

// Connection structure
typedef struct {
    char direction[32];
    int offset;
    char map[64];
} Connection;

// Warp event structure
typedef struct {
    int x;
    int y;
    int elevation;
    int dest_warp_id;
    char dest_map[64];
} WarpEvent;

// Object event structure
typedef struct {
    char type[16];  // "object" or "clone"
    int graphics_id;
    int x;
    int y;
    int elevation;
    char movement_type[32];
    int movement_range_x;
    int movement_range_y;
    char trainer_type[32];
    char trainer_sight_or_berry_tree_id[32];
    char script[64];
    char flag[64];
    // For clone type
    int target_local_id;
    char target_map[64];
    char local_id[64];  // Optional local ID
} ObjectEvent;

// Coord event structure
typedef struct {
    char type[16];  // "trigger" or "weather"
    int x;
    int y;
    int elevation;
    // For trigger type
    char var[32];
    char var_value[32];
    char script[64];
    // For weather type
    char weather[32];
} CoordEvent;

// BG event structure
typedef struct {
    char type[32];  // "sign", "hidden_item", or "secret_base"
    int x;
    int y;
    int elevation;
    // For sign type
    char player_facing_dir[32];
    char script[64];
    // For hidden_item type
    char item[32];
    char flag[64];
    int quantity;
    char underfoot[16];
    // For secret_base type
    char secret_base_id[32];
} BgEvent;

// Intermediate representation for map data
typedef struct {
    char name[MAX_NAME_LENGTH];
    char id[MAX_NAME_LENGTH];
    char layout[MAX_NAME_LENGTH];
    char music[64];
    char region_map_section[64];
    bool requires_flash;
    char weather[32];
    char map_type[32];
    bool allow_cycling;
    bool allow_escaping;
    bool allow_running;
    bool show_map_name;
    int floor_number;
    char battle_scene[64];
    
    // Optional fields
    char shared_events_map[MAX_NAME_LENGTH];
    char shared_scripts_map[MAX_NAME_LENGTH];
    bool connections_no_include;
    
    // Arrays
    Connection connections[MAX_CONNECTIONS];
    uint16_t connection_count;
    
    WarpEvent warps[MAX_WARPS];
    uint16_t warp_count;
    
    ObjectEvent objects[MAX_OBJECTS];
    uint16_t object_count;
    
    CoordEvent coord_events[MAX_COORD_EVENTS];
    uint16_t coord_event_count;
    
    BgEvent bg_events[MAX_BG_EVENTS];
    uint16_t bg_event_count;
} MapData;

// Parse Emerald map header to intermediate format
// Returns true on success, false on failure
// On failure, error_buffer contains detailed error message
bool ParseEmeraldMap(const char* emerald_path, MapData* out_data, 
                     char* error_buffer, size_t error_size);

// Format intermediate data to FireRed map.json
// Returns true on success, false on failure
// On failure, error_buffer contains detailed error message
bool FormatFireRedMap(const MapData* data, const char* output_path, 
                      char* error_buffer, size_t error_size);

// Round-trip validation
// Returns true if maps are equivalent, false otherwise
bool ValidateMapRoundTrip(const MapData* original, const MapData* converted);

// ============================================================================
// Validation Engine
// ============================================================================

// Validation result structure
typedef struct {
    bool is_valid;
    char error_message[ERROR_BUFFER_SIZE];
    char map_name[MAX_NAME_LENGTH];
} ValidationResult;

// Validate map header structure
// Checks that all required fields are present and valid
// Returns ValidationResult with is_valid=true on success
ValidationResult ValidateMapHeader(const MapData* map);

// Validate warp destinations reference valid maps
// map_registry: array of valid map names
// registry_count: number of maps in registry
// Returns ValidationResult with is_valid=true if all warps are valid
ValidationResult ValidateWarpDestinations(const MapData* map, 
                                          const char** map_registry,
                                          size_t registry_count);

// Validate connection integrity
// Checks that all connections reference valid destination maps
// map_registry: array of valid map names
// registry_count: number of maps in registry
// Returns ValidationResult with is_valid=true if all connections are valid
ValidationResult ValidateConnections(const MapData* map,
                                     const char** map_registry,
                                     size_t registry_count);

// Validate tileset references
// Checks that tileset references are valid
// tileset_registry: array of valid tileset names
// registry_count: number of tilesets in registry
// Returns ValidationResult with is_valid=true if tilesets are valid
ValidationResult ValidateTilesetReferences(const MapData* map,
                                           const char** tileset_registry,
                                           size_t registry_count);

// Validate script references
// Checks that all event script references are valid
// script_registry: array of valid script names
// registry_count: number of scripts in registry
// Returns ValidationResult with is_valid=true if all scripts are valid
ValidationResult ValidateScriptReferences(const MapData* map,
                                          const char** script_registry,
                                          size_t registry_count);

// Comprehensive map validation
// Runs all validation checks and returns combined result
// Returns ValidationResult with is_valid=true if all checks pass
ValidationResult ValidateMap(const MapData* map,
                             const char** map_registry,
                             size_t map_count,
                             const char** tileset_registry,
                             size_t tileset_count,
                             const char** script_registry,
                             size_t script_count);

// Helper function to initialize ValidationResult
void InitValidationResult(ValidationResult* result);

// Helper function to initialize MapData with default values
void InitMapData(MapData* data);

// ============================================================================
// Tileset Converter
// ============================================================================

// Tileset data structure for intermediate representation
typedef struct {
    char name[32];
    uint8_t* graphics;
    size_t graphics_size;
    uint16_t* metatiles;
    size_t metatile_count;
    uint8_t* behavior;
    size_t behavior_size;
    uint16_t* palettes;
    uint8_t palette_count;
} TilesetData;

// Extract tileset from Crossroads source
// Returns true on success, false on failure
// On failure, error_buffer contains detailed error message
bool ExtractEmeraldTileset(const char* crossroads_path, const char* tileset_name,
                           TilesetData* out_data, char* error_buffer, size_t error_size);

// Convert Emerald tileset to FireRed format
// Returns true on success, false on failure
// On failure, error_buffer contains detailed error message
bool ConvertTileset(const TilesetData* emerald_tileset, 
                    TilesetData* firered_tileset,
                    char* error_buffer, size_t error_size);

// Helper function to initialize TilesetData with default values
void InitTilesetData(TilesetData* data);

// Helper function to free TilesetData memory
void FreeTilesetData(TilesetData* data);

// ============================================================================
// Script Converter
// ============================================================================

// Script command structure
typedef struct {
    uint16_t command_id;
    uint8_t* parameters;
    size_t param_count;
} ScriptCommand;

// Script structure
typedef struct {
    char name[MAX_NAME_LENGTH];
    ScriptCommand* commands;
    size_t command_count;
} Script;

// Map Emerald script command to FireRed command
// Returns FireRed command ID
// Sets has_equivalent to false if no direct mapping exists
uint16_t MapScriptCommand(uint16_t emerald_command, bool* has_equivalent);

// Convert Emerald script to FireRed format
// Returns true on success, false on failure
// On failure, error_buffer contains detailed error message
// Logs incompatible commands to error_buffer
bool ConvertScript(const Script* emerald_script,
                   Script* firered_script,
                   const char* map_name,
                   char* error_buffer, size_t error_size);

// Helper function to initialize Script with default values
void InitScript(Script* script);

// Helper function to free Script memory
void FreeScript(Script* script);

// ============================================================================
// Trainer Data Converter
// ============================================================================

// Trainer Pokemon structure
typedef struct {
    uint16_t species;
    uint8_t level;
    uint16_t held_item;
    uint16_t moves[4];
    uint8_t iv;
    uint8_t ev_hp;
    uint8_t ev_atk;
    uint8_t ev_def;
    uint8_t ev_speed;
    uint8_t ev_spatk;
    uint8_t ev_spdef;
} TrainerPokemon;

// Trainer data structure
typedef struct {
    char name[32];
    uint8_t trainer_class;
    uint8_t gender;
    uint8_t music;
    uint16_t sprite;
    char intro_text[256];
    char defeat_text[256];
    uint16_t reward;
    uint8_t ai_flags;
    TrainerPokemon* party;
    uint8_t party_size;
} TrainerData;

// Map Emerald trainer class to FireRed equivalent
// Returns FireRed trainer class ID
// Sets is_exact_match to false if mapping is approximate
uint8_t MapTrainerClass(uint8_t emerald_class, bool* is_exact_match);

// Convert Emerald trainer to FireRed format
// Returns true on success, false on failure
// On failure, error_buffer contains detailed error message
bool ConvertTrainer(const TrainerData* emerald_trainer,
                    TrainerData* firered_trainer,
                    char* error_buffer, size_t error_size);

// Helper function to initialize TrainerData with default values
void InitTrainerData(TrainerData* data);

// Helper function to free TrainerData memory
void FreeTrainerData(TrainerData* data);

// ============================================================================
// Wild Encounter Converter
// ============================================================================

// Encounter slot structure
typedef struct {
    uint16_t species;
    uint8_t min_level;
    uint8_t max_level;
} EncounterSlot;

// Wild encounter data structure
typedef struct {
    uint8_t encounter_rate;
    EncounterSlot grass_encounters[12];
    EncounterSlot water_encounters[5];
    EncounterSlot fishing_old[5];
    EncounterSlot fishing_good[5];
    EncounterSlot fishing_super[5];
    EncounterSlot rock_smash[5];
} WildEncounterData;

// Convert Emerald encounters to FireRed format
// Returns true on success, false on failure
// On failure, error_buffer contains detailed error message
bool ConvertEncounters(const WildEncounterData* emerald_encounters,
                       WildEncounterData* firered_encounters,
                       char* error_buffer, size_t error_size);

// Helper function to initialize WildEncounterData with default values
void InitWildEncounterData(WildEncounterData* data);

// ============================================================================
// Error Logging System
// ============================================================================

// Conversion error types
typedef enum {
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
} ConversionError;

// Conversion result structure
typedef struct {
    ConversionError error;
    char error_message[256];
    char map_name[64];
    uint32_t line_number;  // For parse errors
} ConversionResult;

// Conversion statistics structure
typedef struct {
    uint32_t maps_attempted;
    uint32_t maps_succeeded;
    uint32_t maps_failed;
    uint32_t warnings_generated;
    uint32_t tilesets_converted;
    uint32_t scripts_converted;
    uint32_t trainers_converted;
} ConversionStats;

// Log conversion error with context
// map_name: Name of the map being converted
// error: Type of error that occurred
// details: Additional details about the error
void LogConversionError(const char* map_name, ConversionError error, const char* details);

// Generate conversion report
// output_path: Path to write the report file
// Returns true on success, false on failure
bool GenerateConversionReport(const char* output_path);

// Get conversion statistics
// out_stats: Pointer to ConversionStats structure to fill
void GetConversionStats(ConversionStats* out_stats);

// Helper function to initialize ConversionResult
void InitConversionResult(ConversionResult* result);

// Helper function to get error type name as string
const char* GetErrorTypeName(ConversionError error);

// Clear all logged errors and reset statistics
void ClearConversionLog(void);

#endif // MAP_CONVERTER_H
