/**
 * Property Test: Hoenn Map Completeness (Property 4)
 * 
 * Validates: Requirements 3.1, 3.2, 3.3
 * 
 * Property 4: Region Map Completeness
 * For any region (Johto, Hoenn, Sinnoh), all required maps (routes, cities, 
 * caves, dungeons) from the original game should be present in the Map_Bank.
 * 
 * This test verifies:
 * 1. All 34 Hoenn routes (Route101-Route134) are present
 * 2. All 16 Hoenn cities are present
 * 3. All 54 Hoenn dungeon maps are present
 * 4. All maps have region_id = 2 (Hoenn)
 * 5. All maps have valid structure (name, layout, music fields)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define HOENN_REGION_ID 2
#define EXPECTED_ROUTE_COUNT 34
#define EXPECTED_CITY_COUNT 16
#define EXPECTED_DUNGEON_COUNT 54
#define EXPECTED_TOTAL_MAPS 104

// Expected Hoenn routes (Route101 through Route134)
static const char* HOENN_ROUTES[] = {
    "Route101", "Route102", "Route103", "Route104", "Route105",
    "Route106", "Route107", "Route108", "Route109", "Route110",
    "Route111", "Route112", "Route113", "Route114", "Route115",
    "Route116", "Route117", "Route118", "Route119", "Route120",
    "Route121", "Route122", "Route123", "Route124", "Route125",
    "Route126", "Route127", "Route128", "Route129", "Route130",
    "Route131", "Route132", "Route133", "Route134"
};

// Expected Hoenn cities
static const char* HOENN_CITIES[] = {
    "LittlerootTown", "OldaleTown", "PetalburgCity", "RustboroCity",
    "DewfordTown", "SlateportCity", "MauvilleCity", "VerdanturfTown",
    "FallarborTown", "LavaridgeTown", "FortreeCity", "LilycoveCity",
    "MossdeepCity", "SootopolisCity", "PacifidlogTown", "EverGrandeCity"
};

// Expected Hoenn dungeons (caves, forests, etc.)
static const char* HOENN_DUNGEONS[] = {
    // Petalburg Woods
    "PetalburgWoods",
    // Granite Cave
    "GraniteCave_1F", "GraniteCave_B1F", "GraniteCave_B2F", "GraniteCave_StevensRoom",
    // Rusturf Tunnel
    "RusturfTunnel",
    // Meteor Falls
    "MeteorFalls_1F_1R", "MeteorFalls_1F_2R", "MeteorFalls_B1F_1R", 
    "MeteorFalls_B1F_2R", "MeteorFalls_StevensCave",
    // Mt. Chimney
    "MtChimney", "MtChimney_CableCarStation",
    // Jagged Pass
    "JaggedPass",
    // Fiery Path
    "FieryPath",
    // Mt. Pyre
    "MtPyre_1F", "MtPyre_2F", "MtPyre_3F", "MtPyre_4F", 
    "MtPyre_5F", "MtPyre_6F", "MtPyre_Exterior", "MtPyre_Summit",
    // Shoal Cave
    "ShoalCave_HighTideEntranceRoom", "ShoalCave_HighTideInnerRoom",
    "ShoalCave_LowTideEntranceRoom", "ShoalCave_LowTideIceRoom",
    "ShoalCave_LowTideInnerRoom", "ShoalCave_LowTideLowerRoom", 
    "ShoalCave_LowTideStairsRoom",
    // Cave of Origin
    "CaveOfOrigin_Entrance", "CaveOfOrigin_1F", "CaveOfOrigin_B1F",
    // Seafloor Cavern
    "SeafloorCavern_Entrance", "SeafloorCavern_Room1", "SeafloorCavern_Room2",
    "SeafloorCavern_Room3", "SeafloorCavern_Room4", "SeafloorCavern_Room5",
    "SeafloorCavern_Room6", "SeafloorCavern_Room7", "SeafloorCavern_Room8",
    "SeafloorCavern_Room9",
    // Sky Pillar
    "SkyPillar_Entrance", "SkyPillar_Outside", "SkyPillar_1F", "SkyPillar_2F",
    "SkyPillar_3F", "SkyPillar_4F", "SkyPillar_5F", "SkyPillar_Top",
    // Victory Road
    "VictoryRoad_1F", "VictoryRoad_B1F", "VictoryRoad_B2F"
};

// Test result tracking
static int tests_passed = 0;
static int tests_failed = 0;
static char failed_maps[256][64];
static int failed_map_count = 0;

/**
 * Check if a file exists
 */
