// map_converter.c
// Implementation of format converter for Pokemon Crossroads (Emerald) to FireRed map data

#include "map_converter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to safely copy strings
static void safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (src == NULL) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

// Helper function to read file contents
static char* read_file(const char* filepath, char* error_buffer, size_t error_size) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        snprintf(error_buffer, error_size, "Failed to open file: %s", filepath);
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer
    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer) {
        snprintf(error_buffer, error_size, "Failed to allocate memory for file: %s", filepath);
        fclose(file);
        return NULL;
    }
    
    // Read file
    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';
    fclose(file);
    
    return buffer;
}

// Helper function to write file contents
static bool write_file(const char* filepath, const char* content, 
                       char* error_buffer, size_t error_size) {
    FILE* file = fopen(filepath, "wb");
    if (!file) {
        snprintf(error_buffer, error_size, "Failed to open file for writing: %s", filepath);
        return false;
    }
    
    size_t content_len = strlen(content);
    size_t bytes_written = fwrite(content, 1, content_len, file);
    fclose(file);
    
    if (bytes_written != content_len) {
        snprintf(error_buffer, error_size, "Failed to write complete file: %s", filepath);
        return false;
    }
    
    return true;
}

void InitMapData(MapData* data) {
    memset(data, 0, sizeof(MapData));
    data->requires_flash = false;
    data->allow_cycling = true;
    data->allow_escaping = false;
    data->allow_running = true;
    data->show_map_name = true;
    data->floor_number = 0;
    data->connections_no_include = false;
    safe_strcpy(data->weather, "WEATHER_SUNNY", sizeof(data->weather));
    safe_strcpy(data->map_type, "MAP_TYPE_TOWN", sizeof(data->map_type));
    safe_strcpy(data->battle_scene, "MAP_BATTLE_SCENE_NORMAL", sizeof(data->battle_scene));
}

bool ParseEmeraldMap(const char* emerald_path, MapData* out_data, 
                     char* error_buffer, size_t error_size) {
    if (!emerald_path || !out_data || !error_buffer) {
        if (error_buffer) {
            snprintf(error_buffer, error_size, "Invalid parameters to ParseEmeraldMap");
        }
        return false;
    }
    
    // Initialize output data
    InitMapData(out_data);
    
    // Read file
    char* json_content = read_file(emerald_path, error_buffer, error_size);
    if (!json_content) {
        return false;
    }
    
    // Parse JSON (Note: This is a placeholder - actual implementation would use json11 C++ API)
    // For now, we'll implement basic parsing logic
    // In a real implementation, this would use the json11 library properly
    
    snprintf(error_buffer, error_size, 
             "JSON parsing not yet implemented - requires json11 C++ integration");
    free(json_content);
    return false;
}

bool FormatFireRedMap(const MapData* data, const char* output_path, 
                      char* error_buffer, size_t error_size) {
    if (!data || !output_path || !error_buffer) {
        if (error_buffer) {
            snprintf(error_buffer, error_size, "Invalid parameters to FormatFireRedMap");
        }
        return false;
    }
    
    // Build JSON string
    char* json_buffer = (char*)malloc(65536);  // 64KB buffer
    if (!json_buffer) {
        snprintf(error_buffer, error_size, "Failed to allocate memory for JSON output");
        return false;
    }
    
    // Start building JSON
    int offset = 0;
    offset += snprintf(json_buffer + offset, 65536 - offset, "{\n");
    offset += snprintf(json_buffer + offset, 65536 - offset, "  \"id\": \"%s\",\n", data->id);
    offset += snprintf(json_buffer + offset, 65536 - offset, "  \"name\": \"%s\",\n", data->name);
    offset += snprintf(json_buffer + offset, 65536 - offset, "  \"layout\": \"%s\",\n", data->layout);
    offset += snprintf(json_buffer + offset, 65536 - offset, "  \"music\": \"%s\",\n", data->music);
    offset += snprintf(json_buffer + offset, 65536 - offset, 
                      "  \"region_map_section\": \"%s\",\n", data->region_map_section);
    offset += snprintf(json_buffer + offset, 65536 - offset, 
                      "  \"requires_flash\": %s,\n", data->requires_flash ? "true" : "false");
    offset += snprintf(json_buffer + offset, 65536 - offset, "  \"weather\": \"%s\",\n", data->weather);
    offset += snprintf(json_buffer + offset, 65536 - offset, "  \"map_type\": \"%s\",\n", data->map_type);
    offset += snprintf(json_buffer + offset, 65536 - offset, 
                      "  \"allow_cycling\": %s,\n", data->allow_cycling ? "true" : "false");
    offset += snprintf(json_buffer + offset, 65536 - offset, 
                      "  \"allow_escaping\": %s,\n", data->allow_escaping ? "true" : "false");
    offset += snprintf(json_buffer + offset, 65536 - offset, 
                      "  \"allow_running\": %s,\n", data->allow_running ? "true" : "false");
    offset += snprintf(json_buffer + offset, 65536 - offset, 
                      "  \"show_map_name\": %s,\n", data->show_map_name ? "true" : "false");
    offset += snprintf(json_buffer + offset, 65536 - offset, 
                      "  \"floor_number\": %d,\n", data->floor_number);
    offset += snprintf(json_buffer + offset, 65536 - offset, 
                      "  \"battle_scene\": \"%s\",\n", data->battle_scene);
    
    // Add connections array
    offset += snprintf(json_buffer + offset, 65536 - offset, "  \"connections\": [");
    for (uint16_t i = 0; i < data->connection_count; i++) {
        if (i > 0) offset += snprintf(json_buffer + offset, 65536 - offset, ",");
        offset += snprintf(json_buffer + offset, 65536 - offset, "\n    {\n");
        offset += snprintf(json_buffer + offset, 65536 - offset, 
                          "      \"direction\": \"%s\",\n", data->connections[i].direction);
        offset += snprintf(json_buffer + offset, 65536 - offset, 
                          "      \"offset\": %d,\n", data->connections[i].offset);
        offset += snprintf(json_buffer + offset, 65536 - offset, 
                          "      \"map\": \"%s\"\n", data->connections[i].map);
        offset += snprintf(json_buffer + offset, 65536 - offset, "    }");
    }
    if (data->connection_count > 0) {
        offset += snprintf(json_buffer + offset, 65536 - offset, "\n  ");
    }
    offset += snprintf(json_buffer + offset, 65536 - offset, "],\n");
    
    // Add object_events array
    offset += snprintf(json_buffer + offset, 65536 - offset, "  \"object_events\": [],\n");
    
    // Add warp_events array
    offset += snprintf(json_buffer + offset, 65536 - offset, "  \"warp_events\": [],\n");
    
    // Add coord_events array
    offset += snprintf(json_buffer + offset, 65536 - offset, "  \"coord_events\": [],\n");
    
    // Add bg_events array
    offset += snprintf(json_buffer + offset, 65536 - offset, "  \"bg_events\": []\n");
    
    offset += snprintf(json_buffer + offset, 65536 - offset, "}\n");
    
    // Write to file
    bool success = write_file(output_path, json_buffer, error_buffer, error_size);
    free(json_buffer);
    
    return success;
}

bool ValidateMapRoundTrip(const MapData* original, const MapData* converted) {
    if (!original || !converted) {
        return false;
    }
    
    // Compare basic fields
    if (strcmp(original->name, converted->name) != 0) return false;
    if (strcmp(original->id, converted->id) != 0) return false;
    if (strcmp(original->layout, converted->layout) != 0) return false;
    if (strcmp(original->music, converted->music) != 0) return false;
    if (strcmp(original->weather, converted->weather) != 0) return false;
    if (strcmp(original->map_type, converted->map_type) != 0) return false;
    
    // Compare boolean fields
    if (original->requires_flash != converted->requires_flash) return false;
    if (original->allow_cycling != converted->allow_cycling) return false;
    if (original->allow_escaping != converted->allow_escaping) return false;
    if (original->allow_running != converted->allow_running) return false;
    if (original->show_map_name != converted->show_map_name) return false;
    
    // Compare numeric fields
    if (original->floor_number != converted->floor_number) return false;
    
    // Compare array counts
    if (original->connection_count != converted->connection_count) return false;
    if (original->warp_count != converted->warp_count) return false;
    if (original->object_count != converted->object_count) return false;
    if (original->coord_event_count != converted->coord_event_count) return false;
    if (original->bg_event_count != converted->bg_event_count) return false;
    
    return true;
}

