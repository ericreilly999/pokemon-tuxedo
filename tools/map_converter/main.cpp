// main.cpp
// Main program for map converter tool

#include "map_converter.h"
#include <stdio.h>
#include <string.h>

void print_usage(const char* program_name) {
    printf("Usage: %s <mode> [options]\n\n", program_name);
    printf("Modes:\n");
    printf("  convert <input_emerald.json> <output_firered.json>\n");
    printf("    Convert an Emerald map.json to FireRed format\n\n");
    printf("  validate <map1.json> <map2.json>\n");
    printf("    Validate that two maps are equivalent\n\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char* mode = argv[1];
    
    if (strcmp(mode, "convert") == 0) {
        if (argc != 4) {
            printf("Error: convert mode requires input and output paths\n");
            print_usage(argv[0]);
            return 1;
        }
        
        const char* input_path = argv[2];
        const char* output_path = argv[3];
        
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
        return 0;
        
    } else if (strcmp(mode, "validate") == 0) {
        if (argc != 4) {
            printf("Error: validate mode requires two map paths\n");
            print_usage(argv[0]);
            return 1;
        }
        
        const char* map1_path = argv[2];
        const char* map2_path = argv[3];
        
        MapData map1, map2;
        char error_buffer[ERROR_BUFFER_SIZE];
        
        printf("Parsing first map from: %s\n", map1_path);
        if (!ParseEmeraldMap(map1_path, &map1, error_buffer, sizeof(error_buffer))) {
            fprintf(stderr, "Error parsing first map: %s\n", error_buffer);
            return 1;
        }
        
        printf("Parsing second map from: %s\n", map2_path);
        if (!ParseEmeraldMap(map2_path, &map2, error_buffer, sizeof(error_buffer))) {
            fprintf(stderr, "Error parsing second map: %s\n", error_buffer);
            return 1;
        }
        
        printf("\nValidating maps...\n");
        if (ValidateMapRoundTrip(&map1, &map2)) {
            printf("Validation PASSED - maps are equivalent!\n");
            return 0;
        } else {
            printf("Validation FAILED - maps differ\n");
            return 1;
        }
        
    } else {
        printf("Error: Unknown mode '%s'\n", mode);
        print_usage(argv[0]);
        return 1;
    }
}
