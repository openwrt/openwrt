include $(TOPDIR)/rules.mk

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

define Package/uboot-tools
	SECTION:=utils
	CATEGORY:=Utilities
	SUBMENU:=Boot Loaders
	TITLE:=U-Boot bootloader Tools
	URL:=http://www.denx.de/wiki/U-Boot
endef

define Package/uboot-tools/description
	U-Boot tools are a collection of utilities designed
	to work with the U-Boot bootloader,
endef

define Build/Configure
	$(MAKE) -C $(PKG_BUILD_DIR) tools-only_defconfig
	$(MAKE) -C $(PKG_BUILD_DIR) syncconfig
endef

MAKE_FLAGS += \
	ARCH="sandbox" \
	TARGET_CFLAGS="$(TARGET_CFLAGS)" \
	TARGET_LDFLAGS="$(TARGET_LDFLAGS)"

define Build/Compile
endef

