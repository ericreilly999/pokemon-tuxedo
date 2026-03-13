// test_error_logging.c
// Unit tests for error logging system

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "map_converter.h"

// Test helper to check if file exists and contains expected content
static bool file_contains(const char* filepath, const char* expected_text) {
    FILE* f = fopen(filepath, "r");
    if (!f) return false;
    
    char buffer[4096];
    bool found = false;
    
    while (fgets(buffer, sizeof(buffer), f)) {
        if (strstr(buffer, expected_text)) {
            found = true;
            break;
        }
    }
    
    fclose(f);
    return found;
}

void test_init_conversion_result(void) {
    printf("Testing InitConversionResult...\n");
    
    ConversionResult result;
    result.error = CONV_ERROR_PARSE_FAILED;
    strcpy(result.error_message, "Some error");
    strcpy(result.map_name, "TestMap");
    result.line_number = 42;
    
    InitConversionResult(&result);
    
    assert(result.error == CONV_ERROR_NONE);
    assert(strlen(result.error_message) == 0);
    assert(strlen(result.map_name) == 0);
    assert(result.line_number == 0);
    
    printf("  ✓ InitConversionResult works correctly\n");
}

void test_get_error_type_name(void) {
    printf("Testing GetErrorTypeName...\n");
    
    assert(strcmp(GetErrorTypeName(CONV_ERROR_NONE), "NONE") == 0);
    assert(strcmp(GetErrorTypeName(CONV_ERROR_FILE_NOT_FOUND), "FILE_NOT_FOUND") == 0);
    assert(strcmp(GetErrorTypeName(CONV_ERROR_PARSE_FAILED), "PARSE_FAILED") == 0);
    assert(strcmp(GetErrorTypeName(CONV_ERROR_INVALID_FORMAT), "INVALID_FORMAT") == 0);
    assert(strcmp(GetErrorTypeName(CONV_ERROR_MISSING_TILESET), "MISSING_TILESET") == 0);
    assert(strcmp(GetErrorTypeName(CONV_ERROR_INVALID_WARP), "INVALID_WARP") == 0);
    assert(strcmp(GetErrorTypeName(CONV_ERROR_INVALID_CONNECTION), "INVALID_CONNECTION") == 0);
    assert(strcmp(GetErrorTypeName(CONV_ERROR_SCRIPT_INCOMPATIBLE), "SCRIPT_INCOMPATIBLE") == 0);
    assert(strcmp(GetErrorTypeName(CONV_ERROR_TRAINER_CLASS_MISSING), "TRAINER_CLASS_MISSING") == 0);
    assert(strcmp(GetErrorTypeName(CONV_ERROR_VALIDATION_FAILED), "VALIDATION_FAILED") == 0);
    assert(strcmp(GetErrorTypeName(CONV_ERROR_OUTPUT_FAILED), "OUTPUT_FAILED") == 0);
    
    printf("  ✓ GetErrorTypeName returns correct names for all error types\n");
}

void test_log_conversion_error(void) {
    printf("Testing LogConversionError...\n");
    
    // Clear any previous logs
    ClearConversionLog();
    
    // Log some errors
    LogConversionError("Map1", CONV_ERROR_FILE_NOT_FOUND, "File does not exist");
    LogConversionError("Map2", CONV_ERROR_PARSE_FAILED, "Invalid JSON syntax");
    LogConversionError("Map3", CONV_ERROR_MISSING_TILESET, "Tileset 'grass' not found");
    
    // Get statistics
    ConversionStats stats;
    GetConversionStats(&stats);
    
    assert(stats.maps_failed == 3);
    
    printf("  ✓ LogConversionError logs errors correctly\n");
}

void test_get_conversion_stats(void) {
    printf("Testing GetConversionStats...\n");
    
    // Clear logs
    ClearConversionLog();
    
    // Log various errors
    LogConversionError("Map1", CONV_ERROR_FILE_NOT_FOUND, "Test error 1");
    LogConversionError("Map2", CONV_ERROR_PARSE_FAILED, "Test error 2");
    LogConversionError("Map3", CONV_ERROR_INVALID_WARP, "Test error 3");
    
    // Get statistics
    ConversionStats stats;
    GetConversionStats(&stats);
    
    assert(stats.maps_failed == 3);
    assert(stats.maps_attempted == 0);  // Not set by LogConversionError
    assert(stats.maps_succeeded == 0);
    
    printf("  ✓ GetConversionStats returns correct statistics\n");
}

