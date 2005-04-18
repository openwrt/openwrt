# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=chillispot
PKG_VERSION:=1.0RC3
PKG_RELEASE:=1
PKG_MD5SUM:=9ad5f4181ae2e9e2d5ccb165c37dfdde

PKG_SOURCE_URL:= \
	http://www.chillispot.org/download/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

IPKG_CHILLISPOT:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_CHILLISPOT:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_CHILLISPOT:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

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
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		ac_cv_func_malloc_0_nonnull=yes \
		ac_cv_func_memcmp_working=yes \
		ac_cv_func_setvbuf_reversed=no \
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
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS)
	touch $(PKG_BUILD_DIR)/.built

$(PKG_INSTALL_DIR)/usr/sbin/chilli: $(PKG_BUILD_DIR)/.built
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	
$(IPKG_CHILLISPOT): $(PKG_INSTALL_DIR)/usr/sbin/chilli
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_CHILLISPOT) ./ipkg/$(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	install -m0644 ./ipkg/$(PKG_NAME).conffiles $(IDIR_CHILLISPOT)/CONTROL/conffiles
	install -m0755 -d $(IDIR_CHILLISPOT)/etc/init.d
	install -m0755 ./ipkg/$(PKG_NAME).init $(IDIR_CHILLISPOT)/etc/init.d/chilli
	install -m0755 -d $(IDIR_CHILLISPOT)/etc
	install -m0644 $(PKG_BUILD_DIR)/doc/chilli.conf $(IDIR_CHILLISPOT)/etc/
	install -m0755 -d $(IDIR_CHILLISPOT)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/chilli $(IDIR_CHILLISPOT)/usr/sbin/
	$(RSTRIP) $(IDIR_CHILLISPOT)
	$(IPKG_BUILD) $(IDIR_CHILLISPOT) $(PACKAGE_DIR)

$(INFO_CHILLISPOT): $(IPKG_CHILLISPOT)
	$(IPKG) install $(IPKG_CHILLISPOT)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_CHILLISPOT)
install: $(INFO_CHILLISPOT)

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -rf $(IPKG_CHILLISPOT)
