# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=dwarves
PKG_VERSION:=1.29
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://fedorapeople.org/~acme/dwarves/
PKG_HASH:=59c597d4e953c714d6f3ff36aeed2ac30cba85c1d7b94d0c87ca91d611d98a56

PKG_MAINTAINER:=Tony Ambardar <itugrok@yahoo.com>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_HOST_OPTIONS += \
	-D__LIB=lib \
	-DCMAKE_INSTALL_RPATH="$(STAGING_DIR_HOST)/lib" \
	-DCMAKE_SKIP_RPATH=FALSE

define Host/Clean
	$(RM) $(STAGING_DIR_HOST)/bin/{codiff,ctracer,dtagnames,pahole,pdwtags}
	$(RM) $(STAGING_DIR_HOST)/bin/{pfunct,pglobal,prefcnt,scncopy,syscse}
	$(RM) $(STAGING_DIR_HOST)/bin/{ostra-cg,btfdiff,fullcircle}
	$(RM) $(STAGING_DIR_HOST)/lib/libdwarves*.so*
	$(RM) $(STAGING_DIR_HOST)/share/man/man1/pahole.1
	rm -rf $(STAGING_DIR_HOST)/include/dwarves
	rm -rf $(STAGING_DIR_HOST)/share/dwarves
	$(call Host/Clean/Default)
endef

$(eval $(call HostBuild))
