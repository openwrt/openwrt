include $(TOPDIR)/rules.mk

PKG_NAME:=econet-omcid
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_MAINTAINER:=AK Sharma <maskachoska@yahoo.com>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/package.mk

define Package/econet-omcid
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=WWAN
  TITLE:=EcoNet EN7528 GPON OMCI daemon (native userspace)
  DEPENDS:=@TARGET_econet +kmod-econet-xpon
endef

define Package/econet-omcid/description
  Native clean-room OMCI daemon for the econet-xpon GPON port. Speaks baseline
  OMCI to the OLT over the driver's "omci" netdev, maintains a MIB and programs
  the datapath via /proc/econet_xpon_*. Replaces the non-redistributable stock
  omci_v2 chroot binary. The ONU serial is supplied at runtime
  ("omcid <ifname> <serial>" or $OMCID_SERIAL) - no subscriber identity is
  compiled into the binary.
endef

# Single self-contained C source (libc + linux headers only).
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS) \
		-o $(PKG_BUILD_DIR)/omcid $(PKG_BUILD_DIR)/omcid.c
endef

define Package/econet-omcid/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/omcid $(1)/usr/sbin/omcid
endef

$(eval $(call BuildPackage,econet-omcid))