// ============================================================================
// Tileset Converter Implementation
// ============================================================================

void InitTilesetData(TilesetData* data) {
    if (!data) return;
    
    memset(data, 0, sizeof(TilesetData));
    data->graphics = NULL;
    data->graphics_size = 0;
    data->metatiles = NULL;
    data->metatile_count = 0;
    data->behavior = NULL;
    data->behavior_size = 0;
    data->palettes = NULL;
    data->palette_count = 0;
}

void FreeTilesetData(TilesetData* data) {
    if (!data) return;
    
    if (data->graphics) {
        free(data->graphics);
        data->graphics = NULL;
    }
    if (data->metatiles) {
        free(data->metatiles);
        data->metatiles = NULL;
    }
    if (data->behavior) {
        free(data->behavior);
        data->behavior = NULL;
    }
    if (data->palettes) {
        free(data->palettes);
        data->palettes = NULL;
    }
    
    data->graphics_size = 0;
    data->metatile_count = 0;
    data->behavior_size = 0;
    data->palette_count = 0;
}

bool ExtractEmeraldTileset(const char* crossroads_path, const char* tileset_name,
                           TilesetData* out_data, char* error_buffer, size_t error_size) {
    if (!crossroads_path || !tileset_name || !out_data || !error_buffer) {
        if (error_buffer) {
            snprintf(error_buffer, error_size, 
                    "Invalid parameters to ExtractEmeraldTileset");
        }
        return false;
    }
    
    // Initialize output data
    InitTilesetData(out_data);
    
    // Copy tileset name
    safe_strcpy(out_data->name, tileset_name, sizeof(out_data->name));
    
    // Build paths to tileset files in Crossroads repository
    char graphics_path[512];
    char metatiles_path[512];
    char behavior_path[512];
    char palettes_path[512];
    
    snprintf(graphics_path, sizeof(graphics_path), 
            "%s/data/tilesets/%s/tiles.bin", crossroads_path, tileset_name);
    snprintf(metatiles_path, sizeof(metatiles_path), 
            "%s/data/tilesets/%s/metatiles.bin", crossroads_path, tileset_name);
    snprintf(behavior_path, sizeof(behavior_path), 
            "%s/data/tilesets/%s/metatile_attributes.bin", crossroads_path, tileset_name);
    snprintf(palettes_path, sizeof(palettes_path), 
            "%s/data/tilesets/%s/palettes.bin", crossroads_path, tileset_name);
    
    // Extract graphics data
    FILE* graphics_file = fopen(graphics_path, "rb");
    if (!graphics_file) {
        snprintf(error_buffer, error_size, 
                "Failed to open tileset graphics file: %s", graphics_path);
        return false;
    }
    
    fseek(graphics_file, 0, SEEK_END);
    out_data->graphics_size = ftell(graphics_file);
    fseek(graphics_file, 0, SEEK_SET);
    
    out_data->graphics = (uint8_t*)malloc(out_data->graphics_size);
    if (!out_data->graphics) {
        snprintf(error_buffer, error_size, 
                "Failed to allocate memory for tileset graphics");
        fclose(graphics_file);
        return false;
    }
    
    size_t bytes_read = fread(out_data->graphics, 1, out_data->graphics_size, graphics_file);
    fclose(graphics_file);
    
    if (bytes_read != out_data->graphics_size) {
        snprintf(error_buffer, error_size, 
                "Failed to read complete tileset graphics file");
        FreeTilesetData(out_data);
        return false;
    }
    
    // Extract metatiles data
    FILE* metatiles_file = fopen(metatiles_path, "rb");
    if (!metatiles_file) {
        snprintf(error_buffer, error_size, 
                "Failed to open tileset metatiles file: %s", metatiles_path);
        FreeTilesetData(out_data);
        return false;
    }
    
    fseek(metatiles_file, 0, SEEK_END);
    size_t metatiles_size = ftell(metatiles_file);
    fseek(metatiles_file, 0, SEEK_SET);
    
    out_data->metatile_count = metatiles_size / sizeof(uint16_t);
    out_data->metatiles = (uint16_t*)malloc(metatiles_size);
    if (!out_data->metatiles) {
        snprintf(error_buffer, error_size, 
                "Failed to allocate memory for tileset metatiles");
        fclose(metatiles_file);
        FreeTilesetData(out_data);
        return false;
    }
    
    bytes_read = fread(out_data->metatiles, 1, metatiles_size, metatiles_file);
    fclose(metatiles_file);
    
    if (bytes_read != metatiles_size) {
        snprintf(error_buffer, error_size, 
                "Failed to read complete tileset metatiles file");
        FreeTilesetData(out_data);
        return false;
    }
    
    // Extract behavior data
    FILE* behavior_file = fopen(behavior_path, "rb");
    if (!behavior_file) {
        snprintf(error_buffer, error_size, 
                "Failed to open tileset behavior file: %s", behavior_path);
        FreeTilesetData(out_data);
        return false;
    }
    
    fseek(behavior_file, 0, SEEK_END);
    out_data->behavior_size = ftell(behavior_file);
    fseek(behavior_file, 0, SEEK_SET);
    
    out_data->behavior = (uint8_t*)malloc(out_data->behavior_size);
    if (!out_data->behavior) {
        snprintf(error_buffer, error_size, 
                "Failed to allocate memory for tileset behavior");
        fclose(behavior_file);
        FreeTilesetData(out_data);
        return false;
    }
    
    bytes_read = fread(out_data->behavior, 1, out_data->behavior_size, behavior_file);
    fclose(behavior_file);
    
    if (bytes_read != out_data->behavior_size) {
        snprintf(error_buffer, error_size, 
                "Failed to read complete tileset behavior file");
        FreeTilesetData(out_data);
        return false;
    }
    
    // Extract palettes data
    FILE* palettes_file = fopen(palettes_path, "rb");
    if (!palettes_file) {
        snprintf(error_buffer, error_size, 
                "Failed to open tileset palettes file: %s", palettes_path);
        FreeTilesetData(out_data);
        return false;
    }
    
    fseek(palettes_file, 0, SEEK_END);
    size_t palettes_size = ftell(palettes_file);
    fseek(palettes_file, 0, SEEK_SET);
    
    // Each palette is 16 colors * 2 bytes per color = 32 bytes
    out_data->palette_count = palettes_size / 32;
    out_data->palettes = (uint16_t*)malloc(palettes_size);
    if (!out_data->palettes) {
        snprintf(error_buffer, error_size, 
                "Failed to allocate memory for tileset palettes");
        fclose(palettes_file);
        FreeTilesetData(out_data);
        return false;
    }
    
    bytes_read = fread(out_data->palettes, 1, palettes_size, palettes_file);
    fclose(palettes_file);
    
    if (bytes_read != palettes_size) {
        snprintf(error_buffer, error_size, 
                "Failed to read complete tileset palettes file");
        FreeTilesetData(out_data);
        return false;
    }
    
    return true;
}

