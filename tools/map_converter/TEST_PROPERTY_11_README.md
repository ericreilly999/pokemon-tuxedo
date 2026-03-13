# Property 11: Trainer Reward Preservation Test

## Overview

This property-based test validates that trainer reward amounts are correctly preserved during the conversion from Pokemon Emerald (Crossroads) format to Pokemon FireRed format.

**Validates: Requirements 8.4**

## Property Statement

**Property 11: Trainer Reward Preservation**

*For any* trainer converted from Emerald to FireRed format, the reward amount should remain unchanged.

## What This Test Validates

The test ensures that:

1. **Reward Preservation**: The `reward` field is correctly copied during conversion without any modifications
2. **No Transformations**: No arithmetic operations or transformations are applied to the reward value
3. **Magnitude Independence**: The reward value is preserved exactly, regardless of its magnitude (0-9999)
4. **Complete Data Integrity**: All other trainer data is also preserved (name, class, AI flags, party)

## Test Implementation

### Test Framework

- **Library**: Theft (C property-based testing library)
- **Iterations**: 100 minimum (as per spec requirement)
- **Language**: C99

### Test Strategy

The test uses property-based testing to generate random trainer data with various reward amounts:

1. **Random Generation**: Creates trainers with:
   - Random names (from common trainer classes)
   - Random trainer classes (1-100)
   - Random reward amounts (0-9999) - **the critical field**
   - Random AI flags (0-255)
   - Random party sizes (1-6 Pokemon)
   - Random party data (species, levels, moves, IVs, EVs)

2. **Conversion**: Calls `ConvertTrainer()` to convert from Emerald to FireRed format

3. **Verification**: Checks that:
   - The reward amount is exactly preserved
   - The trainer name is preserved
   - The AI flags are preserved
   - The party size is preserved
   - All party Pokemon data is preserved (species, levels)

### Key Test Cases Covered

The property test automatically covers:

- **Zero rewards**: Trainers with no reward (reward = 0)
- **Small rewards**: Low-level trainers with small rewards (1-100)
- **Medium rewards**: Mid-level trainers with moderate rewards (100-1000)
- **Large rewards**: High-level trainers with large rewards (1000-9999)
- **Edge cases**: Maximum reward values (9999)

## Running the Test

### Build

```bash
cd firered-clone/tools/map_converter
make test_property_11_trainer_reward
```

### Execute

```bash
./test_property_11_trainer_reward
```

### With Custom Seed

```bash
./test_property_11_trainer_reward 12345
```

## Expected Output

### Success

```
=== Property 11: Trainer Reward Preservation Test ===
Validates: Requirements 8.4
Testing that trainer reward amounts are preserved during
Emerald to FireRed conversion.

Using time-based seed: 2876070645745911610
Running 100 trials...

== PROP 'prop_trainer_reward_preservation': 100 trials, seed 0x27e9db03a49d073a
.....................................................................................................

== PASS 'prop_trainer_reward_preservation': pass 100, fail 0, skip 0, dup 0

=== Test Results ===
Reward range tested: 0 - 9999

✓ Property 11 PASSED: All 100 trials succeeded
Trainer reward amounts are correctly preserved during conversion.
All trainer data (name, class, AI flags, party) is also preserved.
```

### Failure Example

If the test fails, it will show a counterexample:

```
Reward not preserved: original=1500, converted=1000 (trainer: Youngster_42)

== FAIL 'prop_trainer_reward_preservation': trial 23 failed
```

## Implementation Details

### Data Structures

The test uses the `TrainerData` structure defined in `map_converter.h`:

```c
typedef struct {
    char name[32];
    uint8_t trainer_class;
    uint8_t gender;
    uint8_t music;
    uint16_t sprite;
    char intro_text[256];
    char defeat_text[256];
    uint16_t reward;           // Critical field being tested
    uint8_t ai_flags;
    TrainerPokemon* party;
    uint8_t party_size;
} TrainerData;
```

### Generator Strategy

The test generator creates realistic trainer data:

- **Names**: Generated from common trainer class prefixes (Youngster, Lass, Bug Catcher, etc.)
- **Rewards**: Uniformly distributed across the full range (0-9999)
- **Party**: Realistic Pokemon with valid species (1-386), levels (1-100), and stats

### Verification Strategy

The test performs comprehensive verification:

1. **Primary Check**: Reward amount must match exactly
2. **Secondary Checks**: Name, AI flags, and party data must also be preserved
3. **Deep Verification**: Party Pokemon species and levels are checked individually

## Why This Property Matters

Trainer rewards are a critical game balance element:

- **Player Progression**: Rewards affect how quickly players earn money
- **Game Balance**: Incorrect rewards can make the game too easy or too hard
- **Data Integrity**: Reward preservation indicates overall conversion quality

If rewards are not preserved correctly, it suggests:
- Memory corruption during conversion
- Incorrect field mapping in the conversion function
- Data structure alignment issues

## Related Tests

- **Unit Test**: `test_convert_trainer_reward()` in `test_trainer_converter.c` - Tests a single specific case
- **Property Test 11**: This test - Validates the property across all possible inputs

## Maintenance Notes

### Updating the Test

If the `TrainerData` structure changes:

1. Update the generator in `alloc_trainer_data()` to handle new fields
2. Update the verification in `prop_trainer_reward_preservation()` to check new fields
3. Update the hash function if new fields affect uniqueness

### Common Issues

1. **Build Failures**: Ensure Theft library is built (`make` in `../../test/theft`)
2. **Linking Errors**: Verify `map_converter.c` is included in compilation
3. **Test Failures**: Check `ConvertTrainer()` implementation for reward handling

## References

- **Design Document**: `.kiro/specs/pokemon-tuxedo-full-regions/design.md` - Property 11
- **Requirements**: `.kiro/specs/pokemon-tuxedo-full-regions/requirements.md` - Requirement 8.4
- **Implementation**: `map_converter.c` - `ConvertTrainer()` function
- **Theft Library**: `../../test/theft/` - Property-based testing framework
