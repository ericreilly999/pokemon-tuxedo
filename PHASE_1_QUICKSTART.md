# Phase 1 Quick Start Guide: Simplified Hub Regions

## Overview

This guide walks you through implementing Phase 1 of the multi-region system: creating simplified hub-based regions for Johto, Hoenn, and Sinnoh.

**Goal**: Deliver a playable multi-region experience in 3-4 weeks.

**What You'll Build**:
- 3 new regions (Johto, Hoenn, Sinnoh)
- 24 gym leaders (8 per region)
- 12 Elite Four members (4 per region)
- Wild Pokemon encounters
- Starter distribution
- Region transitions

## Prerequisites

### Tools Required

1. **Porymap** (Map Editor)
   - Download: https://github.com/huderlem/porymap/releases
   - Install and configure for pokefirered

2. **devkitARM** (Already installed)
   - Location: `C:\devkitPro`

3. **Text Editor** (VS Code recommended)
   - For editing JSON and C files

### Knowledge Required

- Basic understanding of Pokemon game structure
- Familiarity with map editing concepts
- Basic C programming (for trainer data)
- JSON editing

## Step-by-Step Implementation

### WEEK 1: Setup and First Hub

#### Day 1-2: Project Setup

**1. Install Porymap**

Download from GitHub releases and install. Configure for pokefirered:

```
File → Open Project → Select firered-clone directory
```

**2. Define Map Groups**

Edit `include/constants/map_groups.h`:

```c
// Add after existing map groups
#define MAP_GROUP_JOHTO_HUB     43
#define MAP_GROUP_HOENN_HUB     44
#define MAP_GROUP_SINNOH_HUB    45
```

**3. Define Map Constants**

Edit `include/constants/maps.h`:

```c
// Johto Hub Maps
#define MAP_JOHTO_NEW_BARK_TOWN        (43 | (0 << 8))
#define MAP_JOHTO_GYM_DISTRICT         (43 | (1 << 8))
#define MAP_JOHTO_ELITE_FOUR_COMPLEX   (43 | (2 << 8))
#define MAP_JOHTO_GRASS_AREA           (43 | (3 << 8))
#define MAP_JOHTO_CAVE                 (43 | (4 << 8))

// Hoenn Hub Maps
#define MAP_HOENN_LITTLEROOT_TOWN      (44 | (0 << 8))
#define MAP_HOENN_GYM_DISTRICT         (44 | (1 << 8))
#define MAP_HOENN_ELITE_FOUR_COMPLEX   (44 | (2 << 8))
#define MAP_HOENN_GRASS_AREA           (44 | (3 << 8))
#define MAP_HOENN_CAVE                 (44 | (4 << 8))

// Sinnoh Hub Maps
#define MAP_SINNOH_TWINLEAF_TOWN       (45 | (0 << 8))
#define MAP_SINNOH_GYM_DISTRICT        (45 | (1 << 8))
#define MAP_SINNOH_ELITE_FOUR_COMPLEX  (45 | (2 << 8))
#define MAP_SINNOH_GRASS_AREA          (45 | (3 << 8))
#define MAP_SINNOH_CAVE                (45 | (4 << 8))
```

**4. Create Map Directories**

```bash
cd data/maps
mkdir JohtoNewBarkTown JohtoGymDistrict JohtoEliteFourComplex JohtoGrassArea JohtoCave
mkdir HoennLittlerootTown HoennGymDistrict HoennEliteFourComplex HoennGrassArea HoennCave
mkdir SinnohTwinleafTown SinnohGymDistrict SinnohEliteFourComplex SinnohGrassArea SinnohCave
```

#### Day 3-4: Create First Hub Map (Johto New Bark Town)

**1. Open Porymap**

```
File → New Map
```

**2. Configure Map Properties**

