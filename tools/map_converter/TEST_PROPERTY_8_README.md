# Property 8: Conversion Error Logging Test

## Overview

This property-based test validates that the error logging system correctly logs errors, tracks statistics, and generates reports during map format conversion.

## Property Statement

**Property 8: Conversion Error Logging**

*For any* map conversion that fails, the system should log detailed error information including the map name and specific failure reason.

## Requirements Validated

This test validates the following requirements:

- **Requirement 5.7**: Log detailed error information including map name and failure reason when map data conversion fails
- **Requirement 6.6**: Report missing graphics by name when a tileset is missing required graphics
- **Requirement 7.5**: Log incompatible commands and map location when a script command has no FireRed equivalent
- **Requirement 8.6**: Log trainer class substitutions when a trainer class has no FireRed equivalent
- **Requirement 15.6**: Report map name and specific validation failure when validation fails
- **Requirement 17.5**: Report clear error message with repository URL when Crossroads source is unavailable
- **Requirement 18.6**: Report size overflow and affected data sections when ROM exceeds size limits

## Test Structure

The test consists of three sub-properties:

### 1. Basic Error Logging

Tests that individual errors are logged correctly:
- Error information is stored (map name, error type, details)
- Statistics are updated (maps_failed increments)
- Reports can be generated successfully
- Report contains the logged error information

### 2. Multiple Errors Logging

Tests that multiple errors are tracked accurately:
- Multiple errors can be logged sequentially
- Statistics correctly count all errors
- Reports show all logged errors
- Error counts are accurate

### 3. NULL Parameter Handling

Tests that the system handles NULL parameters gracefully:
- NULL map names are handled (replaced with "UNKNOWN")
- NULL details are handled (replaced with "No details provided")
- Errors with NULL parameters are still logged
- Reports can be generated even with NULL parameters

## Test Implementation

### Input Generation

The test generates random error log inputs with:
- **Map names**: Random alphanumeric strings (5-35 characters) or empty (20% chance)
- **Error types**: Random error types from the ConversionError enum (excluding CONV_ERROR_NONE)
- **Details**: Random printable ASCII strings (10-110 characters) or empty (20% chance)

### Property Verification

For each generated input, the test:
1. Clears the conversion log
2. Logs the error using `LogConversionError()`
3. Retrieves statistics using `GetConversionStats()`
4. Verifies `maps_failed` was incremented
5. Generates a report using `GenerateConversionReport()`
6. Reads and validates the report contents
7. Verifies the report contains the error type and map name

### Test Configuration

- **Iterations**: 100 trials per sub-property (300 total)
- **Library**: Theft (C property-based testing library)
- **Seed**: Time-based or provided via command line

## Building the Test

### Prerequisites

- GCC compiler
- Theft library (built in `../../test/theft/`)
- map_converter.c and map_converter.h

### Build Command

```bash
# Using Makefile
make test_property_8_error_logging.exe

# Or manually
gcc -std=c99 -Wall -Wextra -O2 -I../../test/theft/inc \
    test_property_8_error_logging.c map_converter.c \
    -o test_property_8_error_logging.exe \
    ../../test/theft/build/libtheft.a
```

## Running the Test

### Basic Execution

```bash
./test_property_8_error_logging.exe
```

### With Specific Seed

```bash
./test_property_8_error_logging.exe 1234567890
```

## Expected Output

### Successful Run

```
=== Property 8: Conversion Error Logging Test ===
Property 8: Conversion Error Logging
Validates: Requirements 5.7, 6.6, 7.5, 8.6, 15.6, 17.5, 18.6
Testing that errors are logged correctly, statistics are tracked,
and reports are generated properly.

Using time-based seed: 1234567890

--- Testing Property 8: Basic Error Logging ---
Running 100 trials...

=== Property 8 Basic Logging Results ===
✓ Property 8 (Basic) PASSED: All 100 trials succeeded
Error logging correctly stores error information and generates reports.

--- Testing Property 8: Multiple Errors Logging ---
Running 100 trials...

=== Property 8 Multiple Errors Results ===
✓ Property 8 (Multiple) PASSED: All 100 trials succeeded
Multiple errors are logged correctly with accurate statistics.

--- Testing Property 8: NULL Parameter Handling ---
Running 100 trials...

=== Property 8 NULL Handling Results ===
✓ Property 8 (NULL) PASSED: All 100 trials succeeded
NULL parameters are handled gracefully.

=== Overall Test Results ===
✓ ALL PROPERTY 8 TESTS PASSED
Error logging system correctly logs errors, tracks statistics,
and generates reports with proper NULL parameter handling.
```

### Failed Run

If a property fails, the output will show:

```
✗ Property 8 (Basic) FAILED: Found counterexample(s)
Error logging does not work correctly.
```

The test will print detailed information about the failing case, including:
- The generated input that caused the failure
- The specific assertion that failed
- The expected vs. actual values

## Test Files Generated

During test execution, temporary report files are created and cleaned up:
- `test_error_report.txt` - Basic error logging test report
- `test_multiple_errors_report.txt` - Multiple errors test report
- `test_null_params_report.txt` - NULL parameter handling test report

These files are automatically deleted after verification.

## Integration with Error Logging System

