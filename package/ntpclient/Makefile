include $(TOPDIR)/rules.mk

PKG_NAME:=ntpclient
PKG_VERSION:=2003_194
PKG_RELEASE:=1
PKG_MD5SUM:=94e84d5c6023c5e1f3890b28d0a08c92

PKG_SOURCE_URL:=http://doolittle.faludi.com/ntpclient
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.unpacked
	$(MAKE) CC=$(TARGET_CC) CFLAGS="$(TARGET_CFLAGS)" -C $(PKG_BUILD_DIR)

$(PKG_IPK): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/sbin $(PKG_IPK_DIR)/etc/init.d
	install -m 755 $(PKG_BUILD_DIR)/ntpclient $(PKG_IPK_DIR)/usr/sbin/
	install -m 755 ./files/S41ntpclient $(PKG_IPK_DIR)/etc/init.d/
	$(STRIP) $(PKG_IPK_DIR)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/$(PKG_NAME).list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
