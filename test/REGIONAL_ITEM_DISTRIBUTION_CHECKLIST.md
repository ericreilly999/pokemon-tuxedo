# Regional Item Distribution Testing Checklist

## Task 4.7: Test Regional Item Distribution

This checklist guides manual testing of item distribution across Kanto and Hoenn regions to verify region-specific items appear only in their appropriate regions.

**Validates Requirements:** 13.2, 13.4

---

## Prerequisites

### Build and Setup
- [ ] Build the ROM: `make firered_modern`
- [ ] Verify ROM builds without errors
- [ ] Load ROM in emulator (mGBA recommended)
- [ ] Have save files for both Kanto and Hoenn testing

### Test Environment
- **Date:** _______________
- **Tester:** _______________
- **ROM Version:** _______________
- **Emulator:** _______________

---

## Section 1: Hoenn-Specific Items (Requirement 13.2)

### 1.1 Hoenn Key Items
These items should ONLY appear in Hoenn region locations.

| Item | Expected Location | Found in Hoenn | NOT in Kanto |
|------|-------------------|----------------|--------------|
| Devon Scope | Route 120 (from Steven) | [ ] | [ ] |
| Go-Goggles | Lavaridge Town (after Flannery) | [ ] | [ ] |
| Magma Emblem | Mt. Pyre | [ ] | [ ] |
| Scanner | Abandoned Ship | [ ] | [ ] |
| Basement Key | Abandoned Ship | [ ] | [ ] |
| Storage Key | Abandoned Ship | [ ] | [ ] |
| Room 1 Key | Abandoned Ship | [ ] | [ ] |
| Room 2 Key | Abandoned Ship | [ ] | [ ] |
| Room 4 Key | Abandoned Ship | [ ] | [ ] |
| Room 6 Key | Abandoned Ship | [ ] | [ ] |
| Devon Parts | Rustboro City (Devon Corp) | [ ] | [ ] |
| Letter | Petalburg City (from Dad) | [ ] | [ ] |
| Meteorite | Meteor Falls / Mt. Chimney | [ ] | [ ] |
| Claw Fossil | Mirage Tower / Desert | [ ] | [ ] |
| Root Fossil | Mirage Tower / Desert | [ ] | [ ] |
| Contest Pass | Slateport City | [ ] | [ ] |
| Wailmer Pail | Route 104 (Pretty Petal Shop) | [ ] | [ ] |
| Soot Sack | Route 113 (Glass Workshop) | [ ] | [ ] |
| Acro Bike | Mauville City (Rydel's Cycles) | [ ] | [ ] |
| Mach Bike | Mauville City (Rydel's Cycles) | [ ] | [ ] |

### 1.2 Hoenn-Specific Held Items
| Item | Expected Location | Found in Hoenn | NOT in Kanto |
|------|-------------------|----------------|--------------|
| Blue Orb | Mt. Pyre | [ ] | [ ] |
| Red Orb | Mt. Pyre | [ ] | [ ] |
| Shoal Salt | Shoal Cave (low tide) | [ ] | [ ] |
| Shoal Shell | Shoal Cave (low tide) | [ ] | [ ] |
| Deep Sea Scale | Wild Clamperl / Shoal Cave | [ ] | [ ] |
| Deep Sea Tooth | Wild Clamperl / Shoal Cave | [ ] | [ ] |

### 1.3 Hoenn Berries (Region-Specific Locations)
| Berry | Expected Location | Found in Hoenn |
|-------|-------------------|----------------|
| Spelon Berry | Route 123 (Berry Master) | [ ] |
| Pamtre Berry | Route 123 (Berry Master) | [ ] |
| Watmel Berry | Route 123 (Berry Master) | [ ] |
| Durin Berry | Route 123 (Berry Master) | [ ] |
| Belue Berry | Route 123 (Berry Master) | [ ] |
| Liechi Berry | Mirage Island | [ ] |
| Ganlon Berry | Mirage Island | [ ] |
| Salac Berry | Mirage Island | [ ] |
| Petaya Berry | Mirage Island | [ ] |
| Apicot Berry | Mirage Island | [ ] |
| Lansat Berry | Battle Frontier (if applicable) | [ ] |
| Starf Berry | Battle Frontier (if applicable) | [ ] |

**Section 1 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 2: TM Distribution - Kanto Region (Requirement 13.4)

### 2.1 Kanto Gym Leader TMs
| TM | Move | Gym Leader | Location | Correct |
|----|------|------------|----------|---------|
| TM34 | Shock Wave | Lt. Surge | Vermilion Gym | [ ] |
| TM19 | Giga Drain | Erika | Celadon Gym | [ ] |
| TM06 | Toxic | Koga | Fuchsia Gym | [ ] |
| TM29 | Psychic | Sabrina | Saffron Gym | [ ] |
| TM38 | Fire Blast | Blaine | Cinnabar Gym | [ ] |
| TM26 | Earthquake | Giovanni | Viridian Gym | [ ] |
| TM04 | Calm Mind | Brock | Pewter Gym | [ ] |
| TM03 | Water Pulse | Misty | Cerulean Gym | [ ] |

### 2.2 Kanto Overworld TMs
| TM | Move | Location | Found |
|----|------|----------|-------|
| TM01 | Focus Punch | Silph Co. | [ ] |
| TM02 | Dragon Claw | Victory Road | [ ] |
| TM05 | Roar | Route 4 | [ ] |
| TM09 | Bullet Seed | Mt. Moon | [ ] |
| TM10 | Hidden Power | Pickup / Game Corner | [ ] |
| TM11 | Sunny Day | Safari Zone | [ ] |
| TM12 | Taunt | Rocket Hideout | [ ] |
| TM13 | Ice Beam | Celadon Game Corner | [ ] |
| TM14 | Blizzard | Pokemon Mansion | [ ] |
| TM15 | Hyper Beam | Celadon Dept Store | [ ] |
| TM16 | Light Screen | Celadon Dept Store | [ ] |
| TM17 | Protect | Power Plant | [ ] |
| TM18 | Rain Dance | Route 15 | [ ] |
| TM20 | Safeguard | Celadon Dept Store | [ ] |
| TM21 | Frustration | Rocket Hideout | [ ] |
| TM22 | Solar Beam | Pokemon Mansion | [ ] |
| TM24 | Thunderbolt | Celadon Game Corner | [ ] |
| TM25 | Thunder | Power Plant | [ ] |
| TM27 | Return | Route 12 | [ ] |
| TM28 | Dig | Cerulean City | [ ] |
| TM30 | Shadow Ball | Celadon Game Corner | [ ] |
| TM31 | Brick Break | Rock Tunnel | [ ] |
| TM32 | Double Team | Celadon Game Corner | [ ] |
| TM33 | Reflect | Celadon Dept Store | [ ] |
| TM35 | Flamethrower | Celadon Game Corner | [ ] |
| TM36 | Sludge Bomb | Rocket Hideout | [ ] |
| TM37 | Sandstorm | Victory Road | [ ] |
| TM39 | Rock Tomb | Pewter Museum | [ ] |
| TM40 | Aerial Ace | Route 9 | [ ] |
| TM41 | Torment | Silph Co. | [ ] |
| TM42 | Facade | Route 16 | [ ] |
| TM43 | Secret Power | Route 25 | [ ] |
| TM44 | Rest | S.S. Anne | [ ] |
| TM45 | Attract | Route 24 | [ ] |
| TM46 | Thief | Mt. Moon | [ ] |
| TM47 | Steel Wing | Safari Zone | [ ] |
| TM48 | Skill Swap | Saffron City | [ ] |
| TM49 | Snatch | Rocket Hideout | [ ] |
| TM50 | Overheat | Victory Road | [ ] |

**Section 2 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 3: TM Distribution - Hoenn Region (Requirement 13.4)

### 3.1 Hoenn Gym Leader TMs
| TM | Move | Gym Leader | Location | Correct |
|----|------|------------|----------|---------|
| TM39 | Rock Tomb | Roxanne | Rustboro Gym | [ ] |
| TM08 | Bulk Up | Brawly | Dewford Gym | [ ] |
| TM34 | Shock Wave | Wattson | Mauville Gym | [ ] |
| TM50 | Overheat | Flannery | Lavaridge Gym | [ ] |
| TM42 | Facade | Norman | Petalburg Gym | [ ] |
| TM40 | Aerial Ace | Winona | Fortree Gym | [ ] |
| TM04 | Calm Mind | Tate & Liza | Mossdeep Gym | [ ] |
| TM03 | Water Pulse | Wallace/Juan | Sootopolis Gym | [ ] |

### 3.2 Hoenn Overworld TMs
| TM | Move | Location | Found |
|----|------|----------|-------|
| TM01 | Focus Punch | Route 115 | [ ] |
| TM02 | Dragon Claw | Meteor Falls | [ ] |
| TM05 | Roar | Route 114 | [ ] |
| TM06 | Toxic | Fiery Path | [ ] |
| TM07 | Hail | Shoal Cave | [ ] |
| TM09 | Bullet Seed | Route 104 | [ ] |
| TM10 | Hidden Power | Slateport / Fortree | [ ] |
| TM11 | Sunny Day | Scorched Slab | [ ] |
| TM12 | Taunt | Trick House | [ ] |
| TM13 | Ice Beam | Mauville Game Corner | [ ] |
| TM14 | Blizzard | Lilycove Dept Store | [ ] |
| TM15 | Hyper Beam | Lilycove Dept Store | [ ] |
| TM16 | Light Screen | Lilycove Dept Store | [ ] |
| TM17 | Protect | Lilycove Dept Store | [ ] |
| TM18 | Rain Dance | Abandoned Ship | [ ] |
| TM19 | Giga Drain | Route 123 | [ ] |
| TM20 | Safeguard | Lilycove Dept Store | [ ] |
| TM21 | Frustration | Pacifidlog Town | [ ] |
| TM22 | Solar Beam | Safari Zone | [ ] |
| TM23 | Iron Tail | Meteor Falls | [ ] |
| TM24 | Thunderbolt | Mauville Game Corner | [ ] |
| TM25 | Thunder | Lilycove Dept Store | [ ] |
| TM26 | Earthquake | Seafloor Cavern | [ ] |
| TM27 | Return | Pacifidlog Town | [ ] |
| TM28 | Dig | Route 114 | [ ] |
| TM29 | Psychic | Victory Road | [ ] |
| TM30 | Shadow Ball | Mt. Pyre | [ ] |
| TM31 | Brick Break | Sootopolis City | [ ] |
| TM32 | Double Team | Mauville Game Corner | [ ] |
| TM33 | Reflect | Lilycove Dept Store | [ ] |
| TM35 | Flamethrower | Mauville Game Corner | [ ] |
| TM36 | Sludge Bomb | Dewford Town | [ ] |
| TM37 | Sandstorm | Route 111 | [ ] |
| TM38 | Fire Blast | Lilycove Dept Store | [ ] |
| TM41 | Torment | Slateport City | [ ] |
| TM43 | Secret Power | Route 111 / Slateport | [ ] |
| TM44 | Rest | Lilycove City | [ ] |
| TM45 | Attract | Verdanturf Town | [ ] |
| TM46 | Thief | Slateport Museum | [ ] |
| TM47 | Steel Wing | Granite Cave | [ ] |
| TM48 | Skill Swap | Mt. Pyre | [ ] |
| TM49 | Snatch | S.S. Tidal | [ ] |

**Section 3 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 4: Regional Mart Inventories

### 4.1 Kanto Mart Items
Verify these items are available in Kanto Poke Marts:

| Item | Mart Location | Available |
|------|---------------|-----------|
| Poke Ball | All Marts | [ ] |
| Great Ball | After 3 badges | [ ] |
| Ultra Ball | After 5 badges | [ ] |
| Potion | All Marts | [ ] |
| Super Potion | After 1 badge | [ ] |
| Hyper Potion | After 4 badges | [ ] |
| Max Potion | After 7 badges | [ ] |
| Full Restore | After 8 badges | [ ] |
| Antidote | All Marts | [ ] |
| Parlyz Heal | All Marts | [ ] |
| Awakening | All Marts | [ ] |
| Burn Heal | All Marts | [ ] |
| Ice Heal | All Marts | [ ] |
| Full Heal | After 5 badges | [ ] |
| Revive | After 4 badges | [ ] |
| Repel | All Marts | [ ] |
| Super Repel | After 3 badges | [ ] |
| Max Repel | After 5 badges | [ ] |
| Escape Rope | All Marts | [ ] |

### 4.2 Hoenn Mart Items
Verify these items are available in Hoenn Poke Marts:

| Item | Mart Location | Available |
|------|---------------|-----------|
| Poke Ball | All Marts | [ ] |
| Great Ball | After 3 badges | [ ] |
| Ultra Ball | After 5 badges | [ ] |
| Net Ball | Mossdeep City | [ ] |
| Dive Ball | Mossdeep City | [ ] |
| Nest Ball | Verdanturf Town | [ ] |
| Repeat Ball | Rustboro City | [ ] |
| Timer Ball | Rustboro City | [ ] |
| Luxury Ball | Lilycove Dept Store | [ ] |
| Premier Ball | (with 10+ Poke Balls) | [ ] |
| Potion | All Marts | [ ] |
| Super Potion | After 1 badge | [ ] |
| Hyper Potion | After 4 badges | [ ] |
| Max Potion | After 7 badges | [ ] |
| Full Restore | After 8 badges | [ ] |
| Lava Cookie | Lavaridge Town | [ ] |
| Moomoo Milk | Lilycove Dept Store | [ ] |
| Harbor Mail | Slateport City | [ ] |
| Glitter Mail | Slateport City | [ ] |
| Mech Mail | Slateport City | [ ] |
| Wood Mail | Slateport City | [ ] |
| Wave Mail | Slateport City | [ ] |
| Bead Mail | Slateport City | [ ] |
| Shadow Mail | Slateport City | [ ] |
| Tropic Mail | Slateport City | [ ] |
| Dream Mail | Slateport City | [ ] |
| Fab Mail | Lilycove Dept Store | [ ] |
| Retro Mail | Lilycove Dept Store | [ ] |

### 4.3 Hoenn-Exclusive Mart Items
These items should ONLY be purchasable in Hoenn:

| Item | Location | In Hoenn | NOT in Kanto |
|------|----------|----------|--------------|
| Net Ball | Mossdeep | [ ] | [ ] |
| Dive Ball | Mossdeep | [ ] | [ ] |
| Nest Ball | Verdanturf | [ ] | [ ] |
| Repeat Ball | Rustboro | [ ] | [ ] |
| Timer Ball | Rustboro | [ ] | [ ] |
| Lava Cookie | Lavaridge | [ ] | [ ] |
| Harbor Mail | Slateport | [ ] | [ ] |
| Glitter Mail | Slateport | [ ] | [ ] |
| Mech Mail | Slateport | [ ] | [ ] |
| Wood Mail | Slateport | [ ] | [ ] |
| Wave Mail | Slateport | [ ] | [ ] |
| Bead Mail | Slateport | [ ] | [ ] |
| Shadow Mail | Slateport | [ ] | [ ] |
| Tropic Mail | Slateport | [ ] | [ ] |
| Dream Mail | Slateport | [ ] | [ ] |
| Fab Mail | Lilycove | [ ] | [ ] |
| Retro Mail | Lilycove | [ ] | [ ] |

**Section 4 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 5: Hidden Items by Region

### 5.1 Kanto Hidden Items (Sample Locations)
| Item | Location | Found |
|------|----------|-------|
| Rare Candy | Route 6 | [ ] |
| Nugget | Route 9 | [ ] |
| PP Up | Route 17 | [ ] |
| Max Elixir | Route 23 | [ ] |
| Full Restore | Victory Road | [ ] |
| Ultra Ball | Silph Co. | [ ] |
| Calcium | Pokemon Mansion | [ ] |
| Carbos | Pokemon Mansion | [ ] |
| Iron | Pokemon Mansion | [ ] |
| Protein | Silph Co. | [ ] |
| HP Up | Silph Co. | [ ] |
| Zinc | Cerulean Cave | [ ] |
| Max Revive | Cerulean Cave | [ ] |
| PP Max | Cerulean Cave | [ ] |

### 5.2 Hoenn Hidden Items (Sample Locations)
| Item | Location | Found |
|------|----------|-------|
| Rare Candy | Route 110 | [ ] |
| Rare Candy | Route 119 | [ ] |
| Rare Candy | Route 123 | [ ] |
| Rare Candy | Shoal Cave | [ ] |
| Nugget | Route 111 | [ ] |
| Nugget | Jagged Pass | [ ] |
| Big Pearl | Route 124 | [ ] |
| Star Piece | Route 134 | [ ] |
| PP Up | Route 115 | [ ] |
| PP Up | Route 123 | [ ] |
| Max Elixir | Route 119 | [ ] |
| Max Elixir | Route 129 | [ ] |
| Full Restore | Route 123 | [ ] |
| Ultra Ball | Route 120 | [ ] |
| Heart Scale | Route 126 | [ ] |
| Heart Scale | Route 128 | [ ] |
| Calcium | Meteor Falls | [ ] |
| Carbos | Route 117 | [ ] |
| Iron | Route 113 | [ ] |
| Protein | Route 114 | [ ] |
| HP Up | Route 111 | [ ] |
| Zinc | Route 120 | [ ] |
| Max Revive | Sky Pillar | [ ] |
| PP Max | Ever Grande City | [ ] |

### 5.3 Hoenn-Specific Hidden Items
These hidden items should ONLY be found in Hoenn:

| Item | Location | In Hoenn | NOT in Kanto |
|------|----------|----------|--------------|
| Heart Scale | Various underwater | [ ] | [ ] |
| Shoal Salt | Shoal Cave | [ ] | [ ] |
| Shoal Shell | Shoal Cave | [ ] | [ ] |
| Red Shard | Various routes | [ ] | [ ] |
| Blue Shard | Various routes | [ ] | [ ] |
| Yellow Shard | Various routes | [ ] | [ ] |
| Green Shard | Various routes | [ ] | [ ] |

**Section 5 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 6: Cross-Region Item Verification

### 6.1 Items That Should Appear in BOTH Regions
| Item | In Kanto | In Hoenn |
|------|----------|----------|
| Poke Ball | [ ] | [ ] |
| Great Ball | [ ] | [ ] |
| Ultra Ball | [ ] | [ ] |
| Master Ball | [ ] | [ ] |
| Potion | [ ] | [ ] |
| Super Potion | [ ] | [ ] |
| Hyper Potion | [ ] | [ ] |
| Max Potion | [ ] | [ ] |
| Full Restore | [ ] | [ ] |
| Revive | [ ] | [ ] |
| Max Revive | [ ] | [ ] |
| Rare Candy | [ ] | [ ] |
| PP Up | [ ] | [ ] |
| PP Max | [ ] | [ ] |
| Ether | [ ] | [ ] |
| Max Ether | [ ] | [ ] |
| Elixir | [ ] | [ ] |
| Max Elixir | [ ] | [ ] |
| Nugget | [ ] | [ ] |
| Big Pearl | [ ] | [ ] |
| Star Piece | [ ] | [ ] |
| Calcium | [ ] | [ ] |
| Carbos | [ ] | [ ] |
| Iron | [ ] | [ ] |
| Protein | [ ] | [ ] |
| HP Up | [ ] | [ ] |
| Zinc | [ ] | [ ] |
| Escape Rope | [ ] | [ ] |
| Repel | [ ] | [ ] |
| Super Repel | [ ] | [ ] |
| Max Repel | [ ] | [ ] |

### 6.2 TMs Available in Both Regions
Verify these TMs can be obtained in BOTH regions (different locations):

| TM | Move | Kanto Location | Hoenn Location | Both Work |
|----|------|----------------|----------------|-----------|
| TM03 | Water Pulse | Cerulean Gym | Sootopolis Gym | [ ] |
| TM04 | Calm Mind | Pewter Gym | Mossdeep Gym | [ ] |
| TM34 | Shock Wave | Vermilion Gym | Mauville Gym | [ ] |
| TM39 | Rock Tomb | Pewter Museum | Rustboro Gym | [ ] |
| TM40 | Aerial Ace | Route 9 | Fortree Gym | [ ] |
| TM42 | Facade | Route 16 | Petalburg Gym | [ ] |

**Section 6 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Summary

### Overall Test Results

| Section | Status | Critical Issues |
|---------|--------|-----------------|
| 1. Hoenn-Specific Items | [ ] PASS / [ ] FAIL | |
| 2. TM Distribution - Kanto | [ ] PASS / [ ] FAIL | |
| 3. TM Distribution - Hoenn | [ ] PASS / [ ] FAIL | |
| 4. Regional Mart Inventories | [ ] PASS / [ ] FAIL | |
| 5. Hidden Items by Region | [ ] PASS / [ ] FAIL | |
| 6. Cross-Region Verification | [ ] PASS / [ ] FAIL | |

### Final Verdict
- [ ] **PASS** - Regional item distribution is correct
- [ ] **FAIL** - Issues found (see below)

### Critical Issues Found
1. _______________
2. _______________
3. _______________

### Minor Issues Found
1. _______________
2. _______________
3. _______________

### Recommendations
_______________

---

## Appendix: Debug Commands

### Item Commands
```
# Give specific TM for testing
giveitem ITEM_TM01
giveitem ITEM_TM50

# Give Hoenn-specific items
giveitem ITEM_DEVON_SCOPE
giveitem ITEM_GO_GOGGLES
giveitem ITEM_ACRO_BIKE
giveitem ITEM_MACH_BIKE

# Give region tickets
giveitem ITEM_HOENN_TICKET
giveitem ITEM_KANTO_TICKET
```

### Warp Commands
```
# Warp to Kanto TM locations
warp MAP_CELADON_CITY_GAME_CORNER
warp MAP_CELADON_CITY_DEPARTMENT_STORE_5F
warp MAP_SILPH_CO_11F

# Warp to Hoenn TM locations
warp MAP_MAUVILLE_CITY_GAME_CORNER
warp MAP_LILYCOVE_CITY_DEPARTMENT_STORE_4F
warp MAP_METEOR_FALLS_1F_1R
```

### Flag Commands
```
# Set badge flags for mart inventory testing
setflag FLAG_BADGE01_GET
setflag FLAG_BADGE02_GET
# ... etc
```

---

**Document Version:** 1.0
**Created:** Task 4.7 - Test regional item distribution
**Last Updated:** _______________
