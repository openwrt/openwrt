# for SGlinux builds
-include $(ROOTDIR)/modules/.config

OCF_OBJS = crypto.o criov.o

ifdef CONFIG_OCF_RANDOMHARVEST
	OCF_OBJS += random.o
endif

ifdef CONFIG_OCF_FIPS
	OCF_OBJS += rndtest.o
endif

# Add in autoconf.h to get #defines for CONFIG_xxx
AUTOCONF_H=$(ROOTDIR)/modules/autoconf.h
ifeq ($(AUTOCONF_H), $(wildcard $(AUTOCONF_H)))
	EXTRA_CFLAGS += -include $(AUTOCONF_H)
	export EXTRA_CFLAGS
endif

ifndef obj
	obj ?= .
	_obj = subdir
	mod-subdirs := safe hifn ixp4xx talitos ocfnull
	export-objs += crypto.o criov.o random.o
	list-multi += ocf.o
	_slash :=
else
	_obj = obj
	_slash := /
endif

EXTRA_CFLAGS += -I$(obj)/.

obj-$(CONFIG_OCF_OCF)         += ocf.o
obj-$(CONFIG_OCF_CRYPTODEV)   += cryptodev.o
obj-$(CONFIG_OCF_CRYPTOSOFT)  += cryptosoft.o
obj-$(CONFIG_OCF_BENCH)       += ocf-bench.o

$(_obj)-$(CONFIG_OCF_SAFE)    += safe$(_slash)
$(_obj)-$(CONFIG_OCF_HIFN)    += hifn$(_slash)
$(_obj)-$(CONFIG_OCF_IXP4XX)  += ixp4xx$(_slash)
$(_obj)-$(CONFIG_OCF_TALITOS) += talitos$(_slash)
$(_obj)-$(CONFIG_OCF_PASEMI)  += pasemi$(_slash)
$(_obj)-$(CONFIG_OCF_EP80579) += ep80579$(_slash)
$(_obj)-$(CONFIG_OCF_CRYPTOCTEON) += cryptocteon$(_slash)
$(_obj)-$(CONFIG_OCF_KIRKWOOD) += kirkwood$(_slash)
$(_obj)-$(CONFIG_OCF_OCFNULL) += ocfnull$(_slash)
$(_obj)-$(CONFIG_OCF_C7108) += c7108$(_slash)
$(_obj)-$(CONFIG_OCF_UBSEC_SSB) += ubsec_ssb$(_slash)

ocf-objs := $(OCF_OBJS)

dummy:
	@echo "Please consult the README for how to build OCF."
	@echo "If you can't wait then the following should do it:"
	@echo ""
	@echo "    make ocf_modules"
	@echo "    sudo make ocf_install"
	@echo ""
	@exit 1
	
$(list-multi) dummy1: $(ocf-objs)
	$(LD) -r -o $@ $(ocf-objs)

.PHONY:
clean:
	rm -f *.o *.ko .*.o.flags .*.ko.cmd .*.o.cmd .*.mod.o.cmd *.mod.c
	rm -f */*.o */*.ko */.*.o.cmd */.*.ko.cmd */.*.mod.o.cmd */*.mod.c */.*.o.flags
	rm -f */modules.order */modules.builtin modules.order modules.builtin

ifdef TOPDIR
-include $(TOPDIR)/Rules.make
endif

#
# targets to build easily on the current machine
#

ocf_make:
	make -C /lib/modules/$(shell uname -r)/build M=`pwd` $(OCF_TARGET) CONFIG_OCF_OCF=m
	make -C /lib/modules/$(shell uname -r)/build M=`pwd` $(OCF_TARGET) CONFIG_OCF_OCF=m CONFIG_OCF_CRYPTOSOFT=m
	-make -C /lib/modules/$(shell uname -r)/build M=`pwd` $(OCF_TARGET) CONFIG_OCF_OCF=m CONFIG_OCF_BENCH=m
	-make -C /lib/modules/$(shell uname -r)/build M=`pwd` $(OCF_TARGET) CONFIG_OCF_OCF=m CONFIG_OCF_OCFNULL=m
	-make -C /lib/modules/$(shell uname -r)/build M=`pwd` $(OCF_TARGET) CONFIG_OCF_OCF=m CONFIG_OCF_HIFN=m

ocf_modules:
	$(MAKE) ocf_make OCF_TARGET=modules

ocf_install:
	$(MAKE) ocf_make OCF_TARGET="modules modules_install"
	depmod
	mkdir -p /usr/include/crypto
	cp cryptodev.h /usr/include/crypto/.

#
# generate full kernel patches for 2.4 and 2.6 kernels to make patching
# your kernel easier
#

.PHONY: patch
patch:
	patchbase=.; \
		[ -d $$patchbase/patches ] || patchbase=..; \
		patch=ocf-linux-base.patch; \
		patch24=ocf-linux-24.patch; \
		patch26=ocf-linux-26.patch; \
		( \
			find . -name Makefile; \
			find . -name Config.in; \
			find . -name Kconfig; \
			find . -name README; \
			find . -name '*.[ch]' | grep -v '.mod.c'; \
		) | while read t; do \
			diff -Nau /dev/null $$t | sed 's?^+++ \./?+++ linux/crypto/ocf/?'; \
		done > $$patch; \
		cat $$patchbase/patches/linux-2.4.35-ocf.patch $$patch > $$patch24; \
		cat $$patchbase/patches/linux-2.6.38-ocf.patch $$patch > $$patch26; \

#
# this target probably does nothing for anyone but me - davidm
#

.PHONY: release
release:
	REL=`date +%Y%m%d`; RELDIR=/tmp/ocf-linux-$$REL; \
		CURDIR=`pwd`; \
		rm -rf /tmp/ocf-linux-$$REL*; \
		mkdir -p $$RELDIR/ocf; \
		mkdir -p $$RELDIR/patches; \
		mkdir -p $$RELDIR/crypto-tools; \
		cp README* $$RELDIR/.; \
		cp patches/[!C]* $$RELDIR/patches/.; \
		cp tools/[!C]* $$RELDIR/crypto-tools/.; \
		cp -r [!C]* Config.in $$RELDIR/ocf/.; \
		rm -rf $$RELDIR/ocf/patches $$RELDIR/ocf/tools; \
		rm -f $$RELDIR/ocf/README*; \
		cp $$CURDIR/../../user/crypto-tools/[!C]* $$RELDIR/crypto-tools/.; \
		make -C $$RELDIR/crypto-tools clean; \
		make -C $$RELDIR/ocf clean; \
		find $$RELDIR/ocf -name CVS | xargs rm -rf; \
		cd $$RELDIR/..; \
		tar cvf ocf-linux-$$REL.tar ocf-linux-$$REL; \
		gzip -9 ocf-linux-$$REL.tar

