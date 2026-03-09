include $(TOPDIR)/rules.mk

PKG_NAME:=sandbox-test
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/sandbox-test
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=Build sandbox escape test
  DEPENDS:=@DEVEL
endef

define Package/sandbox-test/description
  Test package that attempts to escape the build sandbox.
  Every write/network attempt should fail when CONFIG_BUILD_SANDBOX=y.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/Makefile $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) all
endef

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR) install
endef

define Package/sandbox-test/install
	:
endef

$(eval $(call BuildPackage,sandbox-test))
