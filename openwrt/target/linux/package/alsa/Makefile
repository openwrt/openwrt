# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME:=alsa-driver
PKG_VERSION:=1.0.11rc4
PKG_RELEASE:=1
PKG_MD5SUM:=8e5794026082a964b09b46f9d881e6ec

PKG_SOURCE_URL:=ftp://ftp.alsa-project.org/pub/driver/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

ifeq ($(LINUX_KARCH),i386)
KERNEL_C_INCS:= -I$(LINUX_DIR)/include/asm-i386/mach-generic -I$(LINUX_DIR)/include/asm-i386/mach-default
KERNEL_C_OPTS:= -Os -mpreferred-stack-boundary=2 -march=i486 -fno-unit-at-a-time
endif
ifeq ($(LINUX_KARCH),mips)
KERNEL_C_INCS:= -I$(LINUX_DIR)/include/asm-mips/mach-generic
KERNEL_C_OPTS:= -Os -G 0 -mno-abicalls -fno-pic -finline-limit=100000 -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap
endif

$(eval $(call PKG_template,KMOD_ALSA,kmod-alsa,$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); \
		CFLAGS="$(KERNEL_C_INCS)" \
		./configure \
		--with-build=$(LINUX_DIR) \
		--with-kernel=$(LINUX_DIR) \
		--with-cross=$(KERNEL_CROSS) \
		--with-redhat=no \
		--with-suse=no \
		--with-oss=yes \
		--with-isapnp=no \
		--with-sequencer=no \
		--with-cards=usb-audio \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		c_opts="$(KERNEL_C_OPTS)" \
		all
	touch $@

$(IPKG_KMOD_ALSA):
	install -d -m0755 $(IDIR_KMOD_ALSA)/etc/modules.d
	install -m0644 ./files/alsa.modules $(IDIR_KMOD_ALSA)/etc/modules.d/70-alsa
	install -d -m0755 $(IDIR_KMOD_ALSA)/lib/modules/$(LINUX_VERSION)
	install -m0644 $(PKG_BUILD_DIR)/modules/*.$(LINUX_KMOD_SUFFIX) \
		$(IDIR_KMOD_ALSA)/lib/modules/$(LINUX_VERSION)/
	$(IPKG_BUILD) $(IDIR_KMOD_ALSA) $(PACKAGE_DIR)
