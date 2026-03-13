# Map Converter Tool

This tool converts Pokemon map data between Emerald and FireRed formats. It's designed to support porting maps from Pokemon Crossroads (Emerald-based) to the FireRed decomp project.

## Features

- Parse Emerald-format map.json files into an intermediate representation
- Format intermediate representation into FireRed-compatible map.json files
- Round-trip validation to ensure data integrity
- Comprehensive error handling with detailed error messages
- Support for all map event types (objects, warps, coord events, bg events)

## Building

```bash
make
```

This will build two executables:
- `map_converter` - Main conversion tool
- `test_converter` - Test program for validation

## Usage

### Convert a map

```bash
./map_converter convert <input_emerald.json> <output_firered.json>
```

Example:
```bash
./map_converter convert emerald_map.json firered_map.json
```

### Validate two maps

```bash
./map_converter validate <map1.json> <map2.json>
```

Example:
```bash
./map_converter validate original.json converted.json
```

## Data Structures

### MapData

The intermediate representation includes:

- **Basic fields**: name, id, layout, music, region_map_section, weather, map_type, battle_scene
- **Boolean flags**: requires_flash, allow_cycling, allow_escaping, allow_running, show_map_name
- **Numeric fields**: floor_number
- **Event arrays**: connections, warps, objects, coord_events, bg_events

### Supported Event Types

1. **Connections**: Map connections (north, south, east, west)
2. **Warp Events**: Doorways and transitions between maps
3. **Object Events**: NPCs, trainers, and interactive objects
4. **Coord Events**: Trigger scripts and weather changes at specific coordinates
5. **BG Events**: Signs, hidden items, and secret bases

## Error Handling

The tool provides detailed error messages including:
- File I/O errors with file paths
- JSON parsing errors with error descriptions
- Validation failures with specific field mismatches

All errors are written to stderr with descriptive messages.

## Testing

A test map (`test_map.json`) is included to verify functionality:

```bash
./test_converter test_map.json output.json
```

This will:
1. Parse the test map
2. Convert it to FireRed format
3. Parse the converted map
4. Validate round-trip equivalence

## Implementation Details

- Written in C++ using the json11 library for JSON parsing
- Uses intermediate MapData structure to decouple input/output formats
- Supports up to:
  - 16 connections per map
  - 128 warps per map
  - 128 objects per map
  - 64 coord events per map
  - 128 bg events per map

## Requirements

This task implements:
- **Requirement 5.1**: Export map data from Crossroads source
- **Requirement 5.2**: Convert Emerald-format Map_Header to FireRed format
- **Requirement 19.1**: Parse valid Emerald map data into intermediate object
- **Requirement 19.2**: Return descriptive errors for invalid Emerald map data
- **Requirement 19.3**: Format intermediate Map_Header objects into valid FireRed map data files

## Future Enhancements

- Batch conversion of multiple maps
- Automatic tileset reference mapping
- Script command conversion
- Trainer data conversion
- Wild encounter table conversion
