include $(TOPDIR)/rules.mk

PKG_NAME:=rmtfs
PKG_VERSION:=0.2+git20220718
PKG_RELEASE:=1
_COMMIT:=695d0668ffa6e2a4bf6e676f3c58a444a5d67690

PKG_BUILD_DIR:=$(BUILD_DIR)/rmtfs-$(_COMMIT)
PKG_SOURCE:=$(_COMMIT).tar.gz
PKG_SOURCE_URL:=https://github.com/andersson/rmtfs/archive/
PKG_HASH:=aa76b14d86b3508ba4d9389dbcad9ccfb2857334bfd1fd20c437e8671c834f28

PKG_LICENSE:=BSD-3-Clause

include $(INCLUDE_DIR)/package.mk

define Package/rmtfs
	SECTION:=base
	CATEGORY:=Network
	DEPENDS:=+qrtr-lib +libudev-zero
	TITLE:=Qualcomm Remote Filesystem Service Implementation 
	URL:=https://github.com/andersson/rmtfs
endef

define Package/rmtfs/description
	Qualcomm RMTFS daemon. This service boots up the modem core
	and provides it access to the modem data partitions.
	This service is essential to boot the modem remoteproc.
endef

define Build/Compile
	$(call Build/Compile/Default,rmtfs)
endef

Build/Install:=:

define Package/rmtfs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rmtfs $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/rmtfs.init $(1)/etc/init.d/rmtfs
endef

$(eval $(call BuildPackage,rmtfs))