- Name: `JohtoNewBarkTown`
- Width: 20 tiles
- Height: 20 tiles
- Primary Tileset: `gTileset_General`
- Secondary Tileset: `gTileset_PalletTown` (reuse Kanto tileset)

**3. Design Layout**

```
┌─────────────────────┐
│                     │
│   [Pokemon Center]  │
│                     │
│   [Pokemart]        │
│                     │
│   [Transition NPC]  │
│                     │
│   [Starter NPC]     │
│                     │
│   [Warp to Gyms]    │
│                     │
└─────────────────────┘
```

**4. Place Buildings**

- Use existing building tiles from Kanto
- Place Pokemon Center (copy from Pallet Town)
- Place Pokemart (copy from Viridian City)

**5. Add Warp Points**

In Porymap:
- Click "Events" tab
- Add warp to Pokemon Center interior
- Add warp to Pokemart interior
- Add warp to Gym District

**6. Save Map**

```
File → Save
```

This creates `data/maps/JohtoNewBarkTown/map.json`

#### Day 5-7: Add NPCs and Scripts

**1. Create Transition NPC**

In Porymap Events tab:
- Add Object Event
- Graphics: `OBJ_EVENT_GFX_SAILOR`
- Movement: `MOVEMENT_TYPE_FACE_DOWN`
- Script: `JohtoNewBarkTown_EventScript_TransitionNPC`

**2. Create Script File**

Create `data/maps/JohtoNewBarkTown/scripts.pory`:

```pory
script JohtoNewBarkTown_EventScript_TransitionNPC {
    lock
    faceplayer
    msgbox("Welcome to Johto!\nWould you like to return to Kanto?", MSGBOX_YESNO)
    if (var(VAR_RESULT) == YES) {
        msgbox("Traveling to Kanto...")
        closemessage
        warp(MAP_PALLET_TOWN, 5, 5)
        waitstate
    }
    release
}
```

**3. Create Starter Distribution NPC**

Add another Object Event:
- Graphics: `OBJ_EVENT_GFX_PROF_OAK`
- Script: `JohtoNewBarkTown_EventScript_StarterNPC`

```pory
script JohtoNewBarkTown_EventScript_StarterNPC {
    lock
    faceplayer
    checkflag(FLAG_RECEIVED_JOHTO_STARTER)
    if (flag(FLAG_RECEIVED_JOHTO_STARTER)) {
        msgbox("I hope your starter is doing well!")
        release
        end
    }
    msgbox("Welcome to Johto!\nPlease choose a starter Pokemon.")
    multichoice(0, 0, MULTI_JOHTO_STARTERS, FALSE)
    switch (var(VAR_RESULT)) {
        case 0:
            givemon(SPECIES_CHIKORITA, 60)
            setflag(FLAG_RECEIVED_JOHTO_STARTER)
            msgbox("You received Chikorita!")
        case 1:
            givemon(SPECIES_CYNDAQUIL, 60)
            setflag(FLAG_RECEIVED_JOHTO_STARTER)
            msgbox("You received Cyndaquil!")
        case 2:
            givemon(SPECIES_TOTODILE, 60)
            setflag(FLAG_RECEIVED_JOHTO_STARTER)
            msgbox("You received Totodile!")
    }
    release
}
```

**4. Define Flags**

Edit `include/constants/flags.h`:

```c
// Johto flags
#define FLAG_RECEIVED_JOHTO_STARTER    (FLAG_UNUSED_0x500 + 1)
#define FLAG_VISITED_JOHTO             (FLAG_UNUSED_0x500 + 2)
```

**5. Compile and Test**

```bash
make
```

Test in emulator:
- Load ROM
- Use debug warp to Johto map
- Test NPCs
- Test scripts

### WEEK 2: Gym District and Gyms

#### Day 8-10: Create Gym District Map

**1. Create New Map in Porymap**

- Name: `JohtoGymDistrict`
- Size: 30x30 tiles
- Layout: Grid of 8 gym buildings

**2. Design Layout**

