ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

#AUDIOFILES := res/pollen8.it
#export SOUNDBANK := res/soundbank

.PHONY: all

# ROMNAME is used in snes_rules file
export ROMNAME := ac2016

#all: musics bitmaps $(ROMNAME).sfc
all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx cleanAudio

# to build musics, define SMCONVFLAGS with parameters you want
#SMCONVFLAGS	:= -s -o $(SOUNDBANK) -v -b 5
#musics: $(SOUNDBANK).obj

snesfont.pic: snesfont.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -n -gs8 -po4 -pc16 -pe1 -mR! -m! -p! $<
	
snesfont-rot.pic: snesfont-rot.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -n -gs8 -po4 -pc16 -pe1 -mR! -m! -p! $<

bg1.pic: bg1.bmp
	@echo convert without rearranging color order ... $(notdir $@)
	$(GFXCONV) -pc16 -n -gs8 -pe0 -fbmp -m $<
	
title.pic: title.bmp
	@echo convert without rearranging color order ... $(notdir $@)
	$(GFXCONV) -pc16 -n -gs8 -pe0 -fbmp -m $<

sprites.pic: sprites.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -gs32 -pc16 -po16 -n $<

bitmaps : bg1.pic snesfont.pic snesfont-rot.pic sprites.pic title.pic
