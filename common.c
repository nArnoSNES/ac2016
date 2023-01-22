/*---------------------------------------------------------------------------------

	AC2016 - common lib (i'll add my custom common function here)
		-- n_Arno

---------------------------------------------------------------------------------*/
#include <snes.h>

u8 pvsneslibfont_map[0x800];
u16  maptext_adress;
u8   palette_adress, palette_number;

// copy paste of the consoleSetTextCol, but for color 2 of the "paletteNumber"
// see play.c for a comment on "how" i believe it works
void consoleSetShadowCol(u8 paletteNumber, u16 colorShad) {
	REG_CGADD = 0x02+(paletteNumber<<4); *CGRAM_PALETTE = colorShad & 0xFF; *CGRAM_PALETTE = colorShad>>8;
}

void consoleSetTextCol(u16  colorChar, u16 colorBG) {
	// Font Border Color
	REG_CGADD = 0x00+(palette_number<<4); *CGRAM_PALETTE = colorBG & 0xFF; *CGRAM_PALETTE = colorBG>>8;

	// Font Color
	REG_CGADD = 0x01+(palette_number<<4); *CGRAM_PALETTE = colorChar & 0xFF; *CGRAM_PALETTE = colorChar>>8;
}

// old c version of console init text used before newer version of lib
void oldConsoleInitText(u8 bgNumber,u8 paletteNumber, u8 *gfxText) {
	u16 x;

	// Copy data to VRAM (also clear screen)
	for (x=0;x<0x800;x++) pvsneslibfont_map[x] = 0x00;

	setBrightness(0); // Force VBkank
	dmaCopyVram(gfxText, 0x000, 3072); 
	dmaCopyVram(pvsneslibfont_map, 0x800, 0x800);

	// Initialize background tile & map
	maptext_adress = 0x800;
	palette_number = paletteNumber;
	palette_adress = (palette_number<<2) | (1<<5); // (10-7) because only high byte are addressed
        bgSetGfxPtr(bgNumber, 0x000);
        bgSetMapPtr(bgNumber, 0x800, SC_32x32);

	// Font Border Color
	REG_CGADD = 0x00+(paletteNumber<<4); *CGRAM_PALETTE = RGB5(0,0,0) & 0xFF; *CGRAM_PALETTE = RGB5(0,0,0)>>8;

	// Font Color
	REG_CGADD = 0x01+(paletteNumber<<4); *CGRAM_PALETTE = RGB5(31,31,31)  & 0xFF; *CGRAM_PALETTE = RGB5(31,31,31)>>8;

	setBrightness(0);  // Force VBlank Interrupt
	WaitForVBlank(); 
	setBrightness(0xF);  // Screen with all brightness
}