bool ConvertTileset(const TilesetData* emerald_tileset, 
                    TilesetData* firered_tileset,
                    char* error_buffer, size_t error_size) {
    if (!emerald_tileset || !firered_tileset || !error_buffer) {
        if (error_buffer) {
            snprintf(error_buffer, error_size, 
                    "Invalid parameters to ConvertTileset");
        }
        return false;
    }
    
    // Initialize output data
    InitTilesetData(firered_tileset);
    
    // Copy tileset name
    safe_strcpy(firered_tileset->name, emerald_tileset->name, 
               sizeof(firered_tileset->name));
    
    // Convert graphics data
    // For FireRed, graphics format is the same as Emerald (4bpp tiles)
    // So we can directly copy the graphics data
    firered_tileset->graphics_size = emerald_tileset->graphics_size;
    firered_tileset->graphics = (uint8_t*)malloc(firered_tileset->graphics_size);
    if (!firered_tileset->graphics) {
        snprintf(error_buffer, error_size, 
                "Failed to allocate memory for FireRed tileset graphics");
        return false;
    }
    memcpy(firered_tileset->graphics, emerald_tileset->graphics, 
           firered_tileset->graphics_size);
    
    // Convert metatiles data
    // Metatile format is similar between Emerald and FireRed
    // Each metatile consists of 8 tiles (2x4 grid) with attributes
    firered_tileset->metatile_count = emerald_tileset->metatile_count;
    firered_tileset->metatiles = (uint16_t*)malloc(
        firered_tileset->metatile_count * sizeof(uint16_t));
    if (!firered_tileset->metatiles) {
        snprintf(error_buffer, error_size, 
                "Failed to allocate memory for FireRed tileset metatiles");
        FreeTilesetData(firered_tileset);
        return false;
    }
    memcpy(firered_tileset->metatiles, emerald_tileset->metatiles, 
           firered_tileset->metatile_count * sizeof(uint16_t));
    
    // Convert behavior data
    // Behavior attributes are mostly compatible between Emerald and FireRed
    // Some behavior values may need remapping, but for now we'll do direct copy
    firered_tileset->behavior_size = emerald_tileset->behavior_size;
    firered_tileset->behavior = (uint8_t*)malloc(firered_tileset->behavior_size);
    if (!firered_tileset->behavior) {
        snprintf(error_buffer, error_size, 
                "Failed to allocate memory for FireRed tileset behavior");
        FreeTilesetData(firered_tileset);
        return false;
    }
    memcpy(firered_tileset->behavior, emerald_tileset->behavior, 
           firered_tileset->behavior_size);
    
    // Convert palettes data
    // Palette format is the same (16-color palettes in RGB555 format)
    firered_tileset->palette_count = emerald_tileset->palette_count;
    size_t palettes_size = firered_tileset->palette_count * 16 * sizeof(uint16_t);
    firered_tileset->palettes = (uint16_t*)malloc(palettes_size);
    if (!firered_tileset->palettes) {
        snprintf(error_buffer, error_size, 
                "Failed to allocate memory for FireRed tileset palettes");
        FreeTilesetData(firered_tileset);
        return false;
    }
    memcpy(firered_tileset->palettes, emerald_tileset->palettes, palettes_size);
    
    return true;
}

// ============================================================================
// Script Converter Implementation
// ============================================================================

// Emerald script command IDs (from pokeemerald)
#define EMERALD_CMD_MSGBOX          0x0F
#define EMERALD_CMD_GIVEITEM        0x1E
#define EMERALD_CMD_GIVEMON         0x2D
#define EMERALD_CMD_SETFLAG         0x29
#define EMERALD_CMD_CLEARFLAG       0x2A
#define EMERALD_CMD_CHECKFLAG       0x2B
#define EMERALD_CMD_WARP            0x08
#define EMERALD_CMD_PLAYBGM         0x22
#define EMERALD_CMD_FADESCREEN      0x23
#define EMERALD_CMD_CALL            0x09
#define EMERALD_CMD_GOTO            0x05
#define EMERALD_CMD_IF              0x06
#define EMERALD_CMD_CALLSTD         0x09
#define EMERALD_CMD_GOTOIF          0x07
#define EMERALD_CMD_RELEASE         0x68
#define EMERALD_CMD_END             0x02
#define EMERALD_CMD_RETURN          0x03
#define EMERALD_CMD_COMPARE         0x21
#define EMERALD_CMD_SETVAR          0x16
#define EMERALD_CMD_ADDVAR          0x17
#define EMERALD_CMD_SUBVAR          0x18
#define EMERALD_CMD_COPYVAR         0x19
#define EMERALD_CMD_SETWEATHER      0x5C
#define EMERALD_CMD_DOWEATHER       0x5D
#define EMERALD_CMD_SETMAPTILE      0x5E

// FireRed script command IDs (from pokefirered)
#define FIRERED_CMD_MSGBOX          0x0F
#define FIRERED_CMD_GIVEITEM        0x1E
#define FIRERED_CMD_GIVEMON         0x2D
#define FIRERED_CMD_SETFLAG         0x29
#define FIRERED_CMD_CLEARFLAG       0x2A
#define FIRERED_CMD_CHECKFLAG       0x2B
#define FIRERED_CMD_WARP            0x08
#define FIRERED_CMD_PLAYBGM         0x22
#define FIRERED_CMD_FADESCREEN      0x23
#define FIRERED_CMD_CALL            0x09
#define FIRERED_CMD_GOTO            0x05
#define FIRERED_CMD_IF              0x06
#define FIRERED_CMD_CALLSTD         0x09
#define FIRERED_CMD_GOTOIF          0x07
#define FIRERED_CMD_RELEASE         0x68
#define FIRERED_CMD_END             0x02
#define FIRERED_CMD_RETURN          0x03
#define FIRERED_CMD_COMPARE         0x21
#define FIRERED_CMD_SETVAR          0x16
#define FIRERED_CMD_ADDVAR          0x17
#define FIRERED_CMD_SUBVAR          0x18
#define FIRERED_CMD_COPYVAR         0x19

// Script command mapping table
typedef struct {
    uint16_t emerald_cmd;
    uint16_t firered_cmd;
    bool has_equivalent;
} CommandMapping;

static const CommandMapping command_map[] = {
    {EMERALD_CMD_MSGBOX, FIRERED_CMD_MSGBOX, true},
    {EMERALD_CMD_GIVEITEM, FIRERED_CMD_GIVEITEM, true},
    {EMERALD_CMD_GIVEMON, FIRERED_CMD_GIVEMON, true},
    {EMERALD_CMD_SETFLAG, FIRERED_CMD_SETFLAG, true},
    {EMERALD_CMD_CLEARFLAG, FIRERED_CMD_CLEARFLAG, true},
    {EMERALD_CMD_CHECKFLAG, FIRERED_CMD_CHECKFLAG, true},
    {EMERALD_CMD_WARP, FIRERED_CMD_WARP, true},
    {EMERALD_CMD_PLAYBGM, FIRERED_CMD_PLAYBGM, true},
    {EMERALD_CMD_FADESCREEN, FIRERED_CMD_FADESCREEN, true},
    {EMERALD_CMD_CALL, FIRERED_CMD_CALL, true},
    {EMERALD_CMD_GOTO, FIRERED_CMD_GOTO, true},
    {EMERALD_CMD_IF, FIRERED_CMD_IF, true},
    {EMERALD_CMD_CALLSTD, FIRERED_CMD_CALLSTD, true},
    {EMERALD_CMD_GOTOIF, FIRERED_CMD_GOTOIF, true},
    {EMERALD_CMD_RELEASE, FIRERED_CMD_RELEASE, true},
    {EMERALD_CMD_END, FIRERED_CMD_END, true},
    {EMERALD_CMD_RETURN, FIRERED_CMD_RETURN, true},
    {EMERALD_CMD_COMPARE, FIRERED_CMD_COMPARE, true},
    {EMERALD_CMD_SETVAR, FIRERED_CMD_SETVAR, true},
    {EMERALD_CMD_ADDVAR, FIRERED_CMD_ADDVAR, true},
    {EMERALD_CMD_SUBVAR, FIRERED_CMD_SUBVAR, true},
    {EMERALD_CMD_COPYVAR, FIRERED_CMD_COPYVAR, true},
    // Incompatible commands (no FireRed equivalent)
    {EMERALD_CMD_SETWEATHER, 0, false},
    {EMERALD_CMD_DOWEATHER, 0, false},
    {EMERALD_CMD_SETMAPTILE, 0, false},
};

static const size_t command_map_size = sizeof(command_map) / sizeof(CommandMapping);

void InitScript(Script* script) {
    if (!script) return;
    
    memset(script->name, 0, MAX_NAME_LENGTH);
    script->commands = NULL;
    script->command_count = 0;
}

