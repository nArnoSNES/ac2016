/*---------------------------------------------------------------------------------

	AC2016 - play()
		-- n_Arno

---------------------------------------------------------------------------------*/
#include <snes.h>
#include "common.h"

// Get the address of the included binaries
extern char snesfont_rot;
extern char bg1_tiles, bg1_tiles_end;
extern char bg1_pal, bg1_pal_end;
extern char bg1_map, bg1_map_end;

extern char sprites, sprites_end;
extern char sprites_pal, sprites_pal_end;

// Init some self-explaining variables. It is counter-intuitive but most variable MUST be global
// Otherwise TCC does a real crappy job of constantly swapping non-global variable on and off the stack
// Optimore will take care of optimizing this once in assembly
int life;
int score;
int frameskip;
int counter;
int gameover;
int speed;
int mul;

char lifeStr[1];
char scoreStr[3];
char tempStr[1];

// Print in-game score and lives left
void printInfo(void) {

        sprintf(lifeStr,"%u",life);
        consoleDrawText(4,12,lifeStr);

		// Get the score in a string, padded with 0
		sprintf(scoreStr,"%03u",score);
		
		// Get the first char and display it
		sprintf(tempStr,"%c",scoreStr[0]);
		consoleDrawText(2,13,tempStr);
		
		// second...
		sprintf(tempStr,"%c",scoreStr[1]);
		consoleDrawText(2,12,tempStr);
		
		// and third!
		sprintf(tempStr,"%c",scoreStr[2]);
		consoleDrawText(2,11,tempStr);
}

// Describe the player
typedef struct
{
	int x,y,s; // x pos, y pos, state
} Player;

// Move the player, which is in fact 4 sprites
void movePlayer(Player player) {
	// Small reminder of the parameters of this function
	// #define oamSet(id,  xspr, yspr, priority, hflip, vflip, gfxoffset, paletteoffset)
	// The sprite ID MUST BE a multiple of 4. Don't ask.
	// The gfxoffset is far more fun. The sprites tiles are 8x8, and wrap after 128px BUT
	// the snes will know, by the object size, how to display the full sprite by its first tile.
	// Plus, the gfx converter will arrange data to match the sprite size.
	// To give explanation, i'll upload a picture. It is more important for the obstacle sprite.

	oamSet(0,  player.x, player.y, 3, 0, 0, 0, player.s);
	oamSet(4,  player.x, player.y-32, 3, 0, 0, 4, player.s); 
	oamSet(8,  player.x+32, player.y, 3, 0, 0, 8, player.s); 
	oamSet(12,  player.x+32, player.y-32, 3, 0, 0, 12, player.s);
	int i; for (i=0;i<13;i+=4) oamSetEx(i, OBJ_SMALL, OBJ_SHOW);
	return;
}

// Describe a "line" of obstacle
typedef struct
{
        int pos; // x position on the screen
        u16 left; // either 0 nothing, 1 an egg or 2 a beholder
        u16 center; // same
        u16 right; // same
} Line;

// Get the sprite number to display for each obstacle type, based on "distance"
int get_sprite(int type, int zoom) {
	int result=0;
	switch(type) {
		case 0: // good old "should not happen"
			break;
		case 1: // egg
			result = 64 + zoom*4;
			break;
		case 2: // beholder
			if (zoom == 0) {
				result = 76;
			} else {
				result = 124 + zoom*4;
			}
			break;
	}
	return result;
}

// Display the line of obstacle
void printLine(Line line) {
	// Zoom is either 0 (far), 1 (middle) or 2 (close)
	int zoom = 0;
	if (line.pos>50) zoom++;
	if (line.pos>100) zoom++;

	// if something (not 0), get sprite (previous func) and display the sprite 
	if (line.right) {
	oamSet(16, line.pos, 45-(0.125*line.pos), 3, 0, 0, get_sprite(line.right,zoom), 0); // y position is calculated based on x to be a kind of diagonal to follow scrolling
	oamSetEx(16, OBJ_SMALL, OBJ_SHOW);
	} else {
	// if nothing (0), hide the sprite
		oamSetEx(16, OBJ_SMALL, OBJ_HIDE);
	}

	// same as right but don't move on y
	if (line.center) {
	oamSet(20, line.pos, 80, 3, 0, 0, get_sprite(line.center,zoom), 0);
	oamSetEx(20, OBJ_SMALL, OBJ_SHOW);
	} else {
		oamSetEx(20, OBJ_SMALL, OBJ_HIDE);
	}

	// same as right but opposite side :D
	if (line.left) {
	oamSet(24, line.pos, 120+(0.125*line.pos), 3, 0, 0, get_sprite(line.left,zoom), 0);
	oamSetEx(24, OBJ_SMALL, OBJ_SHOW);
	} else {
		oamSetEx(24, OBJ_SMALL, OBJ_HIDE);
	}
}

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

