# $Id$

include $(TOPDIR)/rules.mk
include $(TOPDIR)/include/kernel.mk

PKG_NAME:=mini_fo
PKG_VERSION:=0.6.2pre1
PKG_RELEASE:=1
PKG_MD5SUM:=acd12157be25d7b3c8df90914049403e

PKG_BUILD_DIR:=$(BUILD_DIR)/mini_fo-0-6-2-pre1
PKG_SOURCE_URL:=http://www.denx.de/twiki/pub/Know/MiniFOHome/
PKG_SOURCE:=mini_fo-0-6-2-pre1.tar.bz2
PKG_CAT:=bzcat

include $(TOPDIR)/include/package.mk

define Package/kmod-mini-fo
SECTION:=drivers
CATEGORY:=Drivers
DEPENDS:=
TITLE:=The mini fanout overlay file system
DESCRIPTION:=The mini fanout overlay file system
URL:=http://www.denx.de/wiki/Know.MiniFOHome
VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		PATH="$(TARGET_PATH)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		KERNELVERSION="$(KERNEL)" \
		KERNEL_SRC="$(LINUX_DIR)" \
		all
endef

define Package/kmod-mini-fo/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/mini_fo.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
	install -m0755 -d $(1)/etc/modules.d
	echo "mini_fo" > $(1)/etc/modules.d/15-mini-fo
endef

$(eval $(call BuildPackage,kmod-mini-fo))
