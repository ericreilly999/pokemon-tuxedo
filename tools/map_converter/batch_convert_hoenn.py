#!/usr/bin/env python3
"""
Batch converter for all Hoenn routes from Pokemon Crossroads to FireRed format.
Converts Routes 101-134 (34 routes total).
"""

import json
import os
import sys

# Add the simple_converter module
sys.path.insert(0, os.path.dirname(__file__))
from simple_converter import convert_map

# List of all Hoenn routes (101-134)
HOENN_ROUTES = [f"Route{i}" for i in range(101, 135)]

def main():
    # Paths relative to this script's location
    script_dir = os.path.dirname(os.path.abspath(__file__))
    base_input_dir = os.path.join(script_dir, "../../pokemon-crossroads/data/maps")
    base_output_dir = os.path.join(script_dir, "../../data/maps/hoenn_converted")
    
    # Create output directory
    os.makedirs(base_output_dir, exist_ok=True)
    
    success_count = 0
    fail_count = 0
    skip_count = 0
    conversion_report = []
    
    print("=" * 70)
    print("HOENN ROUTES CONVERSION REPORT")
    print("=" * 70)
    print(f"Input directory: {base_input_dir}")
    print(f"Output directory: {base_output_dir}")
    print()
    
    for route in HOENN_ROUTES:
        input_path = os.path.join(base_input_dir, route, "map.json")
        output_path = os.path.join(base_output_dir, f"{route}.json")
        
        print(f"Converting {route}...", end=" ")
        
        if not os.path.exists(input_path):
            print(f"SKIP (source not found at {input_path})")
            conversion_report.append({
                "route": route,
                "status": "SKIP",
                "reason": f"Source file not found: {input_path}"
            })
            skip_count += 1
            continue
        
        try:
            if convert_map(input_path, output_path):
                print("SUCCESS")
                conversion_report.append({
                    "route": route,
                    "status": "SUCCESS",
                    "input": input_path,
                    "output": output_path
                })
                success_count += 1
            else:
                print("FAILED")
                conversion_report.append({
                    "route": route,
                    "status": "FAILED",
                    "reason": "Conversion error"
                })
                fail_count += 1
        except Exception as e:
            print(f"ERROR: {e}")
            conversion_report.append({
                "route": route,
                "status": "ERROR",
                "reason": str(e)
            })
            fail_count += 1
    
    print()
    print("=" * 70)
    print("SUMMARY")
    print("=" * 70)
    print(f"Total routes: {len(HOENN_ROUTES)}")
    print(f"Successfully converted: {success_count}")
    print(f"Failed: {fail_count}")
    print(f"Skipped: {skip_count}")
    print()
    
    # Write detailed report
    report_path = os.path.join(base_output_dir, "conversion_report.json")
    with open(report_path, 'w') as f:
        json.dump({
            "region": "Hoenn",
            "type": "routes",
            "total": len(HOENN_ROUTES),
            "success": success_count,
            "failed": fail_count,
            "skipped": skip_count,
            "details": conversion_report
        }, f, indent=2)
    
    print(f"Detailed report written to: {report_path}")
    print()
    
    # Return success if all routes converted
    if success_count == len(HOENN_ROUTES):
        print("✅ All Hoenn routes converted successfully!")
        return 0
    elif success_count > 0:
        print(f"⚠️ Partial success: {success_count}/{len(HOENN_ROUTES)} routes converted")
        return 1
    else:
        print("❌ No routes were converted")
        return 2

if __name__ == "__main__":
    sys.exit(main())