static bool file_exists(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

/**
 * Simple JSON field checker - looks for a field and returns its value
 * This is a minimal parser for our specific use case
 */
static bool json_has_field(const char* json_content, const char* field_name) {
    char search_pattern[128];
    snprintf(search_pattern, sizeof(search_pattern), "\"%s\"", field_name);
    return strstr(json_content, search_pattern) != NULL;
}

/**
 * Extract integer value from JSON field
 */
static int json_get_int(const char* json_content, const char* field_name) {
    char search_pattern[128];
    snprintf(search_pattern, sizeof(search_pattern), "\"%s\":", field_name);
    
    const char* pos = strstr(json_content, search_pattern);
    if (!pos) return -1;
    
    pos += strlen(search_pattern);
    while (*pos == ' ' || *pos == '\t') pos++;
    
    return atoi(pos);
}

/**
 * Read file contents into buffer
 */
static char* read_file(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* buffer = malloc(size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }
    
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);
    
    return buffer;
}

/**
 * Validate a single map file
 * Returns true if map is valid, false otherwise
 */
static bool validate_map(const char* map_name, const char* base_path) {
    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/%s.json", base_path, map_name);
    
    // Check file exists
    if (!file_exists(file_path)) {
        printf("  FAIL: Map file not found: %s\n", file_path);
        return false;
    }
    
    // Read file contents
    char* content = read_file(file_path);
    if (!content) {
        printf("  FAIL: Could not read map file: %s\n", file_path);
        return false;
    }
    
    bool valid = true;
    
    // Check required fields exist
    if (!json_has_field(content, "name")) {
        printf("  FAIL: Map %s missing 'name' field\n", map_name);
        valid = false;
    }
    
    if (!json_has_field(content, "layout")) {
        printf("  FAIL: Map %s missing 'layout' field\n", map_name);
        valid = false;
    }
    
    if (!json_has_field(content, "music")) {
        printf("  FAIL: Map %s missing 'music' field\n", map_name);
        valid = false;
    }
    
    // Check region_id is correct (Hoenn = 2)
    if (!json_has_field(content, "region_id")) {
        printf("  FAIL: Map %s missing 'region_id' field\n", map_name);
        valid = false;
    } else {
        int region_id = json_get_int(content, "region_id");
        if (region_id != HOENN_REGION_ID) {
            printf("  FAIL: Map %s has region_id=%d, expected %d (Hoenn)\n", 
                   map_name, region_id, HOENN_REGION_ID);
            valid = false;
        }
    }
    
    free(content);
    return valid;
}

/**
 * Property Test: All Hoenn routes are present and valid
 */
static bool prop_hoenn_routes_complete(const char* base_path) {
    printf("\n--- Testing Hoenn Routes (34 expected) ---\n");
    
    int route_count = sizeof(HOENN_ROUTES) / sizeof(HOENN_ROUTES[0]);
    int routes_found = 0;
    int routes_valid = 0;
    
    for (int i = 0; i < route_count; i++) {
        char file_path[512];
        snprintf(file_path, sizeof(file_path), "%s/%s.json", base_path, HOENN_ROUTES[i]);
        
        if (file_exists(file_path)) {
            routes_found++;
            if (validate_map(HOENN_ROUTES[i], base_path)) {
                routes_valid++;
            } else {
                if (failed_map_count < 256) {
                    strncpy(failed_maps[failed_map_count++], HOENN_ROUTES[i], 63);
                }
            }
        } else {
            printf("  MISSING: %s\n", HOENN_ROUTES[i]);
            if (failed_map_count < 256) {
                strncpy(failed_maps[failed_map_count++], HOENN_ROUTES[i], 63);
            }
        }
    }
    
    printf("Routes found: %d/%d\n", routes_found, route_count);
    printf("Routes valid: %d/%d\n", routes_valid, route_count);
    
    return (routes_found == route_count && routes_valid == route_count);
}

/**
 * Property Test: All Hoenn cities are present and valid
 */
static bool prop_hoenn_cities_complete(const char* base_path) {
    printf("\n--- Testing Hoenn Cities (16 expected) ---\n");
    
    int city_count = sizeof(HOENN_CITIES) / sizeof(HOENN_CITIES[0]);
    int cities_found = 0;
    int cities_valid = 0;
    
    for (int i = 0; i < city_count; i++) {
        char file_path[512];
        snprintf(file_path, sizeof(file_path), "%s/%s.json", base_path, HOENN_CITIES[i]);
        
        if (file_exists(file_path)) {
            cities_found++;
            if (validate_map(HOENN_CITIES[i], base_path)) {
                cities_valid++;
            } else {
                if (failed_map_count < 256) {
                    strncpy(failed_maps[failed_map_count++], HOENN_CITIES[i], 63);
                }
            }
        } else {
            printf("  MISSING: %s\n", HOENN_CITIES[i]);
            if (failed_map_count < 256) {
                strncpy(failed_maps[failed_map_count++], HOENN_CITIES[i], 63);
            }
        }
    }
    
    printf("Cities found: %d/%d\n", cities_found, city_count);
    printf("Cities valid: %d/%d\n", cities_valid, city_count);
    
    return (cities_found == city_count && cities_valid == city_count);
}

/**
 * Property Test: All Hoenn dungeons are present and valid
 */
