.include "hdr.asm"

.section ".rodata1" superfree

bg1_tiles:
.incbin "bg1.pic" /* 12320 */
bg1_tiles_end:

title_tiles:
.incbin "title.pic" /* 19936 */
title_tiles_end:

sprites_pal:
.incbin "sprites.pal" /* 32 */
sprites_pal_end:

.ends

.section ".rodata2" superfree

bg1_map:
.incbin "bg1.map" /* 1792 */
bg1_map_end:

bg1_pal:
.incbin "bg1.pal" /* 512 */
bg1_pal_end:

sprites:
.incbin "sprites.pic" /* 6144 */
sprites_end:

title_map:
.incbin "title.map" /* 1792 */
title_map_end:

title_pal:
.incbin "title.pal" /* 512 */
title_pal_end:

snesfont_rot: .incbin "snesfont-rot.pic" /* 3072 */

snesfont: .incbin "snesfont.pic" /* 3072 */

.ends
