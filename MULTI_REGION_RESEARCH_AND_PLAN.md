# Multi-Region Implementation Research & Action Plan

## Executive Summary

After extensive research into multi-region ROM hacks, I've identified viable approaches for adding Johto, Hoenn, and Sinnoh to Pokemon Tuxedo. This document outlines the findings and provides a concrete implementation plan.

## Research Findings

### Successful Multi-Region Projects

#### 1. Pokemon Crossroads (Emerald-based) ⭐ MOST RELEVANT
**Repository**: https://github.com/eonlynx/pokecrossroads
**Status**: Active development, Beta 1.0 released (March 2026)
**Regions**: Hoenn + Kanto + Johto (in progress)
**Base**: pokeemerald-expansion

**Key Achievements:**
- ✅ Seamless region switching with proper flag handling
- ✅ All major Kanto overworlds ported from FireRed
- ✅ Multi-region minimaps (each region shows its own map)
- ✅ Updated Fly system respecting current region
- ✅ 16 gym badges (8 Hoenn + 8 Kanto)

**Technical Approach:**
- Built on pokeemerald-expansion engine
- Ported FireRed maps/logic into Emerald
- Custom region switching system
- Separate map banks per region
- Region-aware event system

**Relevance to Pokemon Tuxedo:**
- Same generation (GBA Gen 3)
- Proven multi-region architecture
- Open source - can study implementation
- Uses decomp (similar to our pokefirered approach)

#### 2. Pokemon FireRed 2
**Status**: Complete
**Regions**: Kanto + Johto combined
**Base**: FireRed binary hack (not decomp)

**Approach:**
- Combined regions into single map
- 8 gyms total (not 16)
- Binary patching, not source-based

**Relevance**: Shows Johto can be added to FireRed, but limited by binary approach

#### 3. Pokemon Team Rocket Edition
**Status**: In development
**Regions**: Kanto + Sevii + Johto
**Base**: Emerald

**Approach:**
- Timeline-based progression
- Multiple regions accessible
- Story-driven region transitions

### Available Resources

#### Map Data Sources

**1. PokeCommunity Map Packs**
- "Remastered Kanto and Johto Overworld and Caves Map Pack"
- Pre-made maps for Johto region
- Compatible with AdvanceMap (binary hacking)
- Would need conversion for decomp use

**2. pret Decomp Projects**
- **pokeheartgold**: Complete Johto + Kanto (DS format)
- **pokeemerald**: Complete Hoenn (GBA format)
- **pokediamond**: Sinnoh (DS format)

**Challenge**: Different data formats between projects
- DS games (HGSS, DPPt) use different map structure than GBA
- Direct porting not possible without conversion

**3. Johto Addon Base (FireRed)**
- Map banks 43-48 contain Johto maps
- Binary hack resource
- Would need extraction and conversion

### Technical Challenges Identified

#### Cross-Project Porting Issues

**From PokeCommunity Discussion:**
> "There are quite a number of dilemmas you can run into when trying to port Fire Red's code into Emerald, including warnings and error messages that will drive you insane"

**Specific Issues:**
1. Background music constants differ between games
2. Map type definitions not compatible
3. Battle scene constants different
4. Event structures vary
5. Tileset formats may differ

#### Map Structure Differences

**FireRed vs Emerald:**
- Different map header structures
- Different connection formats
- Different warp point systems
- Different event object formats

**GBA vs DS:**
- Completely different architectures
- Different tile formats
- Different compression methods
- Different scripting systems

## Recommended Approach: Hybrid Strategy

Based on research, I recommend a **three-phase hybrid approach** that balances ambition with practicality:

### Phase 1: Simplified Hub Regions (Immediate - 2-4 weeks)
Create functional hub-based regions to deliver multi-region experience quickly.

### Phase 2: Kanto Map Expansion (Medium-term - 2-3 months)
Port Kanto maps from Crossroads/FireRed to expand existing region.

### Phase 3: Full Region Implementation (Long-term - 6-12 months)
Gradually build out complete Johto, Hoenn, Sinnoh regions.

---

## PHASE 1: Simplified Hub Regions (START HERE)

### Objective
Create minimal but functional regions 2-4 with essential gameplay elements.

### Implementation Plan

#### Step 1.1: Create Hub Map Templates (Week 1)

