// test_property_12_13_encounter_conversion.c
// Property-based tests for encounter conversion
// Property 12: Encounter Rate Preservation
// Property 13: Pokemon Species Preservation
// Validates: Requirements 9.3, 9.4

#include "map_converter.h"
#include "theft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Test environment structure
typedef struct {
    int encounters_tested;
    uint8_t min_rate;
    uint8_t max_rate;
    uint16_t min_species;
    uint16_t max_species;
    uint8_t min_level;
    uint8_t max_level;
} test_env;

// Helper function to generate random encounter slot
static void generate_encounter_slot(struct theft *t, EncounterSlot *slot) {
    // Random species (1-386 for Gen 3, 0 for empty slot)
    // Allow some empty slots (species 0)
    if (theft_random_choice(t, 10) < 3) {
        slot->species = 0;
        slot->min_level = 0;
        slot->max_level = 0;
    } else {
        slot->species = (uint16_t)(theft_random_choice(t, 386) + 1);
        
        // Random level range (1-100)
        uint8_t level1 = (uint8_t)(theft_random_choice(t, 100) + 1);
        uint8_t level2 = (uint8_t)(theft_random_choice(t, 100) + 1);
        
        // Ensure min <= max
        if (level1 <= level2) {
            slot->min_level = level1;
            slot->max_level = level2;
        } else {
            slot->min_level = level2;
            slot->max_level = level1;
        }
    }
}

// Alloc callback: Generate random WildEncounterData
static enum theft_alloc_res
alloc_encounter_data(struct theft *t, void *env, void **instance) {
    test_env *test_env_ptr = (test_env *)env;
    
    WildEncounterData *encounters = (WildEncounterData *)malloc(sizeof(WildEncounterData));
    if (!encounters) {
        return THEFT_ALLOC_ERROR;
    }
    
    InitWildEncounterData(encounters);
    
    // Generate random encounter rate (0-255)
    encounters->encounter_rate = (uint8_t)theft_random_choice(t, 256);
    
    // Track min/max rates for statistics
    if (test_env_ptr) {
        if (encounters->encounter_rate < test_env_ptr->min_rate) {
            test_env_ptr->min_rate = encounters->encounter_rate;
        }
        if (encounters->encounter_rate > test_env_ptr->max_rate) {
            test_env_ptr->max_rate = encounters->encounter_rate;
        }
    }
    
    // Generate grass encounters (12 slots)
    for (int i = 0; i < 12; i++) {
        generate_encounter_slot(t, &encounters->grass_encounters[i]);
        
        // Track species range
        if (test_env_ptr && encounters->grass_encounters[i].species > 0) {
            if (encounters->grass_encounters[i].species < test_env_ptr->min_species) {
                test_env_ptr->min_species = encounters->grass_encounters[i].species;
            }
            if (encounters->grass_encounters[i].species > test_env_ptr->max_species) {
                test_env_ptr->max_species = encounters->grass_encounters[i].species;
            }
            if (encounters->grass_encounters[i].min_level < test_env_ptr->min_level) {
                test_env_ptr->min_level = encounters->grass_encounters[i].min_level;
            }
            if (encounters->grass_encounters[i].max_level > test_env_ptr->max_level) {
                test_env_ptr->max_level = encounters->grass_encounters[i].max_level;
            }
        }
    }
    
    // Generate water encounters (5 slots)
    for (int i = 0; i < 5; i++) {
        generate_encounter_slot(t, &encounters->water_encounters[i]);
    }
    
    // Generate fishing encounters (5 slots each for old/good/super)
    for (int i = 0; i < 5; i++) {
        generate_encounter_slot(t, &encounters->fishing_old[i]);
        generate_encounter_slot(t, &encounters->fishing_good[i]);
        generate_encounter_slot(t, &encounters->fishing_super[i]);
    }
    
    // Generate rock smash encounters (5 slots)
    for (int i = 0; i < 5; i++) {
        generate_encounter_slot(t, &encounters->rock_smash[i]);
    }
    
    *instance = encounters;
    return THEFT_ALLOC_OK;
}

