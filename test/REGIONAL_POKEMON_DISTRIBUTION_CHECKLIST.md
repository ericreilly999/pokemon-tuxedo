# Regional Pokemon Distribution Testing Checklist

## Task 4.8: Test Regional Pokemon Distribution

This checklist guides manual testing of Pokemon distribution across Kanto and Hoenn regions to verify region-exclusive Pokemon appear only in their appropriate regions and cross-regional Pokemon appear in appropriate locations.

**Validates Requirements:** 20.2, 20.5

---

## Prerequisites

### Build and Setup
- [ ] Build the ROM: `make firered_modern`
- [ ] Verify ROM builds without errors
- [ ] Load ROM in emulator (mGBA recommended)
- [ ] Have save files for both Kanto and Hoenn testing
- [ ] Have access to wild encounter debug tools or patience for random encounters

### Test Environment
- **Date:** _______________
- **Tester:** _______________
- **ROM Version:** _______________
- **Emulator:** _______________

---

## Section 1: Hoenn-Exclusive Pokemon (Requirement 20.2)

### 1.1 Hoenn Starter Pokemon
These Pokemon should ONLY be obtainable in Hoenn region.

| Pokemon | Expected Location | Found in Hoenn | NOT in Kanto Wild |
|---------|-------------------|----------------|-------------------|
| Treecko | Littleroot Town (starter) | [ ] | [ ] |
| Grovyle | Evolution only | [ ] | [ ] |
| Sceptile | Evolution only | [ ] | [ ] |
| Torchic | Littleroot Town (starter) | [ ] | [ ] |
| Combusken | Evolution only | [ ] | [ ] |
| Blaziken | Evolution only | [ ] | [ ] |
| Mudkip | Littleroot Town (starter) | [ ] | [ ] |
| Marshtomp | Evolution only | [ ] | [ ] |
| Swampert | Evolution only | [ ] | [ ] |

### 1.2 Hoenn Legendary Pokemon
These Pokemon should ONLY appear in Hoenn region locations.

| Pokemon | Expected Location | Found in Hoenn | NOT in Kanto |
|---------|-------------------|----------------|--------------|
| Kyogre | Cave of Origin / Seafloor Cavern | [ ] | [ ] |
| Groudon | Cave of Origin / Terra Cave | [ ] | [ ] |
| Rayquaza | Sky Pillar | [ ] | [ ] |
| Latias | Roaming Hoenn (Southern Island) | [ ] | [ ] |
| Latios | Roaming Hoenn (Southern Island) | [ ] | [ ] |
| Regirock | Desert Ruins | [ ] | [ ] |
| Regice | Island Cave | [ ] | [ ] |
| Registeel | Ancient Tomb | [ ] | [ ] |
| Jirachi | Event only (if applicable) | [ ] | [ ] |
| Deoxys | Birth Island (if applicable) | [ ] | [ ] |

### 1.3 Hoenn Regional Pokemon (Wild Encounters)
These Pokemon should appear in wild encounters ONLY in Hoenn.

