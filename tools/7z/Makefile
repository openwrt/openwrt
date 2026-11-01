include $(TOPDIR)/rules.mk

PKG_NAME:=7z
PKG_VERSION:=26.05

PKG_SOURCE:=$(PKG_NAME)$(subst .,,$(PKG_VERSION))-src.tar.xz
PKG_SOURCE_URL:=https://github.com/ip7z/7zip/releases/download/$(PKG_VERSION)
PKG_HASH:=0b6080b8c3e7a579161d2d495224c0e51c467576cb4c7048b0ab485ef44cc22e

PKG_CPE_ID:=cpe:/a:7-zip:7-zip

# This builds the 7zr variant which supports only 7z, so no non-LGPL code should be included
PKG_LICENSE:=LGPL-2.1-or-later
PKG_LICENSE_FILES:=DOC/License.txt DOC/copying.txt

HOST_MAKE_PATH:=CPP/7zip/Bundles/Alone7z

include $(INCLUDE_DIR)/host-build.mk

TAR_OPTIONS := -C $(HOST_BUILD_DIR) $(TAR_OPTIONS)
HOST_MAKE_FLAGS += -f makefile.gcc

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/$(HOST_MAKE_PATH)/_o/7zr $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/7zr
endef

$(eval $(call HostBuild))
