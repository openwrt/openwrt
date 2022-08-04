include $(TOPDIR)/rules.mk

PKG_NAME:=wireless-regdb
PKG_VERSION:=2022.06.06
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@KERNEL/software/network/wireless-regdb/
PKG_HASH:=ac00f97efecce5046ed069d1d93f3365fdf994c7c7854a8fc50831e959537230

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk

define Package/wireless-regdb
  PKGARCH:=all
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://git.kernel.org/pub/scm/linux/kernel/git/sforshee/wireless-regdb.git/
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
