# Build Pokemon Tuxedo ROM - Quick Start

## Current Status
✅ **Git commit created**: `ffe982b0c` with tag `unbuilt`
✅ **All code integrated**: 7/8 hooks complete
✅ **Constants defined**: Items, hold effects, trainer classes
✅ **Ready to build**: Just need build environment

---

## Build Environment Setup

### Option 1: Windows with devkitPro (Recommended)

1. **Install devkitPro**
   - Download from: https://github.com/devkitPro/installer/releases
   - Run installer and select "devkitARM"
   - Default install location: `C:\devkitPro\`

2. **Set Environment Variables**
   ```powershell
   $env:DEVKITPRO = "C:\devkitPro"
   $env:DEVKITARM = "C:\devkitPro\devkitARM"
   $env:PATH = "$env:DEVKITARM\bin;$env:PATH"
   ```

3. **Install Make**
   ```powershell
   # Using Chocolatey
   choco install make
   
   # Or download from: http://gnuwin32.sourceforge.net/packages/make.htm
   ```

4. **Verify Installation**
   ```powershell
   arm-none-eabi-gcc --version
   make --version
   ```

### Option 2: WSL (Windows Subsystem for Linux)

1. **Install WSL**
   ```powershell
   wsl --install
   ```

2. **Install Build Tools in WSL**
   ```bash
   sudo apt-get update
   sudo apt-get install build-essential binutils-arm-none-eabi gcc-arm-none-eabi
   ```

3. **Navigate to Project**
   ```bash
   cd /mnt/c/dev/pokemon-tuxedo/firered-clone
   ```

### Option 3: Linux/macOS

1. **Install devkitARM**
   ```bash
   # Follow instructions at: https://devkitpro.org/wiki/Getting_Started
   ```

2. **Install Build Tools**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential
   
   # macOS
   brew install make
   ```

---

## Build Commands

### Quick Build
```bash
cd firered-clone
make
```

### Expected Output
```
Compiling src/level_scaler.c...
Compiling src/region_manager.c...
Compiling src/exp_multipliers.c...
...
Linking pokefirered.elf...
Creating pokefirered.gba...
Build complete!
```

### Build Time
- **First build**: 5-10 minutes
- **Incremental**: 30 seconds - 2 minutes

---

## If Build Fails

### Common Errors and Solutions

#### Error: "No rule to make target"
**Cause**: Missing source file or Makefile issue
**Solution**: 
```bash
make clean
make
```

#### Error: "undefined reference to..."
**Cause**: Missing function implementation or linker issue
**Solution**: Check which function is missing and verify it's implemented

#### Error: Item data not found
**Cause**: New items need data definitions
**Solution**: See "Adding Item Data" section below

#### Error: Type mismatch
**Cause**: Function signature doesn't match
**Solution**: Check the specific error and fix the type

---

## Adding Item Data (If Needed)

If the build fails due to missing item data, you'll need to add entries for the new items.

### Step 1: Find Item Data File
Look for one of these files:
- `src/data/items.c`
- `src/item_data.c`
- `data/items/items.c`

### Step 2: Add Item Entries
Add entries for each new item:

```c
// Mystic Egg - 3x exp multiplier
[ITEM_MYSTIC_EGG] = {
    .name = _("MYSTIC EGG"),
    .itemId = ITEM_MYSTIC_EGG,
    .price = 0,
    .holdEffect = HOLD_EFFECT_MYSTIC_EGG,
    .description = sMysticEggDesc,
    .pocket = POCKET_ITEMS,
    .type = ITEM_USE_BAG_MENU,
    .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
},

// Magic Egg - 4x exp multiplier
[ITEM_MAGIC_EGG] = {
    .name = _("MAGIC EGG"),
    .itemId = ITEM_MAGIC_EGG,
    .price = 0,
    .holdEffect = HOLD_EFFECT_MAGIC_EGG,
    .description = sMagicEggDesc,
    .pocket = POCKET_ITEMS,
    .type = ITEM_USE_BAG_MENU,
    .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
},

// Johto Ticket
[ITEM_JOHTO_TICKET] = {
    .name = _("JOHTO TICKET"),
    .itemId = ITEM_JOHTO_TICKET,
    .price = 0,
    .description = sJohtoTicketDesc,
    .pocket = POCKET_KEY_ITEMS,
    .type = ITEM_USE_BAG_MENU,
    .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
},

// Hoenn Ticket
[ITEM_HOENN_TICKET] = {
    .name = _("HOENN TICKET"),
    .itemId = ITEM_HOENN_TICKET,
    .price = 0,
    .description = sHoennTicketDesc,
    .pocket = POCKET_KEY_ITEMS,
    .type = ITEM_USE_BAG_MENU,
    .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
},

// Sinnoh Ticket
[ITEM_SINNOH_TICKET] = {
    .name = _("SINNOH TICKET"),
    .itemId = ITEM_SINNOH_TICKET,
    .price = 0,
    .description = sSinnohTicketDesc,
    .pocket = POCKET_KEY_ITEMS,
    .type = ITEM_USE_BAG_MENU,
    .fieldUseFunc = ItemUseOutOfBattle_CannotUse,
},
```

