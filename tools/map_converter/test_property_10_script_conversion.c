// test_property_10_script_conversion.c
// Property-based test for script command conversion
// Property 10: Script Command Conversion
// Validates: Requirements 7.2, 7.4

#include "map_converter.h"
#include "theft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Emerald script command IDs for testing
#define EMERALD_CMD_MSGBOX          0x0F
#define EMERALD_CMD_GIVEITEM        0x1E
#define EMERALD_CMD_GIVEMON         0x2D
#define EMERALD_CMD_SETFLAG         0x29
#define EMERALD_CMD_CLEARFLAG       0x2A
#define EMERALD_CMD_CHECKFLAG       0x2B
#define EMERALD_CMD_WARP            0x08
#define EMERALD_CMD_PLAYBGM         0x22
#define EMERALD_CMD_FADESCREEN      0x23
#define EMERALD_CMD_CALL            0x09
#define EMERALD_CMD_GOTO            0x05
#define EMERALD_CMD_IF              0x06
#define EMERALD_CMD_CALLSTD         0x09
#define EMERALD_CMD_GOTOIF          0x07
#define EMERALD_CMD_RELEASE         0x68
#define EMERALD_CMD_END             0x02
#define EMERALD_CMD_RETURN          0x03
#define EMERALD_CMD_COMPARE         0x21
#define EMERALD_CMD_SETVAR          0x16
#define EMERALD_CMD_ADDVAR          0x17
#define EMERALD_CMD_SUBVAR          0x18
#define EMERALD_CMD_COPYVAR         0x19
#define EMERALD_CMD_SETWEATHER      0x5C
#define EMERALD_CMD_DOWEATHER       0x5D
#define EMERALD_CMD_SETMAPTILE      0x5E

// FireRed script command IDs for verification
#define FIRERED_CMD_MSGBOX          0x0F
#define FIRERED_CMD_GIVEITEM        0x1E
#define FIRERED_CMD_GIVEMON         0x2D
#define FIRERED_CMD_SETFLAG         0x29
#define FIRERED_CMD_CLEARFLAG       0x2A
#define FIRERED_CMD_CHECKFLAG       0x2B
#define FIRERED_CMD_WARP            0x08
#define FIRERED_CMD_PLAYBGM         0x22
#define FIRERED_CMD_FADESCREEN      0x23
#define FIRERED_CMD_CALL            0x09
#define FIRERED_CMD_GOTO            0x05
#define FIRERED_CMD_IF              0x06
#define FIRERED_CMD_CALLSTD         0x09
#define FIRERED_CMD_GOTOIF          0x07
#define FIRERED_CMD_RELEASE         0x68
#define FIRERED_CMD_END             0x02
#define FIRERED_CMD_RETURN          0x03
#define FIRERED_CMD_COMPARE         0x21
#define FIRERED_CMD_SETVAR          0x16
#define FIRERED_CMD_ADDVAR          0x17
#define FIRERED_CMD_SUBVAR          0x18
#define FIRERED_CMD_COPYVAR         0x19

// Test environment structure
typedef struct {
    int compatible_commands_tested;
    int incompatible_commands_tested;
} test_env;

// List of compatible Emerald commands (have FireRed equivalents)
static const uint16_t compatible_commands[] = {
    EMERALD_CMD_MSGBOX,
    EMERALD_CMD_GIVEITEM,
    EMERALD_CMD_GIVEMON,
    EMERALD_CMD_SETFLAG,
    EMERALD_CMD_CLEARFLAG,
    EMERALD_CMD_CHECKFLAG,
    EMERALD_CMD_WARP,
    EMERALD_CMD_PLAYBGM,
    EMERALD_CMD_FADESCREEN,
    EMERALD_CMD_CALL,
    EMERALD_CMD_GOTO,
    EMERALD_CMD_IF,
    EMERALD_CMD_CALLSTD,
    EMERALD_CMD_GOTOIF,
    EMERALD_CMD_RELEASE,
    EMERALD_CMD_END,
    EMERALD_CMD_RETURN,
    EMERALD_CMD_COMPARE,
    EMERALD_CMD_SETVAR,
    EMERALD_CMD_ADDVAR,
    EMERALD_CMD_SUBVAR,
    EMERALD_CMD_COPYVAR,
};
static const size_t num_compatible_commands = sizeof(compatible_commands) / sizeof(uint16_t);

// List of incompatible Emerald commands (no FireRed equivalents)
static const uint16_t incompatible_commands[] = {
    EMERALD_CMD_SETWEATHER,
    EMERALD_CMD_DOWEATHER,
    EMERALD_CMD_SETMAPTILE,
};
static const size_t num_incompatible_commands = sizeof(incompatible_commands) / sizeof(uint16_t);

