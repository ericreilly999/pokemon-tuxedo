# Regional Content Integration Guide

## Overview

This guide covers the integration of Johto, Hoenn, and Sinnoh regions into Pokemon Tuxedo. This is the most time-intensive task, requiring map editing, encounter data, trainer data, and storyline adaptation.

## Regional Content Requirements

### Johto Region (Generation II)
- **Source**: Pokemon HeartGold/SoulSilver or Gold/Silver/Crystal
- **Maps**: 8 cities, 8 gym locations, routes, dungeons
- **Gyms**: Violet City, Azalea Town, Goldenrod City, Ecruteak City, Cianwood City, Olivine City, Mahogany Town, Blackthorn City
- **Elite Four**: Will, Koga, Bruno, Karen, Champion Lance
- **Starting Level**: 60
- **Starters**: Chikorita, Cyndaquil, Totodile

### Hoenn Region (Generation III)
- **Source**: Pokemon Ruby/Sapphire/Emerald
- **Maps**: 8 cities, 8 gym locations, routes, dungeons
- **Gyms**: Rustboro City, Dewford Town, Mauville City, Lavaridge Town, Petalburg City, Fortree City, Mossdeep City, Sootopolis City
- **Elite Four**: Sidney, Phoebe, Glacia, Drake, Champion Wallace/Steven
- **Starting Level**: 110
- **Starters**: Treecko, Torchic, Mudkip

### Sinnoh Region (Generation IV)
- **Source**: Pokemon Diamond/Pearl/Platinum
- **Maps**: 8 cities, 8 gym locations, routes, dungeons
- **Gyms**: Oreburgh City, Eterna City, Veilstone City, Pastoria City, Hearthome City, Canalave City, Snowpoint City, Sunyshore City
- **Elite Four**: Aaron, Bertha, Flint, Lucian, Champion Cynthia
- **Starting Level**: 160
- **Starters**: Turtwig, Chimchar, Piplup

## Integration Steps

### Phase 1: Map Data Extraction

#### Tools Required:
- **Advance Map** - GBA map editor
- **SDSME** - DS map editor (for HGSS/DPPt)
- **PokeScript** - Script editor
- **G3HS** - Header editor

#### Process:
1. Extract map data from source ROMs
2. Convert maps to FireRed format
3. Import map tiles and tilesets
4. Set up map connections
5. Place NPCs and objects

#### Map Data Structure:
```
maps/
├── johto/
│   ├── violet_city/
│   ├── azalea_town/
│   ├── goldenrod_city/
│   └── ... (all Johto maps)
├── hoenn/
│   ├── rustboro_city/
│   ├── dewford_town/
│   ├── mauville_city/
│   └── ... (all Hoenn maps)
└── sinnoh/
    ├── oreburgh_city/
    ├── eterna_city/
    ├── veilstone_city/
    └── ... (all Sinnoh maps)
```

### Phase 2: Wild Encounter Data

#### Data Required:
- Wild Pokemon species per route/area
- Encounter rates
- Level ranges (will be overridden by level scaler)

#### Implementation:
```c
// encounters_johto.c
const struct WildPokemonInfo gJohtoWildMons[] = {
    {
        .mapGroup = MAP_GROUP_JOHTO,
        .mapNum = MAP_ROUTE_29,
        .landMonsInfo = {
            .encounterRate = 25,
            .mons = {
                {2, 10, SPECIES_PIDGEY},    // Will be scaled by level scaler
                {2, 10, SPECIES_RATTATA},
                {2, 10, SPECIES_SENTRET},
                // ... more encounters
            }
        }
    },
    // ... more routes
};
```

#### Integration with Level Scaler:
- Base levels in encounter data are ignored
- Level scaler calculates actual levels based on badges
- Encounter species and rates are preserved

### Phase 3: Trainer Data

#### Data Required:
- Gym leader teams
- Elite Four teams
- Rival teams
- Regular trainer teams

#### Implementation:
```c
// trainers_johto.c
const struct Trainer gJohtoTrainers[] = {
    [TRAINER_FALKNER] = {
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .trainerPic = TRAINER_PIC_LEADER_FALKNER,
        .trainerName = _("FALKNER"),
        .items = {},
        .doubleBattle = FALSE,
        .aiFlags = AI_SCRIPT_CHECK_BAD_MOVE | AI_SCRIPT_TRY_TO_FAINT,
        .partySize = 2,
        .party = {
            {
                .species = SPECIES_PIDGEY,
                .lvl = 0,  // Will be calculated by level scaler
                .moves = {MOVE_TACKLE, MOVE_SAND_ATTACK, MOVE_GUST, MOVE_NONE}
            },
            {
                .species = SPECIES_PIDGEOTTO,
                .lvl = 0,  // Will be calculated by level scaler (ace)
                .moves = {MOVE_TACKLE, MOVE_GUST, MOVE_QUICK_ATTACK, MOVE_NONE}
            }
        }
    },
    // ... more trainers
};
```

#### Integration with Level Scaler:
- Trainer levels set to 0 in data
- Level scaler calculates actual levels at runtime
- Gym leaders get average + ace bonus
- Elite Four get higher scaling
- Rivals get special scaling

### Phase 4: Region Transition Events

#### Implementation:
```c
// region_transitions.c
void TransitionToJohto(void)
{
    // Check requirements
    if (!IsEliteFourDefeated(REGION_KANTO))
        return;
    
    if (!CheckBagHasItem(ITEM_JOHTO_TICKET, 1))
        return;
    
    // Set region
    SetCurrentRegion(REGION_JOHTO);
    
    // Unlock region maps
    UnlockRegionMaps(REGION_JOHTO);
    
    // Warp to starting location
    SetWarpDestination(MAP_GROUP_JOHTO, MAP_NEW_BARK_TOWN, 0, 8, 6);
    DoWarp();
}
```

