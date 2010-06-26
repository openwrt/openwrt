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

ocf-objs := $(OCF_OBJS)

$(list-multi) dummy1: $(ocf-objs)
	$(LD) -r -o $@ $(ocf-objs)

.PHONY:
clean:
	rm -f *.o *.ko .*.o.flags .*.ko.cmd .*.o.cmd .*.mod.o.cmd *.mod.c
	rm -f */*.o */*.ko */.*.o.cmd */.*.ko.cmd */.*.mod.o.cmd */*.mod.c */.*.o.flags

ifdef TOPDIR
-include $(TOPDIR)/Rules.make
endif

#
# release gen targets
#

.PHONY: patch
patch:
	REL=`date +%Y%m%d`; \
		patch=ocf-linux-$$REL.patch; \
		patch24=ocf-linux-24-$$REL.patch; \
		patch26=ocf-linux-26-$$REL.patch; \
		( \
			find . -name Makefile; \
			find . -name Config.in; \
			find . -name Kconfig; \
			find . -name README; \
			find . -name '*.[ch]' | grep -v '.mod.c'; \
		) | while read t; do \
			diff -Nau /dev/null $$t | sed 's?^+++ \./?+++ linux/crypto/ocf/?'; \
		done > $$patch; \
		cat patches/linux-2.4.35-ocf.patch $$patch > $$patch24; \
		cat patches/linux-2.6.33-ocf.patch $$patch > $$patch26

.PHONY: tarball
tarball:
	REL=`date +%Y%m%d`; RELDIR=/tmp/ocf-linux-$$REL; \
		CURDIR=`pwd`; \
		rm -rf /tmp/ocf-linux-$$REL*; \
		mkdir -p $$RELDIR/tools; \
		cp README* $$RELDIR; \
		cp patches/openss*.patch $$RELDIR; \
		cp patches/crypto-tools.patch $$RELDIR; \
		cp tools/[!C]* $$RELDIR/tools; \
		cd ..; \
		tar cvf $$RELDIR/ocf-linux.tar \
					--exclude=CVS \
					--exclude=.* \
					--exclude=*.o \
					--exclude=*.ko \
					--exclude=*.mod.* \
					--exclude=README* \
					--exclude=ocf-*.patch \
					--exclude=ocf/patches/openss*.patch \
					--exclude=ocf/patches/crypto-tools.patch \
					--exclude=ocf/tools \
					ocf; \
		gzip -9 $$RELDIR/ocf-linux.tar; \
		cd /tmp; \
		tar cvf ocf-linux-$$REL.tar ocf-linux-$$REL; \
		gzip -9 ocf-linux-$$REL.tar; \
		cd $$CURDIR/../../user; \
		rm -rf /tmp/crypto-tools-$$REL*; \
		tar cvf /tmp/crypto-tools-$$REL.tar \
					--exclude=CVS \
					--exclude=.* \
					--exclude=*.o \
					--exclude=cryptotest \
					--exclude=cryptokeytest \
					crypto-tools; \
		gzip -9 /tmp/crypto-tools-$$REL.tar

