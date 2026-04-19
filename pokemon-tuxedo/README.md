# Pokémon Tuxedo

**Pokémon Tuxedo** is a multi-region ROM hack built on **pokeemerald-expansion**.
It combines the regions of **Hoenn** and **Kanto** (with **Johto** in development) into one seamless adventure.

Based on the work of [Pokémon Crossroads](https://github.com/eonlynx/pokecrossroads) by the Crossroads Dev Team.

## Features

- Two fully explorable regions: **Hoenn** and **Kanto** (plus Sevii Islands)
- 16 Gym Badges (8 Hoenn + 8 Kanto)
- Region transitions via Pokémon Centers in Viridian City or Oldale Town
- Cross-region level scaling, fly system, and badge tracking
- Built on pokeemerald-expansion with modern GCC

## Building

### Requirements
- devkitARM (C:\devkitPro on Windows)
- git, make, python3

### Steps

```bash
make modern -j4
```

This produces `pokemon_tuxedo.gba` in the project root.

## Credits

- **Game Base**: pokeemerald-expansion by rh-hideout
- **Multi-Region Foundation**: Pokémon Crossroads by eonlynx and the Crossroads Dev Team
- **Engine Logic**: cawtds for importing FireRed logic into Emerald
- **Travel System**: AsparagusEduardo for Kanto/Hoenn travel fixes
- **Community**: The decompilation and ROM hacking communities
