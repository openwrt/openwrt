# Makefile for the diag module
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := kmod-diag
PKG_RELEASE := 1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_DIAG,$(PKG_NAME),$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))

DIAG_FLAGS:=$(TARGET_CFLAGS) -D__KERNEL__ -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 \
		-mno-abicalls -fno-pic -finline-limit=100000 -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 \
		-Wa,-mips32 -Wa,--trap -Wstrict-prototypes -Wno-trigraphs -DMODULE -mlong-calls -fno-common -I. -I linux-2.4 -nostdinc \
		-iwithprefix include -I$(LINUX_DIR)/include -I$(LINUX_DIR)/include/asm/gcc -c -I$(LINUX_DIR)/arch/mips/bcm947xx/include -DBCMDRIVER

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $(PKG_BUILD_DIR)
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(TARGET_CC) $(DIAG_FLAGS) -o $(PKG_BUILD_DIR)/diag.o diag_led.c
	touch $@
	
$(IPKG_KMOD_DIAG):
	mkdir -p $(IDIR_KMOD_DIAG)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/diag.o $(IDIR_KMOD_DIAG)/lib/modules/$(LINUX_VERSION)/
	$(IPKG_BUILD) $(IDIR_KMOD_DIAG) $(PACKAGE_DIR)
