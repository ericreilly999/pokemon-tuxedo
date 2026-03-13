#!/usr/bin/env python3
"""
Batch converter for all Johto routes from Crystal Dust to FireRed format.
"""

import json
import os
import sys

# Add the simple_converter module
sys.path.insert(0, os.path.dirname(__file__))
from simple_converter import convert_map

# List of Johto routes available in Crystal Dust
JOHTO_ROUTES = [
    "Route29",
    "Route30",
    "Route31",
    "Route32",
    "Route33",
    "Route34",
    "Route35",
    "Route36",
    "Route45",
    "Route46",
]

def main():
    base_input_dir = "../../../pokemon-crystal-dust/data/maps"
    base_output_dir = "../../../firered-clone/data/maps/johto_converted"
    
    # Create output directory
    os.makedirs(base_output_dir, exist_ok=True)
    
    success_count = 0
    fail_count = 0
    conversion_report = []
    
    print("=" * 70)
    print("JOHTO ROUTES CONVERSION REPORT")
    print("=" * 70)
    print()
    
    for route in JOHTO_ROUTES:
        input_path = os.path.join(base_input_dir, route, "map.json")
        output_path = os.path.join(base_output_dir, f"{route}.json")
        
        print(f"Converting {route}...", end=" ")
        
        if not os.path.exists(input_path):
            print(f"SKIP (source not found)")
            conversion_report.append({
                "route": route,
                "status": "SKIP",
                "reason": "Source file not found"
            })
            fail_count += 1
            continue
        
        try:
            if convert_map(input_path, output_path):
                print("SUCCESS")
                conversion_report.append({
                    "route": route,
                    "status": "SUCCESS",
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
    print(f"Total routes: {len(JOHTO_ROUTES)}")
    print(f"Successfully converted: {success_count}")
    print(f"Failed/Skipped: {fail_count}")
    print()
    
    # Write detailed report
    report_path = os.path.join(base_output_dir, "conversion_report.json")
    with open(report_path, 'w') as f:
        json.dump({
            "total": len(JOHTO_ROUTES),
            "success": success_count,
            "failed": fail_count,
            "details": conversion_report
        }, f, indent=2)
    
    print(f"Detailed report written to: {report_path}")
    print()
    
    return 0 if fail_count == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