| Pokemon | Expected Hoenn Location | Found in Hoenn | NOT in Kanto Wild |
|---------|-------------------------|----------------|-------------------|
| Poochyena | Routes 101, 102, 103 | [ ] | [ ] |
| Mightyena | Routes 120, 121 | [ ] | [ ] |
| Zigzagoon | Routes 101, 102, 103, 104 | [ ] | [ ] |
| Linoone | Routes 118, 119, 120, 121 | [ ] | [ ] |
| Wurmple | Routes 101, 102, Petalburg Woods | [ ] | [ ] |
| Silcoon | Petalburg Woods | [ ] | [ ] |
| Beautifly | Evolution only | [ ] | [ ] |
| Cascoon | Petalburg Woods | [ ] | [ ] |
| Dustox | Evolution only | [ ] | [ ] |
| Lotad | Routes 102, 114 | [ ] | [ ] |
| Lombre | Evolution only | [ ] | [ ] |
| Ludicolo | Evolution only | [ ] | [ ] |
| Seedot | Routes 102, 114 | [ ] | [ ] |
| Nuzleaf | Evolution only | [ ] | [ ] |
| Shiftry | Evolution only | [ ] | [ ] |
| Taillow | Routes 104, 115, 116 | [ ] | [ ] |
| Swellow | Routes 115, 118 | [ ] | [ ] |
| Wingull | Routes 103, 104, 105-109 | [ ] | [ ] |
| Pelipper | Routes 119, 121-123 | [ ] | [ ] |
| Ralts | Route 102 | [ ] | [ ] |
| Kirlia | Evolution only | [ ] | [ ] |
| Gardevoir | Evolution only | [ ] | [ ] |
| Surskit | Routes 102, 111, 114, 117, 120 | [ ] | [ ] |
| Masquerain | Evolution only | [ ] | [ ] |
| Shroomish | Petalburg Woods | [ ] | [ ] |
| Breloom | Evolution only | [ ] | [ ] |
| Slakoth | Petalburg Woods | [ ] | [ ] |
| Vigoroth | Evolution only | [ ] | [ ] |
| Slaking | Evolution only | [ ] | [ ] |
| Nincada | Route 116 | [ ] | [ ] |
| Ninjask | Evolution only | [ ] | [ ] |
| Shedinja | Evolution only (special) | [ ] | [ ] |
| Whismur | Rusturf Tunnel, Victory Road | [ ] | [ ] |
| Loudred | Victory Road | [ ] | [ ] |
| Exploud | Evolution only | [ ] | [ ] |
| Makuhita | Granite Cave | [ ] | [ ] |
| Hariyama | Victory Road | [ ] | [ ] |
| Azurill | Breeding only | [ ] | [ ] |
| Nosepass | Granite Cave | [ ] | [ ] |
| Skitty | Route 116 | [ ] | [ ] |
| Delcatty | Evolution only | [ ] | [ ] |
| Sableye | Granite Cave, Cave of Origin | [ ] | [ ] |
| Mawile | Granite Cave, Cave of Origin | [ ] | [ ] |
| Aron | Granite Cave | [ ] | [ ] |
| Lairon | Victory Road | [ ] | [ ] |
| Aggron | Evolution only | [ ] | [ ] |
| Meditite | Mt. Pyre, Victory Road | [ ] | [ ] |
| Medicham | Victory Road | [ ] | [ ] |
| Electrike | Routes 110, 118 | [ ] | [ ] |
| Manectric | Route 118 | [ ] | [ ] |
| Plusle | Route 110 | [ ] | [ ] |
| Minun | Route 110 | [ ] | [ ] |
| Volbeat | Route 117 | [ ] | [ ] |
| Illumise | Route 117 | [ ] | [ ] |
| Roselia | Route 117 | [ ] | [ ] |
| Gulpin | Route 110 | [ ] | [ ] |
| Swalot | Evolution only | [ ] | [ ] |
| Carvanha | Routes 118, 119 (fishing) | [ ] | [ ] |
| Sharpedo | Routes 118, 119 (fishing) | [ ] | [ ] |
| Wailmer | Routes 103, 105-110 (fishing) | [ ] | [ ] |
| Wailord | Route 129 (rare) | [ ] | [ ] |
| Numel | Route 112, Fiery Path | [ ] | [ ] |
| Camerupt | Evolution only | [ ] | [ ] |
| Torkoal | Fiery Path | [ ] | [ ] |
| Spoink | Jagged Pass | [ ] | [ ] |
| Grumpig | Evolution only | [ ] | [ ] |
| Spinda | Route 113 | [ ] | [ ] |
| Trapinch | Route 111 (desert) | [ ] | [ ] |
| Vibrava | Evolution only | [ ] | [ ] |
| Flygon | Evolution only | [ ] | [ ] |
| Cacnea | Route 111 (desert) | [ ] | [ ] |
| Cacturne | Evolution only | [ ] | [ ] |
| Swablu | Route 114, 115 | [ ] | [ ] |
| Altaria | Sky Pillar | [ ] | [ ] |
| Zangoose | Route 114 | [ ] | [ ] |
| Seviper | Route 114 | [ ] | [ ] |
| Lunatone | Meteor Falls | [ ] | [ ] |
| Solrock | Meteor Falls | [ ] | [ ] |
| Barboach | Routes 111, 114 (fishing) | [ ] | [ ] |
| Whiscash | Meteor Falls (fishing) | [ ] | [ ] |
| Corphish | Routes 102, 117 (fishing) | [ ] | [ ] |
| Crawdaunt | Evolution only | [ ] | [ ] |
| Baltoy | Route 111 (desert) | [ ] | [ ] |
| Claydol | Sky Pillar | [ ] | [ ] |
| Lileep | Fossil revival | [ ] | [ ] |
| Cradily | Evolution only | [ ] | [ ] |
| Anorith | Fossil revival | [ ] | [ ] |
| Armaldo | Evolution only | [ ] | [ ] |
| Feebas | Route 119 (rare fishing) | [ ] | [ ] |
| Milotic | Evolution only | [ ] | [ ] |
| Castform | Weather Institute (gift) | [ ] | [ ] |
| Kecleon | Routes 118, 119, 120, 121 | [ ] | [ ] |
| Shuppet | Mt. Pyre | [ ] | [ ] |
| Banette | Sky Pillar | [ ] | [ ] |
| Duskull | Mt. Pyre | [ ] | [ ] |
| Dusclops | Sky Pillar | [ ] | [ ] |
| Tropius | Route 119 | [ ] | [ ] |
| Chimecho | Mt. Pyre | [ ] | [ ] |
| Absol | Route 120 | [ ] | [ ] |
| Wynaut | Mirage Island / Breeding | [ ] | [ ] |
| Snorunt | Shoal Cave | [ ] | [ ] |
| Glalie | Evolution only | [ ] | [ ] |
| Spheal | Shoal Cave | [ ] | [ ] |
| Sealeo | Evolution only | [ ] | [ ] |
| Walrein | Evolution only | [ ] | [ ] |
| Clamperl | Underwater routes | [ ] | [ ] |
| Huntail | Evolution only | [ ] | [ ] |
| Gorebyss | Evolution only | [ ] | [ ] |
| Relicanth | Underwater routes | [ ] | [ ] |
| Luvdisc | Route 128 (fishing) | [ ] | [ ] |
| Bagon | Meteor Falls (deep) | [ ] | [ ] |
| Shelgon | Evolution only | [ ] | [ ] |
| Salamence | Evolution only | [ ] | [ ] |
| Beldum | Steven's house (gift) | [ ] | [ ] |
| Metang | Evolution only | [ ] | [ ] |
| Metagross | Evolution only | [ ] | [ ] |