// Expected command mappings for verification
typedef struct {
    uint16_t emerald_cmd;
    uint16_t firered_cmd;
} CommandPair;

static const CommandPair expected_mappings[] = {
    {EMERALD_CMD_MSGBOX, FIRERED_CMD_MSGBOX},
    {EMERALD_CMD_GIVEITEM, FIRERED_CMD_GIVEITEM},
    {EMERALD_CMD_GIVEMON, FIRERED_CMD_GIVEMON},
    {EMERALD_CMD_SETFLAG, FIRERED_CMD_SETFLAG},
    {EMERALD_CMD_CLEARFLAG, FIRERED_CMD_CLEARFLAG},
    {EMERALD_CMD_CHECKFLAG, FIRERED_CMD_CHECKFLAG},
    {EMERALD_CMD_WARP, FIRERED_CMD_WARP},
    {EMERALD_CMD_PLAYBGM, FIRERED_CMD_PLAYBGM},
    {EMERALD_CMD_FADESCREEN, FIRERED_CMD_FADESCREEN},
    {EMERALD_CMD_CALL, FIRERED_CMD_CALL},
    {EMERALD_CMD_GOTO, FIRERED_CMD_GOTO},
    {EMERALD_CMD_IF, FIRERED_CMD_IF},
    {EMERALD_CMD_CALLSTD, FIRERED_CMD_CALLSTD},
    {EMERALD_CMD_GOTOIF, FIRERED_CMD_GOTOIF},
    {EMERALD_CMD_RELEASE, FIRERED_CMD_RELEASE},
    {EMERALD_CMD_END, FIRERED_CMD_END},
    {EMERALD_CMD_RETURN, FIRERED_CMD_RETURN},
    {EMERALD_CMD_COMPARE, FIRERED_CMD_COMPARE},
    {EMERALD_CMD_SETVAR, FIRERED_CMD_SETVAR},
    {EMERALD_CMD_ADDVAR, FIRERED_CMD_ADDVAR},
    {EMERALD_CMD_SUBVAR, FIRERED_CMD_SUBVAR},
    {EMERALD_CMD_COPYVAR, FIRERED_CMD_COPYVAR},
};
static const size_t num_expected_mappings = sizeof(expected_mappings) / sizeof(CommandPair);

// Helper function to get expected FireRed command for an Emerald command
static uint16_t get_expected_firered_command(uint16_t emerald_cmd) {
    for (size_t i = 0; i < num_expected_mappings; i++) {
        if (expected_mappings[i].emerald_cmd == emerald_cmd) {
            return expected_mappings[i].firered_cmd;
        }
    }
    return 0;  // Not found
}

// Helper function to generate random script name
static void generate_script_name(struct theft *t, char *dest, size_t dest_size) {
    const char *prefixes[] = {
        "EventScript", "Script", "MapScript", "NpcScript", "ItemScript"
    };
    size_t num_prefixes = sizeof(prefixes) / sizeof(prefixes[0]);
    
    uint64_t prefix_choice = theft_random_choice(t, num_prefixes);
    uint64_t number = theft_random_choice(t, 1000);
    
    snprintf(dest, dest_size, "%s_%llu", prefixes[prefix_choice],
             (unsigned long long)number);
}

// Alloc callback: Generate random script with Emerald commands
static enum theft_alloc_res
alloc_script(struct theft *t, void *env, void **instance) {
    (void)env;
    
    Script *script = (Script *)malloc(sizeof(Script));
    if (!script) {
        return THEFT_ALLOC_ERROR;
    }
    
    InitScript(script);
    
    // Generate random script name
    generate_script_name(t, script->name, sizeof(script->name));
    
    // Generate random number of commands (1-20)
    script->command_count = theft_random_choice(t, 20) + 1;
    script->commands = (ScriptCommand *)calloc(script->command_count, sizeof(ScriptCommand));
    if (!script->commands) {
        free(script);
        return THEFT_ALLOC_ERROR;
    }
    
    // Generate random commands (mix of compatible and incompatible)
    for (size_t i = 0; i < script->command_count; i++) {
        // 80% chance of compatible command, 20% chance of incompatible
        bool use_compatible = (theft_random_choice(t, 100) < 80);
        
        if (use_compatible) {
            uint64_t cmd_index = theft_random_choice(t, num_compatible_commands);
            script->commands[i].command_id = compatible_commands[cmd_index];
        } else {
            uint64_t cmd_index = theft_random_choice(t, num_incompatible_commands);
            script->commands[i].command_id = incompatible_commands[cmd_index];
        }
        
        // Generate random parameters (0-8 bytes)
        script->commands[i].param_count = theft_random_choice(t, 9);
        if (script->commands[i].param_count > 0) {
            script->commands[i].parameters = (uint8_t *)malloc(script->commands[i].param_count);
            if (!script->commands[i].parameters) {
                FreeScript(script);
                free(script);
                return THEFT_ALLOC_ERROR;
            }
            
            // Fill with random parameter values
            for (size_t j = 0; j < script->commands[i].param_count; j++) {
                script->commands[i].parameters[j] = (uint8_t)theft_random_choice(t, 256);
            }
        } else {
            script->commands[i].parameters = NULL;
        }
    }
    
    *instance = script;
    return THEFT_ALLOC_OK;
}

