# Wild Encounter Converter

## Overview

The Wild Encounter Converter transforms Pokemon Crossroads (Emerald) wild encounter data to FireRed format. This converter is part of the multi-region support implementation for Pokemon Tuxedo ROM hack.

## Purpose

The converter enables porting of wild encounter tables from Pokemon Crossroads to FireRed, preserving:
- Encounter rates
- Pokemon species
- Level ranges
- All encounter types (grass, water, fishing, rock smash)

## Data Structures

### EncounterSlot

Represents a single encounter slot with a Pokemon species and level range:

```c
typedef struct {
    uint16_t species;      // Pokemon species ID
    uint8_t min_level;     // Minimum encounter level
    uint8_t max_level;     // Maximum encounter level
} EncounterSlot;
```

### WildEncounterData

Complete wild encounter data for a map location:

```c
typedef struct {
    uint8_t encounter_rate;           // Base encounter rate (0-255)
    EncounterSlot grass_encounters[12];   // Grass/tall grass encounters
    EncounterSlot water_encounters[5];    // Surfing encounters
    EncounterSlot fishing_old[5];         // Old Rod fishing
    EncounterSlot fishing_good[5];        // Good Rod fishing
    EncounterSlot fishing_super[5];       // Super Rod fishing
    EncounterSlot rock_smash[5];          // Rock Smash encounters
} WildEncounterData;
```

## Functions

### InitWildEncounterData

Initializes a WildEncounterData structure with default values (all zeros).

```c
void InitWildEncounterData(WildEncounterData* data);
```

**Parameters:**
- `data`: Pointer to WildEncounterData structure to initialize

**Behavior:**
- Sets encounter_rate to 0
- Clears all encounter slots (species=0, levels=0)

### ConvertEncounters

Converts Emerald wild encounter data to FireRed format.

```c
bool ConvertEncounters(const WildEncounterData* emerald_encounters,
                       WildEncounterData* firered_encounters,
                       char* error_buffer, size_t error_size);
```

**Parameters:**
- `emerald_encounters`: Source encounter data from Emerald/Crossroads
- `firered_encounters`: Output encounter data in FireRed format
- `error_buffer`: Buffer for error/success messages
- `error_size`: Size of error buffer

**Returns:**
- `true` on successful conversion
- `false` on error (with error message in error_buffer)

**Behavior:**
1. Validates input parameters
2. Initializes output structure
3. Preserves encounter rate (Requirement 9.3)
4. Copies all encounter slots preserving species and level ranges (Requirement 9.4)
5. Handles all encounter types correctly for FireRed (Requirement 9.5):
   - Grass encounters (12 slots)
   - Water encounters (5 slots)
   - Fishing encounters (Old/Good/Super Rod, 5 slots each)
   - Rock Smash encounters (5 slots)
6. Validates level ranges (min_level <= max_level)
7. Returns success/error status with descriptive message

## Requirements Validation

The converter satisfies the following requirements from the design document:

- **Requirement 9.1**: Extract Wild_Encounter_Table data from Crossroads_Source
- **Requirement 9.2**: Convert encounter table structures to FireRed format
- **Requirement 9.3**: Preserve encounter rates during conversion
- **Requirement 9.4**: Preserve Pokemon species and level ranges during conversion
- **Requirement 9.5**: Handle encounter types (grass, water, fishing, rock smash) correctly for FireRed

## Usage Example