**For Each Region (Johto, Hoenn, Sinnoh):**

1. **Central Hub Town**
   - Single map (20x20 tiles)
   - Pokemon Center
   - Pokemart
   - Region transition NPC
   - Starter distribution NPC

2. **Gym District**
   - Single map (30x30 tiles)
   - 8 gym buildings (exterior sprites)
   - Warp points to gym interiors

3. **Elite Four Complex**
   - Single map (25x25 tiles)
   - Elite Four building exterior
   - Victory Road entrance (optional)

4. **Wild Pokemon Areas**
   - Grass patch area (15x15 tiles)
   - Cave entrance (leads to cave map)
   - Water area for surfing

**Total Maps Per Region**: ~6-8 maps
**Total for All 3 Regions**: ~18-24 maps

#### Step 1.2: Configure Map Data (Week 1-2)

**Using Porymap:**

1. Create new map groups:
   ```c
   // In include/constants/map_groups.h
   #define MAP_GROUP_JOHTO_HUB    43
   #define MAP_GROUP_HOENN_HUB    44
   #define MAP_GROUP_SINNOH_HUB   45
   ```

2. Define map constants:
   ```c
   // In include/constants/maps.h
   #define MAP_JOHTO_NEW_BARK_TOWN    (43 | (0 << 8))
   #define MAP_JOHTO_GYM_DISTRICT     (43 | (1 << 8))
   #define MAP_JOHTO_ELITE_FOUR       (43 | (2 << 8))
   // ... etc
   ```

3. Create map headers in `data/maps/*/map.json`

4. Set up connections between maps

#### Step 1.3: Add Gym Leaders (Week 2)

**For Each Gym:**

1. Create gym interior map (reuse Kanto gym templates)
2. Place gym leader NPC
3. Configure trainer data:
   ```c
   // In src/data/trainers.h
   [TRAINER_JOHTO_FALKNER] = {
       .trainerClass = TRAINER_CLASS_LEADER,
       .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_GYM_LEADER,
       .trainerPic = TRAINER_PIC_LEADER_FALKNER,
       .trainerName = _("Falkner"),
       .items = {},
       .doubleBattle = FALSE,
       .aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY,
       .partySize = 2,
       .party = {
           {.species = SPECIES_PIDGEY, .lvl = 15},
           {.species = SPECIES_PIDGEOTTO, .lvl = 17}
       }
   };
   ```

4. Add gym badge flags:
   ```c
   // In include/constants/flags.h
   #define FLAG_BADGE_JOHTO_01_GET    (FLAG_BADGE08_GET + 1)
   #define FLAG_BADGE_JOHTO_02_GET    (FLAG_BADGE08_GET + 2)
   // ... up to +8
   ```

5. Create gym scripts (badge award, dialogue)

#### Step 1.4: Add Elite Four (Week 2)

Similar process to gym leaders, but with 4 trainers + champion.

#### Step 1.5: Configure Wild Encounters (Week 2)

**In `src/data/wild_encounters.json`:**

```json
{
    "map": "MAP_JOHTO_GRASS_AREA",
    "base_label": "gJohtoGrassArea",
    "land_mons": {
        "encounter_rate": 20,
        "mons": [
            {"min_level": 60, "max_level": 70, "species": "SPECIES_SENTRET"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_HOOTHOOT"},
            // ... etc
        ]
    }
}
```

**Note**: Actual levels will be overridden by our dynamic scaling system.

#### Step 1.6: Region Transition System (Week 3)

**Create Ticket Items:**

```c
// In include/constants/items.h
#define ITEM_JOHTO_TICKET    (ITEM_SINNOH_TICKET + 1)
#define ITEM_HOENN_TICKET    (ITEM_SINNOH_TICKET + 2)
#define ITEM_SINNOH_TICKET   (ITEM_SINNOH_TICKET + 3)
```

**Create Transition NPC Script:**

```c
// In data/maps/*/scripts.inc
JohtoTransitionNPC::
    lock
    faceplayer
    checkitem ITEM_JOHTO_TICKET
    goto_if_eq VAR_RESULT, FALSE, NoTicket
    msgbox JohtoTransition_Text_Confirm, MSGBOX_YESNO
    goto_if_eq VAR_RESULT, NO, Declined
    msgbox JohtoTransition_Text_Traveling
    closemessage
    setflag FLAG_VISITED_JOHTO
    warp MAP_JOHTO_NEW_BARK_TOWN, 5, 5
    waitstate
    release
    end

NoTicket:
    msgbox JohtoTransition_Text_NeedTicket
    release
    end

Declined:
    msgbox JohtoTransition_Text_Declined
    release
    end
```