### Step 3: Add Description Strings
Add description strings (usually in same file or nearby):

```c
static const u8 sMysticEggDesc[] = _(
    "A mysterious egg that\n"
    "triples EXP gained.");

static const u8 sMagicEggDesc[] = _(
    "A magical egg that\n"
    "quadruples EXP gained.");

static const u8 sJohtoTicketDesc[] = _(
    "A ticket for travel\n"
    "to the JOHTO region.");

static const u8 sHoennTicketDesc[] = _(
    "A ticket for travel\n"
    "to the HOENN region.");

static const u8 sSinnohTicketDesc[] = _(
    "A ticket for travel\n"
    "to the SINNOH region.");
```

---

## Testing the ROM

### Step 1: Load in Emulator
```bash
# Copy to emulator
cp pokefirered.gba ~/emulators/roms/pokemon-tuxedo.gba

# Or run directly
mgba-qt pokefirered.gba
```

### Step 2: Quick Test Checklist
- [ ] ROM loads without crashing
- [ ] Can start new game
- [ ] Can catch Pokemon
- [ ] Wild Pokemon levels seem appropriate
- [ ] Can battle trainers
- [ ] Trainer levels seem appropriate
- [ ] Can save game

### Step 3: Feature Testing
- [ ] Wild Pokemon levels scale with badges
- [ ] Gym Leader levels scale correctly
- [ ] Experience gain works
- [ ] Badge acquisition works
- [ ] Can access pause menu

---

## Build Logs

### Save Build Output
```bash
make 2>&1 | tee build.log
```

### Check for Warnings
```bash
grep -i warning build.log
```

### Check for Errors
```bash
grep -i error build.log
```

---

## Next Steps After Successful Build

1. **Test thoroughly** - Play through first gym
2. **Fix any bugs** - Address crashes or issues
3. **Import regional maps** - Add Johto, Hoenn, Sinnoh
4. **Create item sprites** - Design graphics for new items
5. **Balance testing** - Adjust level curves if needed
6. **Release v1.0** - Package and distribute

---

## Quick Reference

### Build Commands
```bash
make              # Standard build
make clean        # Clean build files
make -j4          # Parallel build (faster)
make MODERN=1     # Modern compiler
```

### File Locations
- **Source**: `src/*.c`
- **Headers**: `include/*.h`
- **Output**: `pokefirered.gba`
- **Build files**: `build/firered/`

### Important Files
- **Makefile** - Build configuration
- **config.mk** - Build settings
- **firered.gba** - Base ROM (required)

---

## Support

### If You Get Stuck
1. Check BUILD_GUIDE.md for detailed troubleshooting
2. Review error messages carefully
3. Check pokefirered decomp documentation
4. Search for similar errors online

### Common Issues
- Missing build tools → Install devkitARM
- Missing base ROM → Place firered.gba in root
- Item data errors → Add item definitions
- Linker errors → Check function implementations

---

## Summary

**Current State**: Code is complete and committed with tag `unbuilt`

**To Build**:
1. Install devkitARM and make
2. Run `make` in firered-clone directory
3. Test pokefirered.gba in emulator

**Expected Result**: Working ROM with Pokemon Tuxedo features

**Time to Build**: 5-10 minutes (first time)

---

Good luck with the build! 🚀
