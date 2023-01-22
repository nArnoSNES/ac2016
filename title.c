/*---------------------------------------------------------------------------------

	AC2016 - title()
		-- n_Arno

---------------------------------------------------------------------------------*/
#include <snes.h>
#include "common.h"

// Get the address of the included binaries
extern char title_tiles, title_tiles_end;
extern char title_pal, title_pal_end;
extern char title_map, title_map_end;

// Struct and function to cycle colors to simulate scrolling
typedef struct
{
        u16 first;
        u16 second;
        u16 third;
} Three_color;

Three_color cycleColor(Three_color currentColors) {
	
	Three_color newColors;

	// Ugly swap? Ugly swap!
	newColors.first = currentColors.second;
	newColors.second = currentColors.third;
	newColors.third = currentColors.first;

	// OK. Those i just hacked from actual Alekmaul code. I vaguely understand how it works.
	// 0C, 0D and 0E are palette color addresses. REG_CGADD will trigger a snes register to change
	// the palette color at indicated address. CGRAM_PALETTE point to the address where the new color
	// must be stored. Otherwise, why & 0xFF and >>8, i didn't try to understand, yet. :P
	REG_CGADD = 0x0C; *CGRAM_PALETTE = newColors.first & 0xFF; *CGRAM_PALETTE = newColors.first>>8;
	REG_CGADD = 0x0D; *CGRAM_PALETTE = newColors.second & 0xFF; *CGRAM_PALETTE = newColors.second>>8;
	REG_CGADD = 0x0E; *CGRAM_PALETTE = newColors.third & 0xFF; *CGRAM_PALETTE = newColors.third>>8;
	
	return newColors;
}

int gm;

int title(void) {
	// This receive the status of the joypad
	unsigned short pad0;

	// some work variables
	int frameskip = 0;
	int speed = 4;
	
	// Init the color of the scrolling. Based on actual colors of the image.
	// Otherwise, it will look strange for a few frame
	Three_color currentColors = {RGB15(10,10,6),RGB15(14,14,11),RGB15(20,20,16)}; 

	// Another Reminder. Rather classic. I'm using BG1 for the actual BG.
	// void bgInitTileSet(u8 bgNumber, u8 *tileSource, u8 *tilePalette, u8 paletteEntry, u16 tileSize, u16 paletteSize, u16 colorMode, u16 address)
	bgInitTileSet(0, &title_tiles, &title_pal, 0, (&title_tiles_end - &title_tiles), (&title_pal_end-&title_pal), BG_16COLORS, 0x4000);
	bgInitMapSet(0, &title_map, (&title_map_end - &title_map),SC_32x32, 0x03000);
	
	// Start the snes mode 1
	setMode(BG_MODE1,0); bgSetDisable(1);  bgSetDisable(2);

	// Wait for an input to start
	pad0 = padsCurrent(0); 
	while(!pad0) {
		// Read joypad state	
		pad0 = padsCurrent(0);
		
		frameskip++;
		if (frameskip>speed) {
			// Cycle colors
			currentColors = cycleColor(currentColors);
			frameskip = 0;
		}
    
		// Wait for next frame.
		WaitForVBlank();
	}

	// If game is started with select key, start in mode B (=1)
	if (pad0 & KEY_SELECT) { gm = 1; } else { gm = 0; }
	
	// OK, this one need some explanation.
	// The rand() function of the framework use a seed to generate pseudo-number
	// But with the unmodified PVSNESLIB framework, the seed is ALWAYS the same.
	// So, it will look random once, but it will always be the same set of num
	// between console reset.
	// I modified the console.c source of the framework to add a function to
	// set the seed. I wait for the player to press start and use the number
	// of vertical blanking since reset as seed. Yeah, a little bit better!
	//srand(snes_vblank_count);

	return gm;
}