```
┌─────────────────────────────┐
│  [Gym 1]    [Gym 2]         │
│                             │
│  [Gym 3]    [Gym 4]         │
│                             │
│  [Gym 5]    [Gym 6]         │
│                             │
│  [Gym 7]    [Gym 8]         │
│                             │
│  [Back to Town]             │
└─────────────────────────────┘
```

**3. Add Gym Buildings**

- Reuse Kanto gym exterior tiles
- Place 8 gym buildings
- Add warp points to gym interiors

**4. Create Gym Interior Maps**

For each gym, create a simple interior:
- Copy existing Kanto gym layout
- Resize to 10x10 tiles
- Place gym leader NPC
- Add warp back to district

#### Day 11-14: Configure Gym Leaders

**1. Define Trainer Constants**

Edit `include/constants/trainers.h`:

```c
// Johto Gym Leaders
#define TRAINER_JOHTO_FALKNER      (TRAINER_CHAMPION + 1)
#define TRAINER_JOHTO_BUGSY        (TRAINER_CHAMPION + 2)
#define TRAINER_JOHTO_WHITNEY      (TRAINER_CHAMPION + 3)
#define TRAINER_JOHTO_MORTY        (TRAINER_CHAMPION + 4)
#define TRAINER_JOHTO_CHUCK        (TRAINER_CHAMPION + 5)
#define TRAINER_JOHTO_JASMINE      (TRAINER_CHAMPION + 6)
#define TRAINER_JOHTO_PRYCE        (TRAINER_CHAMPION + 7)
#define TRAINER_JOHTO_CLAIR        (TRAINER_CHAMPION + 8)
```

**2. Add Trainer Data**

Edit `src/data/trainers.h`:

```c
[TRAINER_JOHTO_FALKNER] = {
    .trainerClass = TRAINER_CLASS_LEADER,
    .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_GYM_LEADER,
    .trainerPic = TRAINER_PIC_LEADER_BROCK, // Reuse for now
    .trainerName = _("Falkner"),
    .items = {},
    .doubleBattle = FALSE,
    .aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY,
    .partySize = 2,
    .party = {
        {.species = SPECIES_PIDGEY, .lvl = 15},
        {.species = SPECIES_PIDGEOTTO, .lvl = 17}
    }
},

[TRAINER_JOHTO_BUGSY] = {
    .trainerClass = TRAINER_CLASS_LEADER,
    .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_GYM_LEADER,
    .trainerPic = TRAINER_PIC_LEADER_BROCK,
    .trainerName = _("Bugsy"),
    .items = {},
    .doubleBattle = FALSE,
    .aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY,
    .partySize = 3,
    .party = {
        {.species = SPECIES_METAPOD, .lvl = 15},
        {.species = SPECIES_KAKUNA, .lvl = 15},
        {.species = SPECIES_SCYTHER, .lvl = 17}
    }
},

// ... Continue for all 8 gym leaders
```

**Note**: Levels will be overridden by dynamic scaling system.

**3. Add Badge Flags**

Edit `include/constants/flags.h`:

```c
// Johto Badges
#define FLAG_BADGE_JOHTO_01_GET    (FLAG_BADGE08_GET + 1)
#define FLAG_BADGE_JOHTO_02_GET    (FLAG_BADGE08_GET + 2)
#define FLAG_BADGE_JOHTO_03_GET    (FLAG_BADGE08_GET + 3)
#define FLAG_BADGE_JOHTO_04_GET    (FLAG_BADGE08_GET + 4)
#define FLAG_BADGE_JOHTO_05_GET    (FLAG_BADGE08_GET + 5)
#define FLAG_BADGE_JOHTO_06_GET    (FLAG_BADGE08_GET + 6)
#define FLAG_BADGE_JOHTO_07_GET    (FLAG_BADGE08_GET + 7)
#define FLAG_BADGE_JOHTO_08_GET    (FLAG_BADGE08_GET + 8)
```

