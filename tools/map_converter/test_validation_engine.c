// test_validation_engine.c
// Unit tests for validation engine
// Tests Requirements 15.1, 15.2, 15.3, 15.4, 15.5, 15.6

#include "map_converter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Test helper to create a valid map
static void create_valid_map(MapData* map) {
    InitMapData(map);
    strcpy(map->name, "TestMap");
    strcpy(map->id, "MAP_TEST");
    strcpy(map->layout, "LAYOUT_TEST");
    strcpy(map->music, "MUS_TEST");
    strcpy(map->weather, "WEATHER_SUNNY");
    strcpy(map->map_type, "MAP_TYPE_TOWN");
}

// Test 1: ValidateMapHeader with valid map
static void test_validate_map_header_valid() {
    printf("Test 1: ValidateMapHeader with valid map... ");
    
    MapData map;
    create_valid_map(&map);
    
    ValidationResult result = ValidateMapHeader(&map);
    
    assert(result.is_valid == true);
    assert(strlen(result.error_message) == 0);
    assert(strcmp(result.map_name, "TestMap") == 0);
    
    printf("PASSED\n");
}

// Test 2: ValidateMapHeader with NULL pointer
static void test_validate_map_header_null() {
    printf("Test 2: ValidateMapHeader with NULL pointer... ");
    
    ValidationResult result = ValidateMapHeader(NULL);
    
    assert(result.is_valid == false);
    assert(strlen(result.error_message) > 0);
    
    printf("PASSED\n");
}

// Test 3: ValidateMapHeader with empty name
static void test_validate_map_header_empty_name() {
    printf("Test 3: ValidateMapHeader with empty name... ");
    
    MapData map;
    create_valid_map(&map);
    map.name[0] = '\0';
    
    ValidationResult result = ValidateMapHeader(&map);
    
    assert(result.is_valid == false);
    assert(strstr(result.error_message, "name is empty") != NULL);
    
    printf("PASSED\n");
}

// Test 4: ValidateMapHeader with excessive warp count
static void test_validate_map_header_excessive_warps() {
    printf("Test 4: ValidateMapHeader with excessive warp count... ");
    
    MapData map;
    create_valid_map(&map);
    map.warp_count = MAX_WARPS + 1;
    
    ValidationResult result = ValidateMapHeader(&map);
    
    assert(result.is_valid == false);
    assert(strstr(result.error_message, "Warp count") != NULL);
    assert(strstr(result.error_message, "exceeds maximum") != NULL);
    
    printf("PASSED\n");
}

// Test 5: ValidateWarpDestinations with valid warps
static void test_validate_warp_destinations_valid() {
    printf("Test 5: ValidateWarpDestinations with valid warps... ");
    
    MapData map;
    create_valid_map(&map);
    
    // Add a warp
    map.warp_count = 1;
    map.warps[0].x = 5;
    map.warps[0].y = 5;
    map.warps[0].elevation = 0;
    map.warps[0].dest_warp_id = 0;
    strcpy(map.warps[0].dest_map, "MAP_DESTINATION");
    
    // Create map registry
    const char* map_registry[] = {"MAP_TEST", "MAP_DESTINATION"};
    size_t registry_count = 2;
    
    ValidationResult result = ValidateWarpDestinations(&map, map_registry, registry_count);
    
    assert(result.is_valid == true);
    assert(strlen(result.error_message) == 0);
    
    printf("PASSED\n");
}

// Test 6: ValidateWarpDestinations with invalid destination
static void test_validate_warp_destinations_invalid() {
    printf("Test 6: ValidateWarpDestinations with invalid destination... ");
    
    MapData map;
    create_valid_map(&map);
    
    // Add a warp with invalid destination
    map.warp_count = 1;
    map.warps[0].x = 5;
    map.warps[0].y = 5;
    map.warps[0].elevation = 0;
    map.warps[0].dest_warp_id = 0;
    strcpy(map.warps[0].dest_map, "MAP_NONEXISTENT");
    
    // Create map registry (doesn't include MAP_NONEXISTENT)
    const char* map_registry[] = {"MAP_TEST", "MAP_DESTINATION"};
    size_t registry_count = 2;
    
    ValidationResult result = ValidateWarpDestinations(&map, map_registry, registry_count);
    
    assert(result.is_valid == false);
    assert(strstr(result.error_message, "invalid destination map") != NULL);
    assert(strstr(result.error_message, "MAP_NONEXISTENT") != NULL);
    
    printf("PASSED\n");
}

// Test 7: ValidateConnections with valid connections
static void test_validate_connections_valid() {
    printf("Test 7: ValidateConnections with valid connections... ");
    
    MapData map;
    create_valid_map(&map);
    
    // Add a connection
    map.connection_count = 1;
    strcpy(map.connections[0].direction, "north");
    map.connections[0].offset = 0;
    strcpy(map.connections[0].map, "MAP_NORTH");
    
    // Create map registry
    const char* map_registry[] = {"MAP_TEST", "MAP_NORTH"};
    size_t registry_count = 2;
    
    ValidationResult result = ValidateConnections(&map, map_registry, registry_count);
    
    assert(result.is_valid == true);
    assert(strlen(result.error_message) == 0);
    
    printf("PASSED\n");
}

