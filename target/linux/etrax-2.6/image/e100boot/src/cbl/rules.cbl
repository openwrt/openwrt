#
# $Id: rules.cbl,v 1.4 2005/04/20 11:18:54 starvik Exp $
#

VPATH     := .:../src

ifeq ($(findstring _noleds,$(IMAGE)),_noleds)
USE_LEDS := 0
else
USE_LEDS := 1
endif

MAKEFLAGS := r
INCS      := -I.
DEFS      := -DUSE_LEDS=$(USE_LEDS)
CFLAGS    := -Os -Wall -Wmissing-prototypes -W -Wa,-N -nostdinc $(INCS) $(DEFS) -MMD
ASFLAGS   := $(CFLAGS)
LNFLAGS   := -Wl,--section-start,.startup=0x380000f0 -nostdlib -Os -Wl,-T../src/ldscript
CC        := cris-axis-elf-gcc 

OUT := $(patsubst %.ima,%.out,$(IMAGE))

OBJS  = crt0.o common_init.o
ifeq ($(findstring net,$(IMAGE)),net)
OBJS += net_init.o
else
OBJS += ser_init.o
endif
OBJS += common.o flash.o hwregs.o e100boot_version.o

all:	../$(IMAGE)

$(OUT):	$(OBJS) ldscript
	$(CC) -o $@ $(LNFLAGS) $(OBJS)

../%.ima: %.out
	bin-cris -o $@ $<
	@find $@ -printf '# Size of image $@ is %s bytes.\n\n'

$(OBJS):	../rules.cbl Makefile

# We don't want this to be a dummy and be recreated every time we build,
# only after a make clean
e100boot_version.c:
	@echo "Generating version file....."
	@echo 'char e100boot_version[] = "This bootloader was built by '`id -u -n`' on '`date`'.\r\n";' > $@

dummy:

clean:
	rm -rf *.o *.d *.out *.ima deps e100boot_version.c

ifneq ($(MAKECMDGOALS),clean)
-include *.d
endif
