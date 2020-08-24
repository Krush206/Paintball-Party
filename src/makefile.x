CC = gcc
CFLAGS = -O2 -fomit-frame-pointer -ffast-math -march=pentium -funroll-loops -DUSE_ALLEGTTF
LIBS = -laldmb -ldumb -lalmp3 -lalogg -lalleg
ELIBS = -lalleg

GAMEOFILES = main.o data.o logo.o title.o game.o stats.o animate.o collide.o tilemap.o font.o level.o control.o player.o joy.o paintbal.o particle.o objects.o replay.o demo.o ncds.o aarot.o aastr.o aautil.o round.o message.o gametime.o mygui.o 
GAMEEXENAME = pp
CHAROFILES = animate.o chared.o itools.o cicon.o ncds.o wincb.o loadpng.o mygui.o
CHAREXENAME = chared
LEVEDOFILES = levedit.o tilemap.o itools.o level.o animate.o collide.o mygui.o
LEVEDEXENAME = levedit
ATTFOFILES = allegttf/source/aaclip.o allegttf/source/aaload1.o allegttf/source/aaload2.o allegttf/source/aaload3.o allegttf/source/aaload4.o allegttf/source/aamap.o allegttf/source/aasetup.o allegttf/source/aatext1x.o allegttf/source/aatext8.o allegttf/source/aatext15.o allegttf/source/aatext16.o allegttf/source/aatext24.o allegttf/source/aatext32.o allegttf/source/aatext.o allegttf/source/freetype/ttapi.o allegttf/source/freetype/ttcache.o allegttf/source/freetype/ttcalc.o allegttf/source/freetype/ttcmap.o allegttf/source/freetype/ttapi.o allegttf/source/freetype/ttdebug.o allegttf/source/freetype/ttextend.o allegttf/source/freetype/ttfile.o allegttf/source/freetype/ttgload.o allegttf/source/freetype/ttinterp.o allegttf/source/freetype/ttload.o allegttf/source/freetype/ttmemory.o allegttf/source/freetype/ttmutex.o allegttf/source/freetype/ttobjs.o allegttf/source/freetype/ttraster.o

all : ../bin/$(GAMEEXENAME) ../bin/$(CHAREXENAME) ../bin/$(LEVEDEXENAME)
	@echo All targets built!

../bin/$(GAMEEXENAME) : $(GAMEOFILES) libalttf.a
	gcc $(GAMEOFILES) libalttf.a $(CFLAGS) $(LIBS) -o ../bin/$(GAMEEXENAME)
#	upx ../bin/pp.exe

../bin/$(CHAREXENAME) : $(CHAROFILES)
	gcc $(CHAROFILES) $(CFLAGS) $(LIBS) -lpng -lz -o ../bin/$(CHAREXENAME)
#	upx ../bin/chared.exe

../bin/$(LEVEDEXENAME) : $(LEVEDOFILES)
	gcc $(LEVEDOFILES) -o ../bin/$(LEVEDEXENAME) -O2 $(ELIBS)
#	upx ../bin/levedit.exe

libalttf.a : $(ATTFOFILES)
	ar rs libalttf.a $(ATTFOFILES)

clean:
	@rm *.o
	@rm *.a
	@rm allegttf\source\*.o
	@rm allegttf\source\freetype\*.o
	@echo Cleanup complete!
