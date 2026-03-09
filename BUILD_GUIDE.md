# Pokemon Tuxedo Build Guide

## Overview
This guide covers building the Pokemon Tuxedo ROM from the integrated pokefirered decomp codebase.

---

## Prerequisites

### Required Tools
1. **devkitARM** - ARM cross-compiler toolchain
   - Already confirmed installed by user
   - Includes: `arm-none-eabi-gcc`, `arm-none-eabi-as`, `arm-none-eabi-ld`

2. **Base ROM** - Pokemon FireRed (USA) v1.0
   - Already present at root as `firered.gba`
   - MD5: `e26ee0d44e809351c8ce2d73c7400cdd`

3. **Build Tools**
   - `make` - GNU Make
   - `bash` - Shell for build scripts
   - `python3` - For build scripts (optional)

### Installation Check
```bash
# Check devkitARM
arm-none-eabi-gcc --version

# Check make
make --version

# Check base ROM
md5sum firered.gba
```

---

## Build Instructions

### Step 1: Navigate to Project Directory
```bash
cd firered-clone
```

### Step 2: Clean Previous Builds (Optional)
```bash
make clean
```

### Step 3: Build ROM
```bash
make
```

### Expected Output
```
Compiling src/level_scaler.c...
Compiling src/region_manager.c...
Compiling src/exp_multipliers.c...
Compiling src/full_party_exp.c...
...
Linking pokefirered.elf...
Creating pokefirered.gba...
Build complete!
```

### Step 4: Verify Build
```bash
ls -lh pokefirered.gba
```

The output ROM should be approximately 16MB.

---

## Build Configuration

### Build Variants
The Makefile supports different build configurations:

```bash
# Standard build (MODERN=0, uses agbcc)
make

# Modern build (MODERN=1, uses modern gcc)
make MODERN=1

# Debug build with symbols
make DINFO=1

# Optimized build
make O_LEVEL=3
```

### Recommended Build
```bash
make MODERN=0
```

This uses the original agbcc compiler for maximum compatibility.

---

## Troubleshooting

### Error: "make: command not found"
**Solution**: Install GNU Make
```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS
brew install make

# Windows (use WSL or install via chocolatey)
choco install make
```

### Error: "arm-none-eabi-gcc: command not found"
**Solution**: Ensure devkitARM is in PATH
```bash
export DEVKITARM=/path/to/devkitARM
export PATH=$DEVKITARM/bin:$PATH
```

### Error: "firered.gba: No such file or directory"
**Solution**: Place base ROM in root directory
```bash
cp /path/to/firered.gba .
```

### Error: Compilation errors in Pokemon Tuxedo files
**Possible Issues**:
1. Missing includes
2. Type mismatches
3. Undefined constants

**Solution**: Check the specific error message and:
- Verify all includes are present
- Check constant definitions in `include/constants/`
- Review function signatures

### Error: Linker errors
**Possible Issues**:
1. Undefined references
2. Multiple definitions
3. Missing object files

**Solution**:
- Ensure all `.c` files are in `src/` directory
- Check for duplicate function definitions
- Verify Makefile includes all source files

---

## Post-Build Steps

### Step 1: Test ROM
```bash
# Copy to emulator directory
cp pokefirered.gba ~/emulators/roms/

# Or test with command-line emulator
mgba-qt pokefirered.gba
```

### Step 2: Verify Features
Test the following Pokemon Tuxedo features:
- [ ] Wild Pokemon level scaling
- [ ] Trainer Pokemon level scaling
- [ ] Experience multipliers
- [ ] Badge acquisition
- [ ] Elite Four defeat
- [ ] Fly location discovery

### Step 3: Create Backup
```bash
cp pokefirered.gba pokemon-tuxedo-v1.0.gba
```

---

## Known Build Issues

### Issue 1: Item Data Missing
**Symptom**: Undefined references to item data for new items

**Solution**: Item data needs to be added to data files. The new items are:
- ITEM_MYSTIC_EGG (375)
- ITEM_MAGIC_EGG (376)
- ITEM_JOHTO_TICKET (377)
- ITEM_HOENN_TICKET (378)
- ITEM_SINNOH_TICKET (379)

Item data files are typically in `src/data/` or similar. Look for files like:
- `items.c`
- `item_data.c`
- `data/items/`

