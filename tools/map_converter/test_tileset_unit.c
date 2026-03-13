// test_tileset_unit.c
// Unit test for tileset converter without requiring Crossroads data

#include "map_converter.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Helper function to create mock tileset data
void create_mock_tileset(TilesetData* tileset, const char* name) {
    InitTilesetData(tileset);
    
    // Set name
    strncpy(tileset->name, name, sizeof(tileset->name) - 1);
    
    // Create mock graphics data (1KB)
    tileset->graphics_size = 1024;
    tileset->graphics = (uint8_t*)malloc(tileset->graphics_size);
    for (size_t i = 0; i < tileset->graphics_size; i++) {
        tileset->graphics[i] = (uint8_t)(i % 256);
    }
    
    // Create mock metatiles (64 metatiles)
    tileset->metatile_count = 64;
    tileset->metatiles = (uint16_t*)malloc(tileset->metatile_count * sizeof(uint16_t));
    for (size_t i = 0; i < tileset->metatile_count; i++) {
        tileset->metatiles[i] = (uint16_t)(i * 2);
    }
    
    // Create mock behavior data (64 bytes)
    tileset->behavior_size = 64;
    tileset->behavior = (uint8_t*)malloc(tileset->behavior_size);
    for (size_t i = 0; i < tileset->behavior_size; i++) {
        tileset->behavior[i] = (uint8_t)(i % 16);
    }
    
    // Create mock palettes (8 palettes)
    tileset->palette_count = 8;
    size_t palettes_size = tileset->palette_count * 16 * sizeof(uint16_t);
    tileset->palettes = (uint16_t*)malloc(palettes_size);
    for (size_t i = 0; i < tileset->palette_count * 16; i++) {
        tileset->palettes[i] = (uint16_t)(i * 3);
    }
}

int main(void) {
    printf("=== Tileset Converter Unit Test ===\n\n");
    
    // Test 1: InitTilesetData
    printf("Test 1: InitTilesetData\n");
    TilesetData tileset;
    InitTilesetData(&tileset);
    
    if (tileset.graphics == NULL && tileset.metatiles == NULL && 
        tileset.behavior == NULL && tileset.palettes == NULL &&
        tileset.graphics_size == 0 && tileset.metatile_count == 0 &&
        tileset.behavior_size == 0 && tileset.palette_count == 0) {
        printf("  PASSED: TilesetData initialized correctly\n\n");
    } else {
        printf("  FAILED: TilesetData not initialized correctly\n\n");
        return 1;
    }
    
    // Test 2: Create mock tileset
    printf("Test 2: Create mock tileset\n");
    TilesetData mock_tileset;
    create_mock_tileset(&mock_tileset, "test_tileset");
    
    printf("  Mock tileset created:\n");
    printf("    Name: %s\n", mock_tileset.name);
    printf("    Graphics size: %zu bytes\n", mock_tileset.graphics_size);
    printf("    Metatile count: %zu\n", mock_tileset.metatile_count);
    printf("    Behavior size: %zu bytes\n", mock_tileset.behavior_size);
    printf("    Palette count: %u\n", mock_tileset.palette_count);
    printf("  PASSED\n\n");
    
    // Test 3: ConvertTileset
    printf("Test 3: ConvertTileset\n");
    TilesetData converted_tileset;
    char error_buffer[512];
    
    if (!ConvertTileset(&mock_tileset, &converted_tileset, error_buffer, sizeof(error_buffer))) {
        printf("  FAILED: ConvertTileset returned false: %s\n\n", error_buffer);
        FreeTilesetData(&mock_tileset);
        return 1;
    }
    
    printf("  Converted tileset:\n");
    printf("    Name: %s\n", converted_tileset.name);
    printf("    Graphics size: %zu bytes\n", converted_tileset.graphics_size);
    printf("    Metatile count: %zu\n", converted_tileset.metatile_count);
    printf("    Behavior size: %zu bytes\n", converted_tileset.behavior_size);
    printf("    Palette count: %u\n", converted_tileset.palette_count);
    
    // Test 4: Verify data integrity
    printf("\nTest 4: Verify data integrity\n");
    bool integrity_ok = true;
    
    if (strcmp(mock_tileset.name, converted_tileset.name) != 0) {
        printf("  FAILED: Name mismatch\n");
        integrity_ok = false;
    }
    
    if (mock_tileset.graphics_size != converted_tileset.graphics_size) {
        printf("  FAILED: Graphics size mismatch\n");
        integrity_ok = false;
    }
    
    if (mock_tileset.metatile_count != converted_tileset.metatile_count) {
        printf("  FAILED: Metatile count mismatch\n");
        integrity_ok = false;
    }
    
    if (mock_tileset.behavior_size != converted_tileset.behavior_size) {
        printf("  FAILED: Behavior size mismatch\n");
        integrity_ok = false;
    }
    
    if (mock_tileset.palette_count != converted_tileset.palette_count) {
        printf("  FAILED: Palette count mismatch\n");
        integrity_ok = false;
    }
    
    if (memcmp(mock_tileset.graphics, converted_tileset.graphics, 
               mock_tileset.graphics_size) != 0) {
        printf("  FAILED: Graphics data mismatch\n");
        integrity_ok = false;
    }
    
    if (memcmp(mock_tileset.metatiles, converted_tileset.metatiles, 
               mock_tileset.metatile_count * sizeof(uint16_t)) != 0) {
        printf("  FAILED: Metatiles data mismatch\n");
        integrity_ok = false;
    }
    
    if (memcmp(mock_tileset.behavior, converted_tileset.behavior, 
               mock_tileset.behavior_size) != 0) {
        printf("  FAILED: Behavior data mismatch\n");
        integrity_ok = false;
    }
    
    if (memcmp(mock_tileset.palettes, converted_tileset.palettes, 
               mock_tileset.palette_count * 16 * sizeof(uint16_t)) != 0) {
        printf("  FAILED: Palettes data mismatch\n");
        integrity_ok = false;
    }
    
    if (integrity_ok) {
        printf("  PASSED: All data matches\n\n");
    } else {
        printf("  FAILED: Data integrity check failed\n\n");
        FreeTilesetData(&mock_tileset);
        FreeTilesetData(&converted_tileset);
        return 1;
    }
    
    // Test 5: FreeTilesetData
    printf("Test 5: FreeTilesetData\n");
    FreeTilesetData(&mock_tileset);
    FreeTilesetData(&converted_tileset);
    
    if (mock_tileset.graphics == NULL && mock_tileset.metatiles == NULL && 
        mock_tileset.behavior == NULL && mock_tileset.palettes == NULL) {
        printf("  PASSED: Mock tileset freed correctly\n");
    } else {
        printf("  FAILED: Mock tileset not freed correctly\n");
        return 1;
    }
    
    if (converted_tileset.graphics == NULL && converted_tileset.metatiles == NULL && 
        converted_tileset.behavior == NULL && converted_tileset.palettes == NULL) {
        printf("  PASSED: Converted tileset freed correctly\n\n");
    } else {
        printf("  FAILED: Converted tileset not freed correctly\n\n");
        return 1;
    }
    
    printf("=== All Tests PASSED ===\n");
    return 0;
}
