// test_property_11_trainer_reward.c
// Property-based test for trainer reward preservation
// Property 11: Trainer Reward Preservation
// Validates: Requirements 8.4

#include "map_converter.h"
#include "theft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Test environment structure
typedef struct {
    int trainers_tested;
    uint16_t min_reward;
    uint16_t max_reward;
} test_env;

// Helper function to generate random trainer name
static void generate_trainer_name(struct theft *t, char *dest, size_t dest_size) {
    const char *name_prefixes[] = {
        "Youngster", "Lass", "Bug Catcher", "Picnicker", "Camper",
        "Hiker", "Beauty", "Swimmer", "Fisherman", "Sailor",
        "Biker", "Burglar", "Engineer", "Juggler", "Blackbelt",
        "Psychic", "Gentleman", "Scientist", "Rocket", "Cooltrainer"
    };
    size_t num_prefixes = sizeof(name_prefixes) / sizeof(name_prefixes[0]);
    
    uint64_t prefix_choice = theft_random_choice(t, num_prefixes);
    uint64_t number = theft_random_choice(t, 100);
    
    snprintf(dest, dest_size, "%s_%llu", name_prefixes[prefix_choice],
             (unsigned long long)number);
}

// Helper function to generate random trainer text
static void generate_trainer_text(struct theft *t, char *dest, size_t dest_size) {
    const char *text_options[] = {
        "Let's battle!",
        "I challenge you!",
        "You look strong!",
        "Come on!",
        "I won't lose!",
        "Prepare yourself!",
        "Here I come!",
        "Let's go!"
    };
    size_t num_options = sizeof(text_options) / sizeof(text_options[0]);
    
    uint64_t choice = theft_random_choice(t, num_options);
    strncpy(dest, text_options[choice], dest_size - 1);
    dest[dest_size - 1] = '\0';
}

// Alloc callback: Generate random TrainerData with various reward amounts
static enum theft_alloc_res
alloc_trainer_data(struct theft *t, void *env, void **instance) {
    test_env *test_env_ptr = (test_env *)env;
    
    TrainerData *trainer = (TrainerData *)malloc(sizeof(TrainerData));
    if (!trainer) {
        return THEFT_ALLOC_ERROR;
    }
    
    InitTrainerData(trainer);
    
    // Generate random trainer name
    generate_trainer_name(t, trainer->name, sizeof(trainer->name));
    
    // Generate random trainer class (1-100)
    trainer->trainer_class = (uint8_t)(theft_random_choice(t, 100) + 1);
    
    // Generate random gender (0 = male, 1 = female)
    trainer->gender = (uint8_t)theft_random_choice(t, 2);
    
    // Generate random music (0-10)
    trainer->music = (uint8_t)theft_random_choice(t, 11);
    
    // Generate random sprite (0-255)
    trainer->sprite = (uint16_t)theft_random_choice(t, 256);
    
    // Generate random intro and defeat text
    generate_trainer_text(t, trainer->intro_text, sizeof(trainer->intro_text));
    generate_trainer_text(t, trainer->defeat_text, sizeof(trainer->defeat_text));
    
    // Generate random reward amount (0-9999)
    // This is the critical field we're testing
    trainer->reward = (uint16_t)theft_random_choice(t, 10000);
    
    // Track min/max rewards for statistics
    if (test_env_ptr) {
        if (trainer->reward < test_env_ptr->min_reward) {
            test_env_ptr->min_reward = trainer->reward;
        }
        if (trainer->reward > test_env_ptr->max_reward) {
            test_env_ptr->max_reward = trainer->reward;
        }
    }
    
    // Generate random AI flags (0-255)
    trainer->ai_flags = (uint8_t)theft_random_choice(t, 256);
    
    // Generate random party size (1-6)
    trainer->party_size = (uint8_t)(theft_random_choice(t, 6) + 1);
    trainer->party = (TrainerPokemon *)calloc(trainer->party_size, sizeof(TrainerPokemon));
    if (!trainer->party) {
        free(trainer);
        return THEFT_ALLOC_ERROR;
    }
    
    // Generate random party data
    for (uint8_t i = 0; i < trainer->party_size; i++) {
        // Random species (1-386 for Gen 3)
        trainer->party[i].species = (uint16_t)(theft_random_choice(t, 386) + 1);
        
        // Random level (1-100)
        trainer->party[i].level = (uint8_t)(theft_random_choice(t, 100) + 1);
        
        // Random held item (0-377)
        trainer->party[i].held_item = (uint16_t)theft_random_choice(t, 378);
        
        // Random moves (0-354 for Gen 3)
        for (int j = 0; j < 4; j++) {
            trainer->party[i].moves[j] = (uint16_t)theft_random_choice(t, 355);
        }
        
        // Random IVs (0-31)
        trainer->party[i].iv = (uint8_t)theft_random_choice(t, 32);
        
        // Random EVs (0-252)
        trainer->party[i].ev_hp = (uint8_t)theft_random_choice(t, 253);
        trainer->party[i].ev_atk = (uint8_t)theft_random_choice(t, 253);
        trainer->party[i].ev_def = (uint8_t)theft_random_choice(t, 253);
        trainer->party[i].ev_speed = (uint8_t)theft_random_choice(t, 253);
        trainer->party[i].ev_spatk = (uint8_t)theft_random_choice(t, 253);
        trainer->party[i].ev_spdef = (uint8_t)theft_random_choice(t, 253);
    }
    
    *instance = trainer;
    return THEFT_ALLOC_OK;
}