**Section 1 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 2: Kanto-Exclusive Pokemon

### 2.1 Kanto Starter Pokemon
These Pokemon should ONLY be obtainable in Kanto region.

| Pokemon | Expected Location | Found in Kanto | NOT in Hoenn Wild |
|---------|-------------------|----------------|-------------------|
| Bulbasaur | Pallet Town (starter) | [ ] | [ ] |
| Ivysaur | Evolution only | [ ] | [ ] |
| Venusaur | Evolution only | [ ] | [ ] |
| Charmander | Pallet Town (starter) | [ ] | [ ] |
| Charmeleon | Evolution only | [ ] | [ ] |
| Charizard | Evolution only | [ ] | [ ] |
| Squirtle | Pallet Town (starter) | [ ] | [ ] |
| Wartortle | Evolution only | [ ] | [ ] |
| Blastoise | Evolution only | [ ] | [ ] |

### 2.2 Kanto Legendary Pokemon
These Pokemon should ONLY appear in Kanto region locations.

| Pokemon | Expected Location | Found in Kanto | NOT in Hoenn |
|---------|-------------------|----------------|--------------|
| Articuno | Seafoam Islands | [ ] | [ ] |
| Zapdos | Power Plant | [ ] | [ ] |
| Moltres | Mt. Ember | [ ] | [ ] |
| Mewtwo | Cerulean Cave | [ ] | [ ] |
| Mew | Event only (if applicable) | [ ] | [ ] |

### 2.3 Kanto Regional Pokemon (Wild Encounters)
These Pokemon should appear in wild encounters ONLY in Kanto.

