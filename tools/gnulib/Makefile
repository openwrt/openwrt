include $(TOPDIR)/rules.mk

PKG_NAME:=gnulib
PKG_CPE_ID:=cpe:/a:gnu:$(PKG_NAME)

PKG_SOURCE_VERSION:=stable-202501
PKG_MIRROR_SOURCE:=$(PKG_NAME)-20250303.bundle
PKG_MIRROR_HASH:=9dae009ef9dd7cff17b74c0cda5d7a423e2ed98b4f5b7aa29a970565b0591c06
PKG_VERSION:=3fbc2c7bb371916a6f3433ef28d84874909d1052
PKG_HASH:=1202af3960a07b5e16a935107eed38d7b5022e833605149b8c0241a66230b2f7

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/$(PKG_NAME)
PKG_SOURCE_PROTO:=$(word 2, $(subst ., ,$(PKG_MIRROR_SOURCE)))

include $(INCLUDE_DIR)/host-build.mk

TAR_OPTIONS := -C $(HOST_BUILD_DIR) $(TAR_OPTIONS)

define Host/Configure
endef

define Host/Install
	$(call Host/Uninstall)
	$(INSTALL_DIR) $(1)/share/aclocal
	$(INSTALL_DATA) $(HOST_BUILD_DIR)/m4/*.m4 $(1)/share/aclocal/
	$(CP) $(HOST_BUILD_DIR)/ $(1)/share/gnulib/
	ln -sf ../share/gnulib/gnulib-tool $(STAGING_DIR_HOST)/bin/gnulib-tool
endef

define Host/Uninstall
	rm -rf $(STAGING_DIR_HOST)/bin/gnulib-tool $(STAGING_DIR_HOST)/share/gnulib
endef

$(eval $(call HostBuild))
