#ifndef GUARD_CHARACTER_CUSTOMIZATION_H
#define GUARD_CHARACTER_CUSTOMIZATION_H

#include "global.h"

// Character sprite IDs
#define SPRITE_PROFESSOR_NANA 0x100
#define SPRITE_MISTER_MANGO 0x101
#define SPRITE_TUXEDO_CAT_LOGO 0x102

// Character name strings
extern const u8 gText_ProfessorNana[];
extern const u8 gText_MisterMango[];
extern const u8 gText_GameTitle[];

// Character customization functions
void LoadProfessorNanaSprite(void);
void LoadMisterMangoSprite(void);
void LoadTuxedoCatLogo(void);
void InitializeCustomCharacters(void);

#endif // GUARD_CHARACTER_CUSTOMIZATION_H
