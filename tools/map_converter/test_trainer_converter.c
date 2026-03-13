// test_trainer_converter.c
// Unit tests for trainer data converter

#include "map_converter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Test helper to print test results
static void print_test_result(const char* test_name, bool passed) {
    printf("[%s] %s\n", passed ? "PASS" : "FAIL", test_name);
}

// Test 1: InitTrainerData initializes all fields correctly
void test_init_trainer_data() {
    TrainerData data;
    InitTrainerData(&data);
    
    bool passed = (data.trainer_class == 0 &&
                   data.gender == 0 &&
                   data.music == 0 &&
                   data.sprite == 0 &&
                   data.reward == 0 &&
                   data.ai_flags == 0 &&
                   data.party == NULL &&
                   data.party_size == 0);
    
    print_test_result("test_init_trainer_data", passed);
    assert(passed);
}

// Test 2: MapTrainerClass returns correct mappings for exact matches
void test_map_trainer_class_exact() {
    bool is_exact = false;
    
    // Test Youngster (should be exact match)
    uint8_t result = MapTrainerClass(1, &is_exact);
    bool passed = (result == 1 && is_exact == true);
    
    print_test_result("test_map_trainer_class_exact", passed);
    assert(passed);
}

// Test 3: MapTrainerClass returns approximate mappings for Emerald-specific classes
void test_map_trainer_class_approximate() {
    bool is_exact = false;
    
    // Test Aqua Grunt (Emerald-specific, should map to Youngster approximately)
    uint8_t result = MapTrainerClass(50, &is_exact);
    bool passed = (result == 1 && is_exact == false);
    
    print_test_result("test_map_trainer_class_approximate", passed);
    assert(passed);
}

// Test 4: MapTrainerClass handles unknown classes with default
void test_map_trainer_class_unknown() {
    bool is_exact = false;
    
    // Test unknown class (should default to Youngster)
    uint8_t result = MapTrainerClass(255, &is_exact);
    bool passed = (result == 1 && is_exact == false);
    
    print_test_result("test_map_trainer_class_unknown", passed);
    assert(passed);
}

// Test 5: ConvertTrainer preserves trainer name
void test_convert_trainer_name() {
    TrainerData emerald, firered;
    char error_buffer[512] = {0};
    
    InitTrainerData(&emerald);
    strcpy(emerald.name, "Brock");
    emerald.trainer_class = 32; // Brock class
    
    bool success = ConvertTrainer(&emerald, &firered, error_buffer, sizeof(error_buffer));
    bool passed = (success && strcmp(firered.name, "Brock") == 0);
    
    FreeTrainerData(&firered);
    print_test_result("test_convert_trainer_name", passed);
    assert(passed);
}

// Test 6: ConvertTrainer preserves reward amount (Requirement 8.4)
void test_convert_trainer_reward() {
    TrainerData emerald, firered;
    char error_buffer[512] = {0};
    
    InitTrainerData(&emerald);
    strcpy(emerald.name, "TestTrainer");
    emerald.trainer_class = 1;
    emerald.reward = 1500;
    
    bool success = ConvertTrainer(&emerald, &firered, error_buffer, sizeof(error_buffer));
    bool passed = (success && firered.reward == 1500);
    
    FreeTrainerData(&firered);
    print_test_result("test_convert_trainer_reward", passed);
    assert(passed);
}

// Test 7: ConvertTrainer preserves AI flags (Requirement 8.3)
void test_convert_trainer_ai_flags() {
    TrainerData emerald, firered;
    char error_buffer[512] = {0};
    
    InitTrainerData(&emerald);
    strcpy(emerald.name, "TestTrainer");
    emerald.trainer_class = 1;
    emerald.ai_flags = 0x0F; // Some AI flags
    
    bool success = ConvertTrainer(&emerald, &firered, error_buffer, sizeof(error_buffer));
    bool passed = (success && firered.ai_flags == 0x0F);
    
    FreeTrainerData(&firered);
    print_test_result("test_convert_trainer_ai_flags", passed);
    assert(passed);
}

