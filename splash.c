/*---------------------------------------------------------------------------------


	AC2016 - splash()
		-- n_Arno


---------------------------------------------------------------------------------*/
#include <snes.h>
#include "common.h"

extern char snesfont;

//---------------------------------------------------------------------------------
void splash(void) {
	unsigned short pad0;
    
	consoleInitText(0, 0, &snesfont);
	consoleSetShadowCol(0,RGB15(16,16,16));

	setMode(BG_MODE1,0);  bgSetDisable(1);  bgSetDisable(2);

	consoleDrawText(12,10,"WARNING !");
	consoleDrawText(1,14,"This is game is played in Tate");
	consoleDrawText(2,16,"(Screen should be vertical)");
	consoleDrawText(8,19,"Press any button");

	pad0 = padsCurrent(0);
	while(!pad0) {
		pad0 = padsCurrent(0);
		WaitForVBlank();
	}
	srand(snes_vblank_count);
	consoleDrawText(12,10,"         ");
	consoleDrawText(1,14,"                              ");
	consoleDrawText(2,16,"                           ");
	consoleDrawText(8,19,"                ");
	return;
}
