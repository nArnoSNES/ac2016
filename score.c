/*---------------------------------------------------------------------------------


	AC2016 - printScore()
		-- n_Arno


---------------------------------------------------------------------------------*/
#include <snes.h>
#include "common.h"

extern char snesfont_rot;
extern char bg1_tiles, bg1_tiles_end;
extern char bg1_pal, bg1_pal_end;
extern char bg1_map, bg1_map_end;

int delay;
char scoreHolder[3];
char tempHolder[1];

// This display the final score of the player. It will get better looking :D
void printScore(int scorefinal, int highscore) {

	// Small Reminder. Rather classic. I'm using BG1 for the actual BG.
	// void bgInitTileSet(u8 bgNumber, u8 *tileSource, u8 *tilePalette, u8 paletteEntry, u16 tileSize, u16 paletteSize, u16 colorMode, u16 address)
	bgInitTileSet(1, &bg1_tiles, &bg1_pal, 0, (&bg1_tiles_end - &bg1_tiles), (&bg1_pal_end-&bg1_pal), BG_16COLORS, 0x4000);
	// The bg1 palette file in this game is 512 byte long: 256 colors instead of 16... even if gfxconv is set for 16 colors.
	// Loading all of it will initialize all colors: good. But it will overwrite colors for console init so it must be done BEFORE!
	// BUT: I'm also using 2 colors of this palette to set colors for the player sprite in "switch" state.
	bgInitMapSet(1, &bg1_map, (&bg1_map_end - &bg1_map),SC_32x32, 0x03000);

	// Another Reminder. I'm lazy and using the built-in Alekmaul console text functions (as BG0)
	// void consoleInitText(u8 bgNumber,u8 paletteNumber, u8 *gfxText)
	// I'm using "paletteNumber" 1 to avoid corrupting the real BG colors (even if i left those aside in case)
	consoleInitText(0, 1, &snesfont_rot);
	
	// Set the Text colors. BTW: Color 0 is transparent anyway.
	consoleSetTextCol(RGB15(20,20,16),RGB15(0,0,0));

	// This one is custom by me, it's in common.c, i'm using a third color in my font as shadow.
	consoleSetShadowCol(1 ,RGB15(10,10,6));

	// Start the snes mode 1
	setMode(BG_MODE1,0);

	int posy=17;
	consoleDrawText(3,posy--,"G");
	consoleDrawText(3,posy--,"A");
	consoleDrawText(3,posy--,"M");
	consoleDrawText(3,posy--,"E");
	consoleDrawText(3,posy--," ");
	consoleDrawText(3,posy--," ");
	consoleDrawText(3,posy--,"O");
	consoleDrawText(3,posy--,"V");
	consoleDrawText(3,posy--,"E");
	consoleDrawText(3,posy--,"R");

	int posy=15;
	consoleDrawText(10,posy--,"S");
	consoleDrawText(10,posy--,"c");
	consoleDrawText(10,posy--,"o");
	consoleDrawText(10,posy--,"r");
	consoleDrawText(10,posy--,"e");
	consoleDrawText(10,posy--," ");
	consoleDrawText(10,posy--,":");

	sprintf(scoreHolder,"%03u",scorefinal);
	sprintf(tempHolder,"%c",scoreHolder[0]);
	consoleDrawText(13,13,tempHolder);
	sprintf(tempHolder,"%c",scoreHolder[1]);
	consoleDrawText(13,12,tempHolder);
	sprintf(tempHolder,"%c",scoreHolder[2]);
	consoleDrawText(13,11,tempHolder);

	// Sad placeholder for the highscore
	// For now it is not saved in sram
	int posy=19;
	consoleDrawText(25,posy--,"T");
	consoleDrawText(25,posy--,"O");
	consoleDrawText(25,posy--,"P");
	consoleDrawText(25,posy--," ");
	consoleDrawText(25,posy--,"S");
	consoleDrawText(25,posy--,"c");
	consoleDrawText(25,posy--,"o");
	consoleDrawText(25,posy--,"r");
	consoleDrawText(25,posy--,"e");
	consoleDrawText(25,posy--,":");
	consoleDrawText(25,posy--," ");
	
	posy--;
	
	sprintf(scoreHolder,"%03u",highscore);
	sprintf(tempHolder,"%c",scoreHolder[0]);
	consoleDrawText(25,posy--,tempHolder);
	sprintf(tempHolder,"%c",scoreHolder[1]);
	consoleDrawText(25,posy--,tempHolder);
	sprintf(tempHolder,"%c",scoreHolder[2]);
	consoleDrawText(25,posy--,tempHolder);

	// Wait a bit before going back to title
	delay = 0;
	while(delay<400) {
		delay++;
		WaitForVBlank();
	}
	return;
}