// Test 8: ConvertTrainer preserves party data
void test_convert_trainer_party() {
    TrainerData emerald, firered;
    char error_buffer[512] = {0};
    
    InitTrainerData(&emerald);
    strcpy(emerald.name, "TestTrainer");
    emerald.trainer_class = 1;
    emerald.party_size = 2;
    emerald.party = (TrainerPokemon*)calloc(2, sizeof(TrainerPokemon));
    
    // Set up first Pokemon
    emerald.party[0].species = 25; // Pikachu
    emerald.party[0].level = 10;
    emerald.party[0].held_item = 0;
    emerald.party[0].moves[0] = 84; // Thundershock
    emerald.party[0].moves[1] = 0;
    emerald.party[0].moves[2] = 0;
    emerald.party[0].moves[3] = 0;
    emerald.party[0].iv = 31;
    emerald.party[0].ev_hp = 252;
    
    // Set up second Pokemon
    emerald.party[1].species = 6; // Charizard
    emerald.party[1].level = 50;
    emerald.party[1].held_item = 169; // Charcoal
    emerald.party[1].moves[0] = 52; // Ember
    emerald.party[1].moves[1] = 0;
    emerald.party[1].moves[2] = 0;
    emerald.party[1].moves[3] = 0;
    emerald.party[1].iv = 31;
    emerald.party[1].ev_atk = 252;
    
    bool success = ConvertTrainer(&emerald, &firered, error_buffer, sizeof(error_buffer));
    bool passed = (success && 
                   firered.party_size == 2 &&
                   firered.party[0].species == 25 &&
                   firered.party[0].level == 10 &&
                   firered.party[0].moves[0] == 84 &&
                   firered.party[0].iv == 31 &&
                   firered.party[0].ev_hp == 252 &&
                   firered.party[1].species == 6 &&
                   firered.party[1].level == 50 &&
                   firered.party[1].held_item == 169 &&
                   firered.party[1].ev_atk == 252);
    
    free(emerald.party);
    FreeTrainerData(&firered);
    print_test_result("test_convert_trainer_party", passed);
    assert(passed);
}

// Test 9: ConvertTrainer handles empty party
void test_convert_trainer_empty_party() {
    TrainerData emerald, firered;
    char error_buffer[512] = {0};
    
    InitTrainerData(&emerald);
    strcpy(emerald.name, "TestTrainer");
    emerald.trainer_class = 1;
    emerald.party_size = 0;
    emerald.party = NULL;
    
    bool success = ConvertTrainer(&emerald, &firered, error_buffer, sizeof(error_buffer));
    bool passed = (success && firered.party_size == 0 && firered.party == NULL);
    
    FreeTrainerData(&firered);
    print_test_result("test_convert_trainer_empty_party", passed);
    assert(passed);
}

// Test 10: ConvertTrainer preserves intro and defeat text
void test_convert_trainer_text() {
    TrainerData emerald, firered;
    char error_buffer[512] = {0};
    
    InitTrainerData(&emerald);
    strcpy(emerald.name, "TestTrainer");
    emerald.trainer_class = 1;
    strcpy(emerald.intro_text, "Let's battle!");
    strcpy(emerald.defeat_text, "You won!");
    
    bool success = ConvertTrainer(&emerald, &firered, error_buffer, sizeof(error_buffer));
    bool passed = (success && 
                   strcmp(firered.intro_text, "Let's battle!") == 0 &&
                   strcmp(firered.defeat_text, "You won!") == 0);
    
    FreeTrainerData(&firered);
    print_test_result("test_convert_trainer_text", passed);
    assert(passed);
}

// Test 11: ConvertTrainer handles NULL parameters gracefully
void test_convert_trainer_null_params() {
    TrainerData emerald, firered;
    char error_buffer[512] = {0};
    
    InitTrainerData(&emerald);
    
    // Test NULL emerald_trainer
    bool success1 = ConvertTrainer(NULL, &firered, error_buffer, sizeof(error_buffer));
    
    // Test NULL firered_trainer
    bool success2 = ConvertTrainer(&emerald, NULL, error_buffer, sizeof(error_buffer));
    
    // Test NULL error_buffer
    bool success3 = ConvertTrainer(&emerald, &firered, NULL, 0);
    
    bool passed = (!success1 && !success2 && !success3);
    
    print_test_result("test_convert_trainer_null_params", passed);
    assert(passed);
}

// Test 12: FreeTrainerData properly frees memory
void test_free_trainer_data() {
    TrainerData data;
    InitTrainerData(&data);
    
    // Allocate party
    data.party_size = 3;
    data.party = (TrainerPokemon*)calloc(3, sizeof(TrainerPokemon));
    
    // Free the data
    FreeTrainerData(&data);
    
    bool passed = (data.party == NULL && data.party_size == 0);
    
    print_test_result("test_free_trainer_data", passed);
    assert(passed);
}

int main() {
    printf("Running Trainer Data Converter Unit Tests\n");
    printf("==========================================\n\n");
    
    test_init_trainer_data();
    test_map_trainer_class_exact();
    test_map_trainer_class_approximate();
    test_map_trainer_class_unknown();
    test_convert_trainer_name();
    test_convert_trainer_reward();
    test_convert_trainer_ai_flags();
    test_convert_trainer_party();
    test_convert_trainer_empty_party();
    test_convert_trainer_text();
    test_convert_trainer_null_params();
    test_free_trainer_data();
    
    printf("\n==========================================\n");
    printf("All tests passed!\n");
    
    return 0;
}