#### Region Transition NPCs:
- Place NPCs at region borders
- Check for Elite Four defeat + ticket
- Trigger region transition
- Display transition cutscene

### Phase 5: Storyline Adaptation

#### Kanto Storyline:
- Keep original FireRed story
- Add region transition at end
- Award Johto Ticket after Elite Four

#### Johto Storyline:
- Simplified story (no Team Rocket takeover)
- Focus on gym challenge
- Award Hoenn Ticket after Elite Four
- Offer Johto starters at start

#### Hoenn Storyline:
- Simplified story (no Team Aqua/Magma)
- Focus on gym challenge
- Award Sinnoh Ticket after Elite Four
- Offer Hoenn starters at start

#### Sinnoh Storyline:
- Simplified story (no Team Galactic)
- Focus on gym challenge
- Final region (no more tickets)
- Offer Sinnoh starters at start

### Phase 6: Script Integration

#### Example: Johto Starter Selection
```
script JohtoStarterSelection {
    lock
    faceplayer
    msgbox("Welcome to Johto!\nPlease choose your starter Pokemon!", MSGBOX_DEFAULT)
    
    multichoice(0, 0, MULTI_JOHTO_STARTERS, FALSE)
    
    switch(var(VAR_RESULT)) {
        case 0:  // Chikorita
            givemon(SPECIES_CHIKORITA, 60, ITEM_NONE)
            break
        case 1:  // Cyndaquil
            givemon(SPECIES_CYNDAQUIL, 60, ITEM_NONE)
            break
        case 2:  // Totodile
            givemon(SPECIES_TOTODILE, 60, ITEM_NONE)
            break
    }
    
    msgbox("Take good care of your Pokemon!", MSGBOX_DEFAULT)
    release
}
```

## Data Files to Create

### Map Headers
```c
// map_headers_johto.h
#define MAP_JOHTO_NEW_BARK_TOWN 0x100
#define MAP_JOHTO_ROUTE_29 0x101
#define MAP_JOHTO_CHERRYGROVE_CITY 0x102
// ... all Johto maps
```

### Encounter Tables
```c
// encounters_johto.h
extern const struct WildPokemonInfo gJohtoWildMons[];
extern const struct WildPokemonInfo gHoennWildMons[];
extern const struct WildPokemonInfo gSinnohWildMons[];
```

### Trainer Tables
```c
// trainers_johto.h
extern const struct Trainer gJohtoTrainers[];
extern const struct Trainer gHoennTrainers[];
extern const struct Trainer gSinnohTrainers[];
```

## Testing Checklist

### Per Region:
- [ ] All maps load correctly
- [ ] Map connections work properly
- [ ] Wild encounters work
- [ ] Trainers battle correctly
- [ ] Gym leaders scale properly
- [ ] Elite Four scale properly
- [ ] Starters are offered at correct level
- [ ] Region transition works
- [ ] Can return to previous regions
- [ ] Level scaling works across regions

### Integration:
- [ ] All 4 regions accessible
- [ ] Region progression enforced
- [ ] Level scaling consistent
- [ ] Save data works across regions
- [ ] No map conflicts or overlaps
- [ ] All 32 gyms functional
- [ ] All 4 Elite Fours functional

## Estimated Time

### Per Region:
- Map extraction and conversion: 20-30 hours
- Encounter data: 5-10 hours
- Trainer data: 10-15 hours
- Script adaptation: 10-15 hours
- Testing and debugging: 10-20 hours
- **Total per region: 55-90 hours**

### All Three Regions:
- **Total: 165-270 hours (4-6 weeks full-time)**

## Simplification Options

### Option 1: Reuse Existing Maps
- Use Kanto maps with different encounters
- Rename cities to Johto/Hoenn/Sinnoh names
- Saves 60-80% of map work

### Option 2: Procedural Generation
- Generate simplified maps programmatically
- Focus on gym locations and routes
- Saves 40-60% of map work

### Option 3: Minimal Maps
- Create only essential locations (gyms, Pokemon centers)
- Skip optional areas and dungeons
- Saves 50-70% of map work

### Option 4: Community Contribution
- Release base ROM with Kanto only
- Allow community to add regions
- Provide tools and documentation

## Recommended Approach

For initial release:
1. Complete Kanto fully (already done)
2. Create simplified Johto with essential maps
3. Test thoroughly
4. Release as "Pokemon Tuxedo v1.0"
5. Add Hoenn and Sinnoh in future updates

This allows for:
- Faster initial release
- Community feedback
- Iterative development
- Manageable scope

## Resources

### Tools:
- Advance Map: https://www.pokecommunity.com/showthread.php?t=149568
- SDSME: https://projectpokemon.org/home/forums/topic/11221-sdsme/
- PokeScript: https://github.com/pret/pokefirered

### Tutorials:
- "How to Add New Maps to Pokemon FireRed"
- "Pokemon ROM Hacking Map Tutorial"
- "Creating Custom Routes in Pokemon"

### Communities:
- PokeCommunity ROM Hacking section
- r/PokemonROMhacks
- ROM Hacking Discord servers

## Notes

- Regional content is the largest remaining task
- Can be done incrementally (one region at a time)
- Not required for core system testing
- Consider simplified approach for initial release
- Full regional content can be added in updates

## Integration with Existing Systems

All regional content integrates with:
- ✅ Level Scaler (automatic level calculation)
- ✅ Region Manager (region tracking and travel)
- ✅ Experience System (full party exp + multipliers)
- ✅ Storage System (accessible everywhere)
- ✅ Game Settings (apply to all regions)

No additional code needed - just data files!