```c
#include "map_converter.h"

// Initialize encounter data
WildEncounterData emerald_data, firered_data;
char error_buffer[512];

InitWildEncounterData(&emerald_data);

// Set up Emerald encounter data
emerald_data.encounter_rate = 20;

// Grass encounters
emerald_data.grass_encounters[0].species = 25;  // Pikachu
emerald_data.grass_encounters[0].min_level = 3;
emerald_data.grass_encounters[0].max_level = 5;

emerald_data.grass_encounters[1].species = 16;  // Pidgey
emerald_data.grass_encounters[1].min_level = 2;
emerald_data.grass_encounters[1].max_level = 4;

// Water encounters
emerald_data.water_encounters[0].species = 129;  // Magikarp
emerald_data.water_encounters[0].min_level = 5;
emerald_data.water_encounters[0].max_level = 10;

// Convert to FireRed format
bool success = ConvertEncounters(&emerald_data, &firered_data, 
                                 error_buffer, sizeof(error_buffer));

if (success) {
    printf("Conversion successful: %s\n", error_buffer);
    printf("Encounter rate: %u\n", firered_data.encounter_rate);
    printf("First grass encounter: Species %u, Levels %u-%u\n",
           firered_data.grass_encounters[0].species,
           firered_data.grass_encounters[0].min_level,
           firered_data.grass_encounters[0].max_level);
} else {
    printf("Conversion failed: %s\n", error_buffer);
}
```

## Testing

### Unit Tests

The converter includes comprehensive unit tests in `test_encounter_converter.c`:

1. **test_init_wild_encounter_data**: Verifies initialization sets all fields to zero
2. **test_convert_empty_encounters**: Tests conversion of empty encounter data
3. **test_convert_grass_encounters**: Tests grass encounter conversion with rate preservation
4. **test_convert_water_encounters**: Tests water encounter conversion
5. **test_convert_fishing_encounters**: Tests all fishing rod types
6. **test_convert_rock_smash_encounters**: Tests rock smash encounters
7. **test_convert_all_encounter_types**: Tests conversion of all types together
8. **test_invalid_level_range**: Tests detection of invalid level ranges (min > max)
9. **test_null_parameters**: Tests null parameter handling

### Running Tests

```bash
# Build the test
make test_encounter_converter

# Run the test
./test_encounter_converter
```

Expected output:
```
=== Wild Encounter Converter Unit Tests ===

Test 1: Initialize wild encounter data...
  PASS: Wild encounter data initialized correctly
Test 2: Convert empty encounter data...
  PASS: Empty encounter data converted correctly
  Message: Wild encounter conversion successful (no encounters defined)
Test 3: Convert grass encounters with encounter rate preservation...
  PASS: Grass encounters converted correctly
  Message: Wild encounter conversion successful (rate=20)
...
=== All tests passed! ===
```

## Error Handling

The converter performs the following validations:

1. **Null Parameter Check**: Returns false if any required parameter is NULL
2. **Level Range Validation**: Ensures min_level <= max_level for all encounters
3. **Descriptive Error Messages**: Provides detailed error information including:
   - Encounter type (grass, water, fishing, rock smash)
   - Slot number
   - Invalid values

Example error messages:
- `"Invalid parameters to ConvertEncounters"`
- `"Invalid level range for grass encounter slot 0: min=10 > max=5"`
- `"Wild encounter conversion successful (rate=20)"`

## Integration

The wild encounter converter integrates with the broader map conversion pipeline:

1. **Map Parser** extracts encounter data from Crossroads maps
2. **Encounter Converter** transforms encounter structures
3. **Validation Engine** verifies converted encounter data
4. **Build System** compiles encounter data into ROM

## Future Enhancements

Potential improvements for the converter:

1. **Encounter Probability Calculation**: Calculate individual slot probabilities
2. **Species Validation**: Verify Pokemon species IDs are valid
3. **Level Cap Enforcement**: Ensure levels don't exceed game limits
4. **Encounter Type Detection**: Auto-detect which encounter types are present
5. **JSON Import/Export**: Support JSON format for encounter data
6. **Batch Conversion**: Convert multiple maps' encounters at once

## References

- Design Document: `.kiro/specs/pokemon-tuxedo-full-regions/design.md`
- Requirements Document: `.kiro/specs/pokemon-tuxedo-full-regions/requirements.md`
- Task List: `.kiro/specs/pokemon-tuxedo-full-regions/tasks.md` (Task 1.9)
- Header File: `map_converter.h`
- Implementation: `map_converter.c`
- Unit Tests: `test_encounter_converter.c`
