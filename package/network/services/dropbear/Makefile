#
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dropbear
PKG_VERSION:=2018.76
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	http://matt.ucc.asn.au/dropbear/releases/ \
	https://dropbear.nl/mirror/releases/
PKG_HASH:=f2fb9167eca8cf93456a5fc1d4faf709902a3ab70dd44e352f3acbc3ffdaea65

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE libtomcrypt/LICENSE libtommath/LICENSE
PKG_CPE_ID:=cpe:/a:matt_johnston:dropbear_ssh_server

PKG_BUILD_PARALLEL:=1
PKG_USE_MIPS16:=0

PKG_CONFIG_DEPENDS:= \
	CONFIG_TARGET_INIT_PATH CONFIG_DROPBEAR_ECC CONFIG_DROPBEAR_ECC_FULL \
	CONFIG_DROPBEAR_CURVE25519 CONFIG_DROPBEAR_ZLIB \
	CONFIG_DROPBEAR_UTMP CONFIG_DROPBEAR_PUTUTLINE

include $(INCLUDE_DIR)/package.mk

ifneq ($(DUMP),1)
  STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell echo $(CONFIG_TARGET_INIT_PATH) | mkhash md5)
endif

define Package/dropbear/Default
  URL:=http://matt.ucc.asn.au/dropbear/
endef

define Package/dropbear/config
	source "$(SOURCE)/Config.in"
endef

define Package/dropbear
  $(call Package/dropbear/Default)
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=Small SSH2 client/server
  DEPENDS:= +DROPBEAR_ZLIB:zlib
endef

define Package/dropbear/description
 A small SSH2 server/client designed for small memory environments.
endef

define Package/dropbear/conffiles
$(if $(CONFIG_DROPBEAR_ECC),/etc/dropbear/dropbear_ecdsa_host_key)
/etc/dropbear/dropbear_rsa_host_key
/etc/config/dropbear 
endef

define Package/dropbearconvert
  $(call Package/dropbear/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Utility for converting SSH keys
endef

CONFIGURE_ARGS += \
	--disable-harden \
	--disable-pam \
	--enable-openpty \
	--enable-syslog \
	--disable-lastlog \
	--disable-utmpx \
	$(if $(CONFIG_DROPBEAR_UTMP),,--disable-utmp) \
	--disable-wtmp \
	--disable-wtmpx \
	--disable-loginfunc \
	$(if $(CONFIG_DROPBEAR_PUTUTLINE),,--disable-pututline) \
	--disable-pututxline \
	$(if $(CONFIG_DROPBEAR_ZLIB),,--disable-zlib) \
	--enable-bundled-libtom

TARGET_CFLAGS += -DARGTYPE=3 -ffunction-sections -fdata-sections
TARGET_LDFLAGS += -Wl,--gc-sections

# remove when pull #786 will be upstreamed
# ref: https://github.com/openwrt/openwrt/pull/786
ESED:=$(STAGING_DIR_HOST)/bin/sed -E -i -e

define Build/Configure
	$(Build/Configure/Default)
	cp $(PKG_BUILD_DIR)/default_options.h $(PKG_BUILD_DIR)/localoptions.h

	# replace obsolete patches/120-openwrt_options.patch
	for OPTION in INETD_MODE DROPBEAR_CLI_NETCAT \
	DROPBEAR_3DES DROPBEAR_DSS DROPBEAR_ENABLE_CBC_MODE \
	DROPBEAR_SHA1_HMAC DROPBEAR_SHA1_96_HMAC \
	DROPBEAR_DH_GROUP1 DROPBEAR_DH_GROUP1_CLIENTONLY \
	DROPBEAR_USE_PASSWORD_ENV; do \
		$(ESED) 's,^(#define '$$$$OPTION') .*$$$$,\1 0,g' \
		$(PKG_BUILD_DIR)/localoptions.h; \
	done

	$(ESED) 's,^(#define DEFAULT_PATH) .*$$$$,\1 "$(TARGET_INIT_PATH)",g' \
	$(PKG_BUILD_DIR)/localoptions.h

	$(ESED) 's,^(#define DROPBEAR_CURVE25519) .*$$$$,\1 $(if $(CONFIG_DROPBEAR_CURVE25519),1,0),g' \
	$(PKG_BUILD_DIR)/localoptions.h

	# Enforce that all replacements are made, otherwise options.h has changed
	# format and this logic is broken.
	for OPTION in DROPBEAR_ECDSA DROPBEAR_ECDH; do \
		$(ESED) 's,^(#define '$$$$OPTION') .*$$$$,\1 $(if $(CONFIG_DROPBEAR_ECC),1,0),g' \
		$(PKG_BUILD_DIR)/localoptions.h; \
	done

	# remove protocol idented software version number
	$(ESED) 's,^(#define LOCAL_IDENT) .*$$$$,\1 "SSH-2.0-dropbear",g' \
	$(PKG_BUILD_DIR)/sysoptions.h

	# enable nistp384 and nistp521 only if full ECC support was requested
	for OPTION in DROPBEAR_ECC_384 DROPBEAR_ECC_521; do \
		$(ESED) 's,^(#define '$$$$OPTION') .*$$$$,\1 $(if $(CONFIG_DROPBEAR_ECC_FULL),1,0),g' \
		$(PKG_BUILD_DIR)/sysoptions.h; \
	done

	# reduce auth timeout to 2 minutes (was: 5 minutes)
	$(ESED) 's,^(#define AUTH_TIMEOUT) .*$$$$,\1 120,g' \
	$(PKG_BUILD_DIR)/sysoptions.h

	# Enforce rebuild of svr-chansession.c
	rm -f $(PKG_BUILD_DIR)/svr-chansession.o
endef

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		PROGRAMS="dropbear dbclient dropbearkey scp" \
		MULTI=1 SCPPROGRESS=1
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		PROGRAMS="dropbearconvert"
endef

define Package/dropbear/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dropbearmulti $(1)/usr/sbin/dropbear
	$(INSTALL_DIR) $(1)/usr/bin
	$(LN) ../sbin/dropbear $(1)/usr/bin/scp
	$(LN) ../sbin/dropbear $(1)/usr/bin/ssh
	$(LN) ../sbin/dropbear $(1)/usr/bin/dbclient
	$(LN) ../sbin/dropbear $(1)/usr/bin/dropbearkey
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/dropbear.config $(1)/etc/config/dropbear
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dropbear.init $(1)/etc/init.d/dropbear
	$(INSTALL_DIR) $(1)/usr/lib/opkg/info
	$(INSTALL_DIR) $(1)/etc/dropbear
	touch $(1)/etc/dropbear/dropbear_rsa_host_key
	$(if $(CONFIG_DROPBEAR_ECC),touch $(1)/etc/dropbear/dropbear_ecdsa_host_key)
endef

define Package/dropbearconvert/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dropbearconvert $(1)/usr/bin/dropbearconvert
endef

$(eval $(call BuildPackage,dropbear))
$(eval $(call BuildPackage,dropbearconvert))
