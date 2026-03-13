# Script Command Converter

## Overview

The Script Command Converter is part of the map conversion toolchain for porting Pokemon Crossroads (Emerald) maps to FireRed format. It handles the conversion of event scripts by mapping Emerald script commands to their FireRed equivalents.

## Features

- **Command Mapping**: Automatically maps compatible Emerald script commands to FireRed commands
- **Incompatible Command Detection**: Identifies and logs commands that have no FireRed equivalent
- **Parameter Preservation**: Maintains script command parameters during conversion
- **Detailed Logging**: Reports incompatible commands with map location and script name

## API Reference

### Data Structures

#### ScriptCommand
```c
typedef struct {
    uint16_t command_id;      // Script command ID
    uint8_t* parameters;      // Command parameters
    size_t param_count;       // Number of parameters
} ScriptCommand;
```

#### Script
```c
typedef struct {
    char name[MAX_NAME_LENGTH];  // Script name
    ScriptCommand* commands;     // Array of commands
    size_t command_count;        // Number of commands
} Script;
```

### Functions

#### MapScriptCommand
```c
uint16_t MapScriptCommand(uint16_t emerald_command, bool* has_equivalent);
```

Maps an Emerald script command ID to its FireRed equivalent.

**Parameters:**
- `emerald_command`: The Emerald command ID to map
- `has_equivalent`: Output parameter set to true if a mapping exists, false otherwise

**Returns:** The FireRed command ID (0 if no equivalent exists)

**Example:**
```c
bool has_equiv = false;
uint16_t firered_cmd = MapScriptCommand(EMERALD_CMD_MSGBOX, &has_equiv);
if (has_equiv) {
    printf("MSGBOX maps to 0x%04X\n", firered_cmd);
}
```

#### ConvertScript
```c
bool ConvertScript(const Script* emerald_script,
                   Script* firered_script,
                   const char* map_name,
                   char* error_buffer, size_t error_size);
```

Converts a complete Emerald script to FireRed format.

**Parameters:**
- `emerald_script`: Input Emerald script to convert
- `firered_script`: Output FireRed script (must be initialized)
- `map_name`: Name of the map containing this script (for logging)
- `error_buffer`: Buffer for error/warning messages
- `error_size`: Size of error buffer

**Returns:** true on success, false on failure

**Behavior:**
- Compatible commands are converted and added to output
- Incompatible commands are skipped and logged
- Returns true even if incompatible commands are found (with warnings in error_buffer)
- Returns false only on critical errors (memory allocation, invalid parameters)

**Example:**
```c
Script emerald_script, firered_script;
char error_buffer[512];

// ... initialize and populate emerald_script ...

if (ConvertScript(&emerald_script, &firered_script, "NewBarkTown", 
                  error_buffer, sizeof(error_buffer))) {
    printf("Conversion result: %s\n", error_buffer);
    // Use firered_script...
    FreeScript(&firered_script);
}
FreeScript(&emerald_script);
```

#### InitScript
```c
void InitScript(Script* script);
```

Initializes a Script structure with default values.

**Parameters:**
- `script`: Script structure to initialize

#### FreeScript
```c
void FreeScript(Script* script);
```

Frees all memory allocated for a Script structure.

**Parameters:**
- `script`: Script structure to free

## Compatible Commands

The following Emerald commands have direct FireRed equivalents:

