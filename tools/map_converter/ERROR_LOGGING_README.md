# Error Logging System

## Overview

The error logging system provides comprehensive tracking and reporting of conversion errors during the map format conversion process. It logs detailed error information, generates summary reports, and tracks conversion statistics.

## Components

### ConversionError Enum

Defines all possible error types that can occur during conversion:

- `CONV_ERROR_NONE` - No error
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

### ConversionResult Struct

Contains detailed information about a conversion error:

```c
typedef struct {
    ConversionError error;        // Type of error
    char error_message[256];      // Detailed error message
    char map_name[64];            // Name of the map being converted
    uint32_t line_number;         // Line number (for parse errors)
} ConversionResult;
```

### ConversionStats Struct

Tracks overall conversion statistics:

```c
typedef struct {
    uint32_t maps_attempted;      // Total maps attempted
    uint32_t maps_succeeded;      // Maps successfully converted
    uint32_t maps_failed;         // Maps that failed conversion
    uint32_t warnings_generated;  // Total warnings generated
    uint32_t tilesets_converted;  // Tilesets successfully converted
    uint32_t scripts_converted;   // Scripts successfully converted
    uint32_t trainers_converted;  // Trainers successfully converted
} ConversionStats;
```

## API Functions

### LogConversionError()

Logs a conversion error with detailed context.

```c
void LogConversionError(const char* map_name, ConversionError error, const char* details);
```

**Parameters:**
- `map_name` - Name of the map being converted (or NULL for unknown)
- `error` - Type of error that occurred
- `details` - Additional details about the error (or NULL)

**Behavior:**
- Stores error in internal log (up to 1000 entries)
- Prints error to stderr for immediate feedback
- Updates conversion statistics (increments maps_failed)
- Handles NULL parameters gracefully

**Example:**
```c
LogConversionError("PalletTown", CONV_ERROR_MISSING_TILESET, "Tileset 'grass_primary' not found");
```

### GenerateConversionReport()

Generates a comprehensive conversion report file.

```c
bool GenerateConversionReport(const char* output_path);
```

**Parameters:**
- `output_path` - Path to write the report file

**Returns:**
- `true` on success
- `false` on failure (e.g., cannot open file)

**Report Contents:**
- Conversion statistics summary
- Success rate calculation
- Errors grouped by type
- Detailed error information for each logged error

**Example:**
```c
if (GenerateConversionReport("conversion_report.txt")) {
    printf("Report generated successfully\n");
}
```

### GetConversionStats()

Retrieves current conversion statistics.

```c
void GetConversionStats(ConversionStats* out_stats);
```

**Parameters:**
- `out_stats` - Pointer to ConversionStats structure to fill

**Example:**
```c
ConversionStats stats;
GetConversionStats(&stats);
printf("Success rate: %.2f%%\n", 
       (float)stats.maps_succeeded / stats.maps_attempted * 100.0f);
```

### ClearConversionLog()

Clears all logged errors and resets statistics.

```c
void ClearConversionLog(void);
```

**Use Case:**
Call this at the start of a new conversion batch to reset the log.

### Helper Functions

#### InitConversionResult()

Initializes a ConversionResult structure with default values.

```c
void InitConversionResult(ConversionResult* result);
```

#### GetErrorTypeName()

Returns a string representation of an error type.

```c
const char* GetErrorTypeName(ConversionError error);
```

**Example:**
```c
printf("Error type: %s\n", GetErrorTypeName(CONV_ERROR_PARSE_FAILED));
// Output: Error type: PARSE_FAILED
```

## Usage Example

### Basic Error Logging

```c
// Clear previous logs
ClearConversionLog();

// Attempt to convert maps
for (int i = 0; i < map_count; i++) {
    MapData map;
    char error_buffer[512];
    
    if (!ParseEmeraldMap(map_paths[i], &map, error_buffer, sizeof(error_buffer))) {
        LogConversionError(map_paths[i], CONV_ERROR_PARSE_FAILED, error_buffer);
        continue;
    }
    
    if (!FormatFireRedMap(&map, output_paths[i], error_buffer, sizeof(error_buffer))) {
        LogConversionError(map.name, CONV_ERROR_OUTPUT_FAILED, error_buffer);
        continue;
    }
    
    // Success - update statistics manually if needed
    // (LogConversionError only updates failure count)
}

// Generate report
GenerateConversionReport("conversion_report.txt");

// Get statistics
ConversionStats stats;
GetConversionStats(&stats);
printf("Converted %u/%u maps successfully\n", 
       stats.maps_succeeded, stats.maps_attempted);
```

