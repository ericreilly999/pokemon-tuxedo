# Property Tests 12 & 13: Encounter Conversion

## Overview

This document describes the property-based tests for wild encounter conversion from Pokemon Crossroads (Emerald) to FireRed format. These tests validate that encounter data is correctly preserved during the conversion process.

## Properties Tested

### Property 12: Encounter Rate Preservation

**Statement**: *For any* wild encounter table converted from Emerald to FireRed format, the encounter rates should remain unchanged.

**Validates**: Requirements 9.3

**Description**: This property ensures that the base encounter rate (0-255) is preserved exactly during conversion. The encounter rate determines how frequently wild Pokemon appear in a location, and any change to this value would alter gameplay balance.

**Test Strategy**:
- Generate random encounter tables with rates ranging from 0 to 255
- Convert each table from Emerald to FireRed format
- Verify the encounter_rate field is identical before and after conversion
- Test with 100+ random encounter tables to ensure comprehensive coverage

**Success Criteria**:
- All 100 trials pass
- Encounter rates are preserved for all values (0-255)
- No arithmetic transformations are applied to the rate

### Property 13: Pokemon Species Preservation

**Statement**: *For any* wild encounter table converted from Emerald to FireRed format, all Pokemon species and their level ranges should remain unchanged.

**Validates**: Requirements 9.4

**Description**: This property ensures that all encounter slots preserve their Pokemon species IDs, minimum levels, and maximum levels during conversion. This applies to all encounter types: grass, water, fishing (old/good/super rod), and rock smash.

**Test Strategy**:
- Generate random encounter tables with various Pokemon species (1-386)
- Generate random level ranges (1-100) for each encounter slot
- Include empty slots (species 0) to test edge cases
- Convert each table from Emerald to FireRed format
- Verify all encounter slots preserve species, min_level, and max_level
- Test all encounter types: grass (12 slots), water (5 slots), fishing (15 slots), rock smash (5 slots)
- Test with 100+ random encounter tables

**Success Criteria**:
- All 100 trials pass
- All species IDs are preserved (including 0 for empty slots)
- All min_level values are preserved
- All max_level values are preserved
- All encounter types (grass, water, fishing, rock smash) are verified

## Test Implementation

### File Structure

```
test_property_12_13_encounter_conversion.c
├── Test environment structure (test_env)
├── Random data generators
│   └── generate_encounter_slot()
├── Theft type info callbacks
│   ├── alloc_encounter_data()
│   ├── free_encounter_data()
│   ├── hash_encounter_data()
│   └── print_encounter_data()
├── Property test functions
│   ├── prop_encounter_rate_preservation()
│   └── prop_pokemon_species_preservation()
└── main() - Test runner
```

### Data Generation

The test generates random `WildEncounterData` structures with:

**Encounter Rate**:
- Random value from 0 to 255
- Covers full range of possible encounter rates

**Encounter Slots** (for each type):
- 30% chance of empty slot (species 0)
- 70% chance of populated slot with:
  - Random species: 1-386 (Gen 3 Pokemon)
  - Random level range: 1-100
  - Ensures min_level <= max_level

**Encounter Types**:
- Grass encounters: 12 slots
- Water encounters: 5 slots
- Fishing old rod: 5 slots
- Fishing good rod: 5 slots
- Fishing super rod: 5 slots
- Rock smash: 5 slots

Total: 37 encounter slots per test case

### Test Execution

```bash
# Build the test
make test_property_12_13_encounter_conversion

# Run with default seed (time-based)
./test_property_12_13_encounter_conversion

# Run with specific seed for reproducibility
./test_property_12_13_encounter_conversion 1234567890
```

### Expected Output

```
=== Property 12 & 13: Encounter Conversion Tests ===
Property 12: Encounter Rate Preservation (Validates: Requirements 9.3)
Property 13: Pokemon Species Preservation (Validates: Requirements 9.4)
Testing that encounter rates, species, and level ranges are preserved
during Emerald to FireRed conversion.

Using time-based seed: 1234567890

--- Testing Property 12: Encounter Rate Preservation ---
Running 100 trials...

=== Property 12 Results ===
Encounter rate range tested: 0 - 255
✓ Property 12 PASSED: All 100 trials succeeded
Encounter rates are correctly preserved during conversion.

--- Testing Property 13: Pokemon Species Preservation ---
Running 100 trials...

=== Property 13 Results ===
Species range tested: 1 - 386
Level range tested: 1 - 100
✓ Property 13 PASSED: All 100 trials succeeded
Pokemon species and level ranges are correctly preserved.
All encounter types (grass, water, fishing, rock smash) verified.

=== Overall Test Results ===
✓ ALL PROPERTIES PASSED
Encounter conversion correctly preserves rates, species, and level ranges.
```

## Integration with Build System

Add to `Makefile`:

```makefile
# Property test for encounter conversion
test_property_12_13_encounter_conversion: test_property_12_13_encounter_conversion.c map_converter.c
	$(CC) $(CFLAGS) -o $@ $^ -ltheft

.PHONY: test_property_12_13
test_property_12_13: test_property_12_13_encounter_conversion
	./test_property_12_13_encounter_conversion
```

## Failure Analysis

### Property 12 Failure

If Property 12 fails, the encounter rate is not being preserved. Possible causes:

1. **Arithmetic transformation**: The conversion function is modifying the rate value
2. **Type mismatch**: Incorrect data type causing truncation or overflow
3. **Uninitialized memory**: The output structure is not properly initialized
4. **Copy error**: The rate field is not being copied from source to destination

**Example failure output**:
```
Encounter rate not preserved: original=128, converted=64
✗ Property 12 FAILED: Found counterexample(s)
```

**Debugging steps**:
1. Check `ConvertEncounters()` implementation
2. Verify `firered_encounters->encounter_rate = emerald_encounters->encounter_rate;`
3. Ensure no arithmetic operations on the rate value
4. Check for type casting issues

### Property 13 Failure

If Property 13 fails, species or level ranges are not being preserved. Possible causes:

1. **Array copy error**: Encounter slots are not being copied correctly
2. **Index mismatch**: Wrong array indices are being used
3. **Partial copy**: Only some fields are being copied (e.g., species but not levels)
4. **Type mismatch**: Data type issues causing value corruption

**Example failure output**:
```
Grass encounter 3 species not preserved: 25 != 0
✗ Property 13 FAILED: Found counterexample(s)
```

**Debugging steps**:
1. Check array copy loops in `ConvertEncounters()`
2. Verify all three fields are copied: species, min_level, max_level
3. Check array bounds (12 grass, 5 water, 5 fishing each, 5 rock smash)
4. Ensure proper structure initialization

## Relationship to Unit Tests

These property tests complement the unit tests in `test_encounter_converter.c`:

**Unit Tests** (test_encounter_converter.c):
- Test specific, hand-crafted examples
- Test edge cases (empty data, invalid ranges, null parameters)
- Test individual encounter types separately
- Provide deterministic, reproducible test cases

**Property Tests** (test_property_12_13_encounter_conversion.c):
- Test randomly generated encounter tables
- Test the full range of possible values (rates 0-255, species 1-386, levels 1-100)
- Test all encounter types together
- Provide statistical confidence through 100+ trials
- Discover edge cases that weren't anticipated

**Together**, they provide comprehensive test coverage:
- Unit tests verify known scenarios work correctly
- Property tests verify the conversion works for *all* possible inputs

## Requirements Traceability

| Property | Requirement | Description |
|----------|-------------|-------------|
| Property 12 | 9.3 | Preserve encounter rates during conversion |
| Property 13 | 9.4 | Preserve Pokemon species and level ranges during conversion |

Both properties also indirectly validate:
- Requirement 9.1: Extract Wild_Encounter_Table data from Crossroads_Source
- Requirement 9.2: Convert encounter table structures to FireRed format
- Requirement 9.5: Handle encounter types correctly for FireRed

## Test Configuration

**Theft Library Configuration**:
- Minimum trials: 100 (as specified in design document)
- Seed: Time-based by default, can be overridden via command line
- Type info: Custom allocator, hasher, and printer for WildEncounterData
- Environment: Tracks statistics (min/max rates, species, levels)

**Test Environment Statistics**:
The test tracks and reports:
- Encounter rate range tested (min-max)
- Species range tested (min-max)
- Level range tested (min-max)

This provides confidence that the test covered a wide range of values.

## Future Enhancements

Potential improvements for these property tests:

1. **Shrinking**: Implement shrinking callbacks to find minimal failing examples
2. **Constraint testing**: Test that level ranges satisfy min <= max
3. **Species validation**: Verify species IDs are valid Pokemon
4. **Probability testing**: Verify encounter slot probabilities are correct
5. **Cross-property testing**: Combine with other properties (e.g., round-trip)
6. **Performance testing**: Measure conversion performance with large datasets

## References

- Design Document: `.kiro/specs/pokemon-tuxedo-full-regions/design.md`
- Requirements Document: `.kiro/specs/pokemon-tuxedo-full-regions/requirements.md`
- Task List: `.kiro/specs/pokemon-tuxedo-full-regions/tasks.md` (Task 1.10)
- Header File: `map_converter.h`
- Implementation: `map_converter.c`
- Unit Tests: `test_encounter_converter.c`
- Property Test Implementation: `test_property_12_13_encounter_conversion.c`
- Theft Library: https://github.com/silentbicycle/theft

## Conclusion

These property-based tests provide strong guarantees that wild encounter conversion preserves all critical data (rates, species, level ranges) across all encounter types. By testing 100+ randomly generated encounter tables, we gain statistical confidence that the conversion works correctly for the full range of possible inputs, not just hand-picked examples.