**Award Ticket After Elite Four:**

```c
// In Kanto Elite Four champion script
    giveitem ITEM_JOHTO_TICKET
    msgbox EliteFour_Text_JohtoTicket
```

#### Step 1.7: Starter Distribution (Week 3)

**Create Starter NPC:**

```c
StarterNPC_Johto::
    lock
    faceplayer
    checkflag FLAG_RECEIVED_JOHTO_STARTER
    goto_if_set AlreadyReceived
    msgbox StarterNPC_Text_Choose
    multichoice 0, 0, MULTI_JOHTO_STARTERS, FALSE
    switch VAR_RESULT
    case 0, GiveChikorita
    case 1, GiveCyndaquil
    case 2, GiveTotodile
    case MULTI_B_PRESSED, Declined
    end

GiveChikorita:
    givemon SPECIES_CHIKORITA, 60
    setflag FLAG_RECEIVED_JOHTO_STARTER
    msgbox StarterNPC_Text_Enjoy
    release
    end
```

#### Step 1.8: Testing & Polish (Week 4)

1. Test region transitions
2. Test gym battles with level scaling
3. Test wild encounters
4. Test starter distribution
5. Fix bugs and polish

### Phase 1 Deliverables

✅ 3 new regions (Johto, Hoenn, Sinnoh) accessible
✅ 24 gym leaders (8 per region)
✅ 12 Elite Four members (4 per region)
✅ 3 champions
✅ Wild Pokemon encounters in each region
✅ Starter distribution for each region
✅ Region transition system working
✅ All integrated with existing level scaling

**Estimated Time**: 3-4 weeks
**Complexity**: Medium
**Playability**: High (delivers core multi-region experience)

---

## PHASE 2: Kanto Map Expansion (Future)

### Objective
Enhance Kanto region with additional maps and content from FireRed.

### Approach

**Source**: Pokemon Crossroads (Emerald) or pokefirered decomp

**Process**:
1. Identify missing Kanto maps (Sevii Islands, etc.)
2. Export maps from source using Porymap
3. Convert map data to pokefirered format
4. Import tilesets and graphics
5. Port events and scripts
6. Test and debug

**Estimated Time**: 2-3 months
**Complexity**: High (requires format conversion)

---

## PHASE 3: Full Region Implementation (Future)

### Objective
Build complete Johto, Hoenn, and Sinnoh regions with all maps.

### Approach Options

#### Option A: Port from Pokemon Crossroads (Recommended)

**Advantages:**
- Already ported Kanto to Emerald
- Proven multi-region architecture
- Open source
- Active development

**Process:**
1. Study Crossroads region switching system
2. Adapt Emerald maps to FireRed format
3. Port map data incrementally
4. Port events and scripts
5. Test extensively

**Challenges:**
- Emerald → FireRed conversion needed
- Different engine features
- Extensive testing required

#### Option B: Port from pret Decomps

**Sources:**
- Johto: pokeheartgold (DS)
- Hoenn: pokeemerald (GBA)
- Sinnoh: pokediamond (DS)

**Advantages:**
- Official map data
- High quality
- Well documented

**Challenges:**
- DS → GBA conversion extremely difficult
- Different data structures
- Would need custom conversion tools

#### Option C: Create Custom Regions

**Advantages:**
- Full creative control
- No legal concerns
- Optimized for Pokemon Tuxedo

**Challenges:**
- Massive time investment
- Requires artistic skills
- Need to design 100+ maps per region

### Recommended: Option A (Port from Crossroads)

**Rationale:**
- Crossroads has already solved the hard problems
- GBA → GBA conversion easier than DS → GBA
- Can leverage their region switching system
- Open source allows studying implementation

**Estimated Time**: 6-12 months (incremental)
**Complexity**: Very High

---

## Technical Implementation Details

### Map Porting Process (Crossroads → FireRed)

#### Step 1: Export Maps from Crossroads

```bash
# Clone Crossroads repository
git clone https://github.com/eonlynx/pokecrossroads.git
cd pokecrossroads

# Build to ensure it works
make

# Open in Porymap
porymap
```

