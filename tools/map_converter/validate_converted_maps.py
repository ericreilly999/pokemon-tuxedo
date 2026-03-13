#!/usr/bin/env python3
"""
Validation script for converted Johto maps.
Checks for:
- Valid JSON structure
- Required fields present
- Map connections reference valid maps
- Warp destinations are valid
"""

import json
import os
import sys

REQUIRED_FIELDS = [
    "id", "name", "layout", "music", "region_map_section",
    "requires_flash", "weather", "map_type", "allow_cycling",
    "allow_escaping", "allow_running", "show_map_name",
    "floor_number", "battle_scene"
]

REQUIRED_ARRAYS = [
    "connections", "object_events", "warp_events",
    "coord_events", "bg_events"
]

def validate_map(map_path):
    """Validate a single converted map."""
    errors = []
    warnings = []
    
    # Check if file exists
    if not os.path.exists(map_path):
        return False, [f"File not found: {map_path}"], []
    
    # Load JSON
    try:
        with open(map_path, 'r') as f:
            map_data = json.load(f)
    except json.JSONDecodeError as e:
        return False, [f"Invalid JSON: {e}"], []
    except Exception as e:
        return False, [f"Error reading file: {e}"], []
    
    # Check required fields
    for field in REQUIRED_FIELDS:
        if field not in map_data:
            errors.append(f"Missing required field: {field}")
    
    # Check required arrays
    for array in REQUIRED_ARRAYS:
        if array not in map_data:
            errors.append(f"Missing required array: {array}")
        elif not isinstance(map_data[array], list):
            errors.append(f"Field {array} must be an array")
    
    # Validate connections
    if "connections" in map_data:
        for i, conn in enumerate(map_data["connections"]):
            if "direction" not in conn:
                errors.append(f"Connection {i}: missing direction")
            if "map" not in conn:
                errors.append(f"Connection {i}: missing map reference")
            if "offset" not in conn:
                warnings.append(f"Connection {i}: missing offset")
    
    # Validate warps
    if "warp_events" in map_data:
        for i, warp in enumerate(map_data["warp_events"]):
            if "dest_map" not in warp:
                errors.append(f"Warp {i}: missing dest_map")
            if "dest_warp_id" not in warp:
                errors.append(f"Warp {i}: missing dest_warp_id")
            if "x" not in warp or "y" not in warp:
                errors.append(f"Warp {i}: missing coordinates")
    
    # Validate object events
    if "object_events" in map_data:
        for i, obj in enumerate(map_data["object_events"]):
            if "x" not in obj or "y" not in obj:
                errors.append(f"Object {i}: missing coordinates")
            if "graphics_id" not in obj:
                warnings.append(f"Object {i}: missing graphics_id")
    
    return len(errors) == 0, errors, warnings

def main():
    converted_dir = "../../../firered-clone/data/maps/johto_converted"
    
    print("=" * 70)
    print("JOHTO ROUTES VALIDATION REPORT")
    print("=" * 70)
    print()
    
    # Get all JSON files (excluding the report)
    map_files = [f for f in os.listdir(converted_dir) 
                 if f.endswith('.json') and f != 'conversion_report.json']
    
    total_maps = len(map_files)
    valid_maps = 0
    invalid_maps = 0
    
    validation_results = []
    
    for map_file in sorted(map_files):
        map_path = os.path.join(converted_dir, map_file)
        map_name = os.path.splitext(map_file)[0]
        
        print(f"Validating {map_name}...", end=" ")
        
        is_valid, errors, warnings = validate_map(map_path)
        
        if is_valid:
            print("VALID")
            if warnings:
                print(f"  Warnings: {len(warnings)}")
                for warning in warnings:
                    print(f"    - {warning}")
            valid_maps += 1
        else:
            print("INVALID")
            print(f"  Errors: {len(errors)}")
            for error in errors:
                print(f"    - {error}")
            if warnings:
                print(f"  Warnings: {len(warnings)}")
                for warning in warnings:
                    print(f"    - {warning}")
            invalid_maps += 1
        
        validation_results.append({
            "map": map_name,
            "valid": is_valid,
            "errors": errors,
            "warnings": warnings
        })
    
    print()
    print("=" * 70)
    print("VALIDATION SUMMARY")
    print("=" * 70)
    print(f"Total maps validated: {total_maps}")
    print(f"Valid maps: {valid_maps}")
    print(f"Invalid maps: {invalid_maps}")
    print()
    
    # Write validation report
    report_path = os.path.join(converted_dir, "validation_report.json")
    with open(report_path, 'w') as f:
        json.dump({
            "total": total_maps,
            "valid": valid_maps,
            "invalid": invalid_maps,
            "results": validation_results
        }, f, indent=2)
    
    print(f"Detailed validation report written to: {report_path}")
    print()
    
    return 0 if invalid_maps == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
