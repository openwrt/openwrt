#
# Copyright (C) 2006-2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

# version follows Mozilla NSS library releases
PKG_NAME:=ca-certificates
PKG_VERSION:=3.83

DL_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

PKG_RELEASE:=1
PKG_MAINTAINER:=

PKG_SOURCE:=certdata.txt
PKG_VERSION_URL:=$(subst .,_,$(PKG_VERSION))
PKG_SOURCE_URL:=https://hg.mozilla.org/projects/nss/raw-file/NSS_$(PKG_VERSION_URL)_RTM/lib/ckfw/builtins
PKG_HASH:=3ff8bd209b5f2e739b9f2b96eacb694a774114685b02978257824f37ff528f71
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/ca-certificates
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=System CA certificates
  PKGARCH:=all
  PROVIDES:=ca-certs
endef

define Package/ca-bundle
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=System CA certificates as a bundle
  PKGARCH:=all
  PROVIDES:=ca-certs
endef

define Build/Prepare
	$(CP) $(DL_DIR)/$(PKG_SOURCE) $(PKG_BUILD_DIR)
	$(CP) ./files/* $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	( \
		cd $(PKG_BUILD_DIR); \
		$(STAGING_DIR_HOST)/bin/python3 \
			$(SCRIPT_DIR)/certdata2pem.py \
			$(PKG_BUILD_DIR)/$(PKG_SOURCE) \
			$(PKG_BUILD_DIR)/blacklist.txt \
		; \
	)
endef

define Build/Install
	mkdir -p \
		$(PKG_INSTALL_DIR)/usr/sbin \
		$(PKG_INSTALL_DIR)/usr/share/ca-certificates
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/update-ca-certificates $(PKG_INSTALL_DIR)/usr/sbin
	for p in $(PKG_BUILD_DIR)/*.crt; do \
		$(INSTALL_DATA) $$$$p $(PKG_INSTALL_DIR)/usr/share/ca-certificates ; \
	done
endef

define Package/ca-certificates/install
	$(INSTALL_DIR) $(1)/etc/ssl/certs
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/share/ca-certificates/*.crt $(1)/etc/ssl/certs/

	for CERTFILE in `ls -1 $(1)/etc/ssl/certs`; do \
		HASH=`openssl x509 -hash -noout -in $(1)/etc/ssl/certs/$$$$CERTFILE` ; \
		SUFFIX=0 ; \
		while [ -h "$(1)/etc/ssl/certs/$$$$HASH.$$$$SUFFIX" ]; do \
			let "SUFFIX += 1" ; \
		done ; \
		$(LN) "$$$$CERTFILE" "$(1)/etc/ssl/certs/$$$$HASH.$$$$SUFFIX" ; \
	done
endef

define Package/ca-bundle/install
	$(INSTALL_DIR) $(1)/etc/ssl/certs
	cat $(PKG_INSTALL_DIR)/usr/share/ca-certificates/*.crt >$(1)/etc/ssl/certs/ca-certificates.crt
	$(LN) /etc/ssl/certs/ca-certificates.crt $(1)/etc/ssl/cert.pem
endef

$(eval $(call BuildPackage,ca-bundle))
$(eval $(call BuildPackage,ca-certificates))
