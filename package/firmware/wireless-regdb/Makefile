include $(TOPDIR)/rules.mk

PKG_NAME:=wireless-regdb
PKG_RELEASE:=1
PKG_LICENSE:=ISC
PKG_LICENSE_FILES:=LICENSE

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://git.kernel.org/pub/scm/linux/kernel/git/wens/wireless-regdb.git
PKG_SOURCE_DATE:=2025-02-12
PKG_SOURCE_VERSION:=f9f6b306a830505218753c86bbf21083c9b4b8ad
PKG_MIRROR_HASH:=7422425acd7934594dc34137cd41f6fa392381a1d8dab53e6c4c593141a436d0

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk

define Package/wireless-regdb
  PKGARCH:=all
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://git.kernel.org/pub/scm/linux/kernel/git/wens/wireless-regdb.git/
  TITLE:=Wireless Regulatory Database
endef

define Build/Compile
	$(STAGING_DIR_HOST)/bin/$(PYTHON) $(PKG_BUILD_DIR)/db2fw.py $(PKG_BUILD_DIR)/regulatory.db $(PKG_BUILD_DIR)/db.txt
endef

define Package/wireless-regdb/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(CP) $(PKG_BUILD_DIR)/regulatory.db $(1)/lib/firmware/
endef

$(eval $(call BuildPackage,wireless-regdb))