// Free callback
static void
free_encounter_data(void *instance, void *env) {
    (void)env;
    WildEncounterData *encounters = (WildEncounterData *)instance;
    free(encounters);
}

// Hash callback
static theft_hash
hash_encounter_data(const void *instance, void *env) {
    (void)env;
    const WildEncounterData *encounters = (const WildEncounterData *)instance;
    struct theft_hasher h;
    theft_hash_init(&h);
    
    // Hash encounter rate
    theft_hash_sink(&h, (const uint8_t *)&encounters->encounter_rate, 
                    sizeof(encounters->encounter_rate));
    
    // Hash grass encounters
    for (int i = 0; i < 12; i++) {
        theft_hash_sink(&h, (const uint8_t *)&encounters->grass_encounters[i], 
                        sizeof(EncounterSlot));
    }
    
    // Hash water encounters
    for (int i = 0; i < 5; i++) {
        theft_hash_sink(&h, (const uint8_t *)&encounters->water_encounters[i], 
                        sizeof(EncounterSlot));
    }
    
    // Hash fishing encounters
    for (int i = 0; i < 5; i++) {
        theft_hash_sink(&h, (const uint8_t *)&encounters->fishing_old[i], 
                        sizeof(EncounterSlot));
        theft_hash_sink(&h, (const uint8_t *)&encounters->fishing_good[i], 
                        sizeof(EncounterSlot));
        theft_hash_sink(&h, (const uint8_t *)&encounters->fishing_super[i], 
                        sizeof(EncounterSlot));
    }
    
    // Hash rock smash encounters
    for (int i = 0; i < 5; i++) {
        theft_hash_sink(&h, (const uint8_t *)&encounters->rock_smash[i], 
                        sizeof(EncounterSlot));
    }
    
    return theft_hash_done(&h);
}

// Print callback
static void
print_encounter_data(FILE *f, const void *instance, void *env) {
    (void)env;
    const WildEncounterData *encounters = (const WildEncounterData *)instance;
    
    fprintf(f, "WildEncounterData{rate=%u, grass=[", encounters->encounter_rate);
    
    // Print first few grass encounters
    int grass_count = 0;
    for (int i = 0; i < 12 && grass_count < 3; i++) {
        if (encounters->grass_encounters[i].species > 0) {
            if (grass_count > 0) fprintf(f, ", ");
            fprintf(f, "{sp=%u,lvl=%u-%u}", 
                    encounters->grass_encounters[i].species,
                    encounters->grass_encounters[i].min_level,
                    encounters->grass_encounters[i].max_level);
            grass_count++;
        }
    }
    fprintf(f, "...]}");
}

// Type info for WildEncounterData
static struct theft_type_info encounter_data_info = {
    .alloc = alloc_encounter_data,
    .free = free_encounter_data,
    .hash = hash_encounter_data,
    .print = print_encounter_data,
};

/**
 * Property 12: Encounter Rate Preservation
 * 
 * For any wild encounter table converted from Emerald to FireRed format,
 * the encounter rates should remain unchanged.
 * 
 * This property tests that:
 * 1. The encounter_rate field is correctly copied during conversion
 * 2. No arithmetic operations or transformations are applied to the rate
 * 3. The rate value is preserved exactly, regardless of its magnitude (0-255)
 * 
 * **Validates: Requirements 9.3**
 */
static enum theft_trial_res
prop_encounter_rate_preservation(struct theft *t, void *arg1) {
    (void)t;
    WildEncounterData *emerald_encounters = (WildEncounterData *)arg1;
    WildEncounterData firered_encounters;
    char error_buffer[ERROR_BUFFER_SIZE];
    
    // Store original rate for comparison
    uint8_t original_rate = emerald_encounters->encounter_rate;
    
    // Convert the encounters
    if (!ConvertEncounters(emerald_encounters, &firered_encounters, 
                           error_buffer, sizeof(error_buffer))) {
        fprintf(stderr, "ConvertEncounters failed: %s\n", error_buffer);
        return THEFT_TRIAL_ERROR;
    }
    
    // Verify encounter rate is preserved
    if (firered_encounters.encounter_rate != original_rate) {
        fprintf(stderr, "Encounter rate not preserved: original=%u, converted=%u\n",
                original_rate, firered_encounters.encounter_rate);
        return THEFT_TRIAL_FAIL;
    }
    
    return THEFT_TRIAL_PASS;
}