| Pokemon | Expected Kanto Location | Found in Kanto | NOT in Hoenn Wild |
|---------|-------------------------|----------------|-------------------|
| Rattata | Routes 1, 2, 22 | [ ] | [ ] |
| Raticate | Routes 16, 17, 18 | [ ] | [ ] |
| Spearow | Routes 3, 4, 9, 10 | [ ] | [ ] |
| Fearow | Routes 17, 18, 23 | [ ] | [ ] |
| Ekans | Routes 4, 8, 9, 10, 11 | [ ] | [ ] |
| Arbok | Routes 23, Victory Road | [ ] | [ ] |
| Nidoran♀ | Routes 3, Safari Zone | [ ] | [ ] |
| Nidorina | Safari Zone | [ ] | [ ] |
| Nidoqueen | Evolution only | [ ] | [ ] |
| Nidoran♂ | Routes 3, Safari Zone | [ ] | [ ] |
| Nidorino | Safari Zone | [ ] | [ ] |
| Nidoking | Evolution only | [ ] | [ ] |
| Clefairy | Mt. Moon | [ ] | [ ] |
| Clefable | Evolution only | [ ] | [ ] |
| Vulpix | Routes 7, 8, Pokemon Mansion | [ ] | [ ] |
| Ninetales | Evolution only | [ ] | [ ] |
| Jigglypuff | Route 3 | [ ] | [ ] |
| Wigglytuff | Evolution only | [ ] | [ ] |
| Paras | Mt. Moon, Safari Zone | [ ] | [ ] |
| Parasect | Safari Zone | [ ] | [ ] |
| Venonat | Routes 12, 13, 14, 15 | [ ] | [ ] |
| Venomoth | Safari Zone | [ ] | [ ] |
| Diglett | Diglett's Cave | [ ] | [ ] |
| Dugtrio | Diglett's Cave | [ ] | [ ] |
| Meowth | Routes 5, 6, 7, 8 | [ ] | [ ] |
| Persian | Evolution only | [ ] | [ ] |
| Mankey | Routes 3, 4, Rock Tunnel | [ ] | [ ] |
| Primeape | Routes 23, Victory Road | [ ] | [ ] |
| Growlithe | Routes 7, 8, Pokemon Mansion | [ ] | [ ] |
| Arcanine | Evolution only | [ ] | [ ] |
| Bellsprout | Routes 5, 6, 7 | [ ] | [ ] |
| Weepinbell | Routes 12, 13, 14, 15 | [ ] | [ ] |
| Victreebel | Evolution only | [ ] | [ ] |
| Ponyta | Pokemon Mansion | [ ] | [ ] |
| Rapidash | Pokemon Mansion | [ ] | [ ] |
| Slowpoke | Seafoam Islands | [ ] | [ ] |
| Slowbro | Seafoam Islands | [ ] | [ ] |
| Farfetch'd | Trade only | [ ] | [ ] |
| Doduo | Safari Zone | [ ] | [ ] |
| Dodrio | Safari Zone | [ ] | [ ] |
| Seel | Seafoam Islands | [ ] | [ ] |
| Dewgong | Seafoam Islands | [ ] | [ ] |
| Grimer | Pokemon Mansion | [ ] | [ ] |
| Muk | Pokemon Mansion | [ ] | [ ] |
| Shellder | Seafoam Islands (fishing) | [ ] | [ ] |
| Cloyster | Evolution only | [ ] | [ ] |
| Gastly | Pokemon Tower | [ ] | [ ] |
| Haunter | Pokemon Tower | [ ] | [ ] |
| Gengar | Trade evolution | [ ] | [ ] |
| Onix | Rock Tunnel, Victory Road | [ ] | [ ] |
| Drowzee | Routes 11 | [ ] | [ ] |
| Hypno | Berry Forest | [ ] | [ ] |
| Krabby | Routes 4, 10, 12-13 (fishing) | [ ] | [ ] |
| Kingler | Evolution only | [ ] | [ ] |
| Voltorb | Power Plant | [ ] | [ ] |
| Electrode | Power Plant | [ ] | [ ] |
| Exeggcute | Safari Zone | [ ] | [ ] |
| Exeggutor | Evolution only | [ ] | [ ] |
| Cubone | Pokemon Tower | [ ] | [ ] |
| Marowak | Victory Road | [ ] | [ ] |
| Hitmonlee | Fighting Dojo (gift) | [ ] | [ ] |
| Hitmonchan | Fighting Dojo (gift) | [ ] | [ ] |
| Lickitung | Safari Zone | [ ] | [ ] |
| Koffing | Pokemon Mansion | [ ] | [ ] |
| Weezing | Pokemon Mansion | [ ] | [ ] |
| Rhyhorn | Safari Zone | [ ] | [ ] |
| Rhydon | Evolution only | [ ] | [ ] |
| Chansey | Safari Zone | [ ] | [ ] |
| Tangela | Safari Zone | [ ] | [ ] |
| Kangaskhan | Safari Zone | [ ] | [ ] |
| Mr. Mime | Trade only | [ ] | [ ] |
| Scyther | Safari Zone | [ ] | [ ] |
| Jynx | Seafoam Islands | [ ] | [ ] |
| Electabuzz | Power Plant | [ ] | [ ] |
| Magmar | Pokemon Mansion | [ ] | [ ] |
| Pinsir | Safari Zone | [ ] | [ ] |
| Tauros | Safari Zone | [ ] | [ ] |
| Lapras | Silph Co. (gift) | [ ] | [ ] |
| Ditto | Pokemon Mansion, Cerulean Cave | [ ] | [ ] |
| Eevee | Celadon City (gift) | [ ] | [ ] |
| Porygon | Game Corner (prize) | [ ] | [ ] |
| Omanyte | Fossil revival | [ ] | [ ] |
| Omastar | Evolution only | [ ] | [ ] |
| Kabuto | Fossil revival | [ ] | [ ] |
| Kabutops | Evolution only | [ ] | [ ] |
| Aerodactyl | Fossil revival | [ ] | [ ] |
| Snorlax | Routes 12, 16 (static) | [ ] | [ ] |
| Dratini | Safari Zone (fishing) | [ ] | [ ] |
| Dragonair | Safari Zone (fishing) | [ ] | [ ] |
| Dragonite | Evolution only | [ ] | [ ] |

