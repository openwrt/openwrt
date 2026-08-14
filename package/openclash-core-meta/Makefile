include $(TOPDIR)/rules.mk

PKG_NAME:=openclash-core-meta
# APK package versions must start with a numeric component and cannot use the
# upstream alpha tag verbatim. Keep the actual core tag separate: the binary
# and OpenClash UI still report alpha-ge183c58.
PKG_VERSION:=0.0.0_alpha~e183c58
PKG_RELEASE:=1

OPENCLASH_CORE_VERSION:=alpha-ge183c58
OPENCLASH_CORE_COMMIT:=7dedf5508d1aba0360820bff3532eb4257c79fae
OPENCLASH_CORE_ARCHIVE:=$(PKG_NAME)-$(OPENCLASH_CORE_VERSION)-linux-arm64.tar.gz

PKG_MAINTAINER:=ZJJCKA
PKG_LICENSE:=GPL-3.0

# The downloaded Mihomo core is a statically linked, self-contained binary.
# Running the generic target stripper invokes patchelf on it and produces a
# misleading "no section headers" diagnostic without changing the binary.
RSTRIP:=:
STRIP:=:

include $(INCLUDE_DIR)/package.mk

define Download/openclash-core-meta
  FILE:=$(OPENCLASH_CORE_ARCHIVE)
  URL:=https://raw.githubusercontent.com/vernesong/OpenClash/$(OPENCLASH_CORE_COMMIT)/master/meta
  URL_FILE:=clash-linux-arm64.tar.gz
  HASH:=8252d16726041872825cdd9089c798c318f8862466b40b34d8bf62225ef57e34
endef
$(eval $(call Download,openclash-core-meta))

define Package/openclash-core-meta
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Mihomo Meta core for OpenClash
  URL:=https://github.com/MetaCubeX/mihomo
  DEPENDS:=@aarch64
endef

define Package/openclash-core-meta/description
  Preinstalled Mihomo Meta core for luci-app-openclash.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	tar -xzf $(DL_DIR)/$(OPENCLASH_CORE_ARCHIVE) -C $(PKG_BUILD_DIR)
	mv $(PKG_BUILD_DIR)/clash $(PKG_BUILD_DIR)/clash_meta
endef

define Build/Compile
endef

define Package/openclash-core-meta/install
	$(INSTALL_DIR) $(1)/etc/openclash/core
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/clash_meta $(1)/etc/openclash/core/clash_meta
endef

$(eval $(call BuildPackage,openclash-core-meta))
