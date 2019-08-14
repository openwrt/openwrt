include $(TOPDIR)/rules.mk

PKG_NAME:=urngd
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://git.openwrt.org/project/urngd.git
PKG_SOURCE_DATE:=2019-06-17
PKG_SOURCE_VERSION:=c057e177ace7704a3d4e2e1272baaf2e2d21cbed
PKG_MIRROR_HASH:=6ef24be5da227b36fc63098767cf3de8dccbe5578487ffdbd69c7beeeb76bccd

PKG_LICENSE:=GPL-2.0 BSD-3-Clause
PKG_LICENSE_FILES:=

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=OpenWrt non-physical true random number generator based on timing jitter
  DEPENDS:=+libubox
endef

define Package/$(PKG_NAME)/description
  urngd is OpenWrt's micro non-physical true random number generator based on
  timing jitter.

  Using the Jitter RNG core, the rngd provides an entropy source that feeds into
  the Linux /dev/random device if its entropy runs low. It updates the
  /dev/random entropy estimator such that the newly provided entropy unblocks
  /dev/random.

  The seeding of /dev/random also ensures that /dev/urandom benefits from
  entropy. Especially during boot time, when the entropy of Linux is low, the
  Jitter RNGd provides a source of sufficient entropy.
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_NAME).init $(1)/etc/init.d/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/sbin/$(PKG_NAME)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
