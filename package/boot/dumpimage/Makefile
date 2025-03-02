include $(TOPDIR)/rules.mk

PKG_NAME:=dumpimage
PKG_DISTNAME:=u-boot
PKG_VERSION:=2025.01
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	https://ftp.denx.de/pub/u-boot \
	https://mirror.cyberbits.eu/u-boot \
ftp://ftp.denx.de/pub/u-boot
PKG_HASH:=cdef7d507c93f1bbd9f015ea9bc21fa074268481405501945abc6f854d5b686f
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_DISTNAME)-$(PKG_VERSION)

PKG_BUILD_DEPENDS:=fstools

PKG_LICENSE:=GPL-2.0 GPL-2.0+
PKG_LICENSE_FILES:=Licenses/README

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/dumpimage
	SECTION:=utils
	CATEGORY:=Utilities
	SUBMENU:=Boot Loaders
	TITLE:= U-Boot bootloader Tools
	DEPENDS := +libopenssl
	URL:=http://www.denx.de/wiki/U-Boot
endef

define Package/dumpimage/description
	dumpimage is used to extract the ubi from .img file
	to allow luci to handle upgrades of .img files.
endef

define Build/Configure
	$(MAKE) -C $(PKG_BUILD_DIR) defconfig
	$(MAKE) -C $(PKG_BUILD_DIR) syncconfig
	$(SED) 's/CONFIG_TOOLS_LIBCRYPTO=y/# CONFIG_TOOLS_LIBCRYPTO is not set/' $(PKG_BUILD_DIR)/.config
endef

MAKE_FLAGS += \
	ARCH="sandbox" cross_tools \
	TARGET_CFLAGS="$(TARGET_CFLAGS)" \
	TARGET_LDFLAGS="$(TARGET_LDFLAGS)"

define Package/dumpimage/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/dumpimage $(1)/usr/bin
endef

$(eval $(call BuildPackage,dumpimage))
