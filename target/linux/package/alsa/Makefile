# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME:=alsa-driver
PKG_VERSION:=1.0.10rc1
PKG_RELEASE:=1
PKG_MD5SUM:=0fb6b4163c3ed8f4930f00791b8a25c1

PKG_SOURCE_URL:=ftp://ftp.alsa-project.org/pub/driver/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

ifeq ($(KERNEL_DIR),)
KERNEL_DIR:=$(LINUX_DIR)
endif

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_ALSA,kmod-alsa,$(LINUX_VERSION)$(BOARD)+$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH),kernel-$(LINUX_VERSION)-$(BOARD) ($(KERNEL_RELEASE))))

$(PKG_BUILD_DIR)/.configured:	$(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); \
		./configure \
		--with-kernel=$(KERNEL_DIR) \
		--with-cross=$(KERNEL_CROSS) \
		--with-oss=yes \
		--with-isapnp=no \
		--with-sequencer=no \
		--with-cards=usb-audio \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $@

$(IPKG_KMOD_ALSA):
	install -d -m0755 $(IDIR_KMOD_ALSA)/etc/modules.d
	install -m0644 ./files/alsa.modules $(IDIR_KMOD_ALSA)/etc/modules.d/70-alsa
	install -d -m0755 $(IDIR_KMOD_ALSA)/lib/modules/$(LINUX_VERSION)
	install -m0644 $(PKG_BUILD_DIR)/modules/*.o \
		$(IDIR_KMOD_ALSA)/lib/modules/$(LINUX_VERSION)/
	$(IPKG_BUILD) $(IDIR_KMOD_ALSA) $(PACKAGE_DIR)