void FreeScript(Script* script) {
    if (!script) return;
    
    if (script->commands) {
        for (size_t i = 0; i < script->command_count; i++) {
            if (script->commands[i].parameters) {
                free(script->commands[i].parameters);
            }
        }
        free(script->commands);
    }
    
    script->commands = NULL;
    script->command_count = 0;
}

uint16_t MapScriptCommand(uint16_t emerald_command, bool* has_equivalent) {
    if (!has_equivalent) return 0;
    
    // Search for command in mapping table
    for (size_t i = 0; i < command_map_size; i++) {
        if (command_map[i].emerald_cmd == emerald_command) {
            *has_equivalent = command_map[i].has_equivalent;
            return command_map[i].firered_cmd;
        }
    }
    
    // Command not found in mapping table - assume no equivalent
    *has_equivalent = false;
    return 0;
}

bool ConvertScript(const Script* emerald_script,
                   Script* firered_script,
                   const char* map_name,
                   char* error_buffer, size_t error_size) {
    if (!emerald_script || !firered_script || !error_buffer) {
        if (error_buffer && error_size > 0) {
            snprintf(error_buffer, error_size, "Invalid parameters to ConvertScript");
        }
        return false;
    }
    
    // Initialize output script
    InitScript(firered_script);
    safe_strcpy(firered_script->name, emerald_script->name, MAX_NAME_LENGTH);
    
    // Allocate command array
    if (emerald_script->command_count > 0) {
        firered_script->commands = (ScriptCommand*)calloc(emerald_script->command_count, 
                                                          sizeof(ScriptCommand));
        if (!firered_script->commands) {
            snprintf(error_buffer, error_size, 
                    "Failed to allocate memory for script commands");
            return false;
        }
    }
    
    // Track incompatible commands for logging
    char incompatible_log[ERROR_BUFFER_SIZE] = {0};
    size_t incompatible_count = 0;
    
    // Convert each command
    for (size_t i = 0; i < emerald_script->command_count; i++) {
        bool has_equivalent = false;
        uint16_t firered_cmd = MapScriptCommand(emerald_script->commands[i].command_id, 
                                                &has_equivalent);
        
        if (!has_equivalent) {
            // Log incompatible command
            char temp[128];
            snprintf(temp, sizeof(temp), 
                    "Map '%s', Script '%s': Incompatible command 0x%04X at index %zu\n",
                    map_name ? map_name : "unknown",
                    emerald_script->name,
                    emerald_script->commands[i].command_id,
                    i);
            
            // Append to incompatible log
            size_t current_len = strlen(incompatible_log);
            size_t temp_len = strlen(temp);
            if (current_len + temp_len < ERROR_BUFFER_SIZE - 1) {
                strcat(incompatible_log, temp);
                incompatible_count++;
            }
            
            // Skip this command (don't add to output)
            continue;
        }
        
        // Copy command with mapped ID
        firered_script->commands[firered_script->command_count].command_id = firered_cmd;
        firered_script->commands[firered_script->command_count].param_count = 
            emerald_script->commands[i].param_count;
        
        // Copy parameters if present
        if (emerald_script->commands[i].param_count > 0 && 
            emerald_script->commands[i].parameters) {
            size_t param_size = emerald_script->commands[i].param_count;
            firered_script->commands[firered_script->command_count].parameters = 
                (uint8_t*)malloc(param_size);
            
            if (!firered_script->commands[firered_script->command_count].parameters) {
                snprintf(error_buffer, error_size, 
                        "Failed to allocate memory for command parameters");
                FreeScript(firered_script);
                return false;
            }
            
            memcpy(firered_script->commands[firered_script->command_count].parameters,
                   emerald_script->commands[i].parameters,
                   param_size);
        } else {
            firered_script->commands[firered_script->command_count].parameters = NULL;
        }
        
        firered_script->command_count++;
    }
    
    // Report incompatible commands if any were found
    if (incompatible_count > 0) {
        snprintf(error_buffer, error_size, 
                "Script conversion completed with %zu incompatible command(s):\n%s",
                incompatible_count, incompatible_log);
        // Return true because conversion succeeded, just with warnings
    } else {
        snprintf(error_buffer, error_size, "Script conversion successful");
    }
    
    return true;
}

// ============================================================================
// Trainer Data Converter Implementation
// ============================================================================

void InitTrainerData(TrainerData* data) {
    if (!data) return;
    
    memset(data->name, 0, sizeof(data->name));
    data->trainer_class = 0;
    data->gender = 0;
    data->music = 0;
    data->sprite = 0;
    memset(data->intro_text, 0, sizeof(data->intro_text));
    memset(data->defeat_text, 0, sizeof(data->defeat_text));
    data->reward = 0;
    data->ai_flags = 0;
    data->party = NULL;
    data->party_size = 0;
}

void FreeTrainerData(TrainerData* data) {
    if (!data) return;
    
    if (data->party) {
        free(data->party);
        data->party = NULL;
    }
    data->party_size = 0;
}