**4. Create Gym Leader Scripts**

For each gym, create `data/maps/JohtoGym1/scripts.pory`:

```pory
script JohtoGym1_EventScript_Falkner {
    trainerbattle_single(TRAINER_JOHTO_FALKNER, JohtoGym1_Text_FalknerIntro, JohtoGym1_Text_FalknerDefeat, JohtoGym1_EventScript_GiveBadge)
    msgbox(JohtoGym1_Text_FalknerPostBattle, MSGBOX_AUTOCLOSE)
    end
}

script JohtoGym1_EventScript_GiveBadge {
    msgbox(JohtoGym1_Text_ReceivedBadge)
    playfanfare(MUS_OBTAIN_BADGE)
    setflag(FLAG_BADGE_JOHTO_01_GET)
    setflag(FLAG_DEFEATED_JOHTO_GYM_1)
    setvar(VAR_JOHTO_BADGES, 1)
    waitfanfare
    release
    end
}

// Text
text JohtoGym1_Text_FalknerIntro {
    "I'm Falkner, the Violet\n"
    "Pokemon Gym Leader!\p"
    "I'll show you the power of\n"
    "magnificent Flying Pokemon!"
}

text JohtoGym1_Text_FalknerDefeat {
    "I understand...\n"
    "I'll bow out gracefully."
}

text JohtoGym1_Text_ReceivedBadge {
    "{PLAYER} received the\n"
    "Zephyr Badge!"
}

text JohtoGym1_Text_FalknerPostBattle {
    "Your Pokemon's powerful\n"
    "attacks overcame my Flying\l"
    "Pokemon's speed!"
}
```

### WEEK 3: Elite Four and Wild Encounters

#### Day 15-17: Elite Four Complex

**1. Create Elite Four Map**

- Name: `JohtoEliteFourComplex`
- Size: 25x25 tiles
- Layout: 4 rooms + champion room

**2. Add Elite Four Trainers**

Similar to gym leaders, define:
- `TRAINER_JOHTO_WILL` (Psychic)
- `TRAINER_JOHTO_KOGA` (Poison)
- `TRAINER_JOHTO_BRUNO` (Fighting)
- `TRAINER_JOHTO_KAREN` (Dark)
- `TRAINER_JOHTO_LANCE` (Dragon/Champion)

**3. Create Scripts**

Similar to gym scripts, but with:
- No badge rewards
- Sequential battles
- Champion gives ticket to next region

#### Day 18-21: Wild Encounters

**1. Create Grass Area Map**

- Name: `JohtoGrassArea`
- Size: 15x15 tiles
- Add grass tiles
- Add warp back to town

**2. Configure Wild Encounters**

Create `data/maps/JohtoGrassArea/wild_encounters.json`:

```json
{
    "map": "MAP_JOHTO_GRASS_AREA",
    "base_label": "gJohtoGrassArea",
    "land_mons": {
        "encounter_rate": 20,
        "mons": [
            {"min_level": 60, "max_level": 70, "species": "SPECIES_SENTRET"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_HOOTHOOT"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_PIDGEY"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_RATTATA"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_SPINARAK"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_LEDYBA"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_HOPPIP"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_MAREEP"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_WOOPER"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_GEODUDE"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_ZUBAT"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_BELLSPROUT"}
        ]
    },
    "water_mons": {
        "encounter_rate": 5,
        "mons": [
            {"min_level": 60, "max_level": 70, "species": "SPECIES_TENTACOOL"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_MAGIKARP"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_GOLDEEN"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_POLIWAG"},
            {"min_level": 60, "max_level": 70, "species": "SPECIES_KRABBY"}
        ]
    }
}
```

**Note**: Levels will be overridden by dynamic scaling.

**3. Create Cave Map**

Similar to grass area, but with cave tiles and cave Pokemon.

### WEEK 4: Hoenn, Sinnoh, and Testing

