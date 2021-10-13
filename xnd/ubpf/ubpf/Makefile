include $(TOPDIR)/rules.mk

PKG_NAME:=ubpf
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0+
PKG_LICENSE_FILES:=

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/ubpf
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libubox +libbpf +libnl-tiny
  TITLE:=eBPF loader
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include \
	-I$(STAGING_DIR)/usr/include/libnl-tiny

define Package/ubpf/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/xdpload $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/xdplist $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uxdp $(1)/sbin/
endef

$(eval $(call BuildPackage,ubpf))