**Section 2 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 3: Cross-Regional Pokemon (Requirement 20.5)

### 3.1 Pokemon That Should Appear in BOTH Kanto and Hoenn
These Pokemon naturally appear in both regions in the original games.

| Pokemon | Kanto Location | Hoenn Location | In Kanto | In Hoenn |
|---------|----------------|----------------|----------|----------|
| Zubat | Mt. Moon, Rock Tunnel | Granite Cave, Meteor Falls | [ ] | [ ] |
| Golbat | Victory Road, Cerulean Cave | Victory Road, Sky Pillar | [ ] | [ ] |
| Crobat | Evolution only | Evolution only | [ ] | [ ] |
| Oddish | Routes 5, 6, 7 | Routes 110, 117, 119, 120 | [ ] | [ ] |
| Gloom | Routes 12, 13, 14, 15 | Safari Zone | [ ] | [ ] |
| Vileplume | Evolution only | Evolution only | [ ] | [ ] |
| Bellossom | Evolution only | Evolution only | [ ] | [ ] |
| Psyduck | Safari Zone | Safari Zone | [ ] | [ ] |
| Golduck | Safari Zone | Safari Zone | [ ] | [ ] |
| Abra | Routes 24, 25 | Granite Cave | [ ] | [ ] |
| Kadabra | Cerulean Cave | Evolution only | [ ] | [ ] |
| Alakazam | Trade evolution | Trade evolution | [ ] | [ ] |
| Machop | Rock Tunnel | Fiery Path, Jagged Pass | [ ] | [ ] |
| Machoke | Victory Road | Fiery Path | [ ] | [ ] |
| Machamp | Trade evolution | Trade evolution | [ ] | [ ] |
| Geodude | Mt. Moon, Rock Tunnel | Granite Cave | [ ] | [ ] |
| Graveler | Victory Road | Victory Road | [ ] | [ ] |
| Golem | Trade evolution | Trade evolution | [ ] | [ ] |
| Magnemite | Power Plant | New Mauville | [ ] | [ ] |
| Magneton | Power Plant | New Mauville | [ ] | [ ] |
| Tentacool | Routes 4, 10-13 (surfing) | Routes 103, 105-109 (surfing) | [ ] | [ ] |
| Tentacruel | Routes 19, 20 (surfing) | Routes 124-134 (surfing) | [ ] | [ ] |
| Magikarp | All fishing locations | All fishing locations | [ ] | [ ] |
| Gyarados | Fuchsia City (fishing) | Sootopolis City (fishing) | [ ] | [ ] |
| Pikachu | Viridian Forest, Power Plant | Safari Zone | [ ] | [ ] |
| Raichu | Evolution only | Evolution only | [ ] | [ ] |
| Sandshrew | Mt. Moon | Route 111 (desert) | [ ] | [ ] |
| Sandslash | Victory Road | Evolution only | [ ] | [ ] |
| Marill | Safari Zone | Routes 102, 111, 114, 117, 120 | [ ] | [ ] |
| Azumarill | Safari Zone | Evolution only | [ ] | [ ] |
| Goldeen | Routes 4, 10-13 (fishing) | Safari Zone (fishing) | [ ] | [ ] |
| Seaking | Fuchsia City (fishing) | Safari Zone (fishing) | [ ] | [ ] |
| Staryu | Seafoam Islands (fishing) | Lilycove City (fishing) | [ ] | [ ] |
| Starmie | Evolution only | Evolution only | [ ] | [ ] |
| Horsea | Seafoam Islands (fishing) | Routes 132-134 (fishing) | [ ] | [ ] |
| Seadra | Evolution only | Evolution only | [ ] | [ ] |
| Kingdra | Trade evolution | Trade evolution | [ ] | [ ] |
| Wobbuffet | Cerulean Cave | Safari Zone | [ ] | [ ] |
| Heracross | Safari Zone | Safari Zone | [ ] | [ ] |
| Phanpy | Safari Zone | Safari Zone | [ ] | [ ] |
| Donphan | Safari Zone | Safari Zone | [ ] | [ ] |
| Pichu | Breeding only | Breeding only | [ ] | [ ] |
| Igglybuff | Breeding only | Breeding only | [ ] | [ ] |
| Cleffa | Breeding only | Breeding only | [ ] | [ ] |
| Togepi | Breeding only | Breeding only | [ ] | [ ] |
| Togetic | Evolution only | Evolution only | [ ] | [ ] |
| Natu | Safari Zone | Safari Zone | [ ] | [ ] |
| Xatu | Safari Zone | Safari Zone | [ ] | [ ] |
| Girafarig | Safari Zone | Safari Zone | [ ] | [ ] |
| Rhyhorn | Safari Zone | Safari Zone | [ ] | [ ] |
| Rhydon | Evolution only | Evolution only | [ ] | [ ] |
| Doduo | Safari Zone | Safari Zone | [ ] | [ ] |
| Dodrio | Safari Zone | Safari Zone | [ ] | [ ] |

