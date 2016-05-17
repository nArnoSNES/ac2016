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

	int posy=19;
	consoleDrawText(5,posy--,"G");
	consoleDrawText(5,posy--,"A");
	consoleDrawText(5,posy--,"M");
	consoleDrawText(5,posy--,"E");
	consoleDrawText(5,posy--," ");
	consoleDrawText(5,posy--," ");
	consoleDrawText(5,posy--,"O");
	consoleDrawText(5,posy--,"V");
	consoleDrawText(5,posy--,"E");
	consoleDrawText(5,posy--,"R");

	int posy=17;
	consoleDrawText(11,posy--,"S");
	consoleDrawText(11,posy--,"c");
	consoleDrawText(11,posy--,"o");
	consoleDrawText(11,posy--,"r");
	consoleDrawText(11,posy--,"e");
	consoleDrawText(11,posy--," ");
	consoleDrawText(11,posy--,":");

	scoreCent = scorefinal/100;
	sprintf(scoreStr,"%u",scoreCent);
	consoleDrawText(13,15,scoreStr);
	
	scoreTen = (scorefinal-(scoreCent*100))/10;
	sprintf(scoreStr,"%u",scoreTen);
	consoleDrawText(13,14,scoreStr);
	
	scoreUnit = (scorefinal-(scoreCent*100)-(scoreTen*10));
	sprintf(scoreStr,"%u",scoreUnit);
	consoleDrawText(13,13,scoreStr);

	int posy=21;
	consoleDrawText(25,posy--,"n");
	consoleDrawText(25,posy--,"'");
	consoleDrawText(25,posy--,"A");
	consoleDrawText(25,posy--,"r");
	consoleDrawText(25,posy--,"n");
	consoleDrawText(25,posy--,"o");
	consoleDrawText(25,posy--," ");
	consoleDrawText(25,posy--,"P");
	consoleDrawText(25,posy--,"B");
	consoleDrawText(25,posy--,":");
	consoleDrawText(25,posy--," ");
	consoleDrawText(25,posy--,"1");
	consoleDrawText(25,posy--,"1");
	consoleDrawText(25,posy--,"8");

	pad0 = padsCurrent(0);
	while(!pad0) {
		pad0 = padsCurrent(0);
		WaitForVBlank();
	}
	return;
}