Each item needs:
- Name string
- Description string
- Price
- Hold effect
- Pocket (where it's stored)
- Type
- Field use function
- Battle use function
- Sprite/icon

**Template for new items**:
```c
[ITEM_MYSTIC_EGG] = {
    .name = _("MYSTIC EGG"),
    .itemId = ITEM_MYSTIC_EGG,
    .price = 0,  // Cannot be bought
    .holdEffect = HOLD_EFFECT_MYSTIC_EGG,
    .description = sDummyDesc,  // Or create proper description
    .pocket = POCKET_ITEMS,
    .type = ITEM_USE_BAG_MENU,
    .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
    .battleUsage = ITEM_B_USE_OTHER,
    .battleUseFunc = ItemUseInBattle_StatIncrease,
},
```

### Issue 2: Hold Effect Implementation Missing
**Symptom**: Hold effects don't work in battle

**Solution**: Hold effect handlers need to be implemented. Look for files like:
- `src/battle_script_commands.c`
- `src/item.c`
- `src/battle_util.c`

The hold effects are already integrated in `battle_script_commands.c` for exp calculation.

### Issue 3: Trainer Class Constants
**Symptom**: Undefined TRAINER_CLASS_* constants

**Solution**: Already fixed. Using:
- `TRAINER_CLASS_LEADER` (84)
- `TRAINER_CLASS_ELITE_FOUR` (87)
- `TRAINER_CLASS_CHAMPION` (90)
- `TRAINER_CLASS_RIVAL_EARLY` (81)
- `TRAINER_CLASS_RIVAL_LATE` (89)

---

## Build Optimization

### Faster Builds
```bash
# Use parallel compilation
make -j4

# Skip dependency generation
make NODEP=1
```

### Smaller ROM
```bash
# Use higher optimization level
make O_LEVEL=3

# Strip debug symbols
arm-none-eabi-strip pokefirered.elf
```

---

## Advanced Build Options

### Custom Build Name
```bash
make BUILD_NAME=tuxedo
# Creates: poketuxedo.gba
```

### Debug Build
```bash
make DINFO=1
# Creates ROM with debug symbols
```

### Verbose Build
```bash
make V=1
# Shows all compilation commands
```

---

## Integration Verification

### Check Pokemon Tuxedo Files Are Compiled
```bash
# List object files
ls build/firered/src/*.o | grep -E "(level_scaler|region_manager|exp_multipliers|full_party_exp)"
```

Expected output:
```
build/firered/src/level_scaler.o
build/firered/src/region_manager.o
build/firered/src/exp_multipliers.o
build/firered/src/full_party_exp.o
build/firered/src/storage_system.o
build/firered/src/game_settings.o
build/firered/src/fly_location_discovery.o
build/firered/src/map_accessibility.o
build/firered/src/system_integration.o
build/firered/src/error_handling.o
```

### Check Hooks Are Integrated
```bash
# Search for Pokemon Tuxedo comments in modified files
grep -r "Pokemon Tuxedo" src/*.c
```

Expected output:
```
src/wild_encounter.c:    // Pokemon Tuxedo: Use dynamic level scaling
src/battle_main.c:        // Pokemon Tuxedo: Get scaled levels for this trainer
src/battle_script_commands.c:                    // Pokemon Tuxedo: Apply exp multipliers
src/scrcmd.c:    // Pokemon Tuxedo: Check if this is a badge flag
src/post_battle_event_funcs.c:        // Pokemon Tuxedo: Elite Four defeated
src/overworld.c:        // Pokemon Tuxedo: Map entry hook
```

---

## Build Time Estimates

### First Build
- **Clean build**: 5-10 minutes
- **With ccache**: 2-5 minutes

### Incremental Builds
- **Single file change**: 10-30 seconds
- **Multiple files**: 1-2 minutes

### Full Rebuild
```bash
make clean && make
```
- **Time**: 5-10 minutes

---

## Next Steps After Successful Build

1. **Test ROM in Emulator**
   - Use mGBA, VBA, or similar
   - Test all Pokemon Tuxedo features
   - Check for crashes or bugs

2. **Create Release Package**
   ```bash
   mkdir pokemon-tuxedo-v1.0
   cp pokefirered.gba pokemon-tuxedo-v1.0/
   cp README.md pokemon-tuxedo-v1.0/
   cp HOOKS_COMPLETION_STATUS.md pokemon-tuxedo-v1.0/
   zip -r pokemon-tuxedo-v1.0.zip pokemon-tuxedo-v1.0/
   ```

3. **Import Regional Maps**
   - See REGIONAL_CONTENT_INTEGRATION_GUIDE.md
   - Start with Johto maps
   - Add Hoenn and Sinnoh maps

4. **Create Item Sprites**
   - Design sprites for new items
   - Add to graphics data
   - Test in-game display

5. **Balance Testing**
   - Full playthrough
   - Adjust level curves if needed
   - Fine-tune exp multipliers

---

## Support

### Build Issues
If you encounter build issues:
1. Check this guide's troubleshooting section
2. Review error messages carefully
3. Check pokefirered decomp documentation
4. Verify all prerequisites are installed

### Pokemon Tuxedo Issues
For Pokemon Tuxedo-specific issues:
1. Check HOOKS_COMPLETION_STATUS.md
2. Review integration code in modified files
3. Verify all constants are defined
4. Check function signatures match

---

## Summary

### What's Ready
- ✅ All Pokemon Tuxedo source files in `src/`
- ✅ All Pokemon Tuxedo headers in `include/`
- ✅ All system hooks integrated
- ✅ Constants defined (items, hold effects, trainer classes)
- ✅ Makefile configured (auto-detects `.c` files)

### What May Need Attention
- ⚠️ Item data definitions (name, description, sprite)
- ⚠️ Hold effect battle implementations (if not already present)
- ⚠️ Regional map data (Johto, Hoenn, Sinnoh)
- ⚠️ Item sprites/graphics

### Build Readiness
**Estimated**: 80-90% ready to build

The ROM should compile with minimal issues. Any errors will likely be related to:
1. Item data definitions
2. Missing graphics data
3. Minor type mismatches

These can be fixed incrementally as they appear during compilation.

---

Last Updated: Current Session
Status: Ready for first build attempt
Next Action: Run `make` and address any compilation errors
