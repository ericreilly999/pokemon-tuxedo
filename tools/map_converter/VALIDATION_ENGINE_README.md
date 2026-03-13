# Validation Engine

## Overview

The validation engine provides comprehensive validation functions for map data structures, ensuring that all ported maps have valid headers, connections, warps, tileset references, and script references.

**Validates Requirements:** 15.1, 15.2, 15.3, 15.4, 15.5, 15.6

## Features

The validation engine implements the following validation functions:

### 1. ValidateMapHeader()

Validates that a map has a valid header structure with all required fields present and within bounds.

**Checks:**
- Map data pointer is not NULL
- Required string fields are not empty (name, id, layout, music, weather, map_type)
- Array counts are within maximum bounds (connections, warps, objects, coord_events, bg_events)

**Returns:** `ValidationResult` with `is_valid=true` on success, or error details on failure

### 2. ValidateWarpDestinations()

Validates that all warp destinations reference valid maps in the map registry.

**Parameters:**
- `map`: The map to validate
- `map_registry`: Array of valid map names
- `registry_count`: Number of maps in registry

**Checks:**
- All warp destination maps exist in the map registry
- Empty destination maps are allowed (skipped)

**Returns:** `ValidationResult` with details of any invalid warp destinations

### 3. ValidateConnections()

Validates that all map connections reference valid destination maps and have valid directions.

**Parameters:**
- `map`: The map to validate
- `map_registry`: Array of valid map names
- `registry_count`: Number of maps in registry

**Checks:**
- Connection destination maps are not empty
- Connection directions are valid (north, south, east, west, up, down)
- Connection destination maps exist in the map registry

**Returns:** `ValidationResult` with details of any invalid connections

### 4. ValidateTilesetReferences()

Validates that tileset references are valid.

**Parameters:**
- `map`: The map to validate
- `tileset_registry`: Array of valid tileset names
- `registry_count`: Number of tilesets in registry

**Note:** In the current implementation, tileset references are embedded in the layout field. This is a placeholder for future enhancement when tileset data is more explicitly represented in the MapData structure.

**Returns:** `ValidationResult` (currently always valid if map is not NULL)

### 5. ValidateScriptReferences()

Validates that all event script references are valid.

**Parameters:**
- `map`: The map to validate
- `script_registry`: Array of valid script names
- `registry_count`: Number of scripts in registry

**Checks:**
- Object event scripts exist in the script registry
- Coord event trigger scripts exist in the script registry
- BG event sign scripts exist in the script registry
- Empty scripts and "NULL" scripts are allowed (skipped)

**Returns:** `ValidationResult` with details of any invalid script references

### 6. ValidateMap()

Comprehensive validation that runs all validation checks in sequence.

**Parameters:**
- `map`: The map to validate
- `map_registry`: Array of valid map names
- `map_count`: Number of maps in registry
- `tileset_registry`: Array of valid tileset names
- `tileset_count`: Number of tilesets in registry
- `script_registry`: Array of valid script names
- `script_count`: Number of scripts in registry

**Behavior:**
- Runs all validation checks in order
- Stops at first failure
- Returns combined result

**Returns:** `ValidationResult` with details of the first validation failure, or success if all checks pass

## Data Structures

### ValidationResult

```c
typedef struct {
    bool is_valid;                          // True if validation passed
    char error_message[ERROR_BUFFER_SIZE];  // Detailed error message
    char map_name[MAX_NAME_LENGTH];         // Name of map being validated
} ValidationResult;
```

## Usage Example

```c
#include "map_converter.h"

// Create or load a map
MapData map;
InitMapData(&map);
// ... populate map data ...

// Create registries
const char* map_registry[] = {"MAP_PALLET_TOWN", "MAP_VIRIDIAN_CITY"};
const char* tileset_registry[] = {"TILESET_GENERAL"};
const char* script_registry[] = {"PalletTown_EventScript_Oak"};

// Validate the map
ValidationResult result = ValidateMap(&map,
                                      map_registry, 2,
                                      tileset_registry, 1,
                                      script_registry, 1);

if (!result.is_valid) {
    fprintf(stderr, "Validation failed for map '%s': %s\n",
            result.map_name, result.error_message);
    return false;
}

printf("Map '%s' validated successfully!\n", map.name);
```

## Error Reporting

When validation fails, the `ValidationResult` structure provides:

1. **is_valid**: Set to `false` to indicate failure
2. **error_message**: Detailed description of what failed, including:
   - The specific field or element that failed
   - The index of the failing element (for arrays)
   - The invalid value that was found
3. **map_name**: The name of the map being validated (for context)

Example error messages:
- `"Map name is empty"`
- `"Warp 3 in map 'PalletTown' references invalid destination map: MAP_NONEXISTENT"`
- `"Connection 1 in map 'Route1' has invalid direction: diagonal"`
- `"Object 5 in map 'ViridianCity' references invalid script: NonexistentScript"`

