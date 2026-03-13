# Trainer Data Converter

## Overview

The Trainer Data Converter is part of the map conversion toolchain for porting Pokemon Crossroads (Emerald) data to FireRed format. It handles the conversion of trainer data including trainer classes, party Pokemon, AI flags, rewards, and text.

## Data Structures

### TrainerPokemon

Represents a single Pokemon in a trainer's party:

```c
typedef struct {
    uint16_t species;      // Pokemon species ID
    uint8_t level;         // Pokemon level
    uint16_t held_item;    // Held item ID (0 if none)
    uint16_t moves[4];     // Move IDs (0 if no move)
    uint8_t iv;            // Individual Values
    uint8_t ev_hp;         // HP Effort Values
    uint8_t ev_atk;        // Attack Effort Values
    uint8_t ev_def;        // Defense Effort Values
    uint8_t ev_speed;      // Speed Effort Values
    uint8_t ev_spatk;      // Special Attack Effort Values
    uint8_t ev_spdef;      // Special Defense Effort Values
} TrainerPokemon;
```

### TrainerData

Represents a complete trainer:

```c
typedef struct {
    char name[32];              // Trainer name
    uint8_t trainer_class;      // Trainer class ID
    uint8_t gender;             // Gender (0=male, 1=female)
    uint8_t music;              // Battle music ID
    uint16_t sprite;            // Sprite ID
    char intro_text[256];       // Text shown before battle
    char defeat_text[256];      // Text shown after defeat
    uint16_t reward;            // Money reward on defeat
    uint8_t ai_flags;           // AI behavior flags
    TrainerPokemon* party;      // Array of party Pokemon
    uint8_t party_size;         // Number of Pokemon in party
} TrainerData;
```

## Functions

### InitTrainerData

```c
void InitTrainerData(TrainerData* data);
```

Initializes a TrainerData structure with default values (zeros and NULLs).

**Parameters:**
- `data`: Pointer to TrainerData structure to initialize

### FreeTrainerData

```c
void FreeTrainerData(TrainerData* data);
```

Frees memory allocated for a TrainerData structure (specifically the party array).

**Parameters:**
- `data`: Pointer to TrainerData structure to free

### MapTrainerClass

```c
uint8_t MapTrainerClass(uint8_t emerald_class, bool* is_exact_match);
```

Maps an Emerald trainer class ID to the closest FireRed equivalent.

**Parameters:**
- `emerald_class`: Emerald trainer class ID
- `is_exact_match`: Output parameter set to true if exact match, false if approximate

**Returns:** FireRed trainer class ID

**Behavior:**
- Returns exact matches for trainer classes that exist in both games (Youngster, Bug Catcher, Gym Leaders, etc.)
- Returns approximate matches for Emerald-specific classes (Aqua Grunt → Rocket, Magma Admin → Cooltrainer, etc.)
- Returns default class (Youngster) for unknown classes

### ConvertTrainer

```c
bool ConvertTrainer(const TrainerData* emerald_trainer,
                    TrainerData* firered_trainer,
                    char* error_buffer, size_t error_size);
```

Converts an Emerald trainer to FireRed format.

**Parameters:**
- `emerald_trainer`: Source trainer data in Emerald format
- `firered_trainer`: Output trainer data in FireRed format
- `error_buffer`: Buffer for error/warning messages
- `error_size`: Size of error buffer

**Returns:** true on success, false on failure

**Behavior:**
- Preserves trainer name, gender, music, sprite
- Maps trainer class using MapTrainerClass()
- **Preserves reward amount** (Requirement 8.4)
- **Preserves AI flags** (Requirement 8.3)
- Preserves intro and defeat text
- Copies all party Pokemon with species, levels, held items, moves, IVs, and EVs
- Logs warnings for approximate trainer class mappings

## Requirements Validated

This implementation satisfies the following requirements from the design document:

- **Requirement 8.1**: Extract Trainer_Data from Crossroads_Source
- **Requirement 8.2**: Convert trainer party structures to FireRed format
- **Requirement 8.3**: Convert trainer AI flags to FireRed format (preserved as-is)
- **Requirement 8.4**: Preserve trainer reward amounts during conversion
- **Requirement 8.5**: Map Emerald trainer classes to FireRed trainer classes
- **Requirement 8.6**: Log trainer class substitutions when no exact match exists

