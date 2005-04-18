# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := dhcp-forwarder
PKG_VERSION := 0.7
PKG_RELEASE := 4
PKG_MD5SUM := e7f876e615ebc3f96418f6477b4451e2

PKG_SOURCE_URL := \
	http://savannah.nongnu.org/download/dhcp-fwd/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE := $(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT := bzcat

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

IPKG_DHCP_FORWARDER:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_DHCP_FORWARDER:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_DHCP_FORWARDER:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	 $(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	rm -rf $(PKG_BUILD_DIR)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
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
	
$(IPKG_DHCP_FORWARDER): $(PKG_INSTALL_DIR)/usr/sbin/dhcp-fwd
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_DHCP_FORWARDER) ./ipkg/$(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	install -m0644 ./ipkg/$(PKG_NAME).conffiles $(IDIR_DHCP_FORWARDER)/CONTROL/conffiles
	install -m0755 -d $(IDIR_DHCP_FORWARDER)/etc/init.d
	install -m0755 ./ipkg/$(PKG_NAME).init $(IDIR_DHCP_FORWARDER)/etc/init.d/dhcp-fwd
	install -m0755 -d $(IDIR_DHCP_FORWARDER)/etc
	install -m0644 $(PKG_BUILD_DIR)/contrib/dhcp-fwd.conf $(IDIR_DHCP_FORWARDER)/etc/
	install -m0755 -d $(IDIR_DHCP_FORWARDER)/usr/bin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/dhcp-fwd $(IDIR_DHCP_FORWARDER)/usr/bin/
	$(RSTRIP) $(IDIR_DHCP_FORWARDER)
	$(IPKG_BUILD) $(IDIR_DHCP_FORWARDER) $(PACKAGE_DIR)

$(INFO_DHCP_FORWARDER): $(IPKG_DHCP_FORWARDER)
	$(IPKG) install $(IPKG_DHCP_FORWARDER)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_DHCP_FORWARDER)
install: $(INFO_DHCP_FORWARDER)

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -rf $(IPKG_DHCP_FORWARDER)
