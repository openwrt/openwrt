include $(TOPDIR)/rules.mk

PKG_NAME:=cryptsetup
PKG_VERSION:=2.8.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@KERNEL/linux/utils/cryptsetup/v$(subst $(space),.,$(wordlist 1, 2, $(subst .,$(space),$(PKG_VERSION))))
PKG_HASH:=443e46f8964c9acc780f455afbb8e23aa0e8ed7ec504cfc59e04f406fa1e8a83

PKG_MAINTAINER:=Daniel Golle <daniel@makrotopia.org>
PKG_LICENSE:=GPL-2.0-or-later LGPL-2.1-or-later
PKG_LICENSE_FILES:=COPYING COPYING.LGPL
PKG_CPE_ID:=cpe:/a:cryptsetup_project:cryptsetup

PKG_BUILD_DEPENDS:=!USE_GLIBC:argp-standalone
PKG_CONFIG_DEPENDS:=CONFIG_PACKAGE_cryptsetup CONFIG_PACKAGE_cryptsetup-ssh \
		    CONFIG_PACKAGE_integritysetup CONFIG_PACKAGE_veritysetup

HOST_BUILD_DEPENDS:=libjson-c/host popt/host
HOST_BUILD_PREFIX:=$(STAGING_DIR_HOST)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/meson.mk
include $(INCLUDE_DIR)/nls.mk

MESON_ARGS += \
	-Dargon-implementation=internal \
	-Dasciidoc=disabled \
	-Dblkid=true \
	-Dcrypto-backend=kernel \
	-Dcryptsetup=$(if $(CONFIG_PACKAGE_cryptsetup),true,false) \
	-Ddefault-luks2-lock-path=/var/run/cryptsetup \
	-Ddevmapper=enabled \
	-Dexternal-tokens=$(if $(CONFIG_PACKAGE_cryptsetup-ssh),true,false) \
	-Dintegritysetup=$(if $(CONFIG_PACKAGE_integritysetup),true,false) \
	-Dkernel_crypto=true \
	-Dkeyring=true \
	-Dnls=$(if $(CONFIG_BUILD_NLS),true,false) \
	-Dssh-token=$(if $(CONFIG_PACKAGE_cryptsetup-ssh),true,false) \
	-Dudev=false \
	-Dveritysetup=$(if $(CONFIG_PACKAGE_veritysetup),true,false)

TARGET_CFLAGS += -D_LARGEFILE64_SOURCE
TARGET_LDFLAGS += -Wl,--gc-sections $(if $(INTL_FULL),-lintl)

MESON_HOST_ARGS += \
	-Denable-static=true \
	-Dargon-implementation=none \
	-Dasciidoc=disabled \
	-Dblkid=false \
	-Dcrypto-backend=openssl \
	-Dcryptsetup=false \
	-Ddevmapper=disabled \
	-Dexternal-tokens=false \
	-Dhw-opal=false \
	-Dintegritysetup=false \
	-Dkernel_crypto=false \
	-Dkeyring=false \
	-Dnls=false \
	-Dssh-token=false \
	-Dudev=false \
	-Dveritysetup=true

define Package/cryptsetup
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Encryption
  TITLE:=Cryptsetup
  DEPENDS:=$(ICONV_DEPENDS) $(INTL_DEPENDS) +libblkid +libuuid +libpopt +lvm2 \
           +libdevmapper +libjson-c +@KERNEL_DIRECT_IO +kmod-crypto-user
  URL:=https://gitlab.com/cryptsetup/cryptsetup/
endef

define Package/cryptsetup/description
  Cryptsetup is utility used to conveniently setup disk encryption based on DMCrypt kernel module.
endef

define Package/cryptsetup-ssh
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Encryption
  TITLE:=Cryptsetup SSH token
  DEPENDS:=+cryptsetup +PACKAGE_cryptsetup-ssh:libssh
  URL:=https://gitlab.com/cryptsetup/cryptsetup/
endef

define Package/cryptsetup-ssh/description
  Experimental SSH token support for cryptsetup.
endef

define Package/integritysetup
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Encryption
  TITLE:=integritysetup
  DEPENDS:=+cryptsetup
  URL:=https://gitlab.com/cryptsetup/cryptsetup/
endef

define Package/integritysetup/description
  Manage dm-integrity (block level integrity) volumes
endef

define Package/veritysetup
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Encryption
  TITLE:=veritysetup
  DEPENDS:=+cryptsetup
  URL:=https://gitlab.com/cryptsetup/cryptsetup/
endef

define Package/veritysetup/description
  Manage dm-verity (block level verification) volumes
endef


ifeq ($(CONFIG_PACKAGE_cryptsetup-ssh),)
CONFIGURE_ARGS += --disable-ssh-token
endif

CONFIGURE_VARS += \
	LIBSSH_CFLAGS="-I$(STAGING_DIR)/usr/include" \
	LIBSSH_LIBS="-L$(STAGING_DIR)/usr/lib -lssh"

TARGET_CFLAGS += -D_LARGEFILE64_SOURCE
TARGET_LDFLAGS += -Wl,--gc-sections $(if $(INTL_FULL),-lintl)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/include/libcryptsetup.h $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcryptsetup.so* $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libcryptsetup.pc $(1)/usr/lib/pkgconfig
endef

define Package/cryptsetup/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcryptsetup.so* $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/cryptsetup $(1)/usr/sbin
endef

define Package/cryptsetup-ssh/install
	$(INSTALL_DIR) $(1)/usr/lib/cryptsetup
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cryptsetup/* $(1)/usr/lib/cryptsetup
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/cryptsetup-ssh $(1)/usr/sbin
endef

define Package/integritysetup/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/integritysetup $(1)/usr/sbin
endef

define Package/veritysetup/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/veritysetup $(1)/usr/sbin
endef

$(eval $(call BuildPackage,cryptsetup))
$(eval $(call BuildPackage,cryptsetup-ssh))
$(eval $(call BuildPackage,integritysetup))
$(eval $(call BuildPackage,veritysetup))
$(eval $(call HostBuild))