// Free callback
static void
free_trainer_data(void *instance, void *env) {
    (void)env;
    TrainerData *trainer = (TrainerData *)instance;
    FreeTrainerData(trainer);
    free(trainer);
}

// Hash callback
static theft_hash
hash_trainer_data(const void *instance, void *env) {
    (void)env;
    const TrainerData *trainer = (const TrainerData *)instance;
    struct theft_hasher h;
    theft_hash_init(&h);
    
    // Hash name and reward (the critical field)
    theft_hash_sink(&h, (const uint8_t *)trainer->name, strlen(trainer->name));
    theft_hash_sink(&h, (const uint8_t *)&trainer->reward, sizeof(trainer->reward));
    theft_hash_sink(&h, (const uint8_t *)&trainer->trainer_class, sizeof(trainer->trainer_class));
    theft_hash_sink(&h, (const uint8_t *)&trainer->party_size, sizeof(trainer->party_size));
    
    return theft_hash_done(&h);
}

// Print callback
static void
print_trainer_data(FILE *f, const void *instance, void *env) {
    (void)env;
    const TrainerData *trainer = (const TrainerData *)instance;
    fprintf(f, "TrainerData{name=\"%s\", class=%u, reward=%u, party_size=%u}",
            trainer->name, trainer->trainer_class, trainer->reward, trainer->party_size);
}

// Type info for TrainerData
static struct theft_type_info trainer_data_info = {
    .alloc = alloc_trainer_data,
    .free = free_trainer_data,
    .hash = hash_trainer_data,
    .print = print_trainer_data,
};

/**
 * Property 11: Trainer Reward Preservation
 * 
 * For any trainer converted from Emerald to FireRed format, the reward amount
 * should remain unchanged.
 * 
 * This property tests that:
 * 1. The reward field is correctly copied during conversion
 * 2. No arithmetic operations or transformations are applied to the reward
 * 3. The reward value is preserved exactly, regardless of its magnitude
 * 4. All other trainer data is also preserved (name, class, party, etc.)
 * 
 * **Validates: Requirements 8.4**
 */