**Section 3 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 4: Wild Encounter Verification by Region

### 4.1 Kanto Wild Encounter Locations
Test wild encounters at these key Kanto locations:

| Location | Encounter Type | Pokemon Appear | Levels Correct | No Hoenn-Only |
|----------|----------------|----------------|----------------|---------------|
| Route 1 | Grass | [ ] | [ ] | [ ] |
| Route 2 | Grass | [ ] | [ ] | [ ] |
| Viridian Forest | Grass | [ ] | [ ] | [ ] |
| Mt. Moon | Cave | [ ] | [ ] | [ ] |
| Rock Tunnel | Cave | [ ] | [ ] | [ ] |
| Pokemon Tower | Ghost | [ ] | [ ] | [ ] |
| Safari Zone | Grass/Water | [ ] | [ ] | [ ] |
| Seafoam Islands | Cave/Water | [ ] | [ ] | [ ] |
| Pokemon Mansion | Indoor | [ ] | [ ] | [ ] |
| Victory Road | Cave | [ ] | [ ] | [ ] |
| Cerulean Cave | Cave | [ ] | [ ] | [ ] |
| Power Plant | Indoor | [ ] | [ ] | [ ] |
| Diglett's Cave | Cave | [ ] | [ ] | [ ] |

### 4.2 Hoenn Wild Encounter Locations
Test wild encounters at these key Hoenn locations:

| Location | Encounter Type | Pokemon Appear | Levels Correct | No Kanto-Only |
|----------|----------------|----------------|----------------|---------------|
| Route 101 | Grass | [ ] | [ ] | [ ] |
| Route 102 | Grass | [ ] | [ ] | [ ] |
| Route 103 | Grass/Water | [ ] | [ ] | [ ] |
| Petalburg Woods | Forest | [ ] | [ ] | [ ] |
| Granite Cave | Cave | [ ] | [ ] | [ ] |
| Route 110 | Grass | [ ] | [ ] | [ ] |
| Route 111 (Desert) | Sand | [ ] | [ ] | [ ] |
| Fiery Path | Cave | [ ] | [ ] | [ ] |
| Route 117 | Grass | [ ] | [ ] | [ ] |
| Route 119 | Grass/Water | [ ] | [ ] | [ ] |
| Mt. Pyre | Cave/Grass | [ ] | [ ] | [ ] |
| Shoal Cave | Cave | [ ] | [ ] | [ ] |
| Safari Zone | Grass/Water | [ ] | [ ] | [ ] |
| Victory Road | Cave | [ ] | [ ] | [ ] |
| Sky Pillar | Cave | [ ] | [ ] | [ ] |
| Meteor Falls | Cave | [ ] | [ ] | [ ] |

