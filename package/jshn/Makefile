include $(TOPDIR)/rules.mk

PKG_NAME:=jshn
PKG_VERSION:=0.1
PKG_RELEASE=1

include $(INCLUDE_DIR)/package.mk

define Package/jshn
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libjson
  TITLE:=JSON SHell Notation
endef

define Package/jshn/description
  Library for parsing and generating JSON from shell scripts
endef

PKG_BUILD_DEPENDS += libubox
TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include --std=gnu99

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/jshn src/jshn.c $(TARGET_LDFLAGS) -ljson
endef

define Package/jshn/install
	$(INSTALL_DIR) $(1)/bin $(1)/lib/functions
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/jshn $(1)/bin
	$(INSTALL_DATA) ./files/jshn.sh $(1)/lib/functions
endef

$(eval $(call BuildPackage,jshn))
