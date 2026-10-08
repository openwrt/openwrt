#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=binutils
PKG_VERSION:=2.46.0
PKG_RELEASE:=1

PKG_SOURCE_URL:=@GNU/binutils
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_HASH:=d75a94f4d73e7a4086f7513e67e439e8fcdcbb726ffe63f4661744e6256b2cf2

PKG_FIXUP:=patch-libtool
PKG_LIBTOOL_PATHS:=. gas bfd opcodes gprof gprofng binutils ld libiberty gold intl libctf libsframe
PKG_REMOVE_FILES:=libtool.m4
PKG_INSTALL:=1

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>
PKG_LICENSE:=GPL-3.0+
PKG_CPE_ID:=cpe:/a:gnu:binutils
PKG_BUILD_PARALLEL:=1
PKG_BUILD_FLAGS:=no-mips16

include $(INCLUDE_DIR)/nls.mk
include $(INCLUDE_DIR)/package.mk

define Package/libbfd
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libbfd
  DEPENDS:=+zlib $(INTL_DEPENDS)
endef

define Package/libctf
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libctf
  DEPENDS:=+libbfd
endef

define Package/libopcodes
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libopcodes
  DEPENDS:=+libbfd
endef

define Package/binutils
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=binutils
  DEPENDS:=+objdump +ar
  ALTERNATIVES:=200:/usr/bin/strings:/usr/bin/binutils-strings
endef

define Package/objdump
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=objdump
  DEPENDS:=+libopcodes +libctf
endef

define Package/ar
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=ar
  DEPENDS:=+zlib +libbfd
endef

define Package/binutils/description
  The Binutils package contains a linker, an assembler, and other tools for handling object files
endef

TARGET_CFLAGS += $(FPIC) -Wno-unused-value

TARGET_LDFLAGS += $(if $(INTL_FULL),-lintl)

CONFIGURE_ARGS += \
	--host=$(REAL_GNU_TARGET_NAME) \
	--target=$(REAL_GNU_TARGET_NAME) \
	--enable-shared \
	--enable-dependency-tracking \
	--enable-serial-configure \
	--enable-obsolete \
	--enable-deterministic-archives \
	--enable-install-libiberty \
	--enable-install-libbfd \
	--enable-install-libctf \
	--disable-werror \
	--disable-gprofng \
	--disable-nls \
	--disable-sim \
	--disable-gdb \
	--with-system-zlib \
	--without-debuginfod \
	--without-msgpack \
	--without-zstd \


BINUTILS_VARS = \
	ac_cv_path_EXPECT= \
	ac_cv_prog_CARGO= \
	ac_cv_prog_EXPECT= \
	ac_cv_prog_GDC= \
	ac_cv_prog_GNATBIND= \
	ac_cv_prog_GNATMAKE= \
	ac_cv_prog_MAKEINFO= \
	ac_cv_prog_ac_ct_GDC= \
	ac_cv_prog_ac_ct_GNATBIND= \
	ac_cv_prog_ac_ct_GNATMAKE= \
	ac_cv_dejagnu_compat=no \
	ac_cv_libctf_tcl_try=no \
	acx_cv_cc_gcc_supports_ada=no \
	acx_cv_d_compiler_works=no \


CONFIGURE_VARS += $(BINUTILS_VARS)

MAKE_VARS += $(BINUTILS_VARS) \
	CPPFLAGS="$(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)"

define Build/Install
	$(call Build/Install/Default)
	$(MAKE) -C $(PKG_BUILD_DIR)/libiberty \
		target_header_dir=libiberty \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		MULTIOSDIR="" \
		install
endef

define Build/InstallDev
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

define Package/libbfd/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libbfd*.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsframe*.so* $(1)/usr/lib/
endef

define Package/libctf/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libctf*.so* $(1)/usr/lib/
endef

define Package/libopcodes/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libopcodes*.so $(1)/usr/lib/
endef

define Package/objdump/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/objdump $(1)/usr/bin/
endef

define Package/ar/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ar $(1)/usr/bin/
endef

define Package/binutils/install
	$(INSTALL_DIR) $(1)/usr $(1)/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ $(1)/usr/
	mv $(1)/usr/bin/strings $(1)/usr/bin/binutils-strings
	rm -f $(1)/usr/bin/objdump
	rm -f $(1)/usr/bin/ar

	: # deduplicate "/usr/bin/ld.*":
	: # "ld.bfd" is the implementation, "ld" is the variant selector,
	: # so the link should be "ld" -> "ld.bfd"
	_ld='$(1)/usr/bin/ld'; \
	[ -f "$$$${_ld}" ] || exit 0; \
	find "$$$${_ld%/*}/" -follow -name "$$$${_ld##*/}*" -type f \
	| grep -Fxv -e "$$$${_ld}" | sort -V \
	| while read -r _ld_variant; do \
		cmp -s "$$$${_ld}" "$$$${_ld_variant}" || continue; \
		cp -L "$$$${_ld_variant}" "$$$${_ld_variant}.new"; \
		rm -f "$$$${_ld_variant}"; \
		mv "$$$${_ld_variant}.new" "$$$${_ld_variant}"; \
		rm -f "$$$${_ld}"; \
		ln -s "$$$${_ld_variant##*/}" "$$$${_ld}"; \
		break; \
	done
endef

$(eval $(call BuildPackage,libbfd))
$(eval $(call BuildPackage,libctf))
$(eval $(call BuildPackage,libopcodes))
$(eval $(call BuildPackage,binutils))
$(eval $(call BuildPackage,objdump))
$(eval $(call BuildPackage,ar))
