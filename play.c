/*---------------------------------------------------------------------------------

	AC2016 - play()
		-- n_Arno

---------------------------------------------------------------------------------*/
#include <snes.h>
#include "common.h"

extern char snesfont_rot;
extern char bg1_tiles, bg1_tiles_end;
extern char bg1_pal, bg1_pal_end;
extern char bg1_map, bg1_map_end;

extern char sprites, sprites_end;
extern char sprites_pal, sprites_pal_end;

int life = 3;
int score = 0;

char lifeStr[1];
char scoreStr[3];

int scoreCent = 0;
int scoreTen = 0;
int scoreUnit = 0;

typedef struct
{
	u16 first;
	u16 second;
	u16 third;
} Three_color;

typedef struct
{
	int pos;
	u16 left;
	u16 center;
	u16 right;
} Line;

typedef struct
{
	int x,y,s; // x pos, y pos, state
} Player;

void doPause(void) {
	int delay = 0;
	for (delay=0;delay<24;delay++) WaitForVBlank();
	return;
}

void movePlayer(Player player) {
	//#define oamSet(id,  xspr, yspr, priority, hflip, vflip, gfxoffset, paletteoffset)
	oamSet(0,  player.x, player.y, 3, 0, 0, 0, player.s);
	oamSet(4,  player.x, player.y-32, 3, 0, 0, 4, player.s); 
	oamSet(8,  player.x+32, player.y, 3, 0, 0, 8, player.s); 
	oamSet(12,  player.x+32, player.y-32, 3, 0, 0, 12, player.s);
	int i; for (i=0;i<13;i+=4) oamSetEx(i, OBJ_SMALL, OBJ_SHOW);
	return;
}

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

void printLine(Line line) {
	int zoom = 0;
	if (line.pos>50) zoom++;
	if (line.pos>100) zoom++;
	if (line.right) {
	oamSet(16, line.pos, 45-(0.125*line.pos), 3, 0, 0, get_sprite(line.right,zoom), 0);
	oamSetEx(16, OBJ_SMALL, OBJ_SHOW);
	} else {
		oamSetEx(16, OBJ_SMALL, OBJ_HIDE);
	}
	if (line.center) {
	oamSet(20, line.pos, 80, 3, 0, 0, get_sprite(line.center,zoom), 0);
	oamSetEx(20, OBJ_SMALL, OBJ_SHOW);
	} else {
		oamSetEx(20, OBJ_SMALL, OBJ_HIDE);
	}
	if (line.left) {
	oamSet(24, line.pos, 120+(0.125*line.pos), 3, 0, 0, get_sprite(line.left,zoom), 0);
	oamSetEx(24, OBJ_SMALL, OBJ_SHOW);
	} else {
		oamSetEx(24, OBJ_SMALL, OBJ_HIDE);
	}
}

void printInfo(void) {
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

Three_color cycleColor(Three_color currentColors) {
	
	Three_color newColors;
	newColors.first = currentColors.second;
	newColors.second = currentColors.third;
	newColors.third = currentColors.first;
	
	REG_CGADD = 0x0C; *CGRAM_PALETTE = newColors.first & 0xFF; *CGRAM_PALETTE = newColors.first>>8;
	REG_CGADD = 0x0D; *CGRAM_PALETTE = newColors.second & 0xFF; *CGRAM_PALETTE = newColors.second>>8;
	REG_CGADD = 0x0E; *CGRAM_PALETTE = newColors.third & 0xFF; *CGRAM_PALETTE = newColors.third>>8;
	
	return newColors;
}

int play(void) {
	unsigned short pad0;
	Line line = {40,rand() % 3,rand() % 3,rand() % 3};
	Player player = {150,100,0}; // x pos, y pos, state
	int frameskip = 0;
	int counter = 0;
	int gameover = 0;
	int speed = 6;
	
	Three_color currentColors = {RGB15(10,10,6),RGB15(14,14,11),RGB15(20,20,16)}; 

	// void bgInitTileSet(u8 bgNumber, u8 *tileSource, u8 *tilePalette, u8 paletteEntry, u16 tileSize, u16 paletteSize, u16 colorMode, u16 address)
	bgInitTileSet(1, &bg1_tiles, &bg1_pal, 0, (&bg1_tiles_end - &bg1_tiles), (&bg1_pal_end-&bg1_pal), BG_16COLORS, 0x4000);
	//bgInitTileSet(1, &bg1_tiles, &bg1_pal, 0, (&bg1_tiles_end - &bg1_tiles), 16*2, BG_16COLORS, 0x4000);
	bgInitMapSet(1, &bg1_map, (&bg1_map_end - &bg1_map),SC_32x32, 0x03000);
	
	setMode(BG_MODE1,0);

	// void consoleInitText(u8 bgNumber,u8 paletteNumber, u8 *gfxText)
	consoleInitText(0, 1, &snesfont_rot);
	consoleSetTextCol(RGB15(10,10,6),RGB15(0,0,0));
	consoleSetShadowCol(1 ,RGB15(20,20,16));

	int posy=21;
	consoleDrawText(2,posy--,"S P");
	consoleDrawText(2,posy--,"C L");
	consoleDrawText(2,posy--,"O A");
	consoleDrawText(2,posy--,"R Y");
	consoleDrawText(2,posy--,"E E");
	consoleDrawText(2,posy--,": R");
	consoleDrawText(4,posy--,":");
	
	printInfo();
	
	doPause();
	
	// void oamInitGfxSet(u8 *tileSource, u16 tileSize, u8 *tilePalette, u16 paletteSize, u8 tilePaletteNumber, u16 address, u8 oamsize)
	//oamInitGfxSet(&sprites, (&sprites_end-&sprites), &sprites_pal, 16*2, 0, 0x6000, OBJ_SIZE32);
	oamInitGfxSet(&sprites, (&sprites_end-&sprites), &sprites_pal, (&sprites_pal_end-&sprites_pal), 0, 0x6000, OBJ_SIZE32);
	//oamInitGfxSet(&sprites, (&sprites_end-&sprites), &bg1_pal, (&bg1_pal_end-&bg1_pal), 0, 0x6000, OBJ_SIZE32);
	
	while(!gameover) {
		
		pad0 = padsCurrent(0);
    	
		if(pad0 & KEY_LEFT) {
			player.y = 151; 
		} else if(pad0 & KEY_RIGHT) {
			player.y = 45;
		} else {
			player.y = 100;
		}
		if(pad0 & KEY_B) {
			player.s = 1;
		} else {
			player.s = 0;
		}
		player.x = 150;
		
		frameskip++;
		if (frameskip>speed) {
			currentColors = cycleColor(currentColors);
			line.pos+=10; 
			if (line.pos>160) {
				
				switch (player.y) {
					case 45:
						switch (line.right) {
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
					case 100:
						switch (line.center) {
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
						switch (line.left) {
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
				
				if (life <= 0) gameover++;
									
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
		
		if (score > 100) speed = 0;
		else if (score > 50) speed = 1;
		else if (score > 30) speed = 2;
		else if (score > 20) speed = 3;
		else if (score > 10) speed = 4;
		else if (score > 5) speed = 5;
		else speed = 6;
		
		WaitForVBlank();
	}
	int i; for (i=0;i<25;i+=4) oamSetVisible(i, OBJ_HIDE);
	setFadeEffect(FADE_OUT);
	doPause();
	return score;
}
