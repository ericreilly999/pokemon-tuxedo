# Property 10: Script Command Conversion Test

## Overview

This property-based test validates **Property 10** from the design document:

> **Property 10: Script Command Conversion**
> 
> For any Emerald script command that has a FireRed equivalent, the conversion should produce the correct FireRed command with preserved parameters.

**Validates: Requirements 7.2, 7.4**

## What This Test Does

The test generates random scripts containing Emerald script commands (both compatible and incompatible with FireRed) and verifies that:

1. **Compatible commands are correctly mapped** - Each Emerald command with a FireRed equivalent is mapped to the correct FireRed command ID
2. **Parameters are preserved** - All command parameters are copied exactly during conversion
3. **Incompatible commands are skipped** - Commands without FireRed equivalents are properly filtered out
4. **Command order is maintained** - The relative order of compatible commands is preserved
5. **Script name is preserved** - The script name remains unchanged during conversion

## Test Configuration

- **Testing Library**: Theft (C property-based testing library)
- **Minimum Iterations**: 100 trials (as per spec requirements)
- **Command Types Tested**:
  - 22 compatible command types (MSGBOX, GIVEITEM, GIVEMON, SETFLAG, etc.)
  - 3 incompatible command types (SETWEATHER, DOWEATHER, SETMAPTILE)

## Compatible Commands Tested

The test validates conversion of these Emerald commands to FireRed:

- `MSGBOX` (0x0F) → `MSGBOX` (0x0F)
- `GIVEITEM` (0x1E) → `GIVEITEM` (0x1E)
- `GIVEMON` (0x2D) → `GIVEMON` (0x2D)
- `SETFLAG` (0x29) → `SETFLAG` (0x29)
- `CLEARFLAG` (0x2A) → `CLEARFLAG` (0x2A)
- `CHECKFLAG` (0x2B) → `CHECKFLAG` (0x2B)
- `WARP` (0x08) → `WARP` (0x08)
- `PLAYBGM` (0x22) → `PLAYBGM` (0x22)
- `FADESCREEN` (0x23) → `FADESCREEN` (0x23)
- `CALL` (0x09) → `CALL` (0x09)
- `GOTO` (0x05) → `GOTO` (0x05)
- `IF` (0x06) → `IF` (0x06)
- `CALLSTD` (0x09) → `CALLSTD` (0x09)
- `GOTOIF` (0x07) → `GOTOIF` (0x07)
- `RELEASE` (0x68) → `RELEASE` (0x68)
- `END` (0x02) → `END` (0x02)
- `RETURN` (0x03) → `RETURN` (0x03)
- `COMPARE` (0x21) → `COMPARE` (0x21)
- `SETVAR` (0x16) → `SETVAR` (0x16)
- `ADDVAR` (0x17) → `ADDVAR` (0x17)
- `SUBVAR` (0x18) → `SUBVAR` (0x18)
- `COPYVAR` (0x19) → `COPYVAR` (0x19)

## Incompatible Commands Tested

The test verifies that these Emerald commands (which have no FireRed equivalent) are properly skipped:

- `SETWEATHER` (0x5C) - Limited weather support in FireRed
- `DOWEATHER` (0x5D) - Limited weather support in FireRed
- `SETMAPTILE` (0x5E) - Different implementation in FireRed

## Test Strategy

### Input Generation

For each trial, the test generates:
- A random script name (e.g., "EventScript_123", "MapScript_456")
- 1-20 random script commands
- Each command has:
  - 80% chance of being a compatible command
  - 20% chance of being an incompatible command
  - 0-8 bytes of random parameters

### Verification Steps

For each generated script, the test:

1. **Converts the script** using `ConvertScript()`
2. **Verifies script name** is preserved
3. **Counts expected compatible commands** in the original script
4. **Verifies command count** matches expected compatible count
5. **For each converted command**:
   - Verifies command ID is correctly mapped
   - Verifies parameter count is preserved
   - Verifies parameter bytes are identical

## Building and Running

### Build the Test

```bash
cd firered-clone/tools/map_converter
make test_property_10_script_conversion.exe
```

### Run the Test

```bash
./test_property_10_script_conversion.exe
```

### Run with Specific Seed

To reproduce a specific test run:

```bash
./test_property_10_script_conversion.exe <seed>
```

Example:
```bash
./test_property_10_script_conversion.exe 9245053156926407079
```

## Expected Output

When all tests pass:

```
=== Property 10: Script Command Conversion Test ===
Validates: Requirements 7.2, 7.4
Testing that Emerald script commands with FireRed equivalents
are correctly converted with preserved parameters.

Using time-based seed: 9245053156926407079
Running 100 trials...

== PROP 'prop_script_command_conversion': 100 trials, seed 0x804d06dd2433c1a7
.................................................................................................

== PASS 'prop_script_command_conversion': pass 100, fail 0, skip 0, dup 0

=== Test Results ===
✓ Property 10 PASSED: All 100 trials succeeded
Script command conversion correctly maps compatible commands
and preserves all parameters during conversion.

Tested command types:
  - Compatible commands: 22 types
  - Incompatible commands: 3 types
```

## Implementation Details

### Key Functions Tested

- `MapScriptCommand()` - Maps Emerald command IDs to FireRed command IDs
- `ConvertScript()` - Converts entire scripts from Emerald to FireRed format
- `InitScript()` - Initializes script structures
- `FreeScript()` - Cleans up script memory

### Test File Location

- Test implementation: `firered-clone/tools/map_converter/test_property_10_script_conversion.c`
- Script converter: `firered-clone/tools/map_converter/map_converter.c`
- Header file: `firered-clone/tools/map_converter/map_converter.h`

## Related Tests

- **Unit Tests**: `test_script_converter.c` - Tests specific conversion scenarios
- **Property 28**: `test_property_28_roundtrip.c` - Tests map data round-trip conversion
- **Property 9**: `test_property_9_tileset_validity.c` - Tests tileset reference validity

## Success Criteria

The test passes when:
- All 100 trials complete successfully
- No command mapping errors occur
- All parameters are preserved correctly
- Incompatible commands are properly filtered
- Script names are preserved

## Failure Scenarios

The test will fail if:
- A compatible command is mapped to the wrong FireRed command
- Command parameters are corrupted or lost during conversion
- The command count doesn't match the expected compatible count
- Script names are not preserved
- Memory allocation errors occur

## Notes

- The test uses random generation to explore a wide variety of script structures
- Each run uses a different seed (based on current time) unless specified
- The 80/20 ratio of compatible/incompatible commands ensures good coverage of both paths
- Parameter sizes (0-8 bytes) cover typical script command parameter ranges
