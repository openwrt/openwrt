include $(TOPDIR)/rules.mk

PKG_NAME:=dosbox
PKG_VERSION:=0.74-2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=7110ee24a45a2b4951ad52eb1a3722be

include $(INCLUDE_DIR)/package.mk

define Package/dosbox
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=DOSBox
  URL:=http://www.dosbox.com
  DEPENDS:=+libsdl +libstdcpp
endef

define Package/dosbox/description
 DOSBox is an emulator program which emulates an IBM PC compatible computer
 running a DOS operating system. Many IBM PC compatible graphics and sound cards
 are also emulated.
endef

define Build/Configure
	( cd $(PKG_BUILD_DIR); \
	  TARGET_CC="$(TARGET_CROSS)gcc" \
	  ./configure --host=mipsel-openwrt-linux \
	)
endef

define Build/InstallDev
endef

define Package/dosbox/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/dosbox $(1)/usr/bin
endef


$(eval $(call BuildPackage,dosbox))
