/*---------------------------------------------------------------------------------


	AC2016 - printScore()
		-- n_Arno


---------------------------------------------------------------------------------*/
#include <snes.h>
#include "common.h"

extern char snesfont_rot;

int delay;
char scoreHolder[3];
char tempHolder[1];

// This display the final score of the player. It will get better looking :D
void printScore(int scorefinal, int highscore) {

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

	sprintf(scoreHolder,"%03u",scorefinal);
	sprintf(tempHolder,"%c",scoreHolder[0]);
	consoleDrawText(13,15,tempHolder);
	sprintf(tempHolder,"%c",scoreHolder[1]);
	consoleDrawText(13,14,tempHolder);
	sprintf(tempHolder,"%c",scoreHolder[2]);
	consoleDrawText(13,13,tempHolder);

	// Sad placeholder for the highscore
	// For now it is not saved in sram
	int posy=21;
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
	
	sprintf(scoreHolder,"%03u",highscore);
	sprintf(tempHolder,"%c",scoreHolder[0]);
	consoleDrawText(25,10,tempHolder);
	sprintf(tempHolder,"%c",scoreHolder[1]);
	consoleDrawText(25,9,tempHolder);
	sprintf(tempHolder,"%c",scoreHolder[2]);
	consoleDrawText(25,8,tempHolder);

	// Wait a bit before going back to title
	delay = 0;
	while(delay<400) {
		delay++;
		WaitForVBlank();
	}
	return;
}
