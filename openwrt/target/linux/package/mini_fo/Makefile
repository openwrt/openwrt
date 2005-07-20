# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := mini_fo
PKG_VERSION := 0.6.1-pre1
PKG_RELEASE := 1
PKG_MD5SUM := 7e192e01b08023c7f3e63bf1c796bc14

PKG_SOURCE_URL := http://www.denx.de/twiki/pub/Know/MiniFOHome/
PKG_SOURCE := mini_fo-0-6-1-pre1.tar.bz2
PKG_CAT := bzcat

PKG_BUILD_DIR := $(BUILD_DIR)/mini_fo
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

ifeq ($(KERNEL_DIR),)
KERNEL_DIR:=$(LINUX_DIR)
endif
ifeq ($(LINUX_VERSION),2.4.30)
KERNEL_VERSION=2.4
else
KERNEL_VERSION=2.6
endif

KERNEL_FLAGS=-D__KERNEL__ -DMODULE -DFISTGEN -I. -I$(KERNEL_DIR)/include \
 	-Wno-unused -fno-common -fno-schedule-insns -fno-schedule-insns2 \
	-fomit-frame-pointer -finline-limit=100000 -mabi=32 -march=mips32 \
	-Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap -DMODULE -mlong-calls \
	-fno-strict-aliasing -G 0 -mno-abicalls -fno-pic

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_MINI_FO,kmod-mini-fo,$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(KERNEL_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS) $(KERNEL_FLAGS)" \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_CROSS)ld" \
		KERNELVERSION="$(KERNEL_VERSION)" \
		KERNEL_SRC="$(KERNEL_DIR)" \
		ROOT="$(PKG_INSTALL_DIR)"
	touch $@

$(IPKG_KMOD_MINI_FO):
	install -m0755 -d $(IDIR_KMOD_MINI_FO)/lib/modules/$(LINUX_VERSION)
	install -m0755 -d $(IDIR_KMOD_MINI_FO)/etc/modules.d
	cp -fpR $(PKG_BUILD_DIR)/mini_fo.o \
	 $(IDIR_KMOD_MINI_FO)/lib/modules/$(LINUX_VERSION)/
	echo "mini_fo" > $(IDIR_KMOD_MINI_FO)/etc/modules.d/15-mini-fo
	$(RSTRIP_KMOD) $(IDIR_KMOD_MINI_FO)
	$(IPKG_BUILD) $(IDIR_KMOD_MINI_FO) $(PACKAGE_DIR)

