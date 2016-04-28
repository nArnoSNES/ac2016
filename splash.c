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

	// OK, this one need some explanation.
	// The rand() function of the framework use a seed to generate pseudo-number
	// But with the unmodified PVSNESLIB framework, the seed is ALWAYS the same.
	// So, it will look random once, but it will always be the same set of num
	// between console reset.
	// I modified the console.c source of the framework to add a function to
	// set the seed. I wait for the player to press start and use the number
	// of vertical blanking since reset as seed. Yeah, a little bit better!
	srand(snes_vblank_count);
	consoleDrawText(12,10,"         ");
	consoleDrawText(1,14,"                              ");
	consoleDrawText(2,16,"                           ");
	consoleDrawText(8,19,"                ");
	return;
}
