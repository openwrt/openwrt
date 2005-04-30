# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ipkg
PKG_VERSION:=0.99.145
PKG_RELEASE:=1
PKG_MD5SUM:=d98799dbcf911f90a7c3c897713cc37a

PKG_SOURCE_URL:=http://www.handhelds.org/packages/ipkg
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_IPKG:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_IPKG:=$(PKG_BUILD_DIR)/idir/$(PKG_NAME)
INFO_IPKG:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
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

$(IPKG_IPKG): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_IPKG) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_IPKG)/bin
	cp -a $(PKG_BUILD_DIR)/.libs/ipkg-cl $(IDIR_IPKG)/bin/ipkg
	$(STRIP) $(IDIR_IPKG)/bin/*
	mkdir -p $(IDIR_IPKG)/lib
	cp -a $(PKG_BUILD_DIR)/.libs/libipkg.so* $(IDIR_IPKG)/lib/
	$(STRIP) $(IDIR_IPKG)/lib/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_IPKG) $(PACKAGE_DIR)

$(INFO_IPKG): $(IPKG_IPKG)
	$(IPKG) install $(IPKG_IPKG)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_IPKG)
install: $(INFO_IPKG)

clean:
	rm -rf $(PKG_BUILD_DIR) $(IPKG_IPKG)
