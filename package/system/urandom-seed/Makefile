include $(TOPDIR)/rules.mk

PKG_NAME:=urandom-seed
PKG_RELEASE:=3
PKG_LICENSE:=GPL-2.0-only

include $(INCLUDE_DIR)/package.mk

define Package/urandom-seed
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=/etc/urandom.seed handling for OpenWrt
  URL:=https://openwrt.org/
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS) \
		-std=gnu99 -o $(PKG_BUILD_DIR)/seedrng seedrng.c
endef
Build/Compile = $(Build/Compile/Default)

define Package/urandom-seed/install
	$(CP) ./files/* $(1)/
	$(INSTALL_DIR) $(1)/sbin
	$(CP) $(PKG_BUILD_DIR)/seedrng $(1)/sbin/
endef

$(eval $(call BuildPackage,urandom-seed))