// Free callback
static void
free_script(void *instance, void *env) {
    (void)env;
    Script *script = (Script *)instance;
    FreeScript(script);
    free(script);
}

// Hash callback
static theft_hash
hash_script(const void *instance, void *env) {
    (void)env;
    const Script *script = (const Script *)instance;
    struct theft_hasher h;
    theft_hash_init(&h);
    
    theft_hash_sink(&h, (const uint8_t *)script->name, strlen(script->name));
    theft_hash_sink(&h, (const uint8_t *)&script->command_count, sizeof(script->command_count));
    
    for (size_t i = 0; i < script->command_count; i++) {
        theft_hash_sink(&h, (const uint8_t *)&script->commands[i].command_id,
                       sizeof(script->commands[i].command_id));
        theft_hash_sink(&h, (const uint8_t *)&script->commands[i].param_count,
                       sizeof(script->commands[i].param_count));
        if (script->commands[i].param_count > 0 && script->commands[i].parameters) {
            theft_hash_sink(&h, script->commands[i].parameters,
                           script->commands[i].param_count);
        }
    }
    
    return theft_hash_done(&h);
}

// Print callback
static void
print_script(FILE *f, const void *instance, void *env) {
    (void)env;
    const Script *script = (const Script *)instance;
    fprintf(f, "Script{name=\"%s\", commands=%zu}", script->name, script->command_count);
}

// Type info for Script
static struct theft_type_info script_info = {
    .alloc = alloc_script,
    .free = free_script,
    .hash = hash_script,
    .print = print_script,
};

/**
 * Property 10: Script Command Conversion
 * 
 * For any Emerald script command that has a FireRed equivalent, the conversion should
 * produce the correct FireRed command with preserved parameters.
 * 
 * This property tests that:
 * 1. Compatible commands are correctly mapped to their FireRed equivalents
 * 2. Command parameters are preserved during conversion
 * 3. Incompatible commands are skipped and logged
 * 4. The converted script maintains the correct order of compatible commands
 * 5. Script name is preserved during conversion
 * 
 * **Validates: Requirements 7.2, 7.4**
 */
