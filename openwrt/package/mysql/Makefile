# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := mysql
PKG_VERSION := 4.0.24
PKG_RELEASE := 2
PKG_MD5SUM := 408d3001ed715ddc90009c247e548638

PKG_SOURCE_SITE := \
	http://mysql.he.net/Downloads/MySQL-4.0/ \
	http://www.linorg.usp.br/mysql/Downloads/MySQL-4.0/ \
	http://mysql.planetmirror.com/Downloads/MySQL-4.0/ \
	http://mysql.holywar.net/Downloads/MySQL-4.0/ \
	http://netmirror.org/mirror/mysql.com/Downloads/MySQL-4.0/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/libmysqlclient_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk


$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/ipkg/rules: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	rm -rf $(PKG_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	cp -fpR ./ipkg $(PKG_BUILD_DIR)/
	chmod a+x $(PKG_BUILD_DIR)/ipkg/rules
	touch $(PKG_BUILD_DIR)/ipkg/rules

$(PKG_IPK): $(PKG_BUILD_DIR)/ipkg/rules
	cd $(PKG_BUILD_DIR); \
	TOPDIR="$(TOPDIR)" IPKG_RULES_INC="$(TOPDIR)/rules.mk" \
	INSTALL_DIR="$(STAGING_DIR)" \
	./ipkg/rules package

$(IPKG_STATE_DIR)/info/libmysqlclient.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/ipkg/rules
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/libmysqlclient.list

clean:
	-cd $(PKG_BUILD_DIR); \
	TOPDIR="$(TOPDIR)" IPKG_RULES_INC="$(TOPDIR)/rules.mk" \
	INSTALL_DIR="$(STAGING_DIR)" \
	./ipkg/rules clean
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)