## Testing

The validation engine includes comprehensive unit tests in `test_validation_engine.c`:

### Test Coverage

1. **ValidateMapHeader tests:**
   - Valid map header
   - NULL pointer
   - Empty name
   - Excessive warp count

2. **ValidateWarpDestinations tests:**
   - Valid warp destinations
   - Invalid destination map

3. **ValidateConnections tests:**
   - Valid connections
   - Invalid direction

4. **ValidateScriptReferences tests:**
   - Valid script references
   - Invalid script reference

5. **ValidateMap tests:**
   - Comprehensive validation with all valid data
   - Multiple failures (early termination)

### Running Tests

To compile and run the validation engine tests:

```bash
# Using Makefile
make test_validation_engine
./test_validation_engine

# Or compile directly
gcc -std=c99 -Wall -Wextra -O2 test_validation_engine.c map_converter_stub.c -o test_validation_engine
./test_validation_engine
```

Expected output:
```
=== Validation Engine Unit Tests ===
Testing Requirements 15.1, 15.2, 15.3, 15.4, 15.5, 15.6

Test 1: ValidateMapHeader with valid map... PASSED
Test 2: ValidateMapHeader with NULL pointer... PASSED
Test 3: ValidateMapHeader with empty name... PASSED
Test 4: ValidateMapHeader with excessive warp count... PASSED
Test 5: ValidateWarpDestinations with valid warps... PASSED
Test 6: ValidateWarpDestinations with invalid destination... PASSED
Test 7: ValidateConnections with valid connections... PASSED
Test 8: ValidateConnections with invalid direction... PASSED
Test 9: ValidateScriptReferences with valid scripts... PASSED
Test 10: ValidateScriptReferences with invalid script... PASSED
Test 11: ValidateMap comprehensive test... PASSED
Test 12: ValidateMap with invalid warp (should fail early)... PASSED

=== All Validation Engine Tests PASSED ===
```

## Integration with Map Conversion Pipeline

The validation engine is designed to be integrated into the map conversion pipeline:

```
┌─────────────────────────────────────────────────────────────┐
│                    Pokemon Crossroads                        │
│                   (Emerald Source Data)                      │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────────┐
│              Format Conversion Layer                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │ Map Parser   │  │ Tileset      │  │ Script       │     │
│  │              │  │ Converter    │  │ Converter    │     │
│  └──────────────┘  └──────────────┘  └──────────────┘     │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────────┐
│              Validation Engine (NEW)                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │ Header       │  │ Warps        │  │ Connections  │     │
│  │ Validation   │  │ Validation   │  │ Validation   │     │
│  └──────────────┘  └──────────────┘  └──────────────┘     │
│  ┌──────────────┐  ┌──────────────┐                        │
│  │ Tileset      │  │ Script       │                        │
│  │ Validation   │  │ Validation   │                        │
│  └──────────────┘  └──────────────┘                        │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────────┐
│              Data Integration Layer                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │ Map Registry │  │ Build System │  │ Error        │     │
│  │              │  │ Integration  │  │ Reporting    │     │
│  └──────────────┘  └──────────────┘  └──────────────┘     │
└─────────────────────────────────────────────────────────────┘
```

## Future Enhancements

1. **Enhanced Tileset Validation:**
   - Parse layout field to extract tileset references
   - Validate primary and secondary tileset IDs
   - Check tileset compatibility

2. **Cross-Reference Validation:**
   - Validate bidirectional connections (if A connects to B, B should connect to A)
   - Validate warp loops (ensure warps don't create infinite loops)
   - Validate script dependencies

3. **Performance Optimization:**
   - Use hash tables for registry lookups
   - Cache validation results
   - Parallel validation for multiple maps

4. **Extended Error Reporting:**
   - Generate validation reports in JSON format
   - Provide suggestions for fixing validation errors
   - Track validation statistics across all maps

## Requirements Traceability

| Requirement | Validation Function | Description |
|-------------|---------------------|-------------|
| 15.1 | ValidateMapHeader() | Verify all ported maps have valid Map_Header structures |
| 15.2 | ValidateConnections() | Verify all map connections reference valid destination maps |
| 15.3 | ValidateWarpDestinations() | Verify all warp points reference valid destination maps |
| 15.4 | ValidateScriptReferences() | Verify all Event_Script references are valid |
| 15.5 | ValidateTilesetReferences() | Verify all Tileset references are valid |
| 15.6 | All validation functions | Report map name and specific validation failure when validation fails |

## Conclusion

The validation engine provides a robust foundation for ensuring map data integrity during the conversion process. By validating maps at multiple levels (header, warps, connections, tilesets, scripts), the engine helps catch errors early and provides detailed error messages for debugging.