static bool prop_hoenn_dungeons_complete(const char* base_path) {
    printf("\n--- Testing Hoenn Dungeons (54 expected) ---\n");
    
    int dungeon_count = sizeof(HOENN_DUNGEONS) / sizeof(HOENN_DUNGEONS[0]);
    int dungeons_found = 0;
    int dungeons_valid = 0;
    
    for (int i = 0; i < dungeon_count; i++) {
        char file_path[512];
        snprintf(file_path, sizeof(file_path), "%s/%s.json", base_path, HOENN_DUNGEONS[i]);
        
        if (file_exists(file_path)) {
            dungeons_found++;
            if (validate_map(HOENN_DUNGEONS[i], base_path)) {
                dungeons_valid++;
            } else {
                if (failed_map_count < 256) {
                    strncpy(failed_maps[failed_map_count++], HOENN_DUNGEONS[i], 63);
                }
            }
        } else {
            printf("  MISSING: %s\n", HOENN_DUNGEONS[i]);
            if (failed_map_count < 256) {
                strncpy(failed_maps[failed_map_count++], HOENN_DUNGEONS[i], 63);
            }
        }
    }
    
    printf("Dungeons found: %d/%d\n", dungeons_found, dungeon_count);
    printf("Dungeons valid: %d/%d\n", dungeons_valid, dungeon_count);
    
    return (dungeons_found == dungeon_count && dungeons_valid == dungeon_count);
}

/**
 * Property Test: Total map count matches expected
 */
static bool prop_total_map_count(const char* base_path) {
    printf("\n--- Testing Total Map Count ---\n");
    
    DIR* dir = opendir(base_path);
    if (!dir) {
        printf("  FAIL: Could not open directory: %s\n", base_path);
        return false;
    }
    
    int json_count = 0;
    struct dirent* entry;
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            const char* ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".json") == 0) {
                // Exclude report files
                if (strstr(entry->d_name, "report") == NULL) {
                    json_count++;
                }
            }
        }
    }
    
    closedir(dir);
    
    printf("Total map files found: %d\n", json_count);
    printf("Expected map files: %d\n", EXPECTED_TOTAL_MAPS);
    
    if (json_count >= EXPECTED_TOTAL_MAPS) {
        printf("  PASS: Map count meets or exceeds expected\n");
        return true;
    } else {
        printf("  FAIL: Map count below expected (%d < %d)\n", 
               json_count, EXPECTED_TOTAL_MAPS);
        return false;
    }
}

/**
 * Main test runner
 */
int main(int argc, char** argv) {
    const char* base_path = "data/maps/hoenn_converted";
    
    // Allow override via command line
    if (argc > 1) {
        base_path = argv[1];
    }
    
    printf("=============================================================\n");
    printf("Property Test: Hoenn Map Completeness (Property 4)\n");
    printf("=============================================================\n");
    printf("Validates: Requirements 3.1, 3.2, 3.3\n");
    printf("Map directory: %s\n", base_path);
    printf("=============================================================\n");
    
    // Check directory exists
    struct stat st;
    if (stat(base_path, &st) != 0 || !S_ISDIR(st.st_mode)) {
        printf("\nFATAL: Map directory does not exist: %s\n", base_path);
        printf("Please run the Hoenn map conversion first.\n");
        return 1;
    }
    
    // Run property tests
    bool routes_ok = prop_hoenn_routes_complete(base_path);
    bool cities_ok = prop_hoenn_cities_complete(base_path);
    bool dungeons_ok = prop_hoenn_dungeons_complete(base_path);
    bool count_ok = prop_total_map_count(base_path);
    
    // Summary
    printf("\n=============================================================\n");
    printf("TEST SUMMARY\n");
    printf("=============================================================\n");
    
    printf("Routes (34):   %s\n", routes_ok ? "PASS" : "FAIL");
    printf("Cities (16):   %s\n", cities_ok ? "PASS" : "FAIL");
    printf("Dungeons (54): %s\n", dungeons_ok ? "PASS" : "FAIL");
    printf("Total Count:   %s\n", count_ok ? "PASS" : "FAIL");
    
    if (failed_map_count > 0) {
        printf("\nFailed/Missing Maps (%d):\n", failed_map_count);
        for (int i = 0; i < failed_map_count && i < 20; i++) {
            printf("  - %s\n", failed_maps[i]);
        }
        if (failed_map_count > 20) {
            printf("  ... and %d more\n", failed_map_count - 20);
        }
    }
    
    printf("\n=============================================================\n");
    
    if (routes_ok && cities_ok && dungeons_ok && count_ok) {
        printf("RESULT: ALL PROPERTY TESTS PASSED\n");
        printf("Property 4 (Region Map Completeness) VERIFIED for Hoenn\n");
        printf("=============================================================\n");
        return 0;
    } else {
        printf("RESULT: SOME PROPERTY TESTS FAILED\n");
        printf("Property 4 (Region Map Completeness) NOT VERIFIED for Hoenn\n");
        printf("=============================================================\n");
        return 1;
    }
}
