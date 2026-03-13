// test_script_converter.c
// Unit tests for script command converter

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "map_converter.h"

// Test helper macros
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "FAIL: %s\n", message); \
            return false; \
        } \
    } while(0)

#define TEST_PASS(message) \
    do { \
        printf("PASS: %s\n", message); \
        return true; \
    } while(0)

// Emerald command IDs for testing
#define EMERALD_CMD_MSGBOX          0x0F
#define EMERALD_CMD_GIVEITEM        0x1E
#define EMERALD_CMD_SETWEATHER      0x5C
#define EMERALD_CMD_END             0x02

// FireRed command IDs for testing
#define FIRERED_CMD_MSGBOX          0x0F
#define FIRERED_CMD_GIVEITEM        0x1E
#define FIRERED_CMD_END             0x02

// Test 1: MapScriptCommand with compatible command
bool test_map_compatible_command() {
    bool has_equivalent = false;
    uint16_t result = MapScriptCommand(EMERALD_CMD_MSGBOX, &has_equivalent);
    
    TEST_ASSERT(has_equivalent == true, 
                "MSGBOX should have FireRed equivalent");
    TEST_ASSERT(result == FIRERED_CMD_MSGBOX, 
                "MSGBOX should map to FireRed MSGBOX");
    
    TEST_PASS("MapScriptCommand correctly maps compatible command");
}

// Test 2: MapScriptCommand with incompatible command
bool test_map_incompatible_command() {
    bool has_equivalent = false;
    uint16_t result = MapScriptCommand(EMERALD_CMD_SETWEATHER, &has_equivalent);
    
    TEST_ASSERT(has_equivalent == false, 
                "SETWEATHER should not have FireRed equivalent");
    TEST_ASSERT(result == 0, 
                "Incompatible command should return 0");
    
    TEST_PASS("MapScriptCommand correctly identifies incompatible command");
}

// Test 3: ConvertScript with all compatible commands
bool test_convert_compatible_script() {
    Script emerald_script;
    Script firered_script;
    char error_buffer[512];
    
    InitScript(&emerald_script);
    strcpy(emerald_script.name, "TestScript");
    
    // Create script with 3 compatible commands
    emerald_script.command_count = 3;
    emerald_script.commands = (ScriptCommand*)calloc(3, sizeof(ScriptCommand));
    
    emerald_script.commands[0].command_id = EMERALD_CMD_MSGBOX;
    emerald_script.commands[0].param_count = 2;
    emerald_script.commands[0].parameters = (uint8_t*)malloc(2);
    emerald_script.commands[0].parameters[0] = 0x01;
    emerald_script.commands[0].parameters[1] = 0x02;
    
    emerald_script.commands[1].command_id = EMERALD_CMD_GIVEITEM;
    emerald_script.commands[1].param_count = 1;
    emerald_script.commands[1].parameters = (uint8_t*)malloc(1);
    emerald_script.commands[1].parameters[0] = 0x05;
    
    emerald_script.commands[2].command_id = EMERALD_CMD_END;
    emerald_script.commands[2].param_count = 0;
    emerald_script.commands[2].parameters = NULL;
    
    // Convert script
    bool result = ConvertScript(&emerald_script, &firered_script, "TestMap", 
                               error_buffer, sizeof(error_buffer));
    
    TEST_ASSERT(result == true, "ConvertScript should succeed");
    TEST_ASSERT(firered_script.command_count == 3, 
                "Output should have 3 commands");
    TEST_ASSERT(firered_script.commands[0].command_id == FIRERED_CMD_MSGBOX,
                "First command should be MSGBOX");
    TEST_ASSERT(firered_script.commands[1].command_id == FIRERED_CMD_GIVEITEM,
                "Second command should be GIVEITEM");
    TEST_ASSERT(firered_script.commands[2].command_id == FIRERED_CMD_END,
                "Third command should be END");
    
    // Verify parameters were copied
    TEST_ASSERT(firered_script.commands[0].param_count == 2,
                "MSGBOX should have 2 parameters");
    TEST_ASSERT(firered_script.commands[0].parameters[0] == 0x01,
                "MSGBOX param 0 should be 0x01");
    TEST_ASSERT(firered_script.commands[0].parameters[1] == 0x02,
                "MSGBOX param 1 should be 0x02");
    
    // Cleanup
    FreeScript(&emerald_script);
    FreeScript(&firered_script);
    
    TEST_PASS("ConvertScript correctly converts compatible commands");
}

