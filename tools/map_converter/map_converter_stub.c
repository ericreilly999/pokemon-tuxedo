// map_converter_stub.c
// Stub implementation for property testing (without json11 dependency)

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
    
    // Note: Full JSON parsing requires json11 C++ library integration
    // This is a stub that returns false to indicate parsing is not yet implemented
    snprintf(error_buffer, error_size, 
             "JSON parsing not yet implemented - requires json11 C++ integration");
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
