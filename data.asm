.include "hdr.asm"

.section ".rodata1" superfree

bg1_tiles:
.incbin "bg1.pic"
bg1_tiles_end:

sprites:
.incbin "sprites.pic"
sprites_end:

.ends

.section ".rodata2" superfree

bg1_map:
.incbin "bg1.map"
bg1_map_end:

bg1_pal:
.incbin "bg1.pal"
bg1_pal_end:

sprites_pal:
.incbin "sprites.pal"
sprites_pal_end:

.ends

.section ".rodata3" superfree

snesfont: .incbin "snesfont.pic"

snesfont_rot: .incbin "snesfont-rot.pic"

.ends