// Test 8: ValidateConnections with invalid direction
static void test_validate_connections_invalid_direction() {
    printf("Test 8: ValidateConnections with invalid direction... ");
    
    MapData map;
    create_valid_map(&map);
    
    // Add a connection with invalid direction
    map.connection_count = 1;
    strcpy(map.connections[0].direction, "diagonal");
    map.connections[0].offset = 0;
    strcpy(map.connections[0].map, "MAP_NORTH");
    
    // Create map registry
    const char* map_registry[] = {"MAP_TEST", "MAP_NORTH"};
    size_t registry_count = 2;
    
    ValidationResult result = ValidateConnections(&map, map_registry, registry_count);
    
    assert(result.is_valid == false);
    assert(strstr(result.error_message, "invalid direction") != NULL);
    
    printf("PASSED\n");
}

// Test 9: ValidateScriptReferences with valid scripts
static void test_validate_script_references_valid() {
    printf("Test 9: ValidateScriptReferences with valid scripts... ");
    
    MapData map;
    create_valid_map(&map);
    
    // Add an object with a script
    map.object_count = 1;
    strcpy(map.objects[0].type, "object");
    map.objects[0].graphics_id = 1;
    map.objects[0].x = 5;
    map.objects[0].y = 5;
    strcpy(map.objects[0].script, "TestScript");
    
    // Create script registry
    const char* script_registry[] = {"TestScript", "AnotherScript"};
    size_t registry_count = 2;
    
    ValidationResult result = ValidateScriptReferences(&map, script_registry, registry_count);
    
    assert(result.is_valid == true);
    assert(strlen(result.error_message) == 0);
    
    printf("PASSED\n");
}

// Test 10: ValidateScriptReferences with invalid script
static void test_validate_script_references_invalid() {
    printf("Test 10: ValidateScriptReferences with invalid script... ");
    
    MapData map;
    create_valid_map(&map);
    
    // Add an object with an invalid script
    map.object_count = 1;
    strcpy(map.objects[0].type, "object");
    map.objects[0].graphics_id = 1;
    map.objects[0].x = 5;
    map.objects[0].y = 5;
    strcpy(map.objects[0].script, "NonexistentScript");
    
    // Create script registry (doesn't include NonexistentScript)
    const char* script_registry[] = {"TestScript", "AnotherScript"};
    size_t registry_count = 2;
    
    ValidationResult result = ValidateScriptReferences(&map, script_registry, registry_count);
    
    assert(result.is_valid == false);
    assert(strstr(result.error_message, "invalid script") != NULL);
    assert(strstr(result.error_message, "NonexistentScript") != NULL);
    
    printf("PASSED\n");
}

// Test 11: ValidateMap comprehensive test
static void test_validate_map_comprehensive() {
    printf("Test 11: ValidateMap comprehensive test... ");
    
    MapData map;
    create_valid_map(&map);
    
    // Add valid warp
    map.warp_count = 1;
    strcpy(map.warps[0].dest_map, "MAP_DEST");
    
    // Add valid connection
    map.connection_count = 1;
    strcpy(map.connections[0].direction, "south");
    strcpy(map.connections[0].map, "MAP_SOUTH");
    
    // Add valid object with script
    map.object_count = 1;
    strcpy(map.objects[0].script, "TestScript");
    
    // Create registries
    const char* map_registry[] = {"MAP_TEST", "MAP_DEST", "MAP_SOUTH"};
    const char* tileset_registry[] = {"TILESET_GENERAL"};
    const char* script_registry[] = {"TestScript"};
    
    ValidationResult result = ValidateMap(&map, 
                                          map_registry, 3,
                                          tileset_registry, 1,
                                          script_registry, 1);
    
    assert(result.is_valid == true);
    assert(strlen(result.error_message) == 0);
    
    printf("PASSED\n");
}

// Test 12: ValidateMap with multiple failures
static void test_validate_map_multiple_failures() {
    printf("Test 12: ValidateMap with invalid warp (should fail early)... ");
    
    MapData map;
    create_valid_map(&map);
    
    // Add invalid warp (should fail validation)
    map.warp_count = 1;
    strcpy(map.warps[0].dest_map, "MAP_INVALID");
    
    // Add invalid connection (won't be checked due to early failure)
    map.connection_count = 1;
    strcpy(map.connections[0].direction, "invalid");
    strcpy(map.connections[0].map, "MAP_INVALID");
    
    // Create registries
    const char* map_registry[] = {"MAP_TEST"};
    const char* tileset_registry[] = {"TILESET_GENERAL"};
    const char* script_registry[] = {"TestScript"};
    
    ValidationResult result = ValidateMap(&map, 
                                          map_registry, 1,
                                          tileset_registry, 1,
                                          script_registry, 1);
    
    assert(result.is_valid == false);
    assert(strlen(result.error_message) > 0);
    // Should fail on warp validation, not connection validation
    assert(strstr(result.error_message, "Warp") != NULL);
    
    printf("PASSED\n");
}

int main(int argc, char** argv) {
    printf("=== Validation Engine Unit Tests ===\n");
    printf("Testing Requirements 15.1, 15.2, 15.3, 15.4, 15.5, 15.6\n\n");
    
    test_validate_map_header_valid();
    test_validate_map_header_null();
    test_validate_map_header_empty_name();
    test_validate_map_header_excessive_warps();
    test_validate_warp_destinations_valid();
    test_validate_warp_destinations_invalid();
    test_validate_connections_valid();
    test_validate_connections_invalid_direction();
    test_validate_script_references_valid();
    test_validate_script_references_invalid();
    test_validate_map_comprehensive();
    test_validate_map_multiple_failures();
    
    printf("\n=== All Validation Engine Tests PASSED ===\n");
    return 0;
}
