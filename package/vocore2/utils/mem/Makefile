include $(TOPDIR)/rules.mk

PKG_NAME:=mem
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_CONFIG_DEPENDS:=

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/mem
  SECTION:=vocore2
  CATEGORY:=VoCore2
  TITLE:=Modify registers from user space
endef

define Package/mem/description
  used to read/write main chip register.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Package/mem/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mem $(1)/usr/bin
endef

$(eval $(call BuildPackage,mem))

