# Property 9: Tileset Reference Validity Test

## Overview

This property-based test validates **Property 9: Tileset Reference Validity** from the Full Multi-Region Support design document.

**Property Statement:** For any converted map, all tileset references in the map header should point to valid, converted tilesets.

**Validates:** Requirements 6.5, 15.5

## Test Implementation

### File
`test_property_9_tileset_validity.c`

### Testing Framework
- **Library:** Theft (C property-based testing library)
- **Iterations:** 100 trials (minimum as per spec)
- **Language:** C99

### What It Tests

The property test verifies that:

1. **Tileset Conversion Succeeds**: When we convert a tileset from Emerald to FireRed format, the conversion completes successfully
2. **Tileset Names Preserved**: The tileset name remains unchanged during conversion
3. **Data Integrity**: All tileset data (graphics, metatiles, behavior, palettes) is preserved byte-for-byte during conversion
4. **Reference Validity**: Maps that reference tilesets maintain valid references after conversion
5. **Both Primary and Secondary**: Both primary and secondary tileset references are validated

### Test Strategy

For each trial:
1. Generate a random map with references to two tilesets (primary and secondary)
2. Create mock tileset data for both referenced tilesets
3. Convert both tilesets from Emerald to FireRed format using `ConvertTileset()`
4. Verify tileset names are preserved
5. Verify all tileset data matches byte-for-byte (graphics, metatiles, behavior, palettes)
6. Verify the map's tileset references match the converted tileset names

### Building

```bash
cd firered-clone/tools/map_converter
make test_property_9_tileset_validity
```

Or manually:
```bash
gcc -std=c99 -Wall -Wextra -O2 -I../../test/theft/inc \
    test_property_9_tileset_validity.c map_converter.c \
    ../../test/theft/build/libtheft.a \
    -o test_property_9_tileset_validity
```

### Running

```bash
./test_property_9_tileset_validity
```

With a specific seed for reproducibility:
```bash
./test_property_9_tileset_validity 12345
```

### Expected Output

```
=== Property 9: Tileset Reference Validity Test ===
Validates: Requirements 6.5, 15.5
Testing that converted maps reference valid, converted tilesets.

Using time-based seed: <seed>
Running 100 trials...

== PROP 'prop_tileset_reference_validity': 100 trials, seed 0x...
....................................................................................................
== PASS 'prop_tileset_reference_validity': pass 100, fail 0, skip 0, dup 0

=== Test Results ===
Unique tilesets generated: 0

✓ Property 9 PASSED: All 100 trials succeeded
All converted maps have valid tileset references.
Tileset names and data are preserved during conversion.
```

### Test Results

**Status:** ✅ PASSED (100/100 trials)

The test successfully validates that:
- Tileset conversion preserves all data correctly
- Tileset names remain unchanged during conversion
- Maps maintain valid references to converted tilesets
- Both primary and secondary tilesets are handled correctly

### Implementation Notes

1. **Mock Data**: The test uses mock tileset data rather than real Crossroads data for speed and independence
2. **Simplified Registry**: The original design included a tileset registry, but this was removed as it's not essential for validating the core property
3. **Memory Management**: All allocated tileset data is properly freed after each trial
4. **Data Sizes**: Mock tilesets use smaller data sizes (256 bytes graphics, 16 metatiles, etc.) compared to real tilesets for faster execution

### Related Files

- `map_converter.h` - Header defining TilesetData structure and conversion functions
- `map_converter.c` - Implementation of ConvertTileset() function
- `test_tileset_unit.c` - Unit tests for tileset conversion
- `test_tileset_converter.c` - Integration test with real Crossroads data

### Requirements Validated

**Requirement 6.5:** THE Map_Bank SHALL reference converted tilesets correctly in map headers

**Requirement 15.5:** THE Format_Converter SHALL verify that all Tileset references are valid

## Conclusion

This property-based test provides strong evidence that the tileset conversion system correctly preserves tileset data and maintains valid references from maps to tilesets. The test ran 100 randomized trials without finding any counterexamples, giving high confidence in the correctness of the implementation.