### 4.3 Fishing Encounters by Region

#### Kanto Fishing
| Location | Rod Type | Pokemon Expected | Correct |
|----------|----------|------------------|---------|
| Route 4 | Old Rod | Magikarp | [ ] |
| Route 4 | Good Rod | Poliwag, Goldeen | [ ] |
| Route 4 | Super Rod | Poliwhirl, Slowpoke | [ ] |
| Fuchsia City | Old Rod | Magikarp | [ ] |
| Fuchsia City | Good Rod | Goldeen, Poliwag | [ ] |
| Fuchsia City | Super Rod | Gyarados, Seaking | [ ] |
| Seafoam Islands | Old Rod | Magikarp | [ ] |
| Seafoam Islands | Good Rod | Horsea, Krabby | [ ] |
| Seafoam Islands | Super Rod | Shellder, Staryu | [ ] |

#### Hoenn Fishing
| Location | Rod Type | Pokemon Expected | Correct |
|----------|----------|------------------|---------|
| Route 102 | Old Rod | Magikarp | [ ] |
| Route 102 | Good Rod | Goldeen, Corphish | [ ] |
| Route 102 | Super Rod | Corphish | [ ] |
| Route 118 | Old Rod | Magikarp | [ ] |
| Route 118 | Good Rod | Tentacool, Carvanha | [ ] |
| Route 118 | Super Rod | Sharpedo | [ ] |
| Route 119 | Old Rod | Magikarp | [ ] |
| Route 119 | Good Rod | Corphish | [ ] |
| Route 119 | Super Rod | Feebas (rare) | [ ] |
| Sootopolis City | Old Rod | Magikarp | [ ] |
| Sootopolis City | Good Rod | Magikarp | [ ] |
| Sootopolis City | Super Rod | Gyarados | [ ] |

**Section 4 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Section 5: Special Pokemon Distribution

### 5.1 Gift Pokemon by Region
| Pokemon | Region | Location | Method | Correct Region |
|---------|--------|----------|--------|----------------|
| Bulbasaur/Charmander/Squirtle | Kanto | Pallet Town | Starter | [ ] |
| Treecko/Torchic/Mudkip | Hoenn | Littleroot Town | Starter | [ ] |
| Eevee | Kanto | Celadon City | Gift | [ ] |
| Lapras | Kanto | Silph Co. | Gift | [ ] |
| Hitmonlee/Hitmonchan | Kanto | Fighting Dojo | Gift | [ ] |
| Castform | Hoenn | Weather Institute | Gift | [ ] |
| Beldum | Hoenn | Steven's House | Gift | [ ] |
| Wynaut | Hoenn | Lavaridge Town (Egg) | Gift | [ ] |

### 5.2 Fossil Pokemon by Region
| Pokemon | Region | Fossil | Revival Location | Correct |
|---------|--------|--------|------------------|---------|
| Omanyte | Kanto | Helix Fossil | Cinnabar Lab | [ ] |
| Kabuto | Kanto | Dome Fossil | Cinnabar Lab | [ ] |
| Aerodactyl | Kanto | Old Amber | Cinnabar Lab | [ ] |
| Lileep | Hoenn | Root Fossil | Devon Corp | [ ] |
| Anorith | Hoenn | Claw Fossil | Devon Corp | [ ] |

### 5.3 Static Encounter Pokemon
| Pokemon | Region | Location | Correct Region |
|---------|--------|----------|----------------|
| Snorlax | Kanto | Routes 12, 16 | [ ] |
| Electrode | Kanto | Power Plant | [ ] |
| Articuno | Kanto | Seafoam Islands | [ ] |
| Zapdos | Kanto | Power Plant | [ ] |
| Moltres | Kanto | Mt. Ember | [ ] |
| Mewtwo | Kanto | Cerulean Cave | [ ] |
| Kyogre | Hoenn | Cave of Origin | [ ] |
| Groudon | Hoenn | Cave of Origin | [ ] |
| Rayquaza | Hoenn | Sky Pillar | [ ] |
| Regirock | Hoenn | Desert Ruins | [ ] |
| Regice | Hoenn | Island Cave | [ ] |
| Registeel | Hoenn | Ancient Tomb | [ ] |
| Latias | Hoenn | Roaming/Southern Island | [ ] |
| Latios | Hoenn | Roaming/Southern Island | [ ] |
| Kecleon | Hoenn | Routes 118-121 (invisible) | [ ] |