### Integration with Validation

```c
ValidationResult result = ValidateMap(&map, map_registry, map_count,
                                      tileset_registry, tileset_count,
                                      script_registry, script_count);

if (!result.is_valid) {
    LogConversionError(map.name, CONV_ERROR_VALIDATION_FAILED, result.error_message);
}
```

## Report Format

The generated report includes:

```
=============================================================================
                    MAP CONVERSION REPORT                                    
=============================================================================

CONVERSION STATISTICS:
---------------------
Maps Attempted:       150
Maps Succeeded:       142
Maps Failed:          8
Warnings Generated:   23
Tilesets Converted:   45
Scripts Converted:    312
Trainers Converted:   89
Success Rate:         94.67%

CONVERSION ERRORS:
------------------
Total Errors Logged: 8

FILE_NOT_FOUND (2 occurrences):
----------------------------------------
  Map: Route1
  Details: File 'data/maps/Route1/map.json' not found

  Map: Route2
  Details: File 'data/maps/Route2/map.json' not found

MISSING_TILESET (3 occurrences):
----------------------------------------
  Map: PalletTown
  Details: Tileset 'grass_primary' not found

  Map: ViridianCity
  Details: Tileset 'city_primary' not found

  Map: PewterCity
  Details: Tileset 'city_primary' not found

...

=============================================================================
                         END OF REPORT                                       
=============================================================================
```

## Implementation Details

### Error Log Storage

- Maximum 1000 error entries stored in memory
- Errors beyond the limit are not logged (warning printed to stderr)
- Each entry contains: map name, error type, details, line number

### Thread Safety

The current implementation is **not thread-safe**. If you need to use the error logging system in a multi-threaded environment, you must add synchronization (e.g., mutexes) around the global error log and statistics.

### Memory Management

- Error log uses static allocation (no dynamic memory)
- No memory leaks - all data is stored in static arrays
- Call `ClearConversionLog()` to reset if needed

## Testing

Unit tests are provided in `test_error_logging.c`:

- `test_init_conversion_result()` - Tests initialization
- `test_get_error_type_name()` - Tests error type name conversion
- `test_log_conversion_error()` - Tests error logging
- `test_get_conversion_stats()` - Tests statistics retrieval
- `test_generate_conversion_report()` - Tests report generation
- `test_clear_conversion_log()` - Tests log clearing
- `test_error_log_capacity()` - Tests capacity limits
- `test_null_parameters()` - Tests NULL parameter handling

To build and run tests:

```bash
make test_error_logging.exe
./test_error_logging.exe
```

## Requirements Validation

This implementation validates the following requirements:

- **Requirement 5.7**: Log detailed error information including map name and failure reason when map data conversion fails
- **Requirement 6.6**: Report missing graphics by name when a tileset is missing required graphics
- **Requirement 7.5**: Log incompatible commands and map location when a script command has no FireRed equivalent
- **Requirement 8.6**: Log trainer class substitutions when a trainer class has no FireRed equivalent
- **Requirement 15.6**: Report map name and specific validation failure when validation fails
- **Requirement 17.5**: Report clear error message with repository URL when Crossroads source is unavailable
- **Requirement 18.6**: Report size overflow and affected data sections when ROM exceeds size limits

## Future Enhancements

Potential improvements:

1. **Severity Levels**: Add warning vs. error distinction
2. **Filtering**: Filter report by error type or map name
3. **JSON Output**: Generate machine-readable JSON reports
4. **Statistics Tracking**: Automatically track maps_attempted and maps_succeeded
5. **Thread Safety**: Add mutex protection for multi-threaded use
6. **Persistent Storage**: Save/load error log to/from file
7. **Error Recovery**: Suggest fixes for common errors
