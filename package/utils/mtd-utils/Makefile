#
# Copyright (C) 2009-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mtd-utils
PKG_VERSION:=2.0.1
PKG_RELEASE:=1

PKG_SOURCE_URL:=git://git.infradead.org/mtd-utils.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=81049e599775bac4238361752f659a86f67328e6
PKG_MIRROR_HASH:=a79784eb52c77dbc73a08863ce6651b3d22931e7c670cd1782168f9bba13f5a8

PKG_INSTALL:=1
PKG_FIXUP:=autoreconf

PKG_FLAGS:=nonshared

PKG_BUILD_DEPENDS:=util-linux lzo zlib

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define Package/mtd-utils/Default
  SECTION:=utils
  CATEGORY:=Utilities
  URL:=http://www.linux-mtd.infradead.org/
  DEPENDS:=@NAND_SUPPORT
endef

define Package/ubi-utils
 $(call Package/mtd-utils/Default)
  TITLE:=Utilities for ubi info/debug
endef

define Package/ubi-utils/description
  Utilities for manipulating memory technology devices.
endef

define Package/nand-utils
 $(call Package/mtd-utils/Default)
  TITLE:=Utilities for nand flash read/write/test
endef

define Package/nand-utils/description
  Utilities for NAND devices.
endef

MAKE_FLAGS += LDLIBS+="$(LIBGCC_S)"

CONFIGURE_ARGS += \
	--disable-tests \
	--without-xattr \
	--without-lzo

define Package/ubi-utils/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/sbin/{ubiattach,ubicrc32,ubiblock,ubidetach,ubiformat,ubimkvol} $(1)/usr/sbin/
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/sbin/{ubinfo,ubinize,ubirename,ubirmvol,ubirsvol,ubiupdatevol} $(1)/usr/sbin/
endef

define Package/nand-utils/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) \
	$(PKG_INSTALL_DIR)/usr/sbin/{nanddump,nandwrite,nandtest,mtdinfo} $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ubi-utils))
$(eval $(call BuildPackage,nand-utils))
