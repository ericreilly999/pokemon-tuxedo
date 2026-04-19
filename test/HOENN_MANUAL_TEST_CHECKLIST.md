# Hoenn Region Manual Testing Checklist

## Task 4.5: Perform Manual Testing of Hoenn Region

This comprehensive checklist guides a human tester through a complete Hoenn region playthrough to validate all major systems and content.

**Validates Requirements:** 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9

---

## Prerequisites

### Build and Setup
- [ ] Build the ROM: `make firered_modern`
- [ ] Verify ROM builds without errors
- [ ] Load ROM in emulator (mGBA recommended)
- [ ] Have a save file with Kanto Elite Four defeated (to unlock Hoenn)
- [ ] Verify Hoenn Ticket is in inventory

### Test Environment
- **Date:** _______________
- **Tester:** _______________
- **ROM Version:** _______________
- **Emulator:** _______________
- **Save State Used:** _______________

---

## Section 1: Region Accessibility (via Ticket from Kanto)

### 1.1 Hoenn Ticket Usage
- [ ] Hoenn Ticket appears in Key Items pocket
- [ ] Using Hoenn Ticket triggers region transition
- [ ] Transition animation plays correctly
- [ ] Player arrives at Littleroot Town
- [ ] Region_Manager correctly updates to Hoenn (region_id = 2)
- [ ] Party Pokemon preserved during transition
- [ ] Inventory preserved during transition

### 1.2 Return to Kanto
- [ ] Can use Kanto Ticket to return to Kanto
- [ ] Can return to Hoenn after visiting Kanto
- [ ] Progress in Hoenn is preserved

**Section 1 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 2: Hoenn Routes (Requirements 3.1)

### 2.1 Early Routes (Routes 101-110)
| Route | Map Loads | Connections Work | Wild Encounters | Trainers | Items |
|-------|-----------|------------------|-----------------|----------|-------|
| Route 101 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 102 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 103 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 104 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 105 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 106 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 107 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 108 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 109 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 110 | [ ] | [ ] | [ ] | [ ] | [ ] |

### 2.2 Mid Routes (Routes 111-120)
| Route | Map Loads | Connections Work | Wild Encounters | Trainers | Items |
|-------|-----------|------------------|-----------------|----------|-------|
| Route 111 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 112 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 113 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 114 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 115 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 116 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 117 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 118 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 119 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 120 | [ ] | [ ] | [ ] | [ ] | [ ] |

### 2.3 Late Routes (Routes 121-134)
| Route | Map Loads | Connections Work | Wild Encounters | Trainers | Items |
|-------|-----------|------------------|-----------------|----------|-------|
| Route 121 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 122 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 123 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 124 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 125 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 126 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 127 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 128 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 129 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 130 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 131 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 132 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 133 | [ ] | [ ] | [ ] | [ ] | [ ] |
| Route 134 | [ ] | [ ] | [ ] | [ ] | [ ] |

**Section 2 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 3: Hoenn Cities and Towns (Requirements 3.2)

### 3.1 City/Town Checklist
| Location | Map Renders | Buildings Accessible | NPCs Work | Pokemon Center | Mart |
|----------|-------------|---------------------|-----------|----------------|------|
| Littleroot Town | [ ] | [ ] | [ ] | N/A | N/A |
| Oldale Town | [ ] | [ ] | [ ] | [ ] | [ ] |
| Petalburg City | [ ] | [ ] | [ ] | [ ] | [ ] |
| Rustboro City | [ ] | [ ] | [ ] | [ ] | [ ] |
| Dewford Town | [ ] | [ ] | [ ] | [ ] | [ ] |
| Slateport City | [ ] | [ ] | [ ] | [ ] | [ ] |
| Mauville City | [ ] | [ ] | [ ] | [ ] | [ ] |
| Verdanturf Town | [ ] | [ ] | [ ] | [ ] | [ ] |
| Fallarbor Town | [ ] | [ ] | [ ] | [ ] | [ ] |
| Lavaridge Town | [ ] | [ ] | [ ] | [ ] | [ ] |
| Fortree City | [ ] | [ ] | [ ] | [ ] | [ ] |
| Lilycove City | [ ] | [ ] | [ ] | [ ] | [ ] |
| Mossdeep City | [ ] | [ ] | [ ] | [ ] | [ ] |
| Sootopolis City | [ ] | [ ] | [ ] | [ ] | [ ] |
| Pacifidlog Town | [ ] | [ ] | [ ] | [ ] | [ ] |
| Ever Grande City | [ ] | [ ] | [ ] | [ ] | N/A |