This test validates the following error logging functions:

### LogConversionError()

```c
void LogConversionError(const char* map_name, ConversionError error, const char* details);
```

Tests that:
- Errors are stored in the internal log
- Statistics are updated (maps_failed increments)
- NULL parameters are handled gracefully
- Error information is preserved

### GetConversionStats()

```c
void GetConversionStats(ConversionStats* out_stats);
```

Tests that:
- Statistics are retrieved correctly
- Counts are accurate
- NULL parameters are handled

### GenerateConversionReport()

```c
bool GenerateConversionReport(const char* output_path);
```

Tests that:
- Reports are generated successfully
- Reports contain all logged errors
- Reports show accurate statistics
- Report format is correct

### ClearConversionLog()

```c
void ClearConversionLog(void);
```

Tests that:
- Log is cleared completely
- Statistics are reset to zero
- Subsequent logging works correctly

## Error Types Tested

The test covers all error types defined in the ConversionError enum:

- `CONV_ERROR_FILE_NOT_FOUND` - Input file not found
- `CONV_ERROR_PARSE_FAILED` - Failed to parse input file
- `CONV_ERROR_INVALID_FORMAT` - Invalid file format
- `CONV_ERROR_MISSING_TILESET` - Referenced tileset not found
- `CONV_ERROR_INVALID_WARP` - Invalid warp destination
- `CONV_ERROR_INVALID_CONNECTION` - Invalid map connection
- `CONV_ERROR_SCRIPT_INCOMPATIBLE` - Script command has no FireRed equivalent
- `CONV_ERROR_TRAINER_CLASS_MISSING` - Trainer class has no FireRed equivalent
- `CONV_ERROR_VALIDATION_FAILED` - Map validation failed
- `CONV_ERROR_OUTPUT_FAILED` - Failed to write output file

## Troubleshooting

### Test Fails to Compile

**Error**: `theft.h: No such file or directory`

**Solution**: Build the Theft library first:
```bash
cd ../../test/theft
make
cd ../../tools/map_converter
```

### Test Crashes

**Error**: Segmentation fault or access violation

**Possible Causes**:
- NULL pointer dereference in error logging code
- Buffer overflow in string operations
- Memory corruption

**Solution**: Run with a debugger to identify the crash location:
```bash
gdb ./test_property_8_error_logging.exe
run
bt
```

### Test Fails Consistently

**Error**: Property fails on every run

**Possible Causes**:
- Bug in error logging implementation
- Incorrect test expectations
- Missing functionality

**Solution**: 
1. Check the failing counterexample
2. Manually test the error logging functions
3. Verify the implementation matches the specification

### Report Files Not Cleaned Up

**Error**: Test report files remain after test completion

**Possible Causes**:
- Test crashed before cleanup
- File permissions issue

**Solution**: Manually delete the files:
```bash
rm test_error_report.txt test_multiple_errors_report.txt test_null_params_report.txt
```

## Performance Considerations

### Test Execution Time

- **Expected**: 2-5 seconds for 300 trials
- **Factors**: File I/O for report generation, string operations

### Memory Usage

- **Error Log**: Up to 1000 entries (defined by MAX_ERROR_LOG_ENTRIES)
- **Each Entry**: ~400 bytes (map name + details + metadata)
- **Total**: ~400 KB maximum for error log

### Optimization Tips

If the test runs slowly:
1. Reduce the number of trials (change `trials` in config)
2. Disable report generation for some trials
3. Use a faster file system (SSD vs. HDD)

## Related Tests

- **test_error_logging.c** - Unit tests for error logging functions
- **test_property_5_6_7_25_26_validation.c** - Validation property tests (uses error logging)
- **test_property_28_roundtrip.c** - Round-trip property test (uses error logging)

## References

- **Design Document**: `.kiro/specs/pokemon-tuxedo-full-regions/design.md` - Property 8 definition
- **Requirements**: `.kiro/specs/pokemon-tuxedo-full-regions/requirements.md` - Requirements 5.7, 6.6, 7.5, 8.6, 15.6, 17.5, 18.6
- **Implementation**: `map_converter.c` - Error logging system implementation
- **Header**: `map_converter.h` - Error logging API definitions
- **README**: `ERROR_LOGGING_README.md` - Error logging system documentation

## Maintenance

### Updating the Test

If the error logging system changes:

1. **New Error Types**: Add to the ConversionError enum and update the test to cover them
2. **New Statistics**: Update the test to verify new statistics fields
3. **Report Format Changes**: Update report validation logic
4. **API Changes**: Update function calls and parameter handling

### Adding New Sub-Properties

To add a new sub-property:

1. Define a new property function (e.g., `prop_error_capacity_limits`)
2. Create a `theft_run_config` for the new property
3. Run the property with `theft_run()`
4. Add result checking and reporting
5. Update the overall result logic

## Conclusion

This property-based test provides comprehensive validation of the error logging system, ensuring that:

- Errors are logged correctly with all details preserved
- Statistics are tracked accurately
- Reports are generated successfully
- NULL parameters are handled gracefully
- Multiple errors can be logged and reported

The test uses 300 trials across three sub-properties to thoroughly exercise the error logging system with a wide variety of inputs, providing high confidence in the correctness of the implementation.
