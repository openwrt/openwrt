# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=usbutils
PKG_VERSION:=0.70
PKG_RELEASE:=1
PKG_MD5SUM:=05276dc307a0297904bc892e9998bf59

PKG_SOURCE_URL:=@SF/linux-usb
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_LSUSB:=$(PACKAGE_DIR)/lsusb_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_LSUSB:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_LSUSB:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		ac_cv_func_malloc_0_nonnull=yes \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--datadir=/usr/share \
		--includedir=/usr/include \
		--infodir=/usr/share/info \
		--libdir=/usr/lib \
		--libexecdir=/usr/lib \
		--localstatedir=/var \
		--mandir=/usr/share/man \
		--sbindir=/usr/sbin \
		--sysconfdir=/etc \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC)

$(IPKG_LSUSB): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_LSUSB) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_LSUSB)/usr/share
	cp $(PKG_BUILD_DIR)/usb.ids $(IDIR_LSUSB)/usr/share/
	mkdir -p $(IDIR_LSUSB)/usr/sbin
	cp $(PKG_BUILD_DIR)/lsusb $(IDIR_LSUSB)/usr/sbin/
	$(STRIP) $(IDIR_LSUSB)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LSUSB) $(PACKAGE_DIR)

$(INFO_LSUSB): $(IPKG_LSUSB)
	$(IPKG) install $(IPKG_LSUSB)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_LSUSB)
install: $(INFO_LSUSB)

clean:
	rm -rf $(PKG_BUILD_DIR) $(IPKG_LSUSB)
