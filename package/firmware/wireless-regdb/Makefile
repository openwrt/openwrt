include $(TOPDIR)/rules.mk

PKG_NAME:=wireless-regdb

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.kernel.org/pub/scm/linux/kernel/git/sforshee/wireless-regdb.git
PKG_SOURCE_DATE:=2017-10-20
PKG_SOURCE_VERSION:=4343d359ed5e7404de8803a74df186457b26ab79
PKG_MIRROR_HASH:=b827bf760de57b907df159c8d38d7c3fb5b4a691781114c47739e20bffb3a312

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk

define Package/wireless-regdb
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(patsubst pub/scm,cgit,$(PKG_SOURCE_URL))
  TITLE:=Wireless Regulatory Database
endef

define Build/Compile
	python $(PKG_BUILD_DIR)/db2fw.py $(PKG_BUILD_DIR)/regulatory.db $(PKG_BUILD_DIR)/db.txt
endef

define Package/wireless-regdb/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(CP) $(PKG_BUILD_DIR)/regulatory.db $(1)/lib/firmware/
endef

$(eval $(call BuildPackage,wireless-regdb))