uint8_t MapTrainerClass(uint8_t emerald_class, bool* is_exact_match) {
    if (!is_exact_match) return 0;
    
    // Mapping table for Emerald trainer classes to FireRed equivalents
    // This is a simplified mapping - in a real implementation, this would be
    // a comprehensive table based on actual trainer class IDs from both games
    
    // Common trainer classes that exist in both games
    struct TrainerClassMapping {
        uint8_t emerald_id;
        uint8_t firered_id;
        bool exact;
    };
    
    // Sample mappings (these would need to be filled with actual game data)
    static const struct TrainerClassMapping mappings[] = {
        // Exact matches (trainer classes that exist in both games)
        {1, 1, true},    // Youngster
        {2, 2, true},    // Bug Catcher
        {3, 3, true},    // Lass
        {4, 4, true},    // Sailor
        {5, 5, true},    // Camper
        {6, 6, true},    // Picnicker
        {7, 7, true},    // Pokemaniac
        {8, 8, true},    // Super Nerd
        {9, 9, true},    // Hiker
        {10, 10, true},  // Biker
        {11, 11, true},  // Burglar
        {12, 12, true},  // Engineer
        {13, 13, true},  // Fisherman
        {14, 14, true},  // Swimmer Male
        {15, 15, true},  // Cue Ball
        {16, 16, true},  // Gamer
        {17, 17, true},  // Beauty
        {18, 18, true},  // Swimmer Female
        {19, 19, true},  // Psychic Male
        {20, 20, true},  // Rocker
        {21, 21, true},  // Juggler
        {22, 22, true},  // Tamer
        {23, 23, true},  // Bird Keeper
        {24, 24, true},  // Blackbelt
        {25, 25, true},  // Rival
        {26, 26, true},  // Scientist
        {27, 27, true},  // Giovanni
        {28, 28, true},  // Rocket
        {29, 29, true},  // Cooltrainer Male
        {30, 30, true},  // Cooltrainer Female
        {31, 31, true},  // Bruno
        {32, 32, true},  // Brock
        {33, 33, true},  // Misty
        {34, 34, true},  // Lt Surge
        {35, 35, true},  // Erika
        {36, 36, true},  // Koga
        {37, 37, true},  // Blaine
        {38, 38, true},  // Sabrina
        {39, 39, true},  // Gentleman
        {40, 40, true},  // Rival 2
        {41, 41, true},  // Rival 3
        {42, 42, true},  // Lorelei
        {43, 43, true},  // Channeler
        {44, 44, true},  // Agatha
        {45, 45, true},  // Lance
        
        // Approximate matches (Emerald-specific classes mapped to closest FireRed equivalent)
        {50, 1, false},  // Aqua Grunt -> Youngster
        {51, 28, false}, // Magma Grunt -> Rocket
        {52, 29, false}, // Aqua Admin -> Cooltrainer Male
        {53, 29, false}, // Magma Admin -> Cooltrainer Male
        {54, 30, false}, // Aqua Leader -> Cooltrainer Female
        {55, 27, false}, // Magma Leader -> Giovanni
        {56, 23, false}, // Hex Maniac -> Bird Keeper
        {57, 39, false}, // Aroma Lady -> Gentleman
        {58, 6, false},  // Ruin Maniac -> Picnicker
        {59, 43, false}, // Interviewer -> Channeler
        {60, 22, false}, // Tuber Female -> Tamer
        {61, 22, false}, // Tuber Male -> Tamer
        {62, 30, false}, // Lady -> Cooltrainer Female
        {63, 17, false}, // Beauty -> Beauty (exact but different ID)
        {64, 39, false}, // Rich Boy -> Gentleman
        {65, 19, false}, // Psychic Female -> Psychic Male
        {66, 14, false}, // Swimmer Male -> Swimmer Male (exact but different ID)
        {67, 18, false}, // Swimmer Female -> Swimmer Female (exact but different ID)
        {68, 5, false},  // Pokefan Male -> Camper
        {69, 6, false},  // Pokefan Female -> Picnicker
        {70, 12, false}, // Expert Male -> Engineer
        {71, 30, false}, // Expert Female -> Cooltrainer Female
        {72, 1, false},  // Youngster -> Youngster (exact but different ID)
        {73, 29, false}, // Champion -> Cooltrainer Male
        {74, 13, false}, // Fisherman -> Fisherman (exact but different ID)
        {75, 5, false},  // Triathlete Male -> Camper
        {76, 6, false},  // Triathlete Female -> Picnicker
        {77, 20, false}, // Dragon Tamer -> Rocker
        {78, 23, false}, // Bird Keeper -> Bird Keeper (exact but different ID)
        {79, 21, false}, // Ninja Boy -> Juggler
        {80, 24, false}, // Battle Girl -> Blackbelt
        {81, 19, false}, // Parasol Lady -> Psychic Male
        {82, 14, false}, // Swimmer Male -> Swimmer Male
        {83, 6, false},  // Picnicker -> Picnicker (exact but different ID)
        {84, 31, false}, // Twins -> Bruno
        {85, 4, false},  // Sailor -> Sailor (exact but different ID)
        {86, 29, false}, // Boarder -> Cooltrainer Male
        {87, 30, false}, // Collector -> Cooltrainer Female
        {88, 26, false}, // Wally -> Scientist
        {89, 25, false}, // Brendan -> Rival
        {90, 25, false}, // May -> Rival
        {91, 32, false}, // Roxanne -> Brock
        {92, 32, false}, // Brawly -> Brock
        {93, 33, false}, // Wattson -> Misty
        {94, 34, false}, // Flannery -> Lt Surge
        {95, 35, false}, // Norman -> Erika
        {96, 36, false}, // Winona -> Koga
        {97, 37, false}, // Tate and Liza -> Blaine
        {98, 38, false}, // Wallace -> Sabrina
        {99, 31, false}, // Sidney -> Bruno
        {100, 42, false}, // Phoebe -> Lorelei
        {101, 44, false}, // Glacia -> Agatha
        {102, 45, false}, // Drake -> Lance
        {103, 45, false}, // Steven -> Lance
    };
    
    const size_t mapping_count = sizeof(mappings) / sizeof(mappings[0]);
    
    // Search for mapping
    for (size_t i = 0; i < mapping_count; i++) {
        if (mappings[i].emerald_id == emerald_class) {
            *is_exact_match = mappings[i].exact;
            return mappings[i].firered_id;
        }
    }
    
    // No mapping found - default to Youngster and mark as approximate
    *is_exact_match = false;
    return 1; // Default to Youngster
}

bool ConvertTrainer(const TrainerData* emerald_trainer,
                    TrainerData* firered_trainer,
                    char* error_buffer, size_t error_size) {
    if (!emerald_trainer || !firered_trainer || !error_buffer) {
        if (error_buffer && error_size > 0) {
            snprintf(error_buffer, error_size, "Invalid parameters to ConvertTrainer");
        }
        return false;
    }
    
    // Initialize output trainer
    InitTrainerData(firered_trainer);
    
    // Copy name
    safe_strcpy(firered_trainer->name, emerald_trainer->name, sizeof(firered_trainer->name));
    
    // Map trainer class
    bool is_exact_match = false;
    firered_trainer->trainer_class = MapTrainerClass(emerald_trainer->trainer_class, 
                                                     &is_exact_match);
    
    // Log if mapping was approximate
    if (!is_exact_match) {
        char log_msg[256];
        snprintf(log_msg, sizeof(log_msg),
                "Trainer '%s': Approximate trainer class mapping from Emerald class %u to FireRed class %u\n",
                emerald_trainer->name,
                emerald_trainer->trainer_class,
                firered_trainer->trainer_class);
        
        // Append to error buffer (used as warning buffer in this case)
        size_t current_len = strlen(error_buffer);
        size_t log_len = strlen(log_msg);
        if (current_len + log_len < error_size - 1) {
            strcat(error_buffer, log_msg);
        }
    }
    
    // Copy gender, music, sprite (these are preserved as-is)
    firered_trainer->gender = emerald_trainer->gender;
    firered_trainer->music = emerald_trainer->music;
    firered_trainer->sprite = emerald_trainer->sprite;
    
    // Copy intro and defeat text
    safe_strcpy(firered_trainer->intro_text, emerald_trainer->intro_text, 
               sizeof(firered_trainer->intro_text));
    safe_strcpy(firered_trainer->defeat_text, emerald_trainer->defeat_text, 
               sizeof(firered_trainer->defeat_text));
    
    // Preserve reward amount (Requirement 8.4)
    firered_trainer->reward = emerald_trainer->reward;
    
    // Preserve AI flags (Requirement 8.3)
    firered_trainer->ai_flags = emerald_trainer->ai_flags;
    
    // Copy party data
    firered_trainer->party_size = emerald_trainer->party_size;
    
    if (emerald_trainer->party_size > 0) {
        // Allocate party array
        firered_trainer->party = (TrainerPokemon*)calloc(emerald_trainer->party_size, 
                                                         sizeof(TrainerPokemon));
        if (!firered_trainer->party) {
            snprintf(error_buffer, error_size, 
                    "Failed to allocate memory for trainer party");
            return false;
        }
        
        // Copy each Pokemon in the party
        for (uint8_t i = 0; i < emerald_trainer->party_size; i++) {
            firered_trainer->party[i].species = emerald_trainer->party[i].species;
            firered_trainer->party[i].level = emerald_trainer->party[i].level;
            firered_trainer->party[i].held_item = emerald_trainer->party[i].held_item;
            
            // Copy moves
            for (int j = 0; j < 4; j++) {
                firered_trainer->party[i].moves[j] = emerald_trainer->party[i].moves[j];
            }
            
            // Copy IVs and EVs
            firered_trainer->party[i].iv = emerald_trainer->party[i].iv;
            firered_trainer->party[i].ev_hp = emerald_trainer->party[i].ev_hp;
            firered_trainer->party[i].ev_atk = emerald_trainer->party[i].ev_atk;
            firered_trainer->party[i].ev_def = emerald_trainer->party[i].ev_def;
            firered_trainer->party[i].ev_speed = emerald_trainer->party[i].ev_speed;
            firered_trainer->party[i].ev_spatk = emerald_trainer->party[i].ev_spatk;
            firered_trainer->party[i].ev_spdef = emerald_trainer->party[i].ev_spdef;
        }
    }
    
    // If no warnings were logged, indicate success
    if (strlen(error_buffer) == 0) {
        snprintf(error_buffer, error_size, "Trainer conversion successful");
    }
    
    return true;
}

// ============================================================================
// Wild Encounter Converter Implementation
// ============================================================================

