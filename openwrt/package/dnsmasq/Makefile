#############################################################
#
# ebtables
#
#############################################################
include $(TOPDIR)/rules.mk

PKG_NAME:=dnsmasq
PKG_VERSION:=2.20
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://thekelleys.org.uk/dnsmasq
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	 $(WGET) -P $(DL_DIR) $(PKG_SOURCE_URL)/$(PKG_SOURCE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/src/$(PKG_NAME): $(PKG_BUILD_DIR)/.patched
	$(MAKE) CC=$(TARGET_CC) CFLAGS="$(TARGET_CFLAGS)" \
		BINDIR=/usr/sbin MANDIR=/usr/man -C $(PKG_BUILD_DIR)

$(PKG_IPK): $(PKG_BUILD_DIR)/src/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/sbin
	cp $(PKG_BUILD_DIR)/src/$(PKG_NAME) $(PKG_IPK_DIR)/usr/sbin/
	$(STRIP) $(PKG_IPK_DIR)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/dnsmasq.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/dnsmasq.list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
