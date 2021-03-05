include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=bcm63xx-cfe
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://github.com/openwrt/bcm63xx-cfe.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-03-05
PKG_SOURCE_VERSION:=3fb6f1cbcce79544b303d94619c5ca4c77537cbf
PKG_MIRROR_HASH:=35042a44973fad5b3418e9cceaadf79d59bacb46066632a5d865af9545c75d8a

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
