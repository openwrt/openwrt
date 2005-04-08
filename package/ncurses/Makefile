# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := ncurses
PKG_VERSION := 5.2
PKG_RELEASE := 5
PKG_MD5SUM := 464d6a49010cf2a6eb9ce59a264d4d47

PKG_SOURCE_SITE := ftp://ftp.gnu.org/pub/gnu/$(PKG_NAME) \
	    ftp://gatekeeper.dec.com/pub/GNU/$(PKG_NAME) \
        ftp://ftp.uu.net/archive/systems/gnu/$(PKG_NAME) \
        ftp://ftp.eu.uu.net/pub/gnu/$(PKG_NAME) \
        ftp://ftp.funet.fi/pub/gnu/prep/$(PKG_NAME) \
        ftp://ftp.leo.org/pub/comp/os/unix/gnu/$(PKG_NAME) \
        ftp://ftp.digex.net/pub/gnu/$(PKG_NAME) 
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/lib$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

.NOTPARALLEL:

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/ipkg/rules: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
#	$(PATCH) $(PKG_BUILD_DIR) ./patches
	cp -fpR ./ipkg $(PKG_BUILD_DIR)/
	chmod a+x $(PKG_BUILD_DIR)/ipkg/rules

$(PKG_IPK): $(PKG_BUILD_DIR)/ipkg/rules
	cd $(PKG_BUILD_DIR); \
	TOPDIR="$(TOPDIR)" IPKG_RULES_INC="$(TOPDIR)/rules.mk" \
	INSTALL_DIR="$(STAGING_DIR)" \
	$(MAKE) -j1 -f ./ipkg/rules package

$(IPKG_STATE_DIR)/info/lib$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/ipkg/rules
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/lib$(PKG_NAME).list

clean:
	-cd $(PKG_BUILD_DIR); \
	TOPDIR="$(TOPDIR)" IPKG_RULES_INC="$(TOPDIR)/rules.mk" \
	INSTALL_DIR="$(STAGING_DIR)" \
	./ipkg/rules clean
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