// Test 4: ConvertScript with incompatible commands
bool test_convert_incompatible_script() {
    Script emerald_script;
    Script firered_script;
    char error_buffer[512];
    
    InitScript(&emerald_script);
    strcpy(emerald_script.name, "TestScriptWithIncompat");
    
    // Create script with 1 compatible and 1 incompatible command
    emerald_script.command_count = 3;
    emerald_script.commands = (ScriptCommand*)calloc(3, sizeof(ScriptCommand));
    
    emerald_script.commands[0].command_id = EMERALD_CMD_MSGBOX;
    emerald_script.commands[0].param_count = 0;
    emerald_script.commands[0].parameters = NULL;
    
    emerald_script.commands[1].command_id = EMERALD_CMD_SETWEATHER;  // Incompatible
    emerald_script.commands[1].param_count = 0;
    emerald_script.commands[1].parameters = NULL;
    
    emerald_script.commands[2].command_id = EMERALD_CMD_END;
    emerald_script.commands[2].param_count = 0;
    emerald_script.commands[2].parameters = NULL;
    
    // Convert script
    bool result = ConvertScript(&emerald_script, &firered_script, "TestMap", 
                               error_buffer, sizeof(error_buffer));
    
    TEST_ASSERT(result == true, "ConvertScript should succeed with warnings");
    TEST_ASSERT(firered_script.command_count == 2, 
                "Output should have 2 commands (incompatible skipped)");
    TEST_ASSERT(strstr(error_buffer, "incompatible") != NULL,
                "Error buffer should mention incompatible commands");
    TEST_ASSERT(strstr(error_buffer, "TestMap") != NULL,
                "Error buffer should mention map name");
    
    // Cleanup
    FreeScript(&emerald_script);
    FreeScript(&firered_script);
    
    TEST_PASS("ConvertScript correctly handles incompatible commands");
}

// Test 5: ConvertScript with empty script
bool test_convert_empty_script() {
    Script emerald_script;
    Script firered_script;
    char error_buffer[512];
    
    InitScript(&emerald_script);
    strcpy(emerald_script.name, "EmptyScript");
    emerald_script.command_count = 0;
    emerald_script.commands = NULL;
    
    // Convert script
    bool result = ConvertScript(&emerald_script, &firered_script, "TestMap", 
                               error_buffer, sizeof(error_buffer));
    
    TEST_ASSERT(result == true, "ConvertScript should succeed with empty script");
    TEST_ASSERT(firered_script.command_count == 0, 
                "Output should have 0 commands");
    
    // Cleanup
    FreeScript(&emerald_script);
    FreeScript(&firered_script);
    
    TEST_PASS("ConvertScript correctly handles empty script");
}

// Test 6: InitScript and FreeScript
bool test_init_and_free_script() {
    Script script;
    
    InitScript(&script);
    TEST_ASSERT(script.commands == NULL, "InitScript should set commands to NULL");
    TEST_ASSERT(script.command_count == 0, "InitScript should set count to 0");
    
    // Allocate some data
    script.command_count = 2;
    script.commands = (ScriptCommand*)calloc(2, sizeof(ScriptCommand));
    script.commands[0].param_count = 3;
    script.commands[0].parameters = (uint8_t*)malloc(3);
    script.commands[1].param_count = 1;
    script.commands[1].parameters = (uint8_t*)malloc(1);
    
    // Free should clean up everything
    FreeScript(&script);
    TEST_ASSERT(script.commands == NULL, "FreeScript should set commands to NULL");
    TEST_ASSERT(script.command_count == 0, "FreeScript should set count to 0");
    
    TEST_PASS("InitScript and FreeScript work correctly");
}

int main() {
    printf("Running script converter unit tests...\n\n");
    
    int passed = 0;
    int total = 6;
    
    if (test_map_compatible_command()) passed++;
    if (test_map_incompatible_command()) passed++;
    if (test_convert_compatible_script()) passed++;
    if (test_convert_incompatible_script()) passed++;
    if (test_convert_empty_script()) passed++;
    if (test_init_and_free_script()) passed++;
    
    printf("\n========================================\n");
    printf("Test Results: %d/%d passed\n", passed, total);
    printf("========================================\n");
    
    return (passed == total) ? 0 : 1;
}
