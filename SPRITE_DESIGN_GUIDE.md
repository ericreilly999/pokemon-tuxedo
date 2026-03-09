# Sprite Design Guide - Pokemon Tuxedo

## Character Sprites to Create

### 1. Professor Nana (Calico Cat)

**Design Concept**: A calico cat (orange, black, and white patches) wearing a professor's lab coat.

**Sprite Specifications**:
- Size: 64x64 pixels (standard overworld sprite)
- Colors: 16-color palette
- Style: Match Pokemon FireRed sprite style
- Poses needed:
  - Standing (front view)
  - Walking animation (4 frames)
  - Talking animation (2 frames)

**Design Details**:
- Calico cat coloring (tri-color: orange, black, white)
- White lab coat with pockets
- Friendly, welcoming expression
- Glasses (optional, for professor look)
- Standing upright (anthropomorphic)

**Palette**:
- White (#FFFFFF)
- Orange (#FF8C42)
- Black (#2C2C2C)
- Lab coat white (#F0F0F0)
- Skin/fur tones
- Shadow colors

### 2. Mister Mango (Rival)

**Design Concept**: Custom rival character design (can be human or Pokemon-inspired).

**Sprite Specifications**:
- Size: 64x64 pixels (standard overworld sprite)
- Colors: 16-color palette
- Style: Match Pokemon FireRed sprite style
- Poses needed:
  - Standing (front, back, side views)
  - Walking animation (4 frames per direction)
  - Battle pose

**Design Options**:
- Option A: Human rival with mango-themed colors (orange/yellow)
- Option B: Anthropomorphic character
- Option C: Keep default rival sprite (easiest option)

**Palette**:
- Mango colors (orange, yellow, red)
- Clothing colors
- Hair/fur colors
- Shadow colors

### 3. Tuxedo Cat Logo

**Design Concept**: A tuxedo cat (black with white chest/paws) as the game mascot, replacing the Charizard on the title screen.

**Sprite Specifications**:
- Size: 128x128 pixels (title screen logo)
- Colors: 16-color palette
- Style: Bold, iconic, recognizable
- Animation: Optional idle animation (blinking, tail swish)

**Design Details**:
- Tuxedo cat pattern (black body, white chest, white paws)
- Sitting or standing pose
- Confident, friendly expression
- Optional: Wearing an actual tuxedo bow tie
- Should be instantly recognizable as the game's mascot

**Palette**:
- Black (#000000)
- White (#FFFFFF)
- Gray shades for depth
- Eye colors (green, yellow, or blue)
- Optional bow tie color (red or black)

## Sprite Creation Tools

### Recommended Tools:
1. **Aseprite** - Professional pixel art editor ($19.99 or compile from source)
2. **GraphicsGale** - Free pixel art editor
3. **GIMP** - Free image editor with pixel art plugins
4. **Piskel** - Free online pixel art editor

### GBA Sprite Requirements:
- Maximum 16 colors per sprite (including transparency)
- Tile-based (8x8 pixel tiles)
- Palette must be GBA-compatible (15-bit color)
- Sprites must be in indexed color mode

## Integration Steps

### 1. Create Sprite Graphics
- Draw sprites in pixel art editor
- Export as PNG with indexed colors
- Ensure 16-color palette limit

### 2. Convert to GBA Format
Use tools like:
- **grit** - GBA image converter
- **gfx2gba** - Graphics to GBA converter
- **usenti** - Tile editor and converter

### 3. Insert into ROM
- Convert sprites to .c/.h files
- Include in build system
- Reference in character_customization.c

### 4. Test in Game
- Load ROM in emulator
- Verify sprites display correctly
- Check animations work properly

## Sprite File Structure

```
graphics/
├── characters/
│   ├── professor_nana/
│   │   ├── professor_nana.png
│   │   ├── professor_nana_tiles.c
│   │   ├── professor_nana_palette.c
│   │   └── professor_nana.h
│   ├── mister_mango/
│   │   ├── mister_mango.png
│   │   ├── mister_mango_tiles.c
│   │   ├── mister_mango_palette.c
│   │   └── mister_mango.h
│   └── tuxedo_cat_logo/
│       ├── tuxedo_cat_logo.png
│       ├── tuxedo_cat_logo_tiles.c
│       ├── tuxedo_cat_logo_palette.c
│       └── tuxedo_cat_logo.h
```

## Example Sprite Data Format

```c
// professor_nana_tiles.c
const u32 gProfessorNanaTiles[] = {
    0x00000000, 0x00000000, // Tile data...
    // ... (sprite tile data)
};

// professor_nana_palette.c
const u16 gProfessorNanaPalette[] = {
    RGB(31, 31, 31), // White
    RGB(31, 17, 8),  // Orange
    RGB(5, 5, 5),    // Black
    // ... (16 colors total)
};
```

## Alternative: Use Existing Sprites

If custom sprite creation is too time-intensive, you can:

1. **Recolor existing sprites**
   - Modify Professor Oak sprite to have cat-like colors
   - Recolor rival sprite with mango theme
   - Edit title screen logo

2. **Use placeholder sprites**
   - Keep default sprites during development
   - Replace with custom sprites later

3. **Commission sprite artist**
   - Hire pixel artist on Fiverr, Reddit, or Discord
   - Provide design specifications
   - Typical cost: $20-50 per sprite set

## Testing Checklist

- [ ] Professor Nana sprite loads correctly
- [ ] Professor Nana animations work (walking, talking)
- [ ] Mister Mango sprite loads correctly
- [ ] Mister Mango animations work (all directions)
- [ ] Tuxedo Cat logo displays on title screen
- [ ] Logo animation works (if implemented)
- [ ] All sprites use correct palettes
- [ ] No graphical glitches or corruption
- [ ] Sprites match FireRed art style
- [ ] Character names display correctly in dialogue

## Resources

### Sprite References:
- Pokemon FireRed sprite sheets (for style matching)
- Cat sprite references (for anatomy)
- Tuxedo cat photos (for color patterns)

### Communities:
- r/PokemonROMhacks - ROM hacking community
- PokeCommunity - ROM hacking forums
- Discord servers for ROM hacking

### Tutorials:
- "GBA Sprite Creation Tutorial" on PokeCommunity
- "How to Insert Custom Sprites in Pokemon FireRed"
- "Pixel Art for GBA Games" tutorials

## Notes

- Sprite creation is the most artistic/creative task
- Can be done in parallel with code integration
- Not required for testing core gameplay systems
- Can use placeholders during development
- Final sprites should be added before release

## Estimated Time

- Professor Nana sprite: 4-8 hours
- Mister Mango sprite: 4-8 hours
- Tuxedo Cat logo: 2-4 hours
- Integration and testing: 2-4 hours
- **Total: 12-24 hours** (for experienced pixel artist)

For non-artists, consider commissioning or using recolored existing sprites.