### 3.2 Special City Features
- [ ] Slateport Market functions
- [ ] Mauville Game Corner works
- [ ] Lilycove Department Store accessible (all floors)
- [ ] Mossdeep Space Center accessible
- [ ] Sootopolis Gym water puzzle works

**Section 3 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 4: Hoenn Caves and Dungeons (Requirements 3.3)

### 4.1 Dungeon Checklist
| Location | All Floors Accessible | Wild Encounters | Items | Scripts Work |
|----------|----------------------|-----------------|-------|--------------|
| Petalburg Woods | [ ] | [ ] | [ ] | [ ] |
| Granite Cave (all floors) | [ ] | [ ] | [ ] | [ ] |
| Rusturf Tunnel | [ ] | [ ] | [ ] | [ ] |
| Meteor Falls (all areas) | [ ] | [ ] | [ ] | [ ] |
| Mt. Chimney | [ ] | [ ] | [ ] | [ ] |
| Jagged Pass | [ ] | [ ] | [ ] | [ ] |
| Fiery Path | [ ] | [ ] | [ ] | [ ] |
| Mt. Pyre (all floors) | [ ] | [ ] | [ ] | [ ] |
| Shoal Cave (high/low tide) | [ ] | [ ] | [ ] | [ ] |
| Cave of Origin | [ ] | [ ] | [ ] | [ ] |
| Seafloor Cavern (all rooms) | [ ] | [ ] | [ ] | [ ] |
| Sky Pillar (all floors) | [ ] | [ ] | [ ] | [ ] |
| Victory Road (all floors) | [ ] | [ ] | [ ] | [ ] |

### 4.2 Team Hideouts
| Location | Accessible | Puzzles Work | Trainers | Boss Battle |
|----------|------------|--------------|----------|-------------|
| Team Aqua Hideout | [ ] | [ ] | [ ] | [ ] |
| Team Magma Hideout | [ ] | [ ] | [ ] | [ ] |

**Section 4 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 5: Gym Leader Battles (Requirements 3.4)

### 5.1 Gym Leader Checklist
| Gym Leader | City | Type | Gym Accessible | Puzzle Works | Battle Triggers | Team Correct | Badge Awarded |
|------------|------|------|----------------|--------------|-----------------|--------------|---------------|
| Roxanne | Rustboro | Rock | [ ] | [ ] | [ ] | [ ] | [ ] |
| Brawly | Dewford | Fighting | [ ] | [ ] | [ ] | [ ] | [ ] |
| Wattson | Mauville | Electric | [ ] | [ ] | [ ] | [ ] | [ ] |
| Flannery | Lavaridge | Fire | [ ] | [ ] | [ ] | [ ] | [ ] |
| Norman | Petalburg | Normal | [ ] | [ ] | [ ] | [ ] | [ ] |
| Winona | Fortree | Flying | [ ] | [ ] | [ ] | [ ] | [ ] |
| Tate & Liza | Mossdeep | Psychic | [ ] | [ ] | [ ] | [ ] | [ ] |
| Wallace/Juan | Sootopolis | Water | [ ] | [ ] | [ ] | [ ] | [ ] |

### 5.2 Badge Verification
- [ ] Stone Badge registered in Hoenn badge slot 1
- [ ] Knuckle Badge registered in Hoenn badge slot 2
- [ ] Dynamo Badge registered in Hoenn badge slot 3
- [ ] Heat Badge registered in Hoenn badge slot 4
- [ ] Balance Badge registered in Hoenn badge slot 5
- [ ] Feather Badge registered in Hoenn badge slot 6
- [ ] Mind Badge registered in Hoenn badge slot 7
- [ ] Rain Badge registered in Hoenn badge slot 8
- [ ] All 8 Hoenn badges tracked separately from Kanto badges
- [ ] GetRegionBadgeCount(REGION_HOENN) returns correct count

