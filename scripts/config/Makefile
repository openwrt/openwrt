# ===========================================================================
# OpenWrt configuration targets
# These targets are used from top-level makefile

# ===========================================================================
# Shared Makefile for the various kconfig executables:
# conf:	  Used for defconfig, oldconfig and related targets
# mconf:  Used for the mconfig target.
#         Utilizes the lxdialog package
# object files used by all kconfig flavours


# Platform specific fixes
#
# FreeBSD
export CFLAGS+=-DKBUILD_NO_NLS

conf-objs	:= conf.o zconf.tab.o
mconf-objs	:= mconf.o zconf.tab.o

clean-files	:= lkc_defs.h qconf.moc .tmp_qtcheck \
		   .tmp_gtkcheck zconf.tab.c lex.zconf.c zconf.hash.c

all: conf mconf lxdialog/lxdialog

lxdialog/lxdialog:
	$(MAKE) -C lxdialog

conf: $(conf-objs)
mconf: $(mconf-objs) 

clean:
	rm -f *.o $(clean-files) conf mconf
	$(MAKE) -C lxdialog clean

zconf.tab.o: lex.zconf.c zconf.hash.c confdata.c

kconfig_load.o: lkc_defs.h

lkc_defs.h: $(src)/lkc_proto.h
	sed < $< > $@ 's/P(\([^,]*\),.*/#define \1 (\*\1_p)/'

zconf.tab.c: zconf.y
lex.zconf.c: zconf.l
zconf.hash.c: zconf.gperf

%.tab.c: %.y
	cp $@_shipped $@ || bison -l -b $* -p $(notdir $*) $<

lex.%.c: %.l
	cp $@_shipped $@ || flex -L -P$(notdir $*) -o$@ $<

%.hash.c: %.gperf
	cp $@_shipped $@ || gperf < $< > $@
