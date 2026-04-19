import subprocess, sys

# Find all flags used in events.inc files
result = subprocess.run(
    'grep -h "FLAG_" /c/dev/gitrepos/working-branch/data/maps/*/events.inc 2>/dev/null | '
    "grep -o 'FLAG_[A-Z0-9_]*' | sort -u",
    shell=True, capture_output=True, text=True
)
used = set(result.stdout.splitlines())

# Find all flags defined in flags.h
result2 = subprocess.run(
    "grep '^#define FLAG_' /c/dev/gitrepos/working-branch/include/constants/flags.h | "
    "grep -o 'FLAG_[A-Z0-9_]*' | sort -u",
    shell=True, capture_output=True, text=True
)
defined = set(result2.stdout.splitlines())

all_undefined = sorted(used - defined)
hidden_item_flags = sorted([f for f in all_undefined if f.startswith('FLAG_HIDDEN_ITEM_')])
other_flags = sorted([f for f in all_undefined if not f.startswith('FLAG_HIDDEN_ITEM_')])

print("Hidden item flags: " + str(len(hidden_item_flags)))
print("Other flags (HIDE/ITEM/TEMP): " + str(len(other_flags)))

lines = []
lines.append("")
lines.append("/* ===================================================================")
lines.append(" * Hoenn region flag stubs")
lines.append(" * These flags are stubs to allow the ROM to compile.")
lines.append(" * Full flag allocation requires save block expansion (future work).")
lines.append(" * ================================================================= */")
lines.append("")
lines.append("/* Hoenn hidden item flags (byte offset from FLAG_HIDDEN_ITEMS_START) */")
lines.append("/* Indices 191-255; overflow flags (>255) aliased to 255            */")

for i, flag in enumerate(hidden_item_flags):
    idx = min(191 + i, 255)
    pad = max(1, 60 - len(flag))
    lines.append("#define " + flag + (" " * pad) + " (FLAG_HIDDEN_ITEMS_START + " + str(idx) + ")")

lines.append("")
lines.append("/* Hoenn NPC/item pickup flags (stub: cycle through 0x4DC-0x4FF) */")
stub_base = 0x4DC
stub_count = 0x4FF - 0x4DC + 1

for i, flag in enumerate(other_flags):
    stub_val = stub_base + (i % stub_count)
    pad = max(1, 60 - len(flag))
    lines.append("#define " + flag + (" " * pad) + " 0x" + format(stub_val, '03X'))

output = "\n".join(lines) + "\n"
print("Total lines: " + str(len(lines)))
sys.stdout.write(output)