#### Step 2: Identify Map Structure Differences

**Emerald Map Header:**
```c
struct MapHeader {
    const struct MapLayout *mapLayout;
    const struct MapEvents *events;
    const struct MapScripts *mapScripts;
    const struct MapConnections *connections;
    u16 music;
    u16 mapLayoutId;
    u8 regionMapSectionId;
    u8 cave;
    u8 weather;
    u8 mapType;
    u8 allowCycling:1;
    u8 allowEscaping:1;
    u8 allowRunning:1;
    u8 showMapName:6;
    s8 floorNum;
    u8 battleType;
};
```

**FireRed Map Header:**
```c
struct MapHeader {
    const struct MapLayout *mapLayout;
    const struct MapEvents *events;
    const struct MapScripts *mapScripts;
    const struct MapConnections *connections;
    u16 music;
    u16 mapLayoutId;
    u8 regionMapSectionId;
    u8 cave;
    u8 weather;
    u8 mapType;
    u8 filler_18;
    u8 flags;
    u8 battleType;
};
```

**Conversion Required:**
- Merge allowCycling/allowEscaping/allowRunning into flags byte
- Adjust showMapName field
- Handle floorNum differences

#### Step 3: Convert Map Data

**Create Conversion Script:**

```python
# map_converter.py
import json

def convert_emerald_to_firered(emerald_map):
    """Convert Emerald map format to FireRed format"""
    firered_map = {
        "id": emerald_map["id"],
        "name": emerald_map["name"],
        "layout": emerald_map["layout"],
        "music": convert_music_constant(emerald_map["music"]),
        "region_map_section": emerald_map["region_map_section"],
        "cave": emerald_map.get("cave", "FALSE"),
        "weather": emerald_map.get("weather", "WEATHER_SUNNY"),
        "map_type": convert_map_type(emerald_map["map_type"]),
        "battle_scene": convert_battle_scene(emerald_map.get("battle_scene", "MAP_BATTLE_SCENE_NORMAL")),
        "flags": convert_flags(emerald_map),
        "connections": emerald_map.get("connections", []),
        "object_events": convert_events(emerald_map.get("object_events", [])),
        "warp_events": emerald_map.get("warp_events", []),
        "coord_events": emerald_map.get("coord_events", []),
        "bg_events": emerald_map.get("bg_events", [])
    }
    return firered_map

def convert_music_constant(emerald_music):
    """Map Emerald music constants to FireRed equivalents"""
    music_map = {
        "MUS_LITTLEROOT": "MUS_PALLET",
        "MUS_OLDALE": "MUS_VIRIDIAN",
        # ... etc
    }
    return music_map.get(emerald_music, "MUS_PALLET")

def convert_map_type(emerald_type):
    """Convert map type constants"""
    type_map = {
        "MAP_TYPE_TOWN": "MAP_TYPE_TOWN",
        "MAP_TYPE_CITY": "MAP_TYPE_CITY",
        "MAP_TYPE_ROUTE": "MAP_TYPE_ROUTE",
        # ... etc
    }
    return type_map.get(emerald_type, "MAP_TYPE_TOWN")

def convert_battle_scene(emerald_scene):
    """Convert battle scene constants"""
    scene_map = {
        "MAP_BATTLE_SCENE_NORMAL": "MAP_BATTLE_SCENE_NORMAL",
        "MAP_BATTLE_SCENE_GYM": "MAP_BATTLE_SCENE_GYM",
        # ... etc
    }
    return scene_map.get(emerald_scene, "MAP_BATTLE_SCENE_NORMAL")

def convert_flags(emerald_map):
    """Convert Emerald flags to FireRed flags byte"""
    flags = 0
    if emerald_map.get("allow_cycling", False):
        flags |= 0x01
    if emerald_map.get("allow_escaping", False):
        flags |= 0x02
    if emerald_map.get("allow_running", False):
        flags |= 0x04
    if emerald_map.get("show_map_name", 0) > 0:
        flags |= 0x08
    return flags

def convert_events(emerald_events):
    """Convert event object structures"""
    firered_events = []
    for event in emerald_events:
        firered_event = {
            "graphics_id": event["graphics_id"],
            "x": event["x"],
            "y": event["y"],
            "elevation": event.get("elevation", 0),
            "movement_type": event["movement_type"],
            "movement_range_x": event.get("movement_range_x", 0),
            "movement_range_y": event.get("movement_range_y", 0),
            "trainer_type": event.get("trainer_type", "TRAINER_TYPE_NONE"),
            "trainer_sight_or_berry_tree_id": event.get("trainer_sight_or_berry_tree_id", 0),
            "script": event.get("script", "NULL"),
            "flag": event.get("flag", "0")
        }
        firered_events.append(firered_event)
    return firered_events

# Usage
if __name__ == "__main__":
    import sys
    with open(sys.argv[1], 'r') as f:
        emerald_map = json.load(f)
    firered_map = convert_emerald_to_firered(emerald_map)
    with open(sys.argv[2], 'w') as f:
        json.dump(firered_map, f, indent=2)
```

