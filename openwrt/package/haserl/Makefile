# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := haserl
PKG_VERSION := 0.8.0
PKG_RELEASE := 1
PKG_MD5SUM := bd9195d086566f56634c0bcbbbcbebea

PKG_SOURCE_URL := \
	@SF/haserl \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT := zcat

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

IPKG_HASERL:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_HASERL:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_HASERL:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	 $(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	rm -rf $(PKG_BUILD_DIR)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -DMAX_UPLOAD_KB=8192" \
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
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $(PKG_BUILD_DIR)/.built

$(PKG_INSTALL_DIR)/usr/bin/haserl: $(PKG_BUILD_DIR)/.built
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	
$(IPKG_HASERL): $(PKG_INSTALL_DIR)/usr/bin/haserl
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_HASERL) ./ipkg/$(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	install -m0755 -d $(IDIR_HASERL)/usr/bin
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/haserl $(IDIR_HASERL)/usr/bin/
	$(RSTRIP) $(IDIR_HASERL)
	$(IPKG_BUILD) $(IDIR_HASERL) $(PACKAGE_DIR)

$(INFO_HASERL): $(IPKG_HASERL)
	$(IPKG) install $(IPKG_HASERL)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_HASERL)
install: $(INFO_HASERL)

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -rf $(IPKG_HASERL)
