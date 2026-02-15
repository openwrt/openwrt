include $(TOPDIR)/rules.mk

PKG_NAME:=aeolus
PKG_RELEASE=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Broadcom/aeolus.git
PKG_SOURCE_DATE:=2017-05-25
PKG_SOURCE_VERSION:=7acf337f602e381ca2288df9c5bf8ee81b4296d2
PKG_MIRROR_HASH:=4f5a39a52bc786c31c60cdefdc0f8a4858eda1b11f895b67dddc505a6853cfb6

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	+$(HOST_MAKE_VARS) $(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR)/ProgramStore
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/ProgramStore/ProgramStore $(STAGING_DIR_HOST)/bin/aeolus_ProgramStore
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/aeolus_ProgramStore
endef

$(eval $(call HostBuild))
