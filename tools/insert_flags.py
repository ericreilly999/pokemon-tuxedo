import subprocess, sys

flags_h = '/c/dev/gitrepos/working-branch/include/constants/flags.h'
gen_script = '/c/dev/gitrepos/working-branch/tools/gen_hoenn_flags.py'

# Run the generator to get the new flag definitions
result = subprocess.run(
    ['python3', gen_script],
    capture_output=True, text=True
)
# Skip the first 3 status lines
output_lines = result.stdout.splitlines()
status_lines = [l for l in output_lines[:5] if not l.startswith('#') and not l.startswith('/*')]
content_start = 0
for i, l in enumerate(output_lines):
    if l.startswith('/*') or l.startswith('#define') or l == '':
        if i > 0 and output_lines[i-1].strip() == 'Total lines: ' + str(len(output_lines) - i):
            content_start = i
            break
        if l.startswith('/*') and i > 2:
            content_start = i
            break

# Find where the actual flag definitions start (after "Total lines:" line)
for i, l in enumerate(output_lines):
    if l.startswith('/*') or (l.startswith('#define') and 'FLAG_' in l):
        content_start = i
        break

new_content = '\n'.join(output_lines[content_start:])

# Read existing flags.h
with open(flags_h) as f:
    existing = f.read()

# Insert before #endif
if '#endif // GUARD_CONSTANTS_FLAGS_H' in existing:
    existing = existing.replace(
        '#endif // GUARD_CONSTANTS_FLAGS_H',
        new_content + '\n\n#endif /* GUARD_CONSTANTS_FLAGS_H */'
    )
    with open(flags_h, 'w') as f:
        f.write(existing)
    print("Inserted " + str(len(output_lines) - content_start) + " lines into flags.h")
else:
    print("ERROR: Could not find #endif in flags.h")
    sys.exit(1)
