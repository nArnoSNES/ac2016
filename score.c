/*---------------------------------------------------------------------------------


	AC2016 - printScore()
		-- n_Arno


---------------------------------------------------------------------------------*/
#include <snes.h>
#include "common.h"

extern char snesfont_rot;

//---------------------------------------------------------------------------------
void printScore(int scorefinal) {
	unsigned short pad0;
	char scoreStr[3];
	int scoreCent = 0;
	int scoreTen = 0;
	int scoreUnit = 0;
    
	consoleInitText(0, 0, &snesfont_rot);
	consoleSetShadowCol(0,RGB15(16,16,16));

	setMode(BG_MODE1,0);  bgSetDisable(1);  bgSetDisable(2);

	int posy=17;
	consoleDrawText(8,posy--,"S");
	consoleDrawText(8,posy--,"c");
	consoleDrawText(8,posy--,"o");
	consoleDrawText(8,posy--,"r");
	consoleDrawText(8,posy--,"e");
	consoleDrawText(8,posy--,":");

	scoreCent = scorefinal/100;
	sprintf(scoreStr,"%u",scoreCent);
	consoleDrawText(10,15,scoreStr);
	
	scoreTen = (scorefinal-(scoreCent*100))/10;
	sprintf(scoreStr,"%u",scoreTen);
	consoleDrawText(10,14,scoreStr);
	
	scoreUnit = (scorefinal-(scoreCent*100)-(scoreTen*10));
	sprintf(scoreStr,"%u",scoreUnit);
	consoleDrawText(10,13,scoreStr);

	pad0 = padsCurrent(0);
	while(!pad0) {
		pad0 = padsCurrent(0);
		WaitForVBlank();
	}
	return;
}
