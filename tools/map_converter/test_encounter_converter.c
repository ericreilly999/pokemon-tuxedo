// test_encounter_converter.c
// Unit tests for wild encounter converter

#include "map_converter.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Test helper to compare encounter slots
bool compare_encounter_slot(const EncounterSlot* a, const EncounterSlot* b) {
    return a->species == b->species &&
           a->min_level == b->min_level &&
           a->max_level == b->max_level;
}

// Test helper to compare encounter data
bool compare_encounter_data(const WildEncounterData* a, const WildEncounterData* b) {
    if (a->encounter_rate != b->encounter_rate) {
        printf("Encounter rates differ: %u vs %u\n", a->encounter_rate, b->encounter_rate);
        return false;
    }
    
    // Compare grass encounters
    for (int i = 0; i < 12; i++) {
        if (!compare_encounter_slot(&a->grass_encounters[i], &b->grass_encounters[i])) {
            printf("Grass encounter slot %d differs\n", i);
            return false;
        }
    }
    
    // Compare water encounters
    for (int i = 0; i < 5; i++) {
        if (!compare_encounter_slot(&a->water_encounters[i], &b->water_encounters[i])) {
            printf("Water encounter slot %d differs\n", i);
            return false;
        }
    }
    
    // Compare fishing encounters
    for (int i = 0; i < 5; i++) {
        if (!compare_encounter_slot(&a->fishing_old[i], &b->fishing_old[i])) {
            printf("Fishing old slot %d differs\n", i);
            return false;
        }
        if (!compare_encounter_slot(&a->fishing_good[i], &b->fishing_good[i])) {
            printf("Fishing good slot %d differs\n", i);
            return false;
        }
        if (!compare_encounter_slot(&a->fishing_super[i], &b->fishing_super[i])) {
            printf("Fishing super slot %d differs\n", i);
            return false;
        }
    }
    
    // Compare rock smash encounters
    for (int i = 0; i < 5; i++) {
        if (!compare_encounter_slot(&a->rock_smash[i], &b->rock_smash[i])) {
            printf("Rock smash slot %d differs\n", i);
            return false;
        }
    }
    
    return true;
}

// Test 1: Initialize wild encounter data
void test_init_wild_encounter_data() {
    printf("Test 1: Initialize wild encounter data...\n");
    
    WildEncounterData data;
    InitWildEncounterData(&data);
    
    // Verify encounter rate is 0
    assert(data.encounter_rate == 0);
    
    // Verify all grass encounters are empty
    for (int i = 0; i < 12; i++) {
        assert(data.grass_encounters[i].species == 0);
        assert(data.grass_encounters[i].min_level == 0);
        assert(data.grass_encounters[i].max_level == 0);
    }
    
    // Verify all water encounters are empty
    for (int i = 0; i < 5; i++) {
        assert(data.water_encounters[i].species == 0);
        assert(data.water_encounters[i].min_level == 0);
        assert(data.water_encounters[i].max_level == 0);
    }
    
    // Verify all fishing encounters are empty
    for (int i = 0; i < 5; i++) {
        assert(data.fishing_old[i].species == 0);
        assert(data.fishing_good[i].species == 0);
        assert(data.fishing_super[i].species == 0);
    }
    
    // Verify all rock smash encounters are empty
    for (int i = 0; i < 5; i++) {
        assert(data.rock_smash[i].species == 0);
    }
    
    printf("  PASS: Wild encounter data initialized correctly\n");
}

// Test 2: Convert empty encounter data
void test_convert_empty_encounters() {
    printf("Test 2: Convert empty encounter data...\n");
    
    WildEncounterData emerald, firered;
    char error_buffer[ERROR_BUFFER_SIZE] = {0};
    
    InitWildEncounterData(&emerald);
    
    bool result = ConvertEncounters(&emerald, &firered, error_buffer, ERROR_BUFFER_SIZE);
    
    assert(result == true);
    assert(compare_encounter_data(&emerald, &firered));
    
    printf("  PASS: Empty encounter data converted correctly\n");
    printf("  Message: %s\n", error_buffer);
}

