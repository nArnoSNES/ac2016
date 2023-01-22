
#define OBJ_SIZE32	(5<<5)
#define RGB15(r,g,b)  ((r)|((g)<<5)|((b)<<10))

void consoleSetShadowCol(u8 paletteNumber, u16 colorShad);
void consoleSetTextCol(u16  colorChar, u16 colorBG);
void oldConsoleInitText(u8 bgNumber,u8 paletteNumber, u8 *gfxText);
