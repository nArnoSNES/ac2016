/*---------------------------------------------------------------------------------


	AC2016 - splash()
		-- n_Arno


---------------------------------------------------------------------------------*/
#include <snes.h>
#include "common.h"

extern char snesfont;
int counter = 0;

//---------------------------------------------------------------------------------
void splash(void) {
   
	consoleInitText(0, 0, &snesfont);
	consoleSetShadowCol(0,RGB15(16,16,16));

	setMode(BG_MODE1,0);  bgSetDisable(1);  bgSetDisable(2);

	consoleDrawText(12,10,"WARNING !");
	consoleDrawText(1,14,"This is game is played in Tate");
	consoleDrawText(2,16,"(Screen should be vertical)");

	while(counter<400) {
		counter++;
		WaitForVBlank();
	}

	consoleDrawText(12,10,"         ");
	consoleDrawText(1,14,"                              ");
	consoleDrawText(2,16,"                           ");
	return;
}