**Section 5 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 6: Elite Four and Champion (Requirements 3.5, 3.6)

### 6.1 Elite Four Access
- [ ] Victory Road accessible with 8 Hoenn badges
- [ ] Pokemon League entrance works
- [ ] Can heal at Pokemon Center before Elite Four

### 6.2 Elite Four Battles
| Member | Type | Battle Triggers | Team Correct | AI Works | Rewards Given |
|--------|------|-----------------|--------------|----------|---------------|
| Sidney | Dark | [ ] | [ ] | [ ] | [ ] |
| Phoebe | Ghost | [ ] | [ ] | [ ] | [ ] |
| Glacia | Ice | [ ] | [ ] | [ ] | [ ] |
| Drake | Dragon | [ ] | [ ] | [ ] | [ ] |

### 6.3 Champion Battle
| Champion | Battle Triggers | Team Correct | AI Works | Victory Script |
|----------|-----------------|--------------|----------|----------------|
| Steven/Wallace | [ ] | [ ] | [ ] | [ ] |

### 6.4 Post-Victory
- [ ] Hall of Fame sequence plays
- [ ] Credits roll (or appropriate ending)
- [ ] Hoenn Elite Four marked as defeated
- [ ] Next region unlock triggered (Johto ticket awarded if applicable)

**Section 6 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 7: Wild Encounters (Requirements 3.7)

### 7.1 Encounter Types
| Type | Locations Tested | Pokemon Appear | Levels Correct | Scaled Properly |
|------|------------------|----------------|----------------|-----------------|
| Grass | [ ] | [ ] | [ ] | [ ] |
| Water (Surfing) | [ ] | [ ] | [ ] | [ ] |
| Fishing (Old Rod) | [ ] | [ ] | [ ] | [ ] |
| Fishing (Good Rod) | [ ] | [ ] | [ ] | [ ] |
| Fishing (Super Rod) | [ ] | [ ] | [ ] | [ ] |
| Rock Smash | [ ] | [ ] | [ ] | [ ] |
| Cave | [ ] | [ ] | [ ] | [ ] |

### 7.2 Hoenn-Specific Pokemon
- [ ] Hoenn starters available (Treecko, Torchic, Mudkip)
- [ ] Hoenn-exclusive Pokemon appear in appropriate locations
- [ ] Regional variants appear correctly (if applicable)

**Section 7 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 8: Event Scripts (Requirements 3.8)

### 8.1 Major Story Events
| Event | Triggers Correctly | Completes Properly | No Softlocks |
|-------|-------------------|-------------------|--------------|
| Starter selection (if applicable) | [ ] | [ ] | [ ] |
| Devon Goods delivery | [ ] | [ ] | [ ] |
| Petalburg Woods Team encounter | [ ] | [ ] | [ ] |
| Slateport Museum event | [ ] | [ ] | [ ] |
| Mt. Chimney Team battle | [ ] | [ ] | [ ] |
| Weather Institute event | [ ] | [ ] | [ ] |
| Lilycove Team encounter | [ ] | [ ] | [ ] |
| Mt. Pyre orb theft | [ ] | [ ] | [ ] |
| Seafloor Cavern legendary event | [ ] | [ ] | [ ] |
| Sootopolis legendary event | [ ] | [ ] | [ ] |
| Sky Pillar Rayquaza event | [ ] | [ ] | [ ] |

### 8.2 Side Events
| Event | Works Correctly |
|-------|-----------------|
| Berry Master | [ ] |
| Trick House | [ ] |
| Safari Zone | [ ] |
| Contest Hall | [ ] |
| Battle Frontier (if included) | [ ] |
| Secret Bases | [ ] |

**Section 8 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 9: Map Connections and Warps

### 9.1 Connection Testing
- [ ] All route-to-route connections work
- [ ] All route-to-city connections work
- [ ] All city-to-building warps work
- [ ] All cave entrance/exit warps work
- [ ] No invisible walls blocking paths
- [ ] No out-of-bounds areas accessible

