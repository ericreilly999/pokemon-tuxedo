# Hoenn Region In-Game Test Checklist

## Task 2.11: Test Hoenn region in-game

This checklist documents the manual testing required to verify the Hoenn region port is functional.

### Prerequisites
1. Build the ROM: `make firered_modern` or `make firered`
2. Load the ROM in an emulator (mGBA recommended)
3. Use debug features or cheats to warp to Hoenn starting location

### Test Categories

## 1. Map Rendering and Tileset Display
- [ ] Littleroot Town renders correctly
- [ ] Route 101 renders correctly
- [ ] Petalburg City renders correctly
- [ ] Rustboro City renders correctly
- [ ] Tilesets display without graphical glitches
- [ ] Animated tiles work (water, flowers)
- [ ] No missing or corrupted tiles

## 2. Map Connections and Warps
- [ ] Walk from Littleroot Town to Route 101
- [ ] Walk from Route 101 to Oldale Town
- [ ] Enter and exit buildings (Pokemon Center, Mart)
- [ ] Cave entrances work (Granite Cave, Rusturf Tunnel)
- [ ] Warp points function correctly
- [ ] No softlocks at map transitions

## 3. NPC Interactions and Scripts
- [ ] NPCs display correctly
- [ ] NPC dialogue appears when talked to
- [ ] Movement scripts work (NPCs walking around)
- [ ] Sign posts are readable
- [ ] Pokemon Center healing works
- [ ] Mart buying/selling works

## 4. Wild Encounters
- [ ] Wild Pokemon appear in grass (Route 101)
- [ ] Wild Pokemon appear in water (Route 102)
- [ ] Fishing encounters work
- [ ] Rock Smash encounters work (Route 111)
- [ ] Encounter rates feel appropriate
- [ ] Pokemon levels match expected ranges

## 5. Trainer Battles
- [ ] Trainer battles trigger correctly
- [ ] Gym Leader battles work (Roxanne in Rustboro)
- [ ] Trainer AI functions properly
- [ ] Battle rewards are given correctly
- [ ] Trainer rematch system works

## 6. Region-Specific Features
- [ ] Region ID is correctly set to 2 (Hoenn)
- [ ] Fly locations register for Hoenn
- [ ] Badge tracking works for Hoenn badges
- [ ] Weather effects work (Route 119 rain, Route 111 sandstorm)

### Test Results

| Test Category | Status | Notes |
|---------------|--------|-------|
| Map Rendering | [ ] Pass / [ ] Fail | |
| Connections/Warps | [ ] Pass / [ ] Fail | |
| NPC Scripts | [ ] Pass / [ ] Fail | |
| Wild Encounters | [ ] Pass / [ ] Fail | |
| Trainer Battles | [ ] Pass / [ ] Fail | |
| Region Features | [ ] Pass / [ ] Fail | |

### Known Issues
(Document any issues found during testing)

1. 
2. 
3. 

### Debug Commands
To warp to Hoenn starting location, use the debug menu or these warp codes:
- Littleroot Town: MAP_LITTLEROOT_TOWN
- Route 101: MAP_ROUTE101
- Rustboro City: MAP_RUSTBORO_CITY

### Build Verification
Before testing, verify the build completed successfully:
```bash
make firered_modern
# Check for errors in build output
# Verify pokefirered.gba was created
```

### Tester Information
- Date: _______________
- Tester: _______________
- ROM Version: _______________
- Emulator: _______________

---
Generated: 2026-03-15
Task: 2.11 Test Hoenn region in-game
