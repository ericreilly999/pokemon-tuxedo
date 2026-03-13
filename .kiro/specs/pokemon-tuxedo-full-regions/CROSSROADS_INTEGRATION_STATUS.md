# Pokemon Crossroads Source Integration Status

## Task 2.1: Set up Crossroads source integration

**Date:** 2025
**Status:** Repository cloned, Johto maps NOT YET AVAILABLE

## Repository Information

- **Repository URL:** https://github.com/eonlynx/pokecrossroads
- **Clone Location:** `pokemon-crossroads/` (in workspace root)
- **Current Version:** Beta 1.0 (Released March 3, 2026)
- **Base:** Pokemon Emerald decomp with pokeemerald-expansion

## Current Content Status

### Available Regions (Beta 1.0)
1. **Hoenn** - Complete (Routes 101-134, all cities, caves, dungeons)
2. **Kanto** - Complete (Routes 1-25, all cities, caves, dungeons)  
3. **Sevii Islands** - Complete (Islands 1-7)

### Johto Region Status
**⚠️ CRITICAL FINDING: Johto maps are NOT yet available in Pokemon Crossroads Beta 1.0**

According to the official README:
- Beta 1.0 includes: "Three fully explorable regions: Hoenn, Kanto, and the Sevii Islands"
- Johto is listed as "in development" for Beta 2.0
- The repository description states: "joins Kanto, Johto and Hoenn" but Johto is not yet implemented

### Evidence from Repository Structure

Searched for Johto-specific maps in `pokemon-crossroads/data/maps/`:
- ❌ No NewBarkTown maps found
- ❌ No Cherrygrove City maps found
- ❌ No Violet City maps found
- ❌ No Azalea Town maps found
- ❌ No Goldenrod City maps found
- ❌ No Ecruteak City maps found
- ❌ No Olivine City maps found
- ❌ No Cianwood City maps found
- ❌ No Mahogany Town maps found
- ❌ No Blackthorn City maps found
- ❌ No Johto routes (29-46) found

Only references to "Johto" in the codebase are:
- `VAR_DEX_UPGRADE_JOHTO_STARTER_STATE` - Variable for Johto starter selection
- References to Johto starters (Chikorita, Cyndaquil, Totodile) as post-game rewards
- Text mentioning "GOLDENROD CITY" as Bill's hometown (flavor text only)

## Directory Structure

### Maps Location
```
pokemon-crossroads/
├── data/
│   ├── maps/           # Individual map directories
│   │   ├── Route101/   # Hoenn routes
│   │   ├── Route1_Frlg/  # Kanto routes  
│   │   ├── PalletTown_Frlg/  # Kanto cities
│   │   ├── CeladonCity_Frlg/  # Kanto cities
│   │   └── [NO JOHTO MAPS]
│   ├── layouts/        # Map layout data
│   ├── scripts/        # Event scripts
│   └── tilesets/       # Tileset graphics and data
├── graphics/           # Graphics assets
├── include/            # Header files
└── src/                # Source code
```

### Map Naming Convention
- Hoenn maps: `Route101/`, `LittlerootTown/`, `PetalburgCity/`
- Kanto maps: `Route1_Frlg/`, `PalletTown_Frlg/`, `CeladonCity_Frlg/`
- Expected Johto pattern: Would likely be `Route29_Johto/`, `NewBarkTown_Johto/`, etc.

## Implications for Task 2.1

### Requirements Status
- ✅ **Requirement 17.1:** Clone Crossroads repository - **COMPLETED**
- ❌ **Requirement 17.2:** Locate Johto map data - **BLOCKED** (Johto not yet in repository)

### Alternative Approaches

Since Johto maps are not available in Pokemon Crossroads, we have several options:

#### Option 1: Wait for Beta 2.0
- **Pros:** Will have official Johto maps from Crossroads team
- **Cons:** Unknown release date, blocks entire Phase 2

#### Option 2: Use Pokemon Crystal Dust
- **Repository:** https://github.com/Sierraffinity/CrystalDust
- **Description:** "A ROM hack of Pokémon Emerald that aims to recreate Pokémon Crystal on the GBA"
- **Pros:** Complete Johto region in Emerald format
- **Cons:** Different source than originally planned

#### Option 3: Port from Pokemon HeartGold/SoulSilver
- **Pros:** Official Johto maps
- **Cons:** Different engine (DS vs GBA), significant conversion work

#### Option 4: Create Johto maps manually
- **Pros:** Full control over implementation
- **Cons:** Extremely time-consuming, requires extensive mapping work

## Recommended Next Steps

1. **Immediate:** Document this finding and consult with user on preferred approach
2. **Short-term:** Investigate Pokemon Crystal Dust as alternative Johto source
3. **Medium-term:** Monitor Pokemon Crossroads for Beta 2.0 release
4. **Long-term:** Consider hybrid approach (use available regions from Crossroads, supplement Johto from alternative source)

## Available Data for Other Regions

While Johto is unavailable, we can proceed with:
- **Hoenn Region:** Fully available in Crossroads (Phase 3 of implementation plan)
- **Kanto Region:** Already in FireRed base, Crossroads has enhanced version
- **Sinnoh Region:** Not in Crossroads either (would need alternative source)

## Repository Clone Details

- **Clone Command:** `git clone https://github.com/eonlynx/pokecrossroads.git pokemon-crossroads`
- **Clone Size:** 436.31 MiB
- **Total Objects:** 308,090
- **Total Files:** 29,822
- **Clone Date:** 2025
- **Commit:** Latest from main branch

## Contact Information

- **Project:** Pokemon Crossroads
- **Developer:** eonlynx and Crossroads Dev Team
- **Discussion:** https://www.pokecommunity.com/threads/pokémon-crossroads-kanto-johto-and-hoenn-joined.536507/
- **Issues:** https://github.com/eonlynx/pokecrossroads/issues
