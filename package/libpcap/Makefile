# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libpcap
PKG_VERSION:=0.8.3
PKG_RELEASE:=1
PKG_MD5SUM:=56a9d4615d8354fcfe8cff8c8443c77b

PKG_SOURCE_URL:= \
	http://www.tcpdump.org/release \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_LIBPCAP:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_LIBPCAP:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_LIBPCAP:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	mkdir -p $(BUILD_DIR)/$(PKG_NAME)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR)/$(PKG_NAME) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		ac_cv_linux_vers=$(BR2_DEFAULT_KERNEL_HEADERS) \
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
		  --enable-shared \
		  --enable-static \
		  --disable-yydebug \
		  --with-build-cc=$(HOSTCC) \
		  --with-pcap=linux \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CCOPT="$(TARGET_CFLAGS)"
	touch $(PKG_BUILD_DIR)/.built

$(STAGING_DIR)/usr/lib/libpcap.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		install

$(IPKG_LIBPCAP): $(STAGING_DIR)/usr/lib/libpcap.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_LIBPCAP) ./ipkg/$(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	install -m0755 -d $(IDIR_LIBPCAP)/usr/lib
	cp -fpR $(STAGING_DIR)/usr/lib/libpcap.so.* $(IDIR_LIBPCAP)/usr/lib/
	$(RSTRIP) $(IDIR_LIBPCAP)
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBPCAP) $(PACKAGE_DIR)

$(INFO_LIBPCAP): $(IPKG_LIBPCAP)
	$(IPKG) install $(IPKG_LIBPCAP)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_LIBPCAP)
install: $(INFO_LIBPCAP)

clean:
	-$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		uninstall
	rm -rf $(PKG_BUILD_DIR) $(IPKG_LIBPCAP)
