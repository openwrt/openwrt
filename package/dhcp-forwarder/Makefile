# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := dhcp-forwarder
PKG_VERSION := 0.7
PKG_RELEASE := 4
PKG_MD5SUM := e7f876e615ebc3f96418f6477b4451e2

PKG_SOURCE_SITE := http://savannah.nongnu.org/download/dhcp-fwd/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_CAT := bzcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/ipkg/rules: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
#	$(PATCH) $(PKG_BUILD_DIR) ./patches
	cp -fpR ./ipkg $(PKG_BUILD_DIR)/
	chmod a+x $(PKG_BUILD_DIR)/ipkg/rules
	touch $(PKG_BUILD_DIR)/ipkg/rules

$(PKG_IPK): $(PKG_BUILD_DIR)/ipkg/rules
	cd $(PKG_BUILD_DIR); \
	TOPDIR="$(TOPDIR)" IPKG_RULES_INC="$(TOPDIR)/rules.mk" \
	./ipkg/rules package

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/ipkg/rules
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/$(PKG_NAME).list

clean:
	-cd $(PKG_BUILD_DIR); \
	TOPDIR="$(TOPDIR)" IPKG_RULES_INC="$(TOPDIR)/rules.mk" \
	./ipkg/rules clean
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)

