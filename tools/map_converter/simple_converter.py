#!/usr/bin/env python3
"""
Simple map converter from Crystal Dust (Emerald-like) format to FireRed format.
This is a minimal converter that handles the basic map structure.
"""

import json
import sys
import os

def convert_map(input_path, output_path):
    """Convert a map from Crystal Dust format to FireRed format."""
    
    try:
        with open(input_path, 'r') as f:
            emerald_map = json.load(f)
    except Exception as e:
        print(f"Error reading input file: {e}", file=sys.stderr)
        return False
    
    # Create FireRed format map (most fields are compatible)
    firered_map = {
        "id": emerald_map.get("id", ""),
        "name": emerald_map.get("name", ""),
        "layout": emerald_map.get("layout", ""),
        "music": emerald_map.get("music", ""),
        "region_map_section": emerald_map.get("region_map_section", ""),
        "requires_flash": emerald_map.get("requires_flash", False),
        "weather": emerald_map.get("weather", "WEATHER_SUNNY"),
        "map_type": emerald_map.get("map_type", "MAP_TYPE_ROUTE"),
        "allow_cycling": emerald_map.get("allow_cycling", True),
        "allow_escaping": emerald_map.get("allow_escaping", False),
        "allow_running": emerald_map.get("allow_running", True),
        "show_map_name": emerald_map.get("show_map_name", True),
        "floor_number": emerald_map.get("floor_number", 0),
        "battle_scene": emerald_map.get("battle_scene", "MAP_BATTLE_SCENE_NORMAL"),
    }
    
    # Copy connections (format is compatible)
    if "connections" in emerald_map:
        firered_map["connections"] = emerald_map["connections"]
    else:
        firered_map["connections"] = []
    
    # Copy object events (format is compatible)
    if "object_events" in emerald_map:
        firered_map["object_events"] = emerald_map["object_events"]
    else:
        firered_map["object_events"] = []
    
    # Copy warp events (format is compatible)
    if "warp_events" in emerald_map:
        firered_map["warp_events"] = emerald_map["warp_events"]
    else:
        firered_map["warp_events"] = []
    
    # Copy coord events (format is compatible)
    if "coord_events" in emerald_map:
        firered_map["coord_events"] = emerald_map["coord_events"]
    else:
        firered_map["coord_events"] = []
    
    # Copy bg events (format is compatible)
    if "bg_events" in emerald_map:
        firered_map["bg_events"] = emerald_map["bg_events"]
    else:
        firered_map["bg_events"] = []
    
    # Write output
    try:
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        with open(output_path, 'w') as f:
            json.dump(firered_map, f, indent=2)
        return True
    except Exception as e:
        print(f"Error writing output file: {e}", file=sys.stderr)
        return False

def main():
    if len(sys.argv) != 3:
        print("Usage: python simple_converter.py <input.json> <output.json>")
        sys.exit(1)
    
    input_path = sys.argv[1]
    output_path = sys.argv[2]
    
    print(f"Converting {input_path} to {output_path}...")
    
    if convert_map(input_path, output_path):
        print("Conversion successful!")
        sys.exit(0)
    else:
        print("Conversion failed!")
        sys.exit(1)

if __name__ == "__main__":
    main()
