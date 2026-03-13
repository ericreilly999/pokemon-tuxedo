// test_converter.cpp
// Simple test program for map converter

#include "map_converter.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input_emerald_map.json> <output_firered_map.json>\n", argv[0]);
        return 1;
    }
    
    const char* input_path = argv[1];
    const char* output_path = argv[2];
    
    MapData map_data;
    char error_buffer[ERROR_BUFFER_SIZE];
    
    printf("Parsing Emerald map from: %s\n", input_path);
    
    if (!ParseEmeraldMap(input_path, &map_data, error_buffer, sizeof(error_buffer))) {
        fprintf(stderr, "Error parsing Emerald map: %s\n", error_buffer);
        return 1;
    }
    
    printf("Successfully parsed map: %s (ID: %s)\n", map_data.name, map_data.id);
    printf("  Layout: %s\n", map_data.layout);
    printf("  Music: %s\n", map_data.music);
    printf("  Weather: %s\n", map_data.weather);
    printf("  Map Type: %s\n", map_data.map_type);
    printf("  Connections: %d\n", map_data.connection_count);
    printf("  Warps: %d\n", map_data.warp_count);
    printf("  Objects: %d\n", map_data.object_count);
    printf("  Coord Events: %d\n", map_data.coord_event_count);
    printf("  BG Events: %d\n", map_data.bg_event_count);
    
    printf("\nFormatting FireRed map to: %s\n", output_path);
    
    if (!FormatFireRedMap(&map_data, output_path, error_buffer, sizeof(error_buffer))) {
        fprintf(stderr, "Error formatting FireRed map: %s\n", error_buffer);
        return 1;
    }
    
    printf("Successfully wrote FireRed map!\n");
    
    // Test round-trip validation
    printf("\nTesting round-trip validation...\n");
    MapData converted_data;
    
    if (!ParseEmeraldMap(output_path, &converted_data, error_buffer, sizeof(error_buffer))) {
        fprintf(stderr, "Error parsing converted map: %s\n", error_buffer);
        return 1;
    }
    
    if (ValidateMapRoundTrip(&map_data, &converted_data)) {
        printf("Round-trip validation PASSED!\n");
    } else {
        printf("Round-trip validation FAILED - maps differ\n");
        return 1;
    }
    
    return 0;
}