/**
 * Property 13: Pokemon Species Preservation
 * 
 * For any wild encounter table converted from Emerald to FireRed format,
 * all Pokemon species and their level ranges should remain unchanged.
 * 
 * This property tests that:
 * 1. All encounter slots preserve their species IDs
 * 2. All encounter slots preserve their min_level values
 * 3. All encounter slots preserve their max_level values
 * 4. This applies to all encounter types: grass, water, fishing, rock smash
 * 
 * **Validates: Requirements 9.4**
 */
static enum theft_trial_res
prop_pokemon_species_preservation(struct theft *t, void *arg1) {
    (void)t;
    WildEncounterData *emerald_encounters = (WildEncounterData *)arg1;
    WildEncounterData firered_encounters;
    char error_buffer[ERROR_BUFFER_SIZE];
    
    // Convert the encounters
    if (!ConvertEncounters(emerald_encounters, &firered_encounters, 
                           error_buffer, sizeof(error_buffer))) {
        fprintf(stderr, "ConvertEncounters failed: %s\n", error_buffer);
        return THEFT_TRIAL_ERROR;
    }
    
    // Verify grass encounters are preserved
    for (int i = 0; i < 12; i++) {
        if (emerald_encounters->grass_encounters[i].species != 
            firered_encounters.grass_encounters[i].species) {
            fprintf(stderr, "Grass encounter %d species not preserved: %u != %u\n",
                    i, emerald_encounters->grass_encounters[i].species,
                    firered_encounters.grass_encounters[i].species);
            return THEFT_TRIAL_FAIL;
        }
        
        if (emerald_encounters->grass_encounters[i].min_level != 
            firered_encounters.grass_encounters[i].min_level) {
            fprintf(stderr, "Grass encounter %d min_level not preserved: %u != %u\n",
                    i, emerald_encounters->grass_encounters[i].min_level,
                    firered_encounters.grass_encounters[i].min_level);
            return THEFT_TRIAL_FAIL;
        }
        
        if (emerald_encounters->grass_encounters[i].max_level != 
            firered_encounters.grass_encounters[i].max_level) {
            fprintf(stderr, "Grass encounter %d max_level not preserved: %u != %u\n",
                    i, emerald_encounters->grass_encounters[i].max_level,
                    firered_encounters.grass_encounters[i].max_level);
            return THEFT_TRIAL_FAIL;
        }
    }
    
    // Verify water encounters are preserved
    for (int i = 0; i < 5; i++) {
        if (emerald_encounters->water_encounters[i].species != 
            firered_encounters.water_encounters[i].species) {
            fprintf(stderr, "Water encounter %d species not preserved: %u != %u\n",
                    i, emerald_encounters->water_encounters[i].species,
                    firered_encounters.water_encounters[i].species);
            return THEFT_TRIAL_FAIL;
        }
        
        if (emerald_encounters->water_encounters[i].min_level != 
            firered_encounters.water_encounters[i].min_level) {
            fprintf(stderr, "Water encounter %d min_level not preserved: %u != %u\n",
                    i, emerald_encounters->water_encounters[i].min_level,
                    firered_encounters.water_encounters[i].min_level);
            return THEFT_TRIAL_FAIL;
        }
        
        if (emerald_encounters->water_encounters[i].max_level != 
            firered_encounters.water_encounters[i].max_level) {
            fprintf(stderr, "Water encounter %d max_level not preserved: %u != %u\n",
                    i, emerald_encounters->water_encounters[i].max_level,
                    firered_encounters.water_encounters[i].max_level);
            return THEFT_TRIAL_FAIL;
        }
    }
    
    // Verify fishing encounters are preserved (old rod)
    for (int i = 0; i < 5; i++) {
        if (emerald_encounters->fishing_old[i].species != 
            firered_encounters.fishing_old[i].species) {
            fprintf(stderr, "Fishing old %d species not preserved: %u != %u\n",
                    i, emerald_encounters->fishing_old[i].species,
                    firered_encounters.fishing_old[i].species);
            return THEFT_TRIAL_FAIL;
        }
        
        if (emerald_encounters->fishing_old[i].min_level != 
            firered_encounters.fishing_old[i].min_level ||
            emerald_encounters->fishing_old[i].max_level != 
            firered_encounters.fishing_old[i].max_level) {
            fprintf(stderr, "Fishing old %d level range not preserved\n", i);
            return THEFT_TRIAL_FAIL;
        }
    }
    
    // Verify fishing encounters are preserved (good rod)
    for (int i = 0; i < 5; i++) {
        if (emerald_encounters->fishing_good[i].species != 
            firered_encounters.fishing_good[i].species) {
            fprintf(stderr, "Fishing good %d species not preserved: %u != %u\n",
                    i, emerald_encounters->fishing_good[i].species,
                    firered_encounters.fishing_good[i].species);
            return THEFT_TRIAL_FAIL;
        }
        
        if (emerald_encounters->fishing_good[i].min_level != 
            firered_encounters.fishing_good[i].min_level ||
            emerald_encounters->fishing_good[i].max_level != 
            firered_encounters.fishing_good[i].max_level) {
            fprintf(stderr, "Fishing good %d level range not preserved\n", i);
            return THEFT_TRIAL_FAIL;
        }
    }
    
    // Verify fishing encounters are preserved (super rod)
    for (int i = 0; i < 5; i++) {
        if (emerald_encounters->fishing_super[i].species != 
            firered_encounters.fishing_super[i].species) {
            fprintf(stderr, "Fishing super %d species not preserved: %u != %u\n",
                    i, emerald_encounters->fishing_super[i].species,
                    firered_encounters.fishing_super[i].species);
            return THEFT_TRIAL_FAIL;
        }
        
        if (emerald_encounters->fishing_super[i].min_level != 
            firered_encounters.fishing_super[i].min_level ||
            emerald_encounters->fishing_super[i].max_level != 
            firered_encounters.fishing_super[i].max_level) {
            fprintf(stderr, "Fishing super %d level range not preserved\n", i);
            return THEFT_TRIAL_FAIL;
        }
    }
    
    // Verify rock smash encounters are preserved
    for (int i = 0; i < 5; i++) {
        if (emerald_encounters->rock_smash[i].species != 
            firered_encounters.rock_smash[i].species) {
            fprintf(stderr, "Rock smash %d species not preserved: %u != %u\n",
                    i, emerald_encounters->rock_smash[i].species,
                    firered_encounters.rock_smash[i].species);
            return THEFT_TRIAL_FAIL;
        }
        
        if (emerald_encounters->rock_smash[i].min_level != 
            firered_encounters.rock_smash[i].min_level ||
            emerald_encounters->rock_smash[i].max_level != 
            firered_encounters.rock_smash[i].max_level) {
            fprintf(stderr, "Rock smash %d level range not preserved\n", i);
            return THEFT_TRIAL_FAIL;
        }
    }
    
    return THEFT_TRIAL_PASS;
}

