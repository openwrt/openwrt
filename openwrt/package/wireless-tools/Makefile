# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wireless-tools
PKG_VERSION:=27
PKG_RELEASE:=1
PKG_MD5SUM:=29891b66f23565e7e381d28404abb29a

PKG_SOURCE_URL:=http://pcmcia-cs.sourceforge.net/ftp/contrib/
PKG_SOURCE:=wireless_tools.$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/wireless_tools.$(PKG_VERSION)
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.patched
	$(MAKE) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS) -I." -C $(PKG_BUILD_DIR)

$(PKG_IPK): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/lib
	mkdir -p $(PKG_IPK_DIR)/usr/sbin
	cp $(PKG_BUILD_DIR)/libiw.so.27 $(PKG_IPK_DIR)/usr/lib
	cp $(PKG_BUILD_DIR)/iwconfig $(PKG_IPK_DIR)/usr/sbin
	cp $(PKG_BUILD_DIR)/ifrename $(PKG_IPK_DIR)/usr/sbin
	cp $(PKG_BUILD_DIR)/iwlist $(PKG_IPK_DIR)/usr/sbin
	cp $(PKG_BUILD_DIR)/iwspy $(PKG_IPK_DIR)/usr/sbin
	cp $(PKG_BUILD_DIR)/iwpriv $(PKG_IPK_DIR)/usr/sbin
	cp $(PKG_BUILD_DIR)/iwevent $(PKG_IPK_DIR)/usr/sbin
	cp $(PKG_BUILD_DIR)/iwgetid $(PKG_IPK_DIR)/usr/sbin
	$(STRIP) $(PKG_IPK_DIR)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/$(PKG_NAME).list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