// Test 3: Convert grass encounters with encounter rate preservation
void test_convert_grass_encounters() {
    printf("Test 3: Convert grass encounters with encounter rate preservation...\n");
    
    WildEncounterData emerald, firered;
    char error_buffer[ERROR_BUFFER_SIZE] = {0};
    
    InitWildEncounterData(&emerald);
    
    // Set encounter rate (Requirement 9.3)
    emerald.encounter_rate = 20;
    
    // Set up grass encounters (Requirement 9.4)
    emerald.grass_encounters[0].species = 25;  // Pikachu
    emerald.grass_encounters[0].min_level = 3;
    emerald.grass_encounters[0].max_level = 5;
    
    emerald.grass_encounters[1].species = 16;  // Pidgey
    emerald.grass_encounters[1].min_level = 2;
    emerald.grass_encounters[1].max_level = 4;
    
    emerald.grass_encounters[2].species = 19;  // Rattata
    emerald.grass_encounters[2].min_level = 2;
    emerald.grass_encounters[2].max_level = 3;
    
    bool result = ConvertEncounters(&emerald, &firered, error_buffer, ERROR_BUFFER_SIZE);
    
    assert(result == true);
    
    // Verify encounter rate is preserved
    assert(firered.encounter_rate == 20);
    
    // Verify species and level ranges are preserved
    assert(firered.grass_encounters[0].species == 25);
    assert(firered.grass_encounters[0].min_level == 3);
    assert(firered.grass_encounters[0].max_level == 5);
    
    assert(firered.grass_encounters[1].species == 16);
    assert(firered.grass_encounters[1].min_level == 2);
    assert(firered.grass_encounters[1].max_level == 4);
    
    assert(firered.grass_encounters[2].species == 19);
    assert(firered.grass_encounters[2].min_level == 2);
    assert(firered.grass_encounters[2].max_level == 3);
    
    printf("  PASS: Grass encounters converted correctly\n");
    printf("  Message: %s\n", error_buffer);
}

// Test 4: Convert water encounters
void test_convert_water_encounters() {
    printf("Test 4: Convert water encounters...\n");
    
    WildEncounterData emerald, firered;
    char error_buffer[ERROR_BUFFER_SIZE] = {0};
    
    InitWildEncounterData(&emerald);
    
    emerald.encounter_rate = 5;
    
    // Set up water encounters (Requirement 9.5)
    emerald.water_encounters[0].species = 129;  // Magikarp
    emerald.water_encounters[0].min_level = 5;
    emerald.water_encounters[0].max_level = 10;
    
    emerald.water_encounters[1].species = 60;   // Poliwag
    emerald.water_encounters[1].min_level = 10;
    emerald.water_encounters[1].max_level = 15;
    
    bool result = ConvertEncounters(&emerald, &firered, error_buffer, ERROR_BUFFER_SIZE);
    
    assert(result == true);
    assert(firered.encounter_rate == 5);
    
    // Verify water encounters are preserved
    assert(firered.water_encounters[0].species == 129);
    assert(firered.water_encounters[0].min_level == 5);
    assert(firered.water_encounters[0].max_level == 10);
    
    assert(firered.water_encounters[1].species == 60);
    assert(firered.water_encounters[1].min_level == 10);
    assert(firered.water_encounters[1].max_level == 15);
    
    printf("  PASS: Water encounters converted correctly\n");
    printf("  Message: %s\n", error_buffer);
}

// Test 5: Convert fishing encounters
void test_convert_fishing_encounters() {
    printf("Test 5: Convert fishing encounters...\n");
    
    WildEncounterData emerald, firered;
    char error_buffer[ERROR_BUFFER_SIZE] = {0};
    
    InitWildEncounterData(&emerald);
    
    emerald.encounter_rate = 30;
    
    // Set up fishing encounters (Requirement 9.5)
    // Old Rod
    emerald.fishing_old[0].species = 129;  // Magikarp
    emerald.fishing_old[0].min_level = 5;
    emerald.fishing_old[0].max_level = 5;
    
    // Good Rod
    emerald.fishing_good[0].species = 60;  // Poliwag
    emerald.fishing_good[0].min_level = 10;
    emerald.fishing_good[0].max_level = 15;
    
    // Super Rod
    emerald.fishing_super[0].species = 130;  // Gyarados
    emerald.fishing_super[0].min_level = 40;
    emerald.fishing_super[0].max_level = 45;
    
    bool result = ConvertEncounters(&emerald, &firered, error_buffer, ERROR_BUFFER_SIZE);
    
    assert(result == true);
    
    // Verify fishing encounters are preserved
    assert(firered.fishing_old[0].species == 129);
    assert(firered.fishing_old[0].min_level == 5);
    assert(firered.fishing_old[0].max_level == 5);
    
    assert(firered.fishing_good[0].species == 60);
    assert(firered.fishing_good[0].min_level == 10);
    assert(firered.fishing_good[0].max_level == 15);
    
    assert(firered.fishing_super[0].species == 130);
    assert(firered.fishing_super[0].min_level == 40);
    assert(firered.fishing_super[0].max_level == 45);
    
    printf("  PASS: Fishing encounters converted correctly\n");
    printf("  Message: %s\n", error_buffer);
}