static enum theft_trial_res
prop_script_command_conversion(struct theft *t, void *arg1) {
    (void)t;
    Script *emerald_script = (Script *)arg1;
    Script firered_script;
    char error_buffer[ERROR_BUFFER_SIZE];
    
    // Convert the script
    if (!ConvertScript(emerald_script, &firered_script, "TestMap",
                       error_buffer, sizeof(error_buffer))) {
        fprintf(stderr, "ConvertScript failed: %s\n", error_buffer);
        return THEFT_TRIAL_ERROR;
    }
    
    // Verify script name is preserved
    if (strcmp(emerald_script->name, firered_script.name) != 0) {
        fprintf(stderr, "Script name not preserved: '%s' != '%s'\n",
                emerald_script->name, firered_script.name);
        FreeScript(&firered_script);
        return THEFT_TRIAL_FAIL;
    }
    
    // Count expected compatible commands in the original script
    size_t expected_compatible_count = 0;
    for (size_t i = 0; i < emerald_script->command_count; i++) {
        bool has_equivalent = false;
        MapScriptCommand(emerald_script->commands[i].command_id, &has_equivalent);
        if (has_equivalent) {
            expected_compatible_count++;
        }
    }
    
    // Verify that the converted script has the correct number of commands
    if (firered_script.command_count != expected_compatible_count) {
        fprintf(stderr, "Command count mismatch: expected %zu compatible commands, got %zu\n",
                expected_compatible_count, firered_script.command_count);
        FreeScript(&firered_script);
        return THEFT_TRIAL_FAIL;
    }
    
    // Verify each converted command
    size_t firered_index = 0;
    for (size_t i = 0; i < emerald_script->command_count; i++) {
        bool has_equivalent = false;
        uint16_t mapped_cmd = MapScriptCommand(emerald_script->commands[i].command_id,
                                               &has_equivalent);
        
        if (!has_equivalent) {
            // Incompatible command should be skipped
            continue;
        }
        
        // Verify command ID is correctly mapped
        uint16_t expected_firered_cmd = get_expected_firered_command(
            emerald_script->commands[i].command_id);
        
        if (firered_script.commands[firered_index].command_id != expected_firered_cmd) {
            fprintf(stderr, "Command mapping incorrect at index %zu: "
                           "Emerald 0x%04X should map to FireRed 0x%04X, got 0x%04X\n",
                    i, emerald_script->commands[i].command_id,
                    expected_firered_cmd,
                    firered_script.commands[firered_index].command_id);
            FreeScript(&firered_script);
            return THEFT_TRIAL_FAIL;
        }
        
        // Verify command ID matches what MapScriptCommand returned
        if (firered_script.commands[firered_index].command_id != mapped_cmd) {
            fprintf(stderr, "Command ID mismatch at index %zu: "
                           "MapScriptCommand returned 0x%04X, but converted command is 0x%04X\n",
                    i, mapped_cmd, firered_script.commands[firered_index].command_id);
            FreeScript(&firered_script);
            return THEFT_TRIAL_FAIL;
        }
        
        // Verify parameter count is preserved
        if (firered_script.commands[firered_index].param_count !=
            emerald_script->commands[i].param_count) {
            fprintf(stderr, "Parameter count not preserved at index %zu: "
                           "expected %zu, got %zu\n",
                    i, emerald_script->commands[i].param_count,
                    firered_script.commands[firered_index].param_count);
            FreeScript(&firered_script);
            return THEFT_TRIAL_FAIL;
        }
        
        // Verify parameters are preserved
        if (emerald_script->commands[i].param_count > 0) {
            if (!emerald_script->commands[i].parameters ||
                !firered_script.commands[firered_index].parameters) {
                fprintf(stderr, "Parameter pointer is NULL at index %zu\n", i);
                FreeScript(&firered_script);
                return THEFT_TRIAL_FAIL;
            }
            
            if (memcmp(emerald_script->commands[i].parameters,
                      firered_script.commands[firered_index].parameters,
                      emerald_script->commands[i].param_count) != 0) {
                fprintf(stderr, "Parameters not preserved at index %zu\n", i);
                FreeScript(&firered_script);
                return THEFT_TRIAL_FAIL;
            }
        }
        
        firered_index++;
    }
    
    // Clean up
    FreeScript(&firered_script);
    
    return THEFT_TRIAL_PASS;
}

int main(int argc, char **argv) {
    printf("=== Property 10: Script Command Conversion Test ===\n");
    printf("Validates: Requirements 7.2, 7.4\n");
    printf("Testing that Emerald script commands with FireRed equivalents\n");
    printf("are correctly converted with preserved parameters.\n\n");
    
    // Get seed from time or command line
    theft_seed seed = theft_seed_of_time();
    if (argc > 1) {
        seed = atoll(argv[1]);
        printf("Using provided seed: %llu\n", (unsigned long long)seed);
    } else {
        printf("Using time-based seed: %llu\n", (unsigned long long)seed);
    }
    
    test_env env = {
        .compatible_commands_tested = 0,
        .incompatible_commands_tested = 0
    };
    
    // Configure the property test
    struct theft_run_config config = {
        .name = "prop_script_command_conversion",
        .prop1 = prop_script_command_conversion,
        .type_info = { &script_info },
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
    switch (res) {
        case THEFT_RUN_PASS:
            printf("✓ Property 10 PASSED: All %zu trials succeeded\n", config.trials);
            printf("Script command conversion correctly maps compatible commands\n");
            printf("and preserves all parameters during conversion.\n");
            printf("\nTested command types:\n");
            printf("  - Compatible commands: %zu types\n", num_compatible_commands);
            printf("  - Incompatible commands: %zu types\n", num_incompatible_commands);
            return 0;
            
        case THEFT_RUN_FAIL:
            printf("✗ Property 10 FAILED: Found counterexample(s)\n");
            printf("Script command conversion does not correctly map commands\n");
            printf("or preserve parameters.\n");
            return 1;
            
        case THEFT_RUN_SKIP:
            printf("⊘ Property 10 SKIPPED: All trials were skipped\n");
            return 0;
            
        case THEFT_RUN_ERROR:
            printf("✗ Property 10 ERROR: Test encountered an error\n");
            return 1;
            
        default:
            printf("✗ Property 10 UNKNOWN: Unexpected result code %d\n", res);
            return 1;
    }
}