#### Step 4: Import Tilesets

**Process:**
1. Export tileset graphics from Crossroads
2. Convert to FireRed format if needed
3. Import into pokefirered project
4. Update tileset headers

**Tileset Structure:**
```c
// In data/tilesets/headers.h
const struct Tileset gTileset_Johto_General = {
    .isCompressed = TRUE,
    .isSecondary = FALSE,
    .tiles = gTilesetTiles_JohtoGeneral,
    .palettes = gTilesetPalettes_JohtoGeneral,
    .metatiles = gMetatiles_JohtoGeneral,
    .metatileAttributes = gMetatileAttributes_JohtoGeneral,
    .callback = NULL,
};
```

#### Step 5: Port Scripts

**Emerald Script:**
```
NewBarkTown_ProfessorElmsLab_EventScript_Elm::
    lock
    faceplayer
    msgbox NewBarkTown_ProfessorElmsLab_Text_Elm_Intro, MSGBOX_DEFAULT
    release
    end
```

**FireRed Script (same format):**
```
NewBarkTown_ProfessorElmsLab_EventScript_Elm::
    lock
    faceplayer
    msgbox NewBarkTown_ProfessorElmsLab_Text_Elm_Intro, MSGBOX_DEFAULT
    release
    end
```

**Good news**: Poryscript is compatible between Emerald and FireRed!

#### Step 6: Test and Debug

1. Compile after each map import
2. Test map transitions
3. Test events and scripts
4. Fix any compatibility issues
5. Adjust graphics if needed

### Region Switching System (from Crossroads)

**Key Components:**

1. **Region Flag System:**
```c
// In include/constants/flags.h
#define FLAG_CURRENT_REGION_KANTO    0x1000
#define FLAG_CURRENT_REGION_JOHTO    0x1001
#define FLAG_CURRENT_REGION_HOENN    0x1002
#define FLAG_CURRENT_REGION_SINNOH   0x1003
```

2. **Region Detection:**
```c
// In src/region_manager.c
u8 GetCurrentRegionFromMap(u16 mapId) {
    if (mapId >= MAP_GROUP_KANTO_START && mapId < MAP_GROUP_JOHTO_START)
        return REGION_KANTO;
    else if (mapId >= MAP_GROUP_JOHTO_START && mapId < MAP_GROUP_HOENN_START)
        return REGION_JOHTO;
    else if (mapId >= MAP_GROUP_HOENN_START && mapId < MAP_GROUP_SINNOH_START)
        return REGION_HOENN;
    else
        return REGION_SINNOH;
}
```

3. **Fly System Update:**
```c
// In src/field_region_map.c
void UpdateFlyLocationsForRegion(u8 region) {
    u8 i;
    for (i = 0; i < NUM_FLY_LOCATIONS; i++) {
        if (sFlyLocations[i].region == region && 
            FlagGet(sFlyLocations[i].flag)) {
            // Add to available fly locations
        }
    }
}
```

4. **Region Map Display:**
```c
// In src/region_map.c
const struct RegionMapLayout *GetRegionMapLayout(u8 region) {
    switch (region) {
        case REGION_KANTO:
            return &sRegionMapLayout_Kanto;
        case REGION_JOHTO:
            return &sRegionMapLayout_Johto;
        case REGION_HOENN:
            return &sRegionMapLayout_Hoenn;
        case REGION_SINNOH:
            return &sRegionMapLayout_Sinnoh;
        default:
            return &sRegionMapLayout_Kanto;
    }
}
```