#### Day 22-24: Replicate for Hoenn

Copy Johto structure:
- Hub town
- Gym district
- Elite Four complex
- Wild areas

Update with Hoenn-specific:
- Starters: Treecko, Torchic, Mudkip
- Gym leaders: Roxanne, Brawly, Wattson, Flannery, Norman, Winona, Tate & Liza, Wallace
- Elite Four: Sidney, Phoebe, Glacia, Drake, Steven
- Wild Pokemon: Hoenn species

#### Day 25-27: Replicate for Sinnoh

Copy structure again:
- Starters: Turtwig, Chimchar, Piplup
- Gym leaders: Roark, Gardenia, Maylene, Crasher Wake, Fantina, Byron, Candice, Volkner
- Elite Four: Aaron, Bertha, Flint, Lucian, Cynthia
- Wild Pokemon: Sinnoh species

#### Day 28: Final Testing

**Test Checklist:**

- [ ] Can warp to Johto from Kanto
- [ ] Can receive Johto starter
- [ ] Can battle all 8 Johto gym leaders
- [ ] Can battle Johto Elite Four
- [ ] Wild encounters work in Johto
- [ ] Level scaling works correctly
- [ ] Can warp to Hoenn
- [ ] Can receive Hoenn starter
- [ ] Can battle all 8 Hoenn gym leaders
- [ ] Can battle Hoenn Elite Four
- [ ] Wild encounters work in Hoenn
- [ ] Can warp to Sinnoh
- [ ] Can receive Sinnoh starter
- [ ] Can battle all 8 Sinnoh gym leaders
- [ ] Can battle Sinnoh Elite Four
- [ ] Wild encounters work in Sinnoh
- [ ] No crashes or freezes
- [ ] Badge count updates correctly
- [ ] Region manager tracks current region

## Common Issues and Solutions

### Issue: Map won't compile

**Solution**: Check map.json for syntax errors. Ensure all required fields are present.

### Issue: NPC script doesn't trigger

**Solution**: 
1. Check script name matches in map.json
2. Ensure script is compiled (run `make`)
3. Check NPC is on correct elevation

### Issue: Wild encounters don't work

**Solution**:
1. Verify wild_encounters.json syntax
2. Check encounter rate is > 0
3. Ensure map has grass tiles with correct metatile behavior

### Issue: Trainer battle crashes

**Solution**:
1. Check trainer ID is defined in constants
2. Verify trainer data in trainers.h
3. Ensure all Pokemon species are valid

### Issue: Warp doesn't work

**Solution**:
1. Check destination map ID is correct
2. Verify destination coordinates are valid
3. Ensure destination map exists

## Next Steps After Phase 1

Once Phase 1 is complete, you can:

1. **Polish existing content**
   - Add more trainers to routes
   - Add items to maps
   - Improve map aesthetics

2. **Begin Phase 2**
   - Port additional Kanto maps
   - Add Sevii Islands
   - Expand existing regions

3. **Begin Phase 3**
   - Start porting full Johto from Crossroads
   - Add complete route system
   - Add all original locations

## Resources

- **Porymap Documentation**: https://huderlem.github.io/porymap/
- **Poryscript Documentation**: https://github.com/huderlem/poryscript
- **pokefirered Wiki**: https://github.com/pret/pokefirered/wiki
- **Pokemon Crossroads**: https://github.com/eonlynx/pokecrossroads
- **PokeCommunity Forums**: https://www.pokecommunity.com/

## Getting Help

If you encounter issues:

1. Check this guide's troubleshooting section
2. Review Porymap documentation
3. Search PokeCommunity forums
4. Ask in pokefirered Discord
5. Review Pokemon Crossroads source code

## Conclusion

Following this guide, you should have a functional multi-region system in 3-4 weeks. The simplified hub approach delivers the core experience while keeping development time reasonable.

Good luck with your implementation!
