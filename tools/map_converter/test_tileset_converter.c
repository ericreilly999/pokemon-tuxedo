// test_tileset_converter.c
// Simple test program for tileset converter

#include "map_converter.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <crossroads_path> <tileset_name>\n", argv[0]);
        printf("Example: %s /path/to/crossroads general\n", argv[0]);
        return 1;
    }
    
    const char* crossroads_path = argv[1];
    const char* tileset_name = argv[2];
    
    TilesetData emerald_tileset;
    TilesetData firered_tileset;
    char error_buffer[ERROR_BUFFER_SIZE];
    
    printf("Extracting Emerald tileset '%s' from: %s\n", tileset_name, crossroads_path);
    
    if (!ExtractEmeraldTileset(crossroads_path, tileset_name, &emerald_tileset, 
                               error_buffer, sizeof(error_buffer))) {
        fprintf(stderr, "Error extracting Emerald tileset: %s\n", error_buffer);
        return 1;
    }
    
    printf("Successfully extracted tileset: %s\n", emerald_tileset.name);
    printf("  Graphics size: %zu bytes\n", emerald_tileset.graphics_size);
    printf("  Metatile count: %zu\n", emerald_tileset.metatile_count);
    printf("  Behavior size: %zu bytes\n", emerald_tileset.behavior_size);
    printf("  Palette count: %u\n", emerald_tileset.palette_count);
    
    printf("\nConverting tileset to FireRed format...\n");
    
    if (!ConvertTileset(&emerald_tileset, &firered_tileset, 
                        error_buffer, sizeof(error_buffer))) {
        fprintf(stderr, "Error converting tileset: %s\n", error_buffer);
        FreeTilesetData(&emerald_tileset);
        return 1;
    }
    
    printf("Successfully converted tileset!\n");
    printf("  Graphics size: %zu bytes\n", firered_tileset.graphics_size);
    printf("  Metatile count: %zu\n", firered_tileset.metatile_count);
    printf("  Behavior size: %zu bytes\n", firered_tileset.behavior_size);
    printf("  Palette count: %u\n", firered_tileset.palette_count);
    
    // Verify data integrity
    printf("\nVerifying data integrity...\n");
    bool integrity_ok = true;
    
    if (emerald_tileset.graphics_size != firered_tileset.graphics_size) {
        printf("ERROR: Graphics size mismatch\n");
        integrity_ok = false;
    }
    
    if (emerald_tileset.metatile_count != firered_tileset.metatile_count) {
        printf("ERROR: Metatile count mismatch\n");
        integrity_ok = false;
    }
    
    if (emerald_tileset.behavior_size != firered_tileset.behavior_size) {
        printf("ERROR: Behavior size mismatch\n");
        integrity_ok = false;
    }
    
    if (emerald_tileset.palette_count != firered_tileset.palette_count) {
        printf("ERROR: Palette count mismatch\n");
        integrity_ok = false;
    }
    
    // Verify data content
    if (memcmp(emerald_tileset.graphics, firered_tileset.graphics, 
               emerald_tileset.graphics_size) != 0) {
        printf("ERROR: Graphics data mismatch\n");
        integrity_ok = false;
    }
    
    if (memcmp(emerald_tileset.metatiles, firered_tileset.metatiles, 
               emerald_tileset.metatile_count * sizeof(uint16_t)) != 0) {
        printf("ERROR: Metatiles data mismatch\n");
        integrity_ok = false;
    }
    
    if (memcmp(emerald_tileset.behavior, firered_tileset.behavior, 
               emerald_tileset.behavior_size) != 0) {
        printf("ERROR: Behavior data mismatch\n");
        integrity_ok = false;
    }
    
    if (memcmp(emerald_tileset.palettes, firered_tileset.palettes, 
               emerald_tileset.palette_count * 16 * sizeof(uint16_t)) != 0) {
        printf("ERROR: Palettes data mismatch\n");
        integrity_ok = false;
    }
    
    if (integrity_ok) {
        printf("Data integrity check PASSED!\n");
    } else {
        printf("Data integrity check FAILED!\n");
        FreeTilesetData(&emerald_tileset);
        FreeTilesetData(&firered_tileset);
        return 1;
    }
    
    // Clean up
    FreeTilesetData(&emerald_tileset);
    FreeTilesetData(&firered_tileset);
    
    printf("\nAll tests PASSED!\n");
    return 0;
}
