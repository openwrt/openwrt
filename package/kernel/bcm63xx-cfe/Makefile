include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=bcm63xx-cfe
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://github.com/openwrt/bcm63xx-cfe.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2025-04-02
PKG_SOURCE_VERSION:=b8128fa4bdcdaf97549138965b00d5d7ceb7bb0f
PKG_MIRROR_HASH:=d33fad4fb5f4419bb287f3c3df6cceabab01fc7852c24cf29a15befa38cc0d45

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/bcm63xx-cfe
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@(TARGET_bcm4908||TARGET_bcm63xx||TARGET_bmips)
  TITLE:=bcm63xx-cfe
  DEFAULT:=y
endef

define Package/bcm63xx-cfe/description
  CFE RAM binaries for bcm63xx.
endef

define Build/Compile
	true
endef

define Package/bcm63xx-cfe/install
	true
endef

define Build/InstallDev
	rm -rf $(KERNEL_BUILD_DIR)/$(PKG_NAME)
	mkdir -p $(KERNEL_BUILD_DIR)/$(PKG_NAME)
	$(CP) -r $(PKG_BUILD_DIR)/* $(KERNEL_BUILD_DIR)/$(PKG_NAME)
endef

$(eval $(call BuildPackage,bcm63xx-cfe))
