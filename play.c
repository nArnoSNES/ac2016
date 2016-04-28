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

// Init some self-explaining variables
int life = 3;
int score = 0;

// And now, for an ugly "i don't know C" hack to display score
char lifeStr[1];
char scoreStr[3];

int scoreCent = 0;
int scoreTen = 0;
int scoreUnit = 0;

// Print in-game score and lives left
void printInfo(void) {
	// Since i'm printing in tate, i can't use the normal consoleDrawText
	// to display the whole score. And since i was too tired to figure
	// how to get each character of the string one by one...
	// Soooo, i just divided the score into hundreds, tens and units.
	// NOW, YOU CAN LAUGH AT ME >_<

        sprintf(lifeStr,"%u",life);
        consoleDrawText(4,12,lifeStr);

        scoreCent = score/100;
        sprintf(scoreStr,"%u",scoreCent);
        consoleDrawText(2,13,scoreStr);

        scoreTen = (score-(scoreCent*100))/10;
        sprintf(scoreStr,"%u",scoreTen);
        consoleDrawText(2,12,scoreStr);

        scoreUnit = (score-(scoreCent*100)-(scoreTen*10));
        sprintf(scoreStr,"%u",scoreUnit);
        consoleDrawText(2,11,scoreStr);
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
	// the snes will known, by the object size, how to display the full sprite by its first tile.
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

	// if something (not 0), get (previous func) and display the sprite 
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

// Wait 25 frame, so... about a half a second
void doPause(void) {
        int delay = 0;
        for (delay=0;delay<25;delay++) WaitForVBlank();
        return;
}

// OK, let's get this shit done
int play(void) {
	// This receive the status of the joypad
	unsigned short pad0;

	// generate a first line. rand() is a random int, so, modulo 3 will be 0, 1 or 2.
	Line line = {40,rand() % 3,rand() % 3,rand() % 3};

	// initialize the player
	Player player = {150,100,0}; // x pos, y pos, state

	// some work variables
	int frameskip = 0;
	int counter = 0;
	int gameover = 0;
	int speed = 6;
	
	// Init the color of the scrolling. Based on actual colors of the image.
	// Otherwise, it will look strange for a few frame
	Three_color currentColors = {RGB15(10,10,6),RGB15(14,14,11),RGB15(20,20,16)}; 

	// Another Reminder. I'm lazy and using the built-in Alekmaul console text functions (as BG0)
	// void consoleInitText(u8 bgNumber,u8 paletteNumber, u8 *gfxText)
	// I'm using "paletteNumber" 1 to avoid corrupting the real BG colors (even if i left those aside in case)
	consoleInitText(0, 1, &snesfont_rot);
	
	// Set the Text colors. BTW: Color 0 is transparent anyway.
	consoleSetTextCol(RGB15(10,10,6),RGB15(0,0,0));

	// This one is custom by me, it's in common.c, i'm using a third color in my font as shadow.
	consoleSetShadowCol(1 ,RGB15(20,20,16));

	// Another Reminder. Rather classic. I'm using BG1 for the actual BG.
	// void bgInitTileSet(u8 bgNumber, u8 *tileSource, u8 *tilePalette, u8 paletteEntry, u16 tileSize, u16 paletteSize, u16 colorMode, u16 address)
	bgInitTileSet(1, &bg1_tiles, &bg1_pal, 0, (&bg1_tiles_end - &bg1_tiles), (&bg1_pal_end-&bg1_pal), BG_16COLORS, 0x4000);
	bgInitMapSet(1, &bg1_map, (&bg1_map_end - &bg1_map),SC_32x32, 0x03000);
	
	// Start the snes mode 1
	setMode(BG_MODE1,0);

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
	doPause();

	// Another Reminder.... BUT THIS ONE IS THE ONE WHICH GAVE ME AN HEADACHE
	// In fact, loading the sprite is done here all in one go... but i loaded too much data at once.
	// In PAL (50Hz) the DMA (Direct Memory Access) had time to finish... not in PAL (60Hz). 
	// It will work with a bad emulator which will load data more quickly than the real hardware
	// But you can see the problem using Higan/BSNES (by Byuu-Senpai!)
	// SO.... this is where i'll have to split loading in half to avoid loading half the sprite and no palette.
	// void oamInitGfxSet(u8 *tileSource, u16 tileSize, u8 *tilePalette, u16 paletteSize, u8 tilePaletteNumber, u16 address, u8 oamsize)
	oamInitGfxSet(&sprites, (&sprites_end-&sprites), &sprites_pal, (&sprites_pal_end-&sprites_pal), 0, 0x6000, OBJ_SIZE32);
	
	// And let's PLAY!
	while(!gameover) {
		
		// Read joypad state	
		pad0 = padsCurrent(0);
    
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
								if (player.s == 0) score++; // The player is in the normal state to gain score.
								else score--; // Damn! The player broke the egg! Lose score.
								if (score < 0) score = 0; // Don't let the score go below 0.
								break;
							case 2:
								// It's a beholder! If the player is not "switched", it will lose a live.
								if (player.s == 0) {
									life--; 
									doPause();
								}
								break;
								
						}
						break;
					case 100:
						switch (line.center) { // Same as above, but center.
							case 0:
								break;
							case 1:
								if (player.s == 0) score++;
								else score--;
								if (score < 0) score = 0;
								break;
							case 2:
								if (player.s == 0) {
									life--;
									doPause();
								}
								break;
						}
						break;
					case 151:
						switch (line.left) { // Same as above, but left.
							case 0:
								break;
							case 1:
								if (player.s == 0) score++;
								else score--;
								if (score < 0) score = 0;
								break;
							case 2:
								if (player.s == 0) {
									life--;
									doPause();
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
		if (score > 100) speed = 0;
		else if (score > 50) speed = 1;
		else if (score > 30) speed = 2;
		else if (score > 20) speed = 3;
		else if (score > 10) speed = 4;
		else if (score > 5) speed = 5;
		else speed = 6;
		
		// Wait for next frame.
		WaitForVBlank();
	}

	// Bouhou, you died. Let's hide all sprites (i found out later there is an other function to do that)
	int i; for (i=0;i<25;i+=4) oamSetVisible(i, OBJ_HIDE);

	// Fade out the screen
	setFadeEffect(FADE_OUT);

	// A small pause.
	doPause();

	// Exit with the score for display in next screen
	return score;
}