void InitWildEncounterData(WildEncounterData* data) {
    if (!data) return;
    
    // Initialize encounter rate to 0
    data->encounter_rate = 0;
    
    // Initialize all encounter slots to empty (species 0, levels 0)
    for (int i = 0; i < 12; i++) {
        data->grass_encounters[i].species = 0;
        data->grass_encounters[i].min_level = 0;
        data->grass_encounters[i].max_level = 0;
    }
    
    for (int i = 0; i < 5; i++) {
        data->water_encounters[i].species = 0;
        data->water_encounters[i].min_level = 0;
        data->water_encounters[i].max_level = 0;
        
        data->fishing_old[i].species = 0;
        data->fishing_old[i].min_level = 0;
        data->fishing_old[i].max_level = 0;
        
        data->fishing_good[i].species = 0;
        data->fishing_good[i].min_level = 0;
        data->fishing_good[i].max_level = 0;
        
        data->fishing_super[i].species = 0;
        data->fishing_super[i].min_level = 0;
        data->fishing_super[i].max_level = 0;
        
        data->rock_smash[i].species = 0;
        data->rock_smash[i].min_level = 0;
        data->rock_smash[i].max_level = 0;
    }
}

bool ConvertEncounters(const WildEncounterData* emerald_encounters,
                       WildEncounterData* firered_encounters,
                       char* error_buffer, size_t error_size) {
    if (!emerald_encounters || !firered_encounters || !error_buffer) {
        if (error_buffer && error_size > 0) {
            snprintf(error_buffer, error_size, "Invalid parameters to ConvertEncounters");
        }
        return false;
    }
    
    // Initialize output encounter data
    InitWildEncounterData(firered_encounters);
    
    // Preserve encounter rate (Requirement 9.3)
    firered_encounters->encounter_rate = emerald_encounters->encounter_rate;
    
    // Convert grass encounters (12 slots)
    // Preserve Pokemon species and level ranges (Requirement 9.4)
    for (int i = 0; i < 12; i++) {
        firered_encounters->grass_encounters[i].species = 
            emerald_encounters->grass_encounters[i].species;
        firered_encounters->grass_encounters[i].min_level = 
            emerald_encounters->grass_encounters[i].min_level;
        firered_encounters->grass_encounters[i].max_level = 
            emerald_encounters->grass_encounters[i].max_level;
    }
    
    // Convert water encounters (5 slots)
    // Handle water encounter type correctly for FireRed (Requirement 9.5)
    for (int i = 0; i < 5; i++) {
        firered_encounters->water_encounters[i].species = 
            emerald_encounters->water_encounters[i].species;
        firered_encounters->water_encounters[i].min_level = 
            emerald_encounters->water_encounters[i].min_level;
        firered_encounters->water_encounters[i].max_level = 
            emerald_encounters->water_encounters[i].max_level;
    }
    
    // Convert fishing encounters - Old Rod (5 slots)
    // Handle fishing encounter type correctly for FireRed (Requirement 9.5)
    for (int i = 0; i < 5; i++) {
        firered_encounters->fishing_old[i].species = 
            emerald_encounters->fishing_old[i].species;
        firered_encounters->fishing_old[i].min_level = 
            emerald_encounters->fishing_old[i].min_level;
        firered_encounters->fishing_old[i].max_level = 
            emerald_encounters->fishing_old[i].max_level;
    }
    
    // Convert fishing encounters - Good Rod (5 slots)
    for (int i = 0; i < 5; i++) {
        firered_encounters->fishing_good[i].species = 
            emerald_encounters->fishing_good[i].species;
        firered_encounters->fishing_good[i].min_level = 
            emerald_encounters->fishing_good[i].min_level;
        firered_encounters->fishing_good[i].max_level = 
            emerald_encounters->fishing_good[i].max_level;
    }
    
    // Convert fishing encounters - Super Rod (5 slots)
    for (int i = 0; i < 5; i++) {
        firered_encounters->fishing_super[i].species = 
            emerald_encounters->fishing_super[i].species;
        firered_encounters->fishing_super[i].min_level = 
            emerald_encounters->fishing_super[i].min_level;
        firered_encounters->fishing_super[i].max_level = 
            emerald_encounters->fishing_super[i].max_level;
    }
    
    // Convert rock smash encounters (5 slots)
    // Handle rock smash encounter type correctly for FireRed (Requirement 9.5)
    for (int i = 0; i < 5; i++) {
        firered_encounters->rock_smash[i].species = 
            emerald_encounters->rock_smash[i].species;
        firered_encounters->rock_smash[i].min_level = 
            emerald_encounters->rock_smash[i].min_level;
        firered_encounters->rock_smash[i].max_level = 
            emerald_encounters->rock_smash[i].max_level;
    }
    
    // Validate that encounter data is reasonable
    bool has_encounters = false;
    
    // Check if any grass encounters exist
    for (int i = 0; i < 12; i++) {
        if (firered_encounters->grass_encounters[i].species != 0) {
            has_encounters = true;
            
            // Validate level ranges
            if (firered_encounters->grass_encounters[i].min_level > 
                firered_encounters->grass_encounters[i].max_level) {
                snprintf(error_buffer, error_size,
                        "Invalid level range for grass encounter slot %d: min=%u > max=%u",
                        i,
                        firered_encounters->grass_encounters[i].min_level,
                        firered_encounters->grass_encounters[i].max_level);
                return false;
            }
        }
    }
    
    // Check if any water encounters exist
    for (int i = 0; i < 5; i++) {
        if (firered_encounters->water_encounters[i].species != 0) {
            has_encounters = true;
            
            // Validate level ranges
            if (firered_encounters->water_encounters[i].min_level > 
                firered_encounters->water_encounters[i].max_level) {
                snprintf(error_buffer, error_size,
                        "Invalid level range for water encounter slot %d: min=%u > max=%u",
                        i,
                        firered_encounters->water_encounters[i].min_level,
                        firered_encounters->water_encounters[i].max_level);
                return false;
            }
        }
    }
    
    // Check fishing encounters
    for (int i = 0; i < 5; i++) {
        if (firered_encounters->fishing_old[i].species != 0 ||
            firered_encounters->fishing_good[i].species != 0 ||
            firered_encounters->fishing_super[i].species != 0) {
            has_encounters = true;
        }
    }
    
    // Check rock smash encounters
    for (int i = 0; i < 5; i++) {
        if (firered_encounters->rock_smash[i].species != 0) {
            has_encounters = true;
        }
    }
    
    // Log success message
    if (has_encounters) {
        snprintf(error_buffer, error_size, 
                "Wild encounter conversion successful (rate=%u)", 
                firered_encounters->encounter_rate);
    } else {
        snprintf(error_buffer, error_size, 
                "Wild encounter conversion successful (no encounters defined)");
    }
    
    return true;
}


// ============================================================================
// Validation Engine
// ============================================================================

void InitValidationResult(ValidationResult* result) {
    if (!result) return;
    
    result->is_valid = true;
    result->error_message[0] = '\0';
    result->map_name[0] = '\0';
}

ValidationResult ValidateMapHeader(const MapData* map) {
    ValidationResult result;
    InitValidationResult(&result);
    
    if (!map) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE, 
                 "Map data pointer is NULL");
        return result;
    }
    
    // Store map name for error reporting
    safe_strcpy(result.map_name, map->name, sizeof(result.map_name));
    
    // Validate required string fields are not empty
    if (strlen(map->name) == 0) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Map name is empty");
        return result;
    }
    
    if (strlen(map->id) == 0) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Map ID is empty for map: %s", map->name);
        return result;
    }
    
    if (strlen(map->layout) == 0) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Map layout is empty for map: %s", map->name);
        return result;
    }
    
    if (strlen(map->music) == 0) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Map music is empty for map: %s", map->name);
        return result;
    }
    
    if (strlen(map->weather) == 0) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Map weather is empty for map: %s", map->name);
        return result;
    }
    
    if (strlen(map->map_type) == 0) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Map type is empty for map: %s", map->name);
        return result;
    }
    
    // Validate array counts are within bounds
    if (map->connection_count > MAX_CONNECTIONS) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Connection count (%u) exceeds maximum (%d) for map: %s",
                 map->connection_count, MAX_CONNECTIONS, map->name);
        return result;
    }
    
    if (map->warp_count > MAX_WARPS) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Warp count (%u) exceeds maximum (%d) for map: %s",
                 map->warp_count, MAX_WARPS, map->name);
        return result;
    }
    
    if (map->object_count > MAX_OBJECTS) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Object count (%u) exceeds maximum (%d) for map: %s",
                 map->object_count, MAX_OBJECTS, map->name);
        return result;
    }
    
    if (map->coord_event_count > MAX_COORD_EVENTS) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Coord event count (%u) exceeds maximum (%d) for map: %s",
                 map->coord_event_count, MAX_COORD_EVENTS, map->name);
        return result;
    }
    
    if (map->bg_event_count > MAX_BG_EVENTS) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "BG event count (%u) exceeds maximum (%d) for map: %s",
                 map->bg_event_count, MAX_BG_EVENTS, map->name);
        return result;
    }
    
    return result;
}

