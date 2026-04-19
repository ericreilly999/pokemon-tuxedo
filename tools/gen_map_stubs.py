import subprocess, os

base = '/c/dev/gitrepos/working-branch'

# Find undefined MAP_ constants in events.inc files
result = subprocess.run(
    'grep -h "MAP_" ' + base + '/data/maps/*/events.inc 2>/dev/null | '
    "grep -o 'MAP_[A-Z0-9_]*' | sort -u",
    shell=True, capture_output=True, text=True,
    cwd=base
)
used = set(result.stdout.splitlines())

result2 = subprocess.run(
    "grep '^#define MAP_' " + base + "/include/constants/map_groups.h | "
    "grep -o 'MAP_[A-Z0-9_]*' | sort -u",
    shell=True, capture_output=True, text=True,
    cwd=base
)
defined_maps = set(result2.stdout.splitlines())

# Also check existing stubs to avoid redefining
try:
    result3 = subprocess.run(
        "grep '^#define MAP_' " + base + "/include/constants/hoenn_map_stubs.h | "
        "grep -o 'MAP_[A-Z0-9_]*' | sort -u",
        shell=True, capture_output=True, text=True,
        cwd=base
    )
    defined_maps |= set(result3.stdout.splitlines())
except:
    pass

# Also add known built-in constants
defined_maps.add('MAP_DYNAMIC')
defined_maps.add('MAP_UNDEFINED')

missing = sorted(used - defined_maps)
print("Missing MAP_ constants: " + str(len(missing)))
for m in missing[:5]:
    print("  " + m)

lines = []
lines.append("#ifndef GUARD_CONSTANTS_HOENN_MAP_STUBS_H")
lines.append("#define GUARD_CONSTANTS_HOENN_MAP_STUBS_H")
lines.append("")
lines.append("/*")
lines.append(" * Hoenn indoor map stubs.")
lines.append(" * These maps are referenced by Hoenn outdoor maps but not yet implemented.")
lines.append(" * All point to MAP_UNDEFINED to allow the ROM to compile.")
lines.append(" * Replace with real map IDs when Hoenn indoor maps are implemented.")
lines.append(" */")
lines.append("")

for m in missing:
    pad = max(1, 55 - len(m))
    lines.append("#define " + m + (" " * pad) + "MAP_UNDEFINED")

lines.append("")
lines.append("#endif /* GUARD_CONSTANTS_HOENN_MAP_STUBS_H */")

output = "\n".join(lines)
with open(base + '/include/constants/hoenn_map_stubs.h', 'w') as f:
    f.write(output)
print("Written hoenn_map_stubs.h with " + str(len(missing)) + " stubs")
