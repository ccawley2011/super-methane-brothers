SRCDIR ?= ..

METHANE_FLAGS = -mlibscl -I${SRCDIR} -I${SRCDIR}/riscos -DMETHANE_RISCOS -DENABLE_SOUND
METHANE_LIBS = -mlibscl
CROSS = arm-unknown-riscos-
ZIP = $(GCCSDK_INSTALL_ENV)/bin/zip -,

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

all: message !smb/!RunImage,ff8

message:
	@echo "Compiling Super Methane Brothers."
	@echo "================================="
	@mkdir -p build
	@mkdir -p build/data
	@mkdir -p build/riscos

!smb/!RunImage,ff8: ${OBJF}
	@mkdir -p !smb
	${CROSS}g++ ${CXXFLAGS} ${OBJF}	-o $@ ${METHANE_LIBS}

smbgame.zip: !smb/!RunImage,ff8
	@${RM} $@
	${ZIP} -9r $@ !smb

clean:
	@${RM} -R build
	@${RM} !smb/!RunImage,ff8

distclean: clean


# The main source code
build/%.o : ${SRCDIR}/%.cpp
	${CROSS}g++ ${CXXFLAGS} ${METHANE_FLAGS} -c $< -o $@