// Test 6: Convert rock smash encounters
void test_convert_rock_smash_encounters() {
    printf("Test 6: Convert rock smash encounters...\n");
    
    WildEncounterData emerald, firered;
    char error_buffer[ERROR_BUFFER_SIZE] = {0};
    
    InitWildEncounterData(&emerald);
    
    emerald.encounter_rate = 10;
    
    // Set up rock smash encounters (Requirement 9.5)
    emerald.rock_smash[0].species = 74;  // Geodude
    emerald.rock_smash[0].min_level = 5;
    emerald.rock_smash[0].max_level = 10;
    
    emerald.rock_smash[1].species = 95;  // Onix
    emerald.rock_smash[1].min_level = 10;
    emerald.rock_smash[1].max_level = 15;
    
    bool result = ConvertEncounters(&emerald, &firered, error_buffer, ERROR_BUFFER_SIZE);
    
    assert(result == true);
    
    // Verify rock smash encounters are preserved
    assert(firered.rock_smash[0].species == 74);
    assert(firered.rock_smash[0].min_level == 5);
    assert(firered.rock_smash[0].max_level == 10);
    
    assert(firered.rock_smash[1].species == 95);
    assert(firered.rock_smash[1].min_level == 10);
    assert(firered.rock_smash[1].max_level == 15);
    
    printf("  PASS: Rock smash encounters converted correctly\n");
    printf("  Message: %s\n", error_buffer);
}

// Test 7: Convert all encounter types together
void test_convert_all_encounter_types() {
    printf("Test 7: Convert all encounter types together...\n");
    
    WildEncounterData emerald, firered;
    char error_buffer[ERROR_BUFFER_SIZE] = {0};
    
    InitWildEncounterData(&emerald);
    
    emerald.encounter_rate = 25;
    
    // Grass
    emerald.grass_encounters[0].species = 25;
    emerald.grass_encounters[0].min_level = 3;
    emerald.grass_encounters[0].max_level = 5;
    
    // Water
    emerald.water_encounters[0].species = 129;
    emerald.water_encounters[0].min_level = 5;
    emerald.water_encounters[0].max_level = 10;
    
    // Fishing
    emerald.fishing_old[0].species = 129;
    emerald.fishing_old[0].min_level = 5;
    emerald.fishing_old[0].max_level = 5;
    
    // Rock smash
    emerald.rock_smash[0].species = 74;
    emerald.rock_smash[0].min_level = 5;
    emerald.rock_smash[0].max_level = 10;
    
    bool result = ConvertEncounters(&emerald, &firered, error_buffer, ERROR_BUFFER_SIZE);
    
    assert(result == true);
    assert(compare_encounter_data(&emerald, &firered));
    
    printf("  PASS: All encounter types converted correctly\n");
    printf("  Message: %s\n", error_buffer);
}

// Test 8: Invalid level range detection
void test_invalid_level_range() {
    printf("Test 8: Invalid level range detection...\n");
    
    WildEncounterData emerald, firered;
    char error_buffer[ERROR_BUFFER_SIZE] = {0};
    
    InitWildEncounterData(&emerald);
    
    // Set up invalid level range (min > max)
    emerald.grass_encounters[0].species = 25;
    emerald.grass_encounters[0].min_level = 10;
    emerald.grass_encounters[0].max_level = 5;  // Invalid!
    
    bool result = ConvertEncounters(&emerald, &firered, error_buffer, ERROR_BUFFER_SIZE);
    
    assert(result == false);
    assert(strstr(error_buffer, "Invalid level range") != NULL);
    
    printf("  PASS: Invalid level range detected correctly\n");
    printf("  Error message: %s\n", error_buffer);
}

// Test 9: Null parameter handling
void test_null_parameters() {
    printf("Test 9: Null parameter handling...\n");
    
    WildEncounterData emerald, firered;
    char error_buffer[ERROR_BUFFER_SIZE] = {0};
    
    InitWildEncounterData(&emerald);
    
    // Test null emerald_encounters
    bool result1 = ConvertEncounters(NULL, &firered, error_buffer, ERROR_BUFFER_SIZE);
    assert(result1 == false);
    
    // Test null firered_encounters
    bool result2 = ConvertEncounters(&emerald, NULL, error_buffer, ERROR_BUFFER_SIZE);
    assert(result2 == false);
    
    // Test null error_buffer
    bool result3 = ConvertEncounters(&emerald, &firered, NULL, ERROR_BUFFER_SIZE);
    assert(result3 == false);
    
    printf("  PASS: Null parameters handled correctly\n");
}

int main() {
    printf("=== Wild Encounter Converter Unit Tests ===\n\n");
    
    test_init_wild_encounter_data();
    test_convert_empty_encounters();
    test_convert_grass_encounters();
    test_convert_water_encounters();
    test_convert_fishing_encounters();
    test_convert_rock_smash_encounters();
    test_convert_all_encounter_types();
    test_invalid_level_range();
    test_null_parameters();
    
    printf("\n=== All tests passed! ===\n");
    
    return 0;
}