**Section 5 Status:** [ ] PASS / [ ] FAIL
**Notes:** _______________

---

## Summary

### Overall Test Results

| Section | Status | Critical Issues |
|---------|--------|-----------------|
| 1. Hoenn-Exclusive Pokemon | [ ] PASS / [ ] FAIL | |
| 2. Kanto-Exclusive Pokemon | [ ] PASS / [ ] FAIL | |
| 3. Cross-Regional Pokemon | [ ] PASS / [ ] FAIL | |
| 4. Wild Encounter Verification | [ ] PASS / [ ] FAIL | |
| 5. Special Pokemon Distribution | [ ] PASS / [ ] FAIL | |

### Final Verdict
- [ ] **PASS** - Regional Pokemon distribution is correct
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

### Wild Encounter Testing
```
# Enable wild encounter debug mode (if available)
# Shows encounter table info

# Warp to Kanto encounter locations
warp MAP_ROUTE1
warp MAP_VIRIDIAN_FOREST
warp MAP_MT_MOON_1F
warp MAP_ROCK_TUNNEL_1F
warp MAP_POKEMON_TOWER_1F
warp MAP_SAFARI_ZONE_CENTER
warp MAP_SEAFOAM_ISLANDS_1F
warp MAP_POKEMON_MANSION_1F
warp MAP_VICTORY_ROAD_1F
warp MAP_CERULEAN_CAVE_1F
warp MAP_POWER_PLANT

# Warp to Hoenn encounter locations
warp MAP_ROUTE101
warp MAP_ROUTE102
warp MAP_PETALBURG_WOODS
warp MAP_GRANITE_CAVE_1F
warp MAP_ROUTE110
warp MAP_ROUTE111
warp MAP_FIERY_PATH
warp MAP_MT_PYRE_1F
warp MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM
warp MAP_SAFARI_ZONE_SOUTH
warp MAP_VICTORY_ROAD_1F
warp MAP_SKY_PILLAR_1F
warp MAP_METEOR_FALLS_1F_1R
```

### Pokemon Encounter Commands
```
# Give fishing rods for testing
giveitem ITEM_OLD_ROD
giveitem ITEM_GOOD_ROD
giveitem ITEM_SUPER_ROD

# Give repels to control encounters
giveitem ITEM_REPEL
giveitem ITEM_SUPER_REPEL
giveitem ITEM_MAX_REPEL

# Give region tickets for travel
giveitem ITEM_HOENN_TICKET
giveitem ITEM_KANTO_TICKET
```

### Legendary Pokemon Testing
```
# Warp to Kanto legendary locations
warp MAP_SEAFOAM_ISLANDS_B4F  # Articuno
warp MAP_POWER_PLANT          # Zapdos
warp MAP_MT_EMBER_SUMMIT      # Moltres
warp MAP_CERULEAN_CAVE_B1F    # Mewtwo

# Warp to Hoenn legendary locations
warp MAP_CAVE_OF_ORIGIN_B1F   # Kyogre/Groudon
warp MAP_SKY_PILLAR_TOP       # Rayquaza
warp MAP_DESERT_RUINS         # Regirock
warp MAP_ISLAND_CAVE          # Regice
warp MAP_ANCIENT_TOMB         # Registeel
warp MAP_SOUTHERN_ISLAND_EXTERIOR  # Latias/Latios
```

### Flag Commands for Testing
```
# Set flags to enable legendary encounters (if needed)
# Check specific game flags for legendary availability
```

---

## Testing Notes

### Encounter Rate Considerations
- Some Pokemon have very low encounter rates (e.g., Feebas at 1%)
- Use debug tools or extended testing sessions for rare Pokemon
- Document any Pokemon that couldn't be verified due to rarity

### Version Differences
- Some Pokemon may have version-exclusive availability
- Document which version is being tested
- Note any Pokemon that should be version-exclusive

### Level Scaling Impact
- Wild Pokemon levels may be scaled based on progression
- Verify Pokemon species are correct regardless of level
- Note if level scaling affects encounter availability

---

**Document Version:** 1.0
**Created:** Task 4.8 - Test regional Pokemon distribution
**Last Updated:** _______________