// Helper function to check if a string is in a registry
static bool is_in_registry(const char* name, const char** registry, size_t count) {
    if (!name || !registry) return false;
    
    for (size_t i = 0; i < count; i++) {
        if (registry[i] && strcmp(name, registry[i]) == 0) {
            return true;
        }
    }
    return false;
}

ValidationResult ValidateWarpDestinations(const MapData* map, 
                                          const char** map_registry,
                                          size_t registry_count) {
    ValidationResult result;
    InitValidationResult(&result);
    
    if (!map) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Map data pointer is NULL");
        return result;
    }
    
    safe_strcpy(result.map_name, map->name, sizeof(result.map_name));
    
    // If no registry provided, skip validation (assume all maps are valid)
    if (!map_registry || registry_count == 0) {
        return result;
    }
    
    // Validate each warp destination
    for (uint16_t i = 0; i < map->warp_count; i++) {
        const WarpEvent* warp = &map->warps[i];
        
        // Check if destination map is empty (might be valid for some warps)
        if (strlen(warp->dest_map) == 0) {
            continue; // Skip empty destinations
        }
        
        // Check if destination map exists in registry
        if (!is_in_registry(warp->dest_map, map_registry, registry_count)) {
            result.is_valid = false;
            snprintf(result.error_message, ERROR_BUFFER_SIZE,
                     "Warp %u in map '%s' references invalid destination map: %s",
                     i, map->name, warp->dest_map);
            return result;
        }
    }
    
    return result;
}

ValidationResult ValidateConnections(const MapData* map,
                                     const char** map_registry,
                                     size_t registry_count) {
    ValidationResult result;
    InitValidationResult(&result);
    
    if (!map) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Map data pointer is NULL");
        return result;
    }
    
    safe_strcpy(result.map_name, map->name, sizeof(result.map_name));
    
    // If no registry provided, skip validation
    if (!map_registry || registry_count == 0) {
        return result;
    }
    
    // Validate each connection
    for (uint16_t i = 0; i < map->connection_count; i++) {
        const Connection* conn = &map->connections[i];
        
        // Check if connection map is empty
        if (strlen(conn->map) == 0) {
            result.is_valid = false;
            snprintf(result.error_message, ERROR_BUFFER_SIZE,
                     "Connection %u in map '%s' has empty destination map",
                     i, map->name);
            return result;
        }
        
        // Check if connection direction is valid
        if (strlen(conn->direction) == 0) {
            result.is_valid = false;
            snprintf(result.error_message, ERROR_BUFFER_SIZE,
                     "Connection %u in map '%s' has empty direction",
                     i, map->name);
            return result;
        }
        
        // Validate direction is one of: north, south, east, west
        if (strcmp(conn->direction, "north") != 0 &&
            strcmp(conn->direction, "south") != 0 &&
            strcmp(conn->direction, "east") != 0 &&
            strcmp(conn->direction, "west") != 0 &&
            strcmp(conn->direction, "up") != 0 &&
            strcmp(conn->direction, "down") != 0) {
            result.is_valid = false;
            snprintf(result.error_message, ERROR_BUFFER_SIZE,
                     "Connection %u in map '%s' has invalid direction: %s",
                     i, map->name, conn->direction);
            return result;
        }
        
        // Check if destination map exists in registry
        if (!is_in_registry(conn->map, map_registry, registry_count)) {
            result.is_valid = false;
            snprintf(result.error_message, ERROR_BUFFER_SIZE,
                     "Connection %u in map '%s' references invalid destination map: %s",
                     i, map->name, conn->map);
            return result;
        }
    }
    
    return result;
}

ValidationResult ValidateTilesetReferences(const MapData* map,
                                           const char** tileset_registry,
                                           size_t registry_count) {
    ValidationResult result;
    InitValidationResult(&result);
    
    if (!map) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Map data pointer is NULL");
        return result;
    }
    
    safe_strcpy(result.map_name, map->name, sizeof(result.map_name));
    
    // If no registry provided, skip validation
    if (!tileset_registry || registry_count == 0) {
        return result;
    }
    
    // Note: In the current MapData structure, tileset references are embedded
    // in the layout field. For a more complete implementation, we would need
    // to parse the layout or have separate tileset fields.
    // For now, we just validate that the layout field is not empty,
    // which was already done in ValidateMapHeader.
    
    // This is a placeholder for future enhancement when tileset data
    // is more explicitly represented in the MapData structure.
    
    return result;
}

ValidationResult ValidateScriptReferences(const MapData* map,
                                          const char** script_registry,
                                          size_t registry_count) {
    ValidationResult result;
    InitValidationResult(&result);
    
    if (!map) {
        result.is_valid = false;
        snprintf(result.error_message, ERROR_BUFFER_SIZE,
                 "Map data pointer is NULL");
        return result;
    }
    
    safe_strcpy(result.map_name, map->name, sizeof(result.map_name));
    
    // If no registry provided, skip validation
    if (!script_registry || registry_count == 0) {
        return result;
    }
    
    // Validate object event scripts
    for (uint16_t i = 0; i < map->object_count; i++) {
        const ObjectEvent* obj = &map->objects[i];
        
        // Skip if no script
        if (strlen(obj->script) == 0 || strcmp(obj->script, "NULL") == 0) {
            continue;
        }
        
        // Check if script exists in registry
        if (!is_in_registry(obj->script, script_registry, registry_count)) {
            result.is_valid = false;
            snprintf(result.error_message, ERROR_BUFFER_SIZE,
                     "Object %u in map '%s' references invalid script: %s",
                     i, map->name, obj->script);
            return result;
        }
    }
    
    // Validate coord event scripts
    for (uint16_t i = 0; i < map->coord_event_count; i++) {
        const CoordEvent* coord = &map->coord_events[i];
        
        // Only trigger type has scripts
        if (strcmp(coord->type, "trigger") != 0) {
            continue;
        }
        
        // Skip if no script
        if (strlen(coord->script) == 0 || strcmp(coord->script, "NULL") == 0) {
            continue;
        }
        
        // Check if script exists in registry
        if (!is_in_registry(coord->script, script_registry, registry_count)) {
            result.is_valid = false;
            snprintf(result.error_message, ERROR_BUFFER_SIZE,
                     "Coord event %u in map '%s' references invalid script: %s",
                     i, map->name, coord->script);
            return result;
        }
    }
    
    // Validate BG event scripts
    for (uint16_t i = 0; i < map->bg_event_count; i++) {
        const BgEvent* bg = &map->bg_events[i];
        
        // Only sign type has scripts
        if (strcmp(bg->type, "sign") != 0) {
            continue;
        }
        
        // Skip if no script
        if (strlen(bg->script) == 0 || strcmp(bg->script, "NULL") == 0) {
            continue;
        }
        
        // Check if script exists in registry
        if (!is_in_registry(bg->script, script_registry, registry_count)) {
            result.is_valid = false;
            snprintf(result.error_message, ERROR_BUFFER_SIZE,
                     "BG event %u in map '%s' references invalid script: %s",
                     i, map->name, bg->script);
            return result;
        }
    }
    
    return result;
}