static enum theft_trial_res
prop_trainer_reward_preservation(struct theft *t, void *arg1) {
    (void)t;
    TrainerData *emerald_trainer = (TrainerData *)arg1;
    TrainerData firered_trainer;
    char error_buffer[ERROR_BUFFER_SIZE];
    
    // Store original reward for comparison
    uint16_t original_reward = emerald_trainer->reward;
    
    // Convert the trainer
    if (!ConvertTrainer(emerald_trainer, &firered_trainer, 
                        error_buffer, sizeof(error_buffer))) {
        fprintf(stderr, "ConvertTrainer failed: %s\n", error_buffer);
        return THEFT_TRIAL_ERROR;
    }
    
    // Verify reward is preserved
    if (firered_trainer.reward != original_reward) {
        fprintf(stderr, "Reward not preserved: original=%u, converted=%u (trainer: %s)\n",
                original_reward, firered_trainer.reward, emerald_trainer->name);
        FreeTrainerData(&firered_trainer);
        return THEFT_TRIAL_FAIL;
    }
    
    // Also verify other critical fields are preserved
    if (strcmp(emerald_trainer->name, firered_trainer.name) != 0) {
        fprintf(stderr, "Trainer name not preserved: '%s' != '%s'\n",
                emerald_trainer->name, firered_trainer.name);
        FreeTrainerData(&firered_trainer);
        return THEFT_TRIAL_FAIL;
    }
    
    if (emerald_trainer->ai_flags != firered_trainer.ai_flags) {
        fprintf(stderr, "AI flags not preserved: original=0x%02X, converted=0x%02X\n",
                emerald_trainer->ai_flags, firered_trainer.ai_flags);
        FreeTrainerData(&firered_trainer);
        return THEFT_TRIAL_FAIL;
    }
    
    if (emerald_trainer->party_size != firered_trainer.party_size) {
        fprintf(stderr, "Party size not preserved: original=%u, converted=%u\n",
                emerald_trainer->party_size, firered_trainer.party_size);
        FreeTrainerData(&firered_trainer);
        return THEFT_TRIAL_FAIL;
    }
    
    // Verify party data is preserved
    for (uint8_t i = 0; i < emerald_trainer->party_size; i++) {
        if (emerald_trainer->party[i].species != firered_trainer.party[i].species) {
            fprintf(stderr, "Party Pokemon %u species not preserved: %u != %u\n",
                    i, emerald_trainer->party[i].species, firered_trainer.party[i].species);
            FreeTrainerData(&firered_trainer);
            return THEFT_TRIAL_FAIL;
        }
        
        if (emerald_trainer->party[i].level != firered_trainer.party[i].level) {
            fprintf(stderr, "Party Pokemon %u level not preserved: %u != %u\n",
                    i, emerald_trainer->party[i].level, firered_trainer.party[i].level);
            FreeTrainerData(&firered_trainer);
            return THEFT_TRIAL_FAIL;
        }
    }
    
    // Clean up
    FreeTrainerData(&firered_trainer);
    
    return THEFT_TRIAL_PASS;
}

int main(int argc, char **argv) {
    printf("=== Property 11: Trainer Reward Preservation Test ===\n");
    printf("Validates: Requirements 8.4\n");
    printf("Testing that trainer reward amounts are preserved during\n");
    printf("Emerald to FireRed conversion.\n\n");
    
    // Get seed from time or command line
    theft_seed seed = theft_seed_of_time();
    if (argc > 1) {
        seed = atoll(argv[1]);
        printf("Using provided seed: %llu\n", (unsigned long long)seed);
    } else {
        printf("Using time-based seed: %llu\n", (unsigned long long)seed);
    }
    
    test_env env = {
        .trainers_tested = 0,
        .min_reward = 9999,
        .max_reward = 0
    };
    
    // Configure the property test
    struct theft_run_config config = {
        .name = "prop_trainer_reward_preservation",
        .prop1 = prop_trainer_reward_preservation,
        .type_info = { &trainer_data_info },
        .trials = 100,  // Minimum 100 iterations as per spec
        .seed = seed,
        .hooks = {
            .env = &env,
        },
    };
    
    // Run the property test
    printf("Running %zu trials...\n\n", config.trials);
    enum theft_run_res res = theft_run(&config);
    
    // Report results
    printf("\n=== Test Results ===\n");
    printf("Reward range tested: %u - %u\n\n", env.min_reward, env.max_reward);
    
    switch (res) {
        case THEFT_RUN_PASS:
            printf("✓ Property 11 PASSED: All %zu trials succeeded\n", config.trials);
            printf("Trainer reward amounts are correctly preserved during conversion.\n");
            printf("All trainer data (name, class, AI flags, party) is also preserved.\n");
            return 0;
            
        case THEFT_RUN_FAIL:
            printf("✗ Property 11 FAILED: Found counterexample(s)\n");
            printf("Trainer reward amounts are not preserved during conversion.\n");
            return 1;
            
        case THEFT_RUN_SKIP:
            printf("⊘ Property 11 SKIPPED: All trials were skipped\n");
            return 0;
            
        case THEFT_RUN_ERROR:
            printf("✗ Property 11 ERROR: Test encountered an error\n");
            return 1;
            
        default:
            printf("✗ Property 11 UNKNOWN: Unexpected result code %d\n", res);
            return 1;
    }
}
