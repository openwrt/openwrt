include $(TOPDIR)/rules.mk

PKG_NAME := popt
PKG_VERSION := 1.7
PKG_RELEASE := 3

PKG_SOURCE_SITE := ftp://ftp.rpm.org/pub/rpm/dist/rpm-4.1.x/
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/lib$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk


$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) x $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/ipkg/rules: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) . $(PKG_NAME)-\*.diff
	chmod a+x $(PKG_BUILD_DIR)/ipkg/rules

$(PKG_IPK): $(PKG_BUILD_DIR)/ipkg/rules
	cd $(PKG_BUILD_DIR); \
	ipkg/rules \
	  TOPDIR="$(TOPDIR)" \
	  IPKG_RULES_INC="$(TOPDIR)/rules.mk" \
	  INSTALL_DIR="$(STAGING_DIR)" \
	 package

$(IPKG_STATE_DIR)/info/lib$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/ipkg/rules
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/lib$(PKG_NAME).list

clean:
	-cd $(PKG_BUILD_DIR); \
	ipkg/rules \
	  TOPDIR="$(TOPDIR)" IPKG_RULES_INC="$(TOPDIR)/rules.mk" \
	  INSTALL_DIR="$(STAGING_DIR)" \
	 clean
	rm -rf $(PKG_BUILD_DIR)
	rm -r $(PKG_IPK)

