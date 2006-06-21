# $Id: Makefile 3822 2006-05-24 07:01:52Z nico $

include $(TOPDIR)/rules.mk

PKG_NAME:=util-linux
PKG_VERSION:=2.12r
PKG_RELEASE:=1
PKG_MD5SUM:=c261230b27fc0fbcc287c76884caf2d3

PKG_SOURCE_URL:=ftp://ftp.kernel.org/pub/linux/utils/$(PKG_NAME)/ \
	http://ftp.kernel.org/pub/linux/utils/$(PKG_NAME)/ \
	ftp://ftp.de.kernel.org/pub/linux/utils/$(PKG_NAME)/ \
	http://ftp.de.kernel.org/pub/linux/utils/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/include/package.mk

define Package/fdisk
SECTION:=base
CATEGORY:=Utilities
TITLE:=Partition table manipulation utility
DESCRIPTION:=A partition table manipulation utility
URL:=http://www.kernel.org/pub/linux/utils/util-linux/
endef

define Package/losetup
$(call Package/fdisk)
TITLE:=Loopback devices setup and control utility
DESCRIPTION:=A loopback devices setup and control utility
endef

define Package/swap-utils
$(call Package/fdisk)
TITLE:=Swap space management utilities
DESCRIPTION:=A collection of tools to manage swap space \\\
    * mkswap\\\
    * swapon\\\
    * swapoff
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		OPT="$(TARGET_CFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		INSTALLSUID="install -m 4755" \
		all install
endef

define Package/Template
	@if [ \! -f "$(PKG_INSTALL_DIR)/$(1)" ]; then \
		rm -f $(PKG_BUILD_DIR)/.built; \
		$(MAKE) $(PKG_BUILD_DIR)/.built; \
	fi
	install -d -m0755 $(2)
	$(CP) $(PKG_INSTALL_DIR)/$(1) $(2)/
endef

define Package/fdisk/install
	$(call Package/Template,sbin/fdisk,$(1)/usr/sbin)
endef
define Package/losetup/install
	$(call Package/Template,sbin/losetup,$(1)/usr/sbin)
endef
define Package/swap-utils/install
	$(call Package/Template,sbin/mkswap,$(1)/usr/sbin)
	$(call Package/Template,sbin/swapon,$(1)/usr/sbin)
	$(call Package/Template,sbin/swapoff,$(1)/usr/sbin)
endef
$(eval $(call BuildPackage,fdisk))
$(eval $(call BuildPackage,losetup))
$(eval $(call BuildPackage,swap-utils))
