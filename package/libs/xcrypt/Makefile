include $(TOPDIR)/rules.mk

PKG_NAME:=libxcrypt
PKG_VERSION:=4.4.38
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://github.com/besser82/libxcrypt/releases/download/v$(PKG_VERSION)
PKG_HASH:=80304b9c306ea799327f01d9a7549bdb28317789182631f1b54f4511b4206dd6

PKG_MAINTAINER:=
PKG_LICENSE:=LGPL-2.1-or-later
PKG_LICENSE_FILES:=COPYING.LIB

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

PKG_CONFIG_DEPENDS:= \
	CONFIG_USE_GLIBC \
	CONFIG_PACKAGE_libcrypt-compat \
	CONFIG_PACKAGE_libxcrypt

include $(INCLUDE_DIR)/package.mk

define Package/libxcrypt/Default
  SECTION:=libs
  CATEGORY:=Libraries
  URL:=https://github.com/besser82/libxcrypt
  TITLE:=Extended crypt library
endef

define Package/libxcrypt
$(Package/libxcrypt/Default)
  VARIANT:=regular
  BUILDONLY:=1
endef

define Package/libcrypt-compat
$(Package/libxcrypt/Default)
  VARIANT:=compat
  TITLE+= - libc compatibility
  DEPENDS:=@USE_GLIBC
endef

define Package/libxcrypt/description
  libxcrypt is a modern library for one-way hashing of passwords. It supports
  a wide variety of both modern and historical hashing methods: yescrypt,
  gost-yescrypt, scrypt, bcrypt, sha512crypt, sha256crypt, md5crypt, SunMD5,
  sha1crypt, NT, bsdicrypt, bigcrypt, and descrypt. It provides the traditional
  Unix crypt and crypt_r interfaces, as well as a set of extended interfaces
  pioneered by Openwall Linux, crypt_rn, crypt_ra, crypt_gensalt,
  crypt_gensalt_rn, and crypt_gensalt_ra.
endef

Package/libcrypt-compat/description=$(Package/libxcrypt/description)

CONFIGURE_ARGS += \
	--with-pic \
	--enable-year2038 \
	--disable-xcrypt-compat-files

ifeq ($(BUILD_VARIANT),regular)
  CONFIGURE_ARGS += \
	--disable-shared \
	--disable-failure-tokens \
	--disable-obsolete-api \
	--enable-hashes=solaris
endif

ifeq ($(BUILD_VARIANT),compat)
  CONFIGURE_ARGS += \
	--enable-obsolete-api=glibc \
	--enable-hashes=glibc
endif

define Package/libxcrypt/install
	true
endef

define Package/libcrypt-compat/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcrypt.so.* $(1)/usr/lib/
endef

define Build/libxcrypt/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/libxcrypt
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcrypt.{a,la} $(1)/usr/lib/libxcrypt
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libxcrypt.pc $(1)/usr/lib/pkgconfig/
endef

define Build/libcrypt-compat/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcrypt.{a,la,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	# libcrypt.pc is symlink to libxcrypt.pc
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libxcrypt.pc $(1)/usr/lib/pkgconfig/libcrypt.pc
endef

ifeq ($(BUILD_VARIANT),regular)
  Build/InstallDev=$(Build/libxcrypt/InstallDev)
endif

ifeq ($(BUILD_VARIANT),compat)
  Build/InstallDev=$(Build/libcrypt-compat/InstallDev)
endif

$(eval $(call BuildPackage,libcrypt-compat))
$(eval $(call BuildPackage,libxcrypt))