void test_generate_conversion_report(void) {
    printf("Testing GenerateConversionReport...\n");
    
    // Clear logs
    ClearConversionLog();
    
    // Log some test errors
    LogConversionError("TestMap1", CONV_ERROR_FILE_NOT_FOUND, "File not found error");
    LogConversionError("TestMap2", CONV_ERROR_PARSE_FAILED, "Parse failed error");
    LogConversionError("TestMap3", CONV_ERROR_MISSING_TILESET, "Missing tileset error");
    LogConversionError("TestMap4", CONV_ERROR_INVALID_WARP, "Invalid warp error");
    
    // Generate report
    const char* report_path = "test_error_report.txt";
    bool success = GenerateConversionReport(report_path);
    
    assert(success);
    
    // Verify report contains expected content
    assert(file_contains(report_path, "MAP CONVERSION REPORT"));
    assert(file_contains(report_path, "CONVERSION STATISTICS"));
    assert(file_contains(report_path, "Maps Failed:          4"));
    assert(file_contains(report_path, "CONVERSION ERRORS"));
    assert(file_contains(report_path, "FILE_NOT_FOUND"));
    assert(file_contains(report_path, "PARSE_FAILED"));
    assert(file_contains(report_path, "MISSING_TILESET"));
    assert(file_contains(report_path, "INVALID_WARP"));
    assert(file_contains(report_path, "TestMap1"));
    assert(file_contains(report_path, "TestMap2"));
    assert(file_contains(report_path, "TestMap3"));
    assert(file_contains(report_path, "TestMap4"));
    
    printf("  ✓ GenerateConversionReport creates report with correct content\n");
    
    // Clean up
    remove(report_path);
}

void test_clear_conversion_log(void) {
    printf("Testing ClearConversionLog...\n");
    
    // Log some errors
    LogConversionError("Map1", CONV_ERROR_FILE_NOT_FOUND, "Test error");
    LogConversionError("Map2", CONV_ERROR_PARSE_FAILED, "Test error");
    
    // Verify errors were logged
    ConversionStats stats;
    GetConversionStats(&stats);
    assert(stats.maps_failed == 2);
    
    // Clear logs
    ClearConversionLog();
    
    // Verify logs are cleared
    GetConversionStats(&stats);
    assert(stats.maps_failed == 0);
    assert(stats.maps_attempted == 0);
    assert(stats.maps_succeeded == 0);
    assert(stats.warnings_generated == 0);
    
    printf("  ✓ ClearConversionLog clears all logs and statistics\n");
}

void test_error_log_capacity(void) {
    printf("Testing error log capacity...\n");
    
    // Clear logs
    ClearConversionLog();
    
    // Try to log many errors (more than MAX_ERROR_LOG_ENTRIES)
    // This should not crash, but should stop logging after the limit
    for (int i = 0; i < 1100; i++) {
        char map_name[64];
        snprintf(map_name, sizeof(map_name), "Map%d", i);
        LogConversionError(map_name, CONV_ERROR_PARSE_FAILED, "Test error");
    }
    
    // Get statistics - should be capped at MAX_ERROR_LOG_ENTRIES (1000)
    ConversionStats stats;
    GetConversionStats(&stats);
    assert(stats.maps_failed == 1000);
    
    printf("  ✓ Error log capacity limit works correctly\n");
}

void test_null_parameters(void) {
    printf("Testing NULL parameter handling...\n");
    
    // Test InitConversionResult with NULL
    InitConversionResult(NULL);  // Should not crash
    
    // Test GetConversionStats with NULL
    GetConversionStats(NULL);  // Should not crash
    
    // Test LogConversionError with NULL map_name
    LogConversionError(NULL, CONV_ERROR_PARSE_FAILED, "Test error");
    
    // Test LogConversionError with NULL details
    LogConversionError("TestMap", CONV_ERROR_PARSE_FAILED, NULL);
    
    // Test GenerateConversionReport with NULL
    bool success = GenerateConversionReport(NULL);
    assert(!success);
    
    printf("  ✓ NULL parameter handling works correctly\n");
}

int main(void) {
    printf("=============================================================================\n");
    printf("                    ERROR LOGGING SYSTEM UNIT TESTS                         \n");
    printf("=============================================================================\n\n");
    
    test_init_conversion_result();
    test_get_error_type_name();
    test_log_conversion_error();
    test_get_conversion_stats();
    test_generate_conversion_report();
    test_clear_conversion_log();
    test_error_log_capacity();
    test_null_parameters();
    
    printf("\n=============================================================================\n");
    printf("                         ALL TESTS PASSED                                   \n");
    printf("=============================================================================\n");
    
    return 0;
}
