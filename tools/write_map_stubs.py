base = '/c/dev/gitrepos/working-branch'

with open(base + '/tools/missing_maps.txt') as f:
    missing = sorted(f.read().splitlines())

print("Writing stubs for: " + str(len(missing)) + " maps")

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

output = "\n".join(lines) + "\n"
with open(base + '/include/constants/hoenn_map_stubs.h', 'w') as f:
    f.write(output)
print("Done. Written " + str(len(missing)) + " stub definitions.")