// Wait x frame
void doPause(int timing) {
        int delay = 0;
        for (delay=0;delay<timing;delay++) WaitForVBlank();
        return;
}

// OK, let's get this shit done
int play(int game_mode) {
	// This receive the status of the joypad
	unsigned short pad0;
	
	// Classical
	life = 3;

	// Set the variables based on game mode
	if (game_mode == 1) {
		mul = 2;
		speed = 1;
	} else {
		mul = 1;
		speed = 6;
	}
	score = 0;
	frameskip = 0;
	counter = 0;
	gameover = 0;

	// generate a first line. rand() is a random int, so, modulo 3 will be 0, 1 or 2.
	Line line = {40,rand() % 3,rand() % 3,rand() % 3};

	// initialize the player
	Player player = {150,100,0}; // x pos, y pos, state
	
	// Init the color of the scrolling. Based on actual colors of the image.
	// Otherwise, it will look strange for a few frame
	Three_color currentColors = {RGB15(10,10,6),RGB15(14,14,11),RGB15(20,20,16)}; 

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
	oldConsoleInitText(0, 1, &snesfont_rot);
	
	// Set the Text colors. BTW: Color 0 is transparent anyway.
	consoleSetTextCol(RGB15(10,10,6),RGB15(0,0,0));

	// This one is custom by me, it's in common.c, i'm using a third color in my font as shadow.
	consoleSetShadowCol(1 ,RGB15(20,20,16));

	// Start the snes mode 1
	setMode(BG_MODE1,0);

	// Force VBlank Interrupt
	setBrightness(0);

	// Another Reminder.... BUT THIS ONE IS THE ONE WHICH GAVE ME AN HEADACHE
	// In fact, loading the sprite is done here all in one go... but i used to load too much data at once.
	// In PAL (50Hz) the DMA (Direct Memory Access) had time to finish... not in PAL (60Hz). 
	// It will work with a bad emulator which will load data more quickly than the real hardware
	// But it was possible to see the problem using Higan/BSNES (by Byuu-Senpai!)
	// Answer was given by Alekmaul: add setBrightness to force VBlank Interrupt
	// void oamInitGfxSet(u8 *tileSource, u16 tileSize, u8 *tilePalette, u16 paletteSize, u8 tilePaletteNumber, u16 address, u8 oamsize)
	oamInitGfxSet(&sprites, (&sprites_end-&sprites), &sprites_pal, (&sprites_pal_end-&sprites_pal), 0, 0x6000, OBJ_SIZE32);
	
	// Screen with all brightness
	setBrightness(0xF);
	
	// Ugly abuse of the -- operator to write in tate
	int posy=21;
	consoleDrawText(2,posy--,"S P");
	consoleDrawText(2,posy--,"C L");
	consoleDrawText(2,posy--,"O A");
	consoleDrawText(2,posy--,"R Y");
	consoleDrawText(2,posy--,"E E");
	consoleDrawText(2,posy--,": R");
	consoleDrawText(4,posy--,":");
	
	// First printing of the score and lives	
	printInfo();
	
	// Wait a bit before starting the fight!
	doPause(60);
	
	// And let's PLAY!
	while(!gameover) {
		
		if (player.s == 2) {
			doPause(60);
			player.s = 0;
		}
		
		// Read joypad state	
		pad0 = padsCurrent(0);
    
		// Did the player hit start?
		if(pad0 & KEY_START) {
			// Let's do a PAUSE! First remove all sprites. Let's be fair :D
			int i; for (i=0;i<25;i+=4) oamSetVisible(i, OBJ_HIDE);
			// First wait for the player to release start
			pad0 = padsCurrent(0);
			while(pad0 & KEY_START) {
				pad0 = padsCurrent(0);
				WaitForVBlank();
			}
			// Secondly, to quit pause, wait the player to press start again
			pad0 = padsCurrent(0);
			while(!(pad0 & KEY_START)) {
				pad0 = padsCurrent(0);
				WaitForVBlank();
			}
			// And finally, to avoid going immediatly back to pause, wait for release again
			pad0 = padsCurrent(0);
			while(pad0 & KEY_START) {
				pad0 = padsCurrent(0);
				WaitForVBlank();
			}
			// Print the sprites back
			movePlayer(player);
			printLine(line);
		} // NB: THIS is clean way but NOT a great way to pause. Why? The player CAN pause buffer.
    
		// First, movement.	
		if(pad0 & KEY_LEFT) {
			player.y = 151; 
		} else if(pad0 & KEY_RIGHT) {
			player.y = 45;
		} else {
			player.y = 100;
		}

		// Secondly, state.
		if(pad0 & KEY_B) {
			player.s = 1;
		} else {
			player.s = 0;
		}

		// Make sure player x position doesn't change.
		player.x = 150;
		
		// Let's skip some vBlank. Based on speed. (6 currently is the slowest, 0... FAAAASST)
		frameskip++;
		if (frameskip>speed) {
			// Cycle colors
			currentColors = cycleColor(currentColors);

			// Advance the line position.
			line.pos+=10; 

			// If the line reach the player
			if (line.pos>160) {
				
				// Based on the player position, check what is in front of him
				switch (player.y) {
					case 45:
						switch (line.right) { // Player is in front of the right obstacle.
							case 0:
								// Nothing, next! 
								break;
							case 1:
								// It's an egg! 
								if (player.s == 0) score += mul; // The player is in the normal state to gain score.
								else score -= mul; // Damn! The player broke the egg! Lose score.
								if (score < 0) score = 0; // Don't let the score go below 0.
								break;
							case 2:
								// It's a beholder! If the player is not "switched", it will lose a live.
								if (player.s == 0) {
									player.s = 2;
									life--;
								}
								break;
								
						}
						break;
					case 100:
						switch (line.center) { // Same as above, but center.
							case 0:
								break;
							case 1:
								if (player.s == 0) score += mul;
								else score -= mul;
								if (score < 0) score = 0;
								break;
							case 2:
								if (player.s == 0) {
									player.s = 2;
									life--;
								}
								break;
						}
						break;
					case 151:
						switch (line.left) { // Same as above, but left.
							case 0:
								break;
							case 1:
								if (player.s == 0) score += mul;
								else score -= mul;
								if (score < 0) score = 0;
								break;
							case 2:
								if (player.s == 0) {
									player.s = 2;
									life--;
								}
								break;
						}
						break;
				}
				
				if (life <= 0) gameover++; // 0 or less lives? BOUUUUUH! GAMEOVER!
									
				// Since the player hit the line, reset it.
				line.pos = 40;
				line.right = rand() % 3;
				line.center = rand() % 3;
				line.left = rand() % 3;
			}
			frameskip = 0;
			printInfo();
			
		}
		movePlayer(player);
		printLine(line);
		
		// THIS, is me being a sadist. Speed increase with the score.
		// In case of mode B (=1) don't go below speed 1
		if (score > 100) speed = 0;
		else if (score > 50) speed = 1;
		else if (score > 30 && game_mode != 1) speed = 2;
		else if (score > 20 && game_mode != 1) speed = 3;
		else if (score > 10 && game_mode != 1) speed = 4;
		else if (score > 5 && game_mode != 1) speed = 5;
		else if (game_mode != 1 ) speed = 6;
		
		// Wait for next frame.
		WaitForVBlank();
	}

	// A small pause.
	doPause(90);

	// Bouhou, you died. Let's hide all sprites (i found out later there is an other function to do that)
	int i; for (i=0;i<25;i+=4) oamSetVisible(i, OBJ_HIDE);

	// Exit with the score for display in next screen
	return score;
}