### 9.2 Special Warps
- [ ] Ferry system works (Slateport ↔ Dewford ↔ Lilycove)
- [ ] Dive spots work correctly
- [ ] Underwater areas accessible
- [ ] Cable car works (Route 112 ↔ Mt. Chimney)

**Section 9 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 10: NPC Interactions

### 10.1 General NPCs
- [ ] NPCs display correct sprites
- [ ] NPC dialogue displays correctly
- [ ] Movement scripts work (walking NPCs)
- [ ] Facing direction correct when talked to

### 10.2 Functional NPCs
| NPC Type | Functions Correctly |
|----------|---------------------|
| Pokemon Center Nurse | [ ] |
| Mart Clerks | [ ] |
| Move Tutors | [ ] |
| Name Rater | [ ] |
| Move Deleter | [ ] |
| Day Care | [ ] |
| PC Storage | [ ] |

**Section 10 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 11: Fly System Integration

### 11.1 Fly Location Registration
- [ ] Visiting cities registers fly locations
- [ ] Fly locations stored in Hoenn-specific array
- [ ] Fly menu shows only Hoenn locations when in Hoenn

### 11.2 Fly Functionality
- [ ] Can fly to registered Hoenn locations
- [ ] Cannot fly to unvisited locations
- [ ] Fly animation works correctly

**Section 11 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 12: Save/Load Testing

### 12.1 Save Points
- [ ] Save in Littleroot Town, load correctly
- [ ] Save in Pokemon Center, load correctly
- [ ] Save in dungeon, load correctly
- [ ] Save before Gym battle, load correctly
- [ ] Save before Elite Four, load correctly

### 12.2 Data Persistence
- [ ] Hoenn badges persist after save/load
- [ ] Hoenn progress flags persist
- [ ] Fly locations persist
- [ ] Party Pokemon persist
- [ ] Inventory persists

**Section 12 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Summary

### Overall Test Results

| Section | Status | Critical Issues |
|---------|--------|-----------------|
| 1. Region Accessibility | [ ] PASS / [ ] FAIL | |
| 2. Hoenn Routes | [ ] PASS / [ ] FAIL | |
| 3. Hoenn Cities | [ ] PASS / [ ] FAIL | |
| 4. Caves/Dungeons | [ ] PASS / [ ] FAIL | |
| 5. Gym Leaders | [ ] PASS / [ ] FAIL | |
| 6. Elite Four/Champion | [ ] PASS / [ ] FAIL | |
| 7. Wild Encounters | [ ] PASS / [ ] FAIL | |
| 8. Event Scripts | [ ] PASS / [ ] FAIL | |
| 9. Map Connections | [ ] PASS / [ ] FAIL | |
| 10. NPC Interactions | [ ] PASS / [ ] FAIL | |
| 11. Fly System | [ ] PASS / [ ] FAIL | |
| 12. Save/Load | [ ] PASS / [ ] FAIL | |

### Final Verdict
- [ ] **PASS** - Hoenn region is fully functional
- [ ] **FAIL** - Critical issues found (see below)

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

### Warp Commands
```
# Warp to Hoenn starting location
warp MAP_LITTLEROOT_TOWN

# Warp to specific gyms
warp MAP_RUSTBORO_CITY_GYM
warp MAP_DEWFORD_TOWN_GYM
warp MAP_MAUVILLE_CITY_GYM
warp MAP_LAVARIDGE_TOWN_GYM
warp MAP_PETALBURG_CITY_GYM
warp MAP_FORTREE_CITY_GYM
warp MAP_MOSSDEEP_CITY_GYM
warp MAP_SOOTOPOLIS_CITY_GYM

# Warp to Elite Four
warp MAP_EVER_GRANDE_CITY_POKEMON_LEAGUE_1F
```

### Badge Commands
```
# Give all Hoenn badges (for testing Elite Four access)
setflag FLAG_BADGE01_GET through FLAG_BADGE08_GET (Hoenn-specific flags)
```

### Item Commands
```
# Give Hoenn Ticket
giveitem ITEM_HOENN_TICKET
```

---

**Document Version:** 1.0
**Created:** Task 4.5 - Perform manual testing of Hoenn region
**Last Updated:** _______________
