# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=cups
PKG_VERSION:=1.1.23
PKG_RELEASE:=1
PKG_MD5SUM:=4ce09b1dce09b6b9398af0daae9adf63

PKG_SOURCE_URL:=ftp://ftp3.easysw.com/pub/cups/1.1.23/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-source.tar.bz2
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=bzcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	 $(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.patched
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		ac_cv_path_STRIP="$(STRIP)" \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=$(CUPS_IPK_DIR)/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		--program-prefix="" \
		--with-gnu-ld \
		--with-cups-user=root \
		--with-cups-group=root \
		--without-perl \
		--without-python \
		--without-php \
		--disable-slp \
		--disable-gnutls \
		--disable-openssl \
		--disable-cdsassl \
		--disable-ssl \
		--disable-slp \
	);
	touch  $(PKG_BUILD_DIR)/.configured


$(PKG_IPK_DIR)/usr/sbin/cupsd: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)
	$(MAKE) STRIP=$(STRIP) DSTROOT=$(PKG_IPK_DIR) -C $(PKG_BUILD_DIR) install
	cp -a $(PKG_IPK_DIR)/usr/lib/* $(STAGING_DIR)/lib/
	cp -a $(PKG_IPK_DIR)/usr/include/* $(STAGING_DIR)/include/

$(PKG_IPK): $(PKG_IPK_DIR)/usr/sbin/cupsd
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	rm -f $(PKG_IPK_DIR)/usr/bin/cups-config
	rm -f $(PKG_IPK_DIR)/usr/lib/*.a
	rm -f $(PKG_IPK_DIR)/usr/share/doc/cups/* || true
	rm -rf $(PKG_IPK_DIR)/usr/share/doc/cups/??
	rm -rf $(PKG_IPK_DIR)/usr/share/locale
	rm -rf $(PKG_IPK_DIR)/usr/share/cups/banners
	rm -rf $(PKG_IPK_DIR)/usr/share/cups/charsets
	rm -rf $(PKG_IPK_DIR)/usr/share/cups/fonts
	rm -rf $(PKG_IPK_DIR)/usr/share/cups/model
	rm -rf $(PKG_IPK_DIR)/usr/share/cups/data
	rm -rf $(PKG_IPK_DIR)/usr/share/cups/templates/??
	rm -rf $(PKG_IPK_DIR)/usr/include
	rm -rf $(PKG_IPK_DIR)/var
	rm -rf $(PKG_IPK_DIR)/etc/*.d
	$(STRIP) $(PKG_IPK_DIR)/usr/bin/*
	$(STRIP) $(PKG_IPK_DIR)/usr/lib/cups/backend/*
	$(STRIP) $(PKG_IPK_DIR)/usr/lib/cups/cgi-bin/*
	$(STRIP) $(PKG_IPK_DIR)/usr/lib/cups/daemon/*
	cp -a ./files/* $(PKG_IPK_DIR)/
	find $(PKG_IPK_DIR) -name CVS | xargs rm -rf
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(PKG_IPK)
install:
	$(IPKG) install $(PKG_IPK)
clean:
	rm -rf $(PKG_BUILD_DIR)
