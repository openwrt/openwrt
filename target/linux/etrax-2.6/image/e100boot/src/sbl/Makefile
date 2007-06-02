# Generated automatically from Makefile.in by configure.
#
# Makefile for e100boot, sbl side.
#
# $Id: Makefile.in,v 1.21 2003/11/21 11:38:30 pkj Exp $
#

VPATH := .:../cbl

CC = gcc

AXIS_USABLE_LIBS = UCLIBC GLIBC
-include $(AXIS_TOP_DIR)/tools/build/Rules.axis

CFLAGS += -I. -I../cbl/src -g -O2 -I../libpcap-0.4/ -I../libpcap-0.4/bpf/ -Wall -Wmissing-prototypes -Wno-unused
LDFLAGS =
LDLIBS  = -L../libpcap-0.4/ 
LDLIBS  =
LIBPCAP = ../libpcap-0.4/libpcap.a

IMAGES := ../cbl/net/net.ima ../cbl/net_noleds/net_noleds.ima \
	  ../cbl/ser/ser.ima ../cbl/ser_noleds/ser_noleds.ima

AUTOSRCS := $(addsuffix .c, $(subst .ima,_ima,$(notdir $(IMAGES))))

OBJS := common.o network.o serial.o
DEPS := $(addprefix deps/, $(subst .o,.d,$(OBJS)))

STRIP ?= strip

all:	conf.h e100boot e100boot.stripped

depend: $(DEPS)

Makefile: Makefile.in
	@echo -e "\n### New Makefile.in detected, regenerating Makefile..."
	./configure

e100boot: common.o boot_images.o serial.o network.o $(LIBPCAP)
	@echo -e '\n### Finally... Compiling e100boot.'
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

e100boot.stripped:	e100boot
	cp $< $@
	$(STRIP) --strip-unneeded $@

boot_images.o: boot_images.c boot_images.h $(AUTOSRCS)
	$(CC) -c $(CFLAGS) $< -o $@

conf.h:
	./configure

reconf:
	./configure --cache=/dev/null

clean:
	rm -rf e100boot Makefile conf.h config.log config.cache config.status deps *.ima *.o *_ima.c* *~

$(IMAGES):
	@echo -e '\n### Making cbl boot images (*.ima)...'
	$(MAKE) -C ../cbl

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

deps/%.d: %.c
	@[ -d deps ] || mkdir deps
	$(CC) -M $(CFLAGS) $< > $@

deps/%.d: %.S
	@[ -d deps ] || mkdir deps
	$(CC) -M $(CFLAGS) $< > $@

%_ima.c: %.ima
	@echo -e "### Creating $@ from image $<"
	@./cconv $*_ima < $< > $@