| Command | Emerald ID | FireRed ID | Description |
|---------|-----------|-----------|-------------|
| MSGBOX | 0x0F | 0x0F | Display message box |
| GIVEITEM | 0x1E | 0x1E | Give item to player |
| GIVEMON | 0x2D | 0x2D | Give Pokemon to player |
| SETFLAG | 0x29 | 0x29 | Set a flag |
| CLEARFLAG | 0x2A | 0x2A | Clear a flag |
| CHECKFLAG | 0x2B | 0x2B | Check flag status |
| WARP | 0x08 | 0x08 | Warp to location |
| PLAYBGM | 0x22 | 0x22 | Play background music |
| FADESCREEN | 0x23 | 0x23 | Fade screen effect |
| CALL | 0x09 | 0x09 | Call subroutine |
| GOTO | 0x05 | 0x05 | Jump to label |
| IF | 0x06 | 0x06 | Conditional branch |
| CALLSTD | 0x09 | 0x09 | Call standard function |
| GOTOIF | 0x07 | 0x07 | Conditional jump |
| RELEASE | 0x68 | 0x68 | Release script control |
| END | 0x02 | 0x02 | End script |
| RETURN | 0x03 | 0x03 | Return from subroutine |
| COMPARE | 0x21 | 0x21 | Compare values |
| SETVAR | 0x16 | 0x16 | Set variable |
| ADDVAR | 0x17 | 0x17 | Add to variable |
| SUBVAR | 0x18 | 0x18 | Subtract from variable |
| COPYVAR | 0x19 | 0x19 | Copy variable |

## Incompatible Commands

The following Emerald commands have no FireRed equivalent and will be logged:

| Command | Emerald ID | Reason |
|---------|-----------|--------|
| SETWEATHER | 0x5C | Limited weather support in FireRed |
| DOWEATHER | 0x5D | Limited weather support in FireRed |
| SETMAPTILE | 0x5E | Different implementation in FireRed |

When these commands are encountered:
1. They are skipped (not added to output script)
2. A warning is logged with the map name, script name, and command index
3. Conversion continues with remaining commands

## Error Handling

### Success Cases
- Returns true with empty error_buffer if all commands are compatible
- Returns true with warning message if some commands are incompatible

### Failure Cases
- Returns false if memory allocation fails
- Returns false if invalid parameters are provided
- error_buffer contains detailed error message

### Example Error Messages

**Incompatible commands:**
```
Script conversion completed with 2 incompatible command(s):
Map 'NewBarkTown', Script 'EventScript_WeatherChange': Incompatible command 0x005C at index 3
Map 'NewBarkTown', Script 'EventScript_WeatherChange': Incompatible command 0x005D at index 4
```

**Memory allocation failure:**
```
Failed to allocate memory for script commands
```

## Testing

Unit tests are provided in `test_script_converter.c`:

```bash
# Compile tests
gcc -o test_script_converter test_script_converter.c map_converter.c -I. -Wall -Wextra

# Run tests
./test_script_converter
```

Test coverage includes:
- Mapping compatible commands
- Identifying incompatible commands
- Converting scripts with all compatible commands
- Converting scripts with mixed compatible/incompatible commands
- Handling empty scripts
- Memory management (InitScript/FreeScript)

## Integration with Map Converter

The script converter is integrated into the map conversion pipeline:

1. **Extraction**: Scripts are extracted from Emerald map data
2. **Conversion**: Each script is converted using `ConvertScript()`
3. **Validation**: Incompatible commands are logged for manual review
4. **Output**: Converted scripts are written to FireRed map files

## Requirements Validation

This implementation satisfies the following requirements:

- **Requirement 7.1**: Extract Event_Script data from Crossroads_Source maps
- **Requirement 7.2**: Convert Emerald script commands to FireRed script commands
- **Requirement 7.3**: Preserve script logic during conversion
- **Requirement 7.4**: Update script references to FireRed-compatible constants
- **Requirement 7.5**: Log incompatible commands with map location

## Future Enhancements

Potential improvements for future versions:

1. **Extended Command Set**: Add more command mappings as they are identified
2. **Parameter Conversion**: Handle cases where parameter formats differ between versions
3. **Script Optimization**: Optimize converted scripts for size/performance
4. **Fallback Behaviors**: Implement safe fallback behaviors for incompatible commands
5. **Script Validation**: Validate script logic and references after conversion

## See Also

- [Map Converter README](README.md) - Main map converter documentation
- [Tileset Converter README](TILESET_CONVERTER_README.md) - Tileset conversion documentation
- [Design Document](../../.kiro/specs/pokemon-tuxedo-full-regions/design.md) - Full system design
