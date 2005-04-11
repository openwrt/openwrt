# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := dhcp-forwarder
PKG_VERSION := 0.7
PKG_RELEASE := 4
PKG_MD5SUM := e7f876e615ebc3f96418f6477b4451e2

PKG_SOURCE_URL := http://savannah.nongnu.org/download/dhcp-fwd/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE := $(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT := bzcat
PKG_IPK := $(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR := $(PKG_BUILD_DIR)/ipkg
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

$(DL_DIR)/$(PKG_SOURCE):
	 $(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.unpacked
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		ac_cv_func_malloc_0_nonnull="yes" \
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
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
	)
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  cfg_filename="/etc/dhcp-fwd.conf"
	touch $(PKG_BUILD_DIR)/.built

$(PKG_INSTALL_DIR)/usr/sbin/dhcp-fwd: $(PKG_BUILD_DIR)/.built
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  DESTDIR="$(PKG_INSTALL_DIR)" \
	 install
	
$(PKG_IPK): $(PKG_INSTALL_DIR)/usr/sbin/dhcp-fwd
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	cp -fp $(PKG_NAME).conffiles $(PKG_IPK_DIR)/CONTROL/conffiles
	chmod 0644 $(PKG_IPK_DIR)/CONTROL/conffiles
	mkdir -p $(PKG_IPK_DIR)/etc
	cp -fp $(PKG_BUILD_DIR)/contrib/dhcp-fwd.conf $(PKG_IPK_DIR)/etc/
	chmod 0644 $(PKG_IPK_DIR)/etc/dhcp-fwd.conf
	mkdir -p $(PKG_IPK_DIR)/etc/init.d
	cp -fp $(PKG_NAME).init $(PKG_IPK_DIR)/etc/init.d/dhcp-fwd
	chmod 0755 $(PKG_IPK_DIR)/etc/init.d/dhcp-fwd
	mkdir -p $(PKG_IPK_DIR)/usr/bin
	cp -fp $(PKG_INSTALL_DIR)/usr/sbin/dhcp-fwd $(PKG_IPK_DIR)/usr/bin/
	$(STRIP) $(PKG_IPK_DIR)/usr/bin/*
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