int main(int argc, char **argv) {
    printf("=== Property 12 & 13: Encounter Conversion Tests ===\n");
    printf("Property 12: Encounter Rate Preservation (Validates: Requirements 9.3)\n");
    printf("Property 13: Pokemon Species Preservation (Validates: Requirements 9.4)\n");
    printf("Testing that encounter rates, species, and level ranges are preserved\n");
    printf("during Emerald to FireRed conversion.\n\n");
    
    // Get seed from time or command line
    theft_seed seed = theft_seed_of_time();
    if (argc > 1) {
        seed = atoll(argv[1]);
        printf("Using provided seed: %llu\n", (unsigned long long)seed);
    } else {
        printf("Using time-based seed: %llu\n", (unsigned long long)seed);
    }
    
    test_env env = {
        .encounters_tested = 0,
        .min_rate = 255,
        .max_rate = 0,
        .min_species = 386,
        .max_species = 0,
        .min_level = 100,
        .max_level = 0
    };
    
    // Test Property 12: Encounter Rate Preservation
    printf("\n--- Testing Property 12: Encounter Rate Preservation ---\n");
    struct theft_run_config config12 = {
        .name = "prop_encounter_rate_preservation",
        .prop1 = prop_encounter_rate_preservation,
        .type_info = { &encounter_data_info },
        .trials = 100,  // Minimum 100 iterations as per spec
        .seed = seed,
        .hooks = {
            .env = &env,
        },
    };
    
    printf("Running %zu trials...\n", config12.trials);
    enum theft_run_res res12 = theft_run(&config12);
    
    printf("\n=== Property 12 Results ===\n");
    printf("Encounter rate range tested: %u - %u\n", env.min_rate, env.max_rate);
    
    bool prop12_passed = false;
    switch (res12) {
        case THEFT_RUN_PASS:
            printf("✓ Property 12 PASSED: All %zu trials succeeded\n", config12.trials);
            printf("Encounter rates are correctly preserved during conversion.\n");
            prop12_passed = true;
            break;
            
        case THEFT_RUN_FAIL:
            printf("✗ Property 12 FAILED: Found counterexample(s)\n");
            printf("Encounter rates are not preserved during conversion.\n");
            break;
            
        case THEFT_RUN_SKIP:
            printf("⊘ Property 12 SKIPPED: All trials were skipped\n");
            break;
            
        case THEFT_RUN_ERROR:
            printf("✗ Property 12 ERROR: Test encountered an error\n");
            break;
            
        default:
            printf("✗ Property 12 UNKNOWN: Unexpected result code %d\n", res12);
            break;
    }
    
    // Test Property 13: Pokemon Species Preservation
    printf("\n--- Testing Property 13: Pokemon Species Preservation ---\n");
    struct theft_run_config config13 = {
        .name = "prop_pokemon_species_preservation",
        .prop1 = prop_pokemon_species_preservation,
        .type_info = { &encounter_data_info },
        .trials = 100,  // Minimum 100 iterations as per spec
        .seed = seed,
        .hooks = {
            .env = &env,
        },
    };
    
    printf("Running %zu trials...\n", config13.trials);
    enum theft_run_res res13 = theft_run(&config13);
    
    printf("\n=== Property 13 Results ===\n");
    printf("Species range tested: %u - %u\n", env.min_species, env.max_species);
    printf("Level range tested: %u - %u\n", env.min_level, env.max_level);
    
    bool prop13_passed = false;
    switch (res13) {
        case THEFT_RUN_PASS:
            printf("✓ Property 13 PASSED: All %zu trials succeeded\n", config13.trials);
            printf("Pokemon species and level ranges are correctly preserved.\n");
            printf("All encounter types (grass, water, fishing, rock smash) verified.\n");
            prop13_passed = true;
            break;
            
        case THEFT_RUN_FAIL:
            printf("✗ Property 13 FAILED: Found counterexample(s)\n");
            printf("Pokemon species or level ranges are not preserved.\n");
            break;
            
        case THEFT_RUN_SKIP:
            printf("⊘ Property 13 SKIPPED: All trials were skipped\n");
            break;
            
        case THEFT_RUN_ERROR:
            printf("✗ Property 13 ERROR: Test encountered an error\n");
            break;
            
        default:
            printf("✗ Property 13 UNKNOWN: Unexpected result code %d\n", res13);
            break;
    }
    
    // Overall result
    printf("\n=== Overall Test Results ===\n");
    if (prop12_passed && prop13_passed) {
        printf("✓ ALL PROPERTIES PASSED\n");
        printf("Encounter conversion correctly preserves rates, species, and level ranges.\n");
        return 0;
    } else {
        printf("✗ SOME PROPERTIES FAILED\n");
        if (!prop12_passed) printf("  - Property 12 (Encounter Rate Preservation) failed\n");
        if (!prop13_passed) printf("  - Property 13 (Pokemon Species Preservation) failed\n");
        return 1;
    }
}
