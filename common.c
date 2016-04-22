/*---------------------------------------------------------------------------------

	AC2016 - common lib
		-- n_Arno

---------------------------------------------------------------------------------*/
#include <snes.h>

void consoleSetShadowCol(u8 paletteNumber, u16 colorShad) {
	REG_CGADD = 0x02+(paletteNumber<<4); *CGRAM_PALETTE = colorShad & 0xFF; *CGRAM_PALETTE = colorShad>>8;
}