---

## Resource Requirements

### Tools Needed

1. **Porymap** (latest version)
   - Map editor for decomp projects
   - Download: https://github.com/huderlem/porymap

2. **Poryscript** (optional but recommended)
   - Modern scripting language
   - Compiles to GBA scripts
   - Download: https://github.com/huderlem/poryscript

3. **Python 3.x**
   - For conversion scripts
   - For automation

4. **Git**
   - Version control
   - Cloning repositories

5. **Image Editor** (GIMP, Photoshop, etc.)
   - For tileset editing if needed

### Knowledge Requirements

1. **Porymap Usage**
   - Creating maps
   - Editing tilesets
   - Placing events
   - Setting up connections

2. **Poryscript/GBA Scripting**
   - Event scripts
   - NPC dialogue
   - Item distribution
   - Warp points

3. **C Programming** (for advanced features)
   - Modifying engine code
   - Adding new systems
   - Debugging

4. **JSON** (for map data)
   - Understanding map structure
   - Editing map properties

### Time Investment

**Phase 1 (Simplified Hubs):**
- Setup: 2-3 days
- Map creation: 1 week
- Gym/E4 setup: 1 week
- Scripts/events: 3-4 days
- Testing: 3-4 days
- **Total: 3-4 weeks**

**Phase 2 (Kanto Expansion):**
- Research: 1 week
- Map porting: 4-6 weeks
- Script porting: 2-3 weeks
- Testing: 2 weeks
- **Total: 2-3 months**

**Phase 3 (Full Regions):**
- Per region: 2-4 months
- All 3 regions: 6-12 months
- **Total: 6-12 months (incremental)**

---

## Recommended Action Plan

### Immediate Next Steps (This Week)

1. **Set up Porymap**
   ```bash
   # Download from GitHub releases
   # Install and configure for pokefirered
   ```

2. **Create Phase 1 Map Structure**
   - Define map groups (43-45)
   - Create map constants
   - Set up basic map headers

3. **Build First Hub (Johto)**
   - Create New Bark Town hub map
   - Add Pokemon Center
   - Add Pokemart
   - Add transition NPC

4. **Test Basic Functionality**
   - Compile ROM
   - Test map loading
   - Test warps
   - Test NPCs

### Week 2-3: Complete Phase 1

1. Finish all hub maps
2. Add all gym leaders
3. Add Elite Four
4. Configure wild encounters
5. Add starter distribution
6. Implement region transitions

### Week 4: Testing & Polish

1. Full playthrough test
2. Bug fixes
3. Balance adjustments
4. Documentation

### Month 2+: Begin Phase 2 (Optional)

1. Clone Pokemon Crossroads
2. Study their implementation
3. Begin porting Kanto maps
4. Incremental integration

---

## Success Criteria

### Phase 1 Complete When:
- ✅ Can travel to Johto, Hoenn, Sinnoh
- ✅ Can battle all 24 gym leaders
- ✅ Can battle all Elite Four members
- ✅ Wild Pokemon encounters work
- ✅ Level scaling works correctly
- ✅ Starters distributed properly
- ✅ No game-breaking bugs

### Phase 2 Complete When:
- ✅ Kanto has all original maps
- ✅ Sevii Islands accessible
- ✅ All Kanto content functional
- ✅ Smooth transitions between areas

### Phase 3 Complete When:
- ✅ All regions have complete maps
- ✅ All routes, cities, caves present
- ✅ All trainers placed
- ✅ All items placed
- ✅ All events functional
- ✅ Full playthrough possible

---

## Conclusion

The research shows that multi-region implementation is **definitely possible** but requires a phased approach:

1. **Phase 1 (Simplified Hubs)**: Delivers playable multi-region experience in 3-4 weeks
2. **Phase 2 (Kanto Expansion)**: Enhances existing region over 2-3 months
3. **Phase 3 (Full Regions)**: Complete implementation over 6-12 months

**Recommendation**: Start with Phase 1 to prove the concept and deliver value quickly. Then evaluate whether to proceed with Phase 2/3 based on feedback and available time.

The Pokemon Crossroads project proves this is achievable and provides a roadmap we can follow. Their open-source implementation gives us a huge advantage.

**Next Step**: Begin Phase 1 implementation this week, starting with Johto hub maps.
