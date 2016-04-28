/*---------------------------------------------------------------------------------

	AC2016 - main()
		-- n_Arno

---------------------------------------------------------------------------------*/
#include <snes.h>
#include "main.h"

//---------------------------------------------------------------------------------
int main(void) {
	consoleInit();

	// As Usual, don't exit the main()
	while(1) {
		// Display splash screen
		splash();

		// Init score
		int hs = 0;

		// Execute main function
		hs = play();

		// Display score in a screen
		printScore(hs);

		// Just in case
		WaitForVBlank();
	}
	return 0;
}