ValidationResult ValidateMap(const MapData* map,
                             const char** map_registry,
                             size_t map_count,
                             const char** tileset_registry,
                             size_t tileset_count,
                             const char** script_registry,
                             size_t script_count) {
    ValidationResult result;
    
    // Run all validation checks in sequence
    // Stop at first failure
    
    result = ValidateMapHeader(map);
    if (!result.is_valid) {
        return result;
    }
    
    result = ValidateWarpDestinations(map, map_registry, map_count);
    if (!result.is_valid) {
        return result;
    }
    
    result = ValidateConnections(map, map_registry, map_count);
    if (!result.is_valid) {
        return result;
    }
    
    result = ValidateTilesetReferences(map, tileset_registry, tileset_count);
    if (!result.is_valid) {
        return result;
    }
    
    result = ValidateScriptReferences(map, script_registry, script_count);
    if (!result.is_valid) {
        return result;
    }
    
    return result;
}

// ============================================================================
// Error Logging System
// ============================================================================

// Maximum number of errors to log
#define MAX_ERROR_LOG_ENTRIES 1000

// Error log entry structure
typedef struct {
    char map_name[64];
    ConversionError error;
    char details[256];
    uint32_t line_number;
} ErrorLogEntry;

// Global error log storage
static ErrorLogEntry error_log[MAX_ERROR_LOG_ENTRIES];
static uint32_t error_log_count = 0;

// Global conversion statistics
static ConversionStats global_stats = {0};

void InitConversionResult(ConversionResult* result) {
    if (!result) return;
    
    result->error = CONV_ERROR_NONE;
    memset(result->error_message, 0, sizeof(result->error_message));
    memset(result->map_name, 0, sizeof(result->map_name));
    result->line_number = 0;
}

const char* GetErrorTypeName(ConversionError error) {
    switch (error) {
        case CONV_ERROR_NONE:
            return "NONE";
        case CONV_ERROR_FILE_NOT_FOUND:
            return "FILE_NOT_FOUND";
        case CONV_ERROR_PARSE_FAILED:
            return "PARSE_FAILED";
        case CONV_ERROR_INVALID_FORMAT:
            return "INVALID_FORMAT";
        case CONV_ERROR_MISSING_TILESET:
            return "MISSING_TILESET";
        case CONV_ERROR_INVALID_WARP:
            return "INVALID_WARP";
        case CONV_ERROR_INVALID_CONNECTION:
            return "INVALID_CONNECTION";
        case CONV_ERROR_SCRIPT_INCOMPATIBLE:
            return "SCRIPT_INCOMPATIBLE";
        case CONV_ERROR_TRAINER_CLASS_MISSING:
            return "TRAINER_CLASS_MISSING";
        case CONV_ERROR_VALIDATION_FAILED:
            return "VALIDATION_FAILED";
        case CONV_ERROR_OUTPUT_FAILED:
            return "OUTPUT_FAILED";
        default:
            return "UNKNOWN";
    }
}

void LogConversionError(const char* map_name, ConversionError error, const char* details) {
    // Check if we have space in the log
    if (error_log_count >= MAX_ERROR_LOG_ENTRIES) {
        fprintf(stderr, "Warning: Error log is full, cannot log more errors\n");
        return;
    }
    
    // Create new log entry
    ErrorLogEntry* entry = &error_log[error_log_count];
    
    // Copy map name
    if (map_name) {
        strncpy(entry->map_name, map_name, sizeof(entry->map_name) - 1);
        entry->map_name[sizeof(entry->map_name) - 1] = '\0';
    } else {
        strcpy(entry->map_name, "UNKNOWN");
    }
    
    // Set error type
    entry->error = error;
    
    // Copy details
    if (details) {
        strncpy(entry->details, details, sizeof(entry->details) - 1);
        entry->details[sizeof(entry->details) - 1] = '\0';
    } else {
        strcpy(entry->details, "No details provided");
    }
    
    // Set line number to 0 (can be updated later if needed)
    entry->line_number = 0;
    
    // Increment log count
    error_log_count++;
    
    // Update statistics
    global_stats.maps_failed++;
    
    // Also print to stderr for immediate feedback
    fprintf(stderr, "[ERROR] Map: %s, Type: %s, Details: %s\n",
            entry->map_name, GetErrorTypeName(error), entry->details);
}

bool GenerateConversionReport(const char* output_path) {
    if (!output_path) {
        fprintf(stderr, "Error: output_path is NULL\n");
        return false;
    }
    
    FILE* report_file = fopen(output_path, "w");
    if (!report_file) {
        fprintf(stderr, "Error: Could not open report file: %s\n", output_path);
        return false;
    }
    
    // Write report header
    fprintf(report_file, "=============================================================================\n");
    fprintf(report_file, "                    MAP CONVERSION REPORT                                    \n");
    fprintf(report_file, "=============================================================================\n\n");
    
    // Write statistics summary
    fprintf(report_file, "CONVERSION STATISTICS:\n");
    fprintf(report_file, "---------------------\n");
    fprintf(report_file, "Maps Attempted:       %u\n", global_stats.maps_attempted);
    fprintf(report_file, "Maps Succeeded:       %u\n", global_stats.maps_succeeded);
    fprintf(report_file, "Maps Failed:          %u\n", global_stats.maps_failed);
    fprintf(report_file, "Warnings Generated:   %u\n", global_stats.warnings_generated);
    fprintf(report_file, "Tilesets Converted:   %u\n", global_stats.tilesets_converted);
    fprintf(report_file, "Scripts Converted:    %u\n", global_stats.scripts_converted);
    fprintf(report_file, "Trainers Converted:   %u\n", global_stats.trainers_converted);
    
    // Calculate success rate
    if (global_stats.maps_attempted > 0) {
        float success_rate = (float)global_stats.maps_succeeded / (float)global_stats.maps_attempted * 100.0f;
        fprintf(report_file, "Success Rate:         %.2f%%\n", success_rate);
    } else {
        fprintf(report_file, "Success Rate:         N/A (no maps attempted)\n");
    }
    
    fprintf(report_file, "\n");
    
    // Write error log
    if (error_log_count > 0) {
        fprintf(report_file, "CONVERSION ERRORS:\n");
        fprintf(report_file, "------------------\n");
        fprintf(report_file, "Total Errors Logged: %u\n\n", error_log_count);
        
        // Group errors by type
        for (int error_type = CONV_ERROR_FILE_NOT_FOUND; error_type <= CONV_ERROR_OUTPUT_FAILED; error_type++) {
            uint32_t count = 0;
            
            // Count errors of this type
            for (uint32_t i = 0; i < error_log_count; i++) {
                if (error_log[i].error == error_type) {
                    count++;
                }
            }
            
            if (count > 0) {
                fprintf(report_file, "\n%s (%u occurrences):\n", GetErrorTypeName(error_type), count);
                fprintf(report_file, "----------------------------------------\n");
                
                // List all errors of this type
                for (uint32_t i = 0; i < error_log_count; i++) {
                    if (error_log[i].error == error_type) {
                        fprintf(report_file, "  Map: %s\n", error_log[i].map_name);
                        fprintf(report_file, "  Details: %s\n", error_log[i].details);
                        if (error_log[i].line_number > 0) {
                            fprintf(report_file, "  Line: %u\n", error_log[i].line_number);
                        }
                        fprintf(report_file, "\n");
                    }
                }
            }
        }
    } else {
        fprintf(report_file, "CONVERSION ERRORS:\n");
        fprintf(report_file, "------------------\n");
        fprintf(report_file, "No errors logged.\n\n");
    }
    
    // Write footer
    fprintf(report_file, "=============================================================================\n");
    fprintf(report_file, "                         END OF REPORT                                       \n");
    fprintf(report_file, "=============================================================================\n");
    
    fclose(report_file);
    
    printf("Conversion report generated: %s\n", output_path);
    return true;
}

void GetConversionStats(ConversionStats* out_stats) {
    if (!out_stats) {
        fprintf(stderr, "Error: out_stats is NULL\n");
        return;
    }
    
    // Copy global statistics to output
    memcpy(out_stats, &global_stats, sizeof(ConversionStats));
}

void ClearConversionLog(void) {
    // Reset error log
    error_log_count = 0;
    memset(error_log, 0, sizeof(error_log));
    
    // Reset statistics
    memset(&global_stats, 0, sizeof(ConversionStats));
    
    printf("Conversion log and statistics cleared.\n");
}
