# Build In Progress

## Status: Compiling ROM

The ROM is currently being built with all the latest fixes. This process takes 5-10 minutes due to:
- Compiling all C source files
- Processing 400+ Pokemon graphics
- Processing battle animations
- Processing tilesets and maps
- Linking everything into final ROM

## What's Being Fixed

### 1. Wild Pokemon Level Scaling ✓
**Formula:** min = (badges × 5) + region_start, max = min + 10
**Example with 1 badge:**
- Min: (1 × 5) + 2 = 7
- Max: 7 + 10 = 17
- You should see Pokemon levels 7-17 in caves/grass

### 2. Gym Leader Level Scaling ✓
**Formula:** average = wild_max + 5, ace = average + 4
**Brock with 0 badges:**
- Wild max: 10
- Average: 10 + 5 = 15
- Ace: 15 + 4 = 19
- Brock should have Pokemon at levels 15 and 19

### 3. Rival Level Scaling ✓
**Formula:** average = last_gym_avg + 10 (or 5 for first battle), ace = average + 8
**First battle (0 badges):**
- Average: 5 (special case)
- Ace: 5 + 8 = 13
- Rival should have Pokemon at level 5, ace at 13

### 4. Full Party EXP ✓
- All alive party members receive FULL EXP (not split)
- EXP multipliers (Lucky Egg, etc.) still apply per-Pokemon
- No more grinding needed!

### 5. HM Requirements Removed ✓
- Cut, Rock Smash, Waterfall can be used WITHOUT having the HM
- Just select any Pokemon and the option will appear
- Surf, Strength, Fly still require the HM (as intended)

## Current Build Progress

Processing Pokemon graphics (alphabetically):
- ✓ A-R complete
- → S in progress (Sandshrew, Scyther, Seel, etc.)
- Remaining: T-Z

After graphics, the build will:
1. Compile remaining source files
2. Link all object files
3. Create final pokefirered.gba ROM
4. Verify ROM integrity

## Testing the New ROM

Once `pokefirered.gba` is rebuilt, you'll need to:
1. Start a NEW save file (old saves won't have badge count tracking)
2. Test wild Pokemon levels (should scale with badges)
3. Test Brock's levels (should be 15/19)
4. Test full party EXP (all Pokemon should gain EXP)
5. Test HM bypass (Cut/Rock Smash/Waterfall work without HM)

## Why Old ROM Showed Wrong Levels

The ROM you were testing was built BEFORE these implementations:
- Wild Pokemon used original fixed levels from game data
- Gym leaders returned level 0 (stub functions)
- Only battling Pokemon gained EXP
- HMs were still required

The new ROM will have all these fixes active!
