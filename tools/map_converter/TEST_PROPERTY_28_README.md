# Property 28: Map Data Round-Trip Test

## Overview

This document describes the property-based test for **Property 28: Map Data Round-Trip**, which validates Requirements 19.4 and 19.5 from the Full Multi-Region Support specification.

## Property Definition

**For any valid map data**, parsing from Emerald format, converting to FireRed format, and parsing as FireRed should produce an equivalent map structure with all metadata preserved (dimensions, tileset references, music, weather).

## Test Implementation

### File: `test_property_28_roundtrip.c`

The test uses the **Theft** property-based testing library for C to generate random MapData structures and verify the round-trip conversion property.

### Test Configuration

- **Minimum Iterations**: 100 trials (as specified in the design document)
- **Testing Library**: Theft (C property-based testing)
- **Property Number**: 28
- **Validates**: Requirements 19.4, 19.5

### Test Structure

1. **Generator (`alloc_map_data`)**: Generates random MapData structures with:
   - Random map names and IDs
   - Random music selections from predefined options
   - Random weather types (SUNNY, RAIN, SNOW, THUNDERSTORM)
   - Random map types (TOWN, CITY, ROUTE, UNDERGROUND, INDOOR)
   - Random boolean flags (requires_flash, allow_cycling, etc.)
   - Random floor numbers (0-10)
   - Random connections (0-4 connections with random directions and offsets)

2. **Property Test (`prop_map_data_roundtrip`)**: For each generated MapData:
   - Step 1: Format the MapData to FireRed JSON using `FormatFireRedMap()`
   - Step 2: Parse the FireRed JSON back to MapData using `ParseEmeraldMap()`
   - Step 3: Validate equivalence using `ValidateMapRoundTrip()`

3. **Validation (`ValidateMapRoundTrip`)**: Compares all fields:
   - String fields: name, id, layout, music, weather, map_type
   - Boolean fields: requires_flash, allow_cycling, allow_escaping, allow_running, show_map_name
   - Numeric fields: floor_number
   - Array counts: connection_count, warp_count, object_count, coord_event_count, bg_event_count

### Hash Function

The test includes a hash function (`hash_map_data`) that hashes all MapData fields to enable Theft's duplicate detection, ensuring that the same input combinations aren't tested multiple times.

### Print Function

The test includes a print function (`print_map_data`) that outputs MapData in a human-readable format for debugging counterexamples.

## Building the Test

### Prerequisites

1. Theft library must be built:
   ```bash
   cd firered-clone/test/theft
   make
   ```

2. Compile the test:
   ```bash
   cd firered-clone/tools/map_converter
   gcc -std=c99 -Wall -Wextra -O2 -I../../test/theft/inc \
       test_property_28_roundtrip.c map_converter_stub.c \
       -o test_property_28_roundtrip.exe \
       ../../test/theft/build/libtheft.a
   ```

## Running the Test

### Basic Execution

```bash
./test_property_28_roundtrip.exe
```

This will run 100 trials with a time-based random seed.

### With Specific Seed

To reproduce a specific test run:

```bash
./test_property_28_roundtrip.exe <seed>
```

Example:
```bash
./test_property_28_roundtrip.exe 7052383092678860826
```

## Current Status

### Implementation Status: PARTIAL

The test is fully implemented and compiles successfully. However, it currently **skips all trials** because:

- `ParseEmeraldMap()` is not yet fully implemented
- JSON parsing requires integration with the json11 C++ library
- The stub implementation returns false to indicate parsing is not available

### Expected Behavior

**Current**: All 100 trials are skipped with message:
```
⊘ Property 28 SKIPPED: All trials were skipped
Note: ParseEmeraldMap is not yet fully implemented.
This test will pass once the JSON parsing is implemented.
```

**After ParseEmeraldMap Implementation**: The test will:
- Generate 100 random MapData structures
- Perform round-trip conversion for each
- Report PASS if all conversions preserve metadata
- Report FAIL with counterexamples if any conversion loses data

## Next Steps

To make this test fully functional:

1. **Implement JSON Parsing** (Task 1.1 continuation):
   - Integrate json11 C++ library into `ParseEmeraldMap()`
   - Parse all JSON fields into MapData structure
   - Handle arrays (connections, warps, objects, coord_events, bg_events)

2. **Extend Test Coverage**:
   - Currently only tests basic fields and connections
   - Add warp_events, object_events, coord_events, bg_events generation
   - Ensure all event types are properly round-tripped

3. **Run Full Test Suite**:
   - Once parsing is implemented, run the test
   - Fix any failures discovered by the property test
   - Verify that all 100 trials pass

## Test Output Format

### Success (Future)
```
=== Property 28: Map Data Round-Trip Test ===
Validates: Requirements 19.4, 19.5
...
Running 100 trials...

== PROP 'prop_map_data_roundtrip': 100 trials, seed 0x...
.....................(PASS x 100)
== PASS 'prop_map_data_roundtrip': pass 100, fail 0, skip 0, dup 0

=== Test Results ===
✓ Property 28 PASSED: All 100 trials succeeded
Map data round-trip conversion preserves all metadata correctly.
```

### Failure (If Issues Found)
```
=== Property 28: Map Data Round-Trip Test ===
...
Running 100 trials...

== PROP 'prop_map_data_roundtrip': 100 trials, seed 0x...
.....F
== FAIL 'prop_map_data_roundtrip': pass 5, fail 1, skip 0, dup 0
-- Counter-example: MapData{name="TestMap", id="MAP_TestMap", ...}

=== Test Results ===
✗ Property 28 FAILED: Found counterexample(s)
Map data round-trip conversion does not preserve all metadata.
```

## Integration with Build System

The test can be integrated into the Makefile for automated testing:

```makefile
# Add to Makefile
test_property_28: $(PROPERTY_TEST_TARGET)
	./$(PROPERTY_TEST_TARGET)

.PHONY: test_property_28
```

Then run with:
```bash
make test_property_28
```

## References

- **Specification**: `.kiro/specs/pokemon-tuxedo-full-regions/design.md`
- **Requirements**: 19.4, 19.5
- **Task**: 1.2 in `.kiro/specs/pokemon-tuxedo-full-regions/tasks.md`
- **Theft Library**: `firered-clone/test/theft/`
- **Property Definition**: Design document, Property 28

## Notes

- The test uses a stub implementation (`map_converter_stub.c`) to avoid C++/C linkage issues
- Once JSON parsing is implemented, the stub can be replaced with the full implementation
- The test is designed to be deterministic when given the same seed
- Theft's shrinking capability will help identify minimal failing cases if issues are found
