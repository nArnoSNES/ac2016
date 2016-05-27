/*---------------------------------------------------------------------------------

	AC2016 - main()
		-- n_Arno

---------------------------------------------------------------------------------*/
#include <snes.h>
#include "main.h"

//---------------------------------------------------------------------------------

// Init score and Game mode
int hs = 0;
int sf = 0;
int game_mode = 0;

int main(void) {
	consoleInit();

	// Display splash screen (once)
	splash();

	// As Usual, don't exit the main()
	while(1) {
		// Display title screen
		game_mode = title();

		// Execute main function
		sf = play(game_mode);

		// If final score is higher than high score, update
		if (sf > hs) hs = sf;

		// Display score in a screen
		printScore(sf, hs);

		// Just in case
		WaitForVBlank();
	}
	return 0;
}
