include $(TOPDIR)/rules.mk

PKG_NAME:=bwm
PKG_VERSION:=1.1.0
PKG_RELEASE:=1
PKG_MD5SUM:=51021a036acb92d2bda0c0c0483f9552

PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/b/bwm
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION).orig
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.unpacked
	$(TARGET_CC) $(TARGET_CFLAGS) $(PKG_BUILD_DIR)/bwm.c -o $(PKG_BUILD_DIR)/bwm

$(PKG_IPK): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/bin
	install -m 755 $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_IPK_DIR)/usr/bin/
	$(STRIP) $(PKG_IPK_DIR)/usr/bin/*
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