## Trainer Class Mappings

The converter includes a comprehensive mapping table for trainer classes:

### Exact Matches (Examples)
- Youngster (1 → 1)
- Bug Catcher (2 → 2)
- Lass (3 → 3)
- Gym Leaders (Brock, Misty, Lt. Surge, etc.)
- Elite Four (Bruno, Lorelei, Agatha, Lance)

### Approximate Matches (Examples)
- Aqua Grunt (50) → Youngster (1)
- Magma Grunt (51) → Rocket (28)
- Aqua Admin (52) → Cooltrainer Male (29)
- Hex Maniac (56) → Bird Keeper (23)
- Aroma Lady (57) → Gentleman (39)
- Ruin Maniac (58) → Picnicker (6)

### Default Mapping
- Unknown classes → Youngster (1)

## Usage Example

```c
#include "map_converter.h"

// Create source trainer
TrainerData emerald_trainer;
InitTrainerData(&emerald_trainer);
strcpy(emerald_trainer.name, "Brock");
emerald_trainer.trainer_class = 32; // Brock class in Emerald
emerald_trainer.reward = 1400;
emerald_trainer.ai_flags = 0x03;

// Set up party
emerald_trainer.party_size = 2;
emerald_trainer.party = calloc(2, sizeof(TrainerPokemon));
emerald_trainer.party[0].species = 74;  // Geodude
emerald_trainer.party[0].level = 12;
emerald_trainer.party[1].species = 95;  // Onix
emerald_trainer.party[1].level = 14;

// Convert to FireRed format
TrainerData firered_trainer;
char error_buffer[512];
bool success = ConvertTrainer(&emerald_trainer, &firered_trainer, 
                              error_buffer, sizeof(error_buffer));

if (success) {
    printf("Conversion successful!\n");
    printf("Trainer: %s\n", firered_trainer.name);
    printf("Class: %u\n", firered_trainer.trainer_class);
    printf("Reward: %u\n", firered_trainer.reward);
    printf("Party size: %u\n", firered_trainer.party_size);
}

// Clean up
free(emerald_trainer.party);
FreeTrainerData(&firered_trainer);
```

## Testing

Unit tests are provided in `test_trainer_converter.c`. To run the tests:

```bash
cd firered-clone/tools/map_converter
gcc -o test_trainer_converter test_trainer_converter.c map_converter.c -I. -std=c99
./test_trainer_converter
```

### Test Coverage

The unit tests verify:
1. Initialization of TrainerData structures
2. Exact trainer class mappings
3. Approximate trainer class mappings
4. Unknown trainer class handling
5. Trainer name preservation
6. **Reward amount preservation** (Requirement 8.4)
7. **AI flags preservation** (Requirement 8.3)
8. Party data preservation (species, levels, moves, IVs, EVs)
9. Empty party handling
10. Intro/defeat text preservation
11. NULL parameter handling
12. Memory management (FreeTrainerData)

All tests pass successfully.

## Integration with Map Converter

The trainer data converter is part of the larger map conversion pipeline:

1. **Map Parser** - Extracts map data from Emerald format
2. **Tileset Converter** - Converts tileset graphics and behavior
3. **Script Converter** - Converts event scripts
4. **Trainer Converter** - Converts trainer data (this component)
5. **Wild Encounter Converter** - Converts wild Pokemon encounters

The trainer converter will be used during Phase 2-4 of the implementation plan when porting Johto, Hoenn, and Sinnoh regions.

## Future Enhancements

Potential improvements for future versions:

1. **Extended Trainer Class Mappings**: Add more comprehensive mappings as more trainer classes are identified
2. **Move Compatibility Checking**: Validate that moves are available in FireRed
3. **Species Compatibility Checking**: Validate that Pokemon species exist in FireRed
4. **Held Item Mapping**: Map Emerald-specific held items to FireRed equivalents
5. **AI Flag Translation**: Translate Emerald AI flags to FireRed AI system if they differ
6. **Batch Conversion**: Support converting multiple trainers at once from a file

## Notes

- The trainer class mapping table is based on common trainer classes between Emerald and FireRed
- Approximate mappings are logged as warnings in the error buffer
- All trainer data (rewards, AI flags, party data) is preserved during conversion
- Memory management is handled through InitTrainerData and FreeTrainerData functions
- The converter is designed to be robust with NULL parameter checking and error reporting
