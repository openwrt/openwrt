# $Id$

include $(TOPDIR)/rules.mk
include $(TOPDIR)/include/kernel.mk

PKG_NAME:=spca5xx
PKG_VERSION:=20060301
PKG_RELEASE:=1
PKG_MD5SUM:=4160e03c408ac254111febacf058b88a

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=http://mxhaard.free.fr/spca50x/Download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

include $(TOPDIR)/include/package.mk

#FIXME: add proper dependency handling on kmod-usb-core & kmod-videodev packages
define Package/kmod-spca5xx
SECTION:=drivers
CATEGORY:=Drivers
DEPENDS:=@PACKAGE_KMOD_USB&&PACKAGE_KMOD_VIDEODEV
TITLE:=Driver for SPCA5xx based USB cameras
DESCRIPTION:=Driver for SPCA5xx based USB cameras
URL:=http://mxhaard.free.fr/
VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		KERNELVERSION="$(KERNEL)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
endef

define Package/kmod-spca5xx/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/spca5xx.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call BuildPackage,kmod-spca5xx))
