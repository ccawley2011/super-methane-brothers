SRCDIR ?= ..

METHANE_FLAGS = -mlibscl -I${SRCDIR} -I${SRCDIR}/riscos -DMETHANE_RISCOS -DENABLE_SOUND
METHANE_LIBS = -mlibscl

ifeq ($(MAKE_HOST),arm-unknown-riscos)
  ZIP = zip
  RM = remove
  RM_R = X wipe $1 ~CFR~V
  MKDIR = cdir
  / = .
  . = /
else
  CROSS = arm-unknown-riscos-
  ZIP = $(GCCSDK_INSTALL_ENV)/bin/zip -,
  EXEEXT = ,ff8
  RM_R = $(RM) -R $1
  MKDIR = mkdir -p
  / = /
  . = .
endif

OBJF =	build/riscos/doc.o \
	build/riscos/audiodrv.o \
	build/riscos/swicalls.o \
	build/gfxoff.o \
	build/baddie.o \
	build/bitdraw.o \
	build/bitgroup.o \
	build/bititem.o \
	build/boss.o \
	build/game.o \
	build/gasobj.o \
	build/global.o \
	build/goodie.o \
	build/maps.o \
	build/misc.o \
	build/objlist.o \
	build/player.o \
	build/power.o \
	build/target.o \
	build/suck.o \
	build/weapon.o \
	build/sound.o \
	build/data/snddata.o \
	build/data/gfxdata.o \
	build/data/gfxdata2.o \
	build/data/gfxdata3.o \
	build/data/mapdata.o

all: message !smb/!RunImage$(EXEEXT)

message:
	@echo "Compiling Super Methane Brothers."
	@echo "================================="
	@$(MKDIR) build
	@$(MKDIR) build$(/)data
	@$(MKDIR) build$(/)riscos

!smb/!RunImage$(EXEEXT): ${OBJF}
	${CROSS}g++ ${CXXFLAGS} ${OBJF}	-o $@ ${METHANE_LIBS}

smbgame.zip: !smb/!RunImage$(EXEEXT)
	@${RM} smbgame$(.)zip
	${ZIP} -9r smbgame$(.)zip !smb

clean:
	@$(call RM_R, build)
	@${RM} !smb$(/)!RunImage$(EXEEXT)

distclean: clean


# The main source code
build/%.o : ${SRCDIR}/%.cpp
	${CROSS}g++ ${CXXFLAGS} ${METHANE_FLAGS} -c $< -o $@


