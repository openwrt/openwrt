# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=nmap
PKG_VERSION:=3.81
PKG_RELEASE:=1
PKG_MD5SUM:=0713306dda85aee2c95ef31b4b7d2838

PKG_SOURCE_URL:=http://download.insecure.org/nmap/dist
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_NMAP:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_NMAP:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_NMAP:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		libpcapdir="$(STAGING_DIR)/usr/lib" \
		CFLAGS="$(TARGET_CFLAGS)" \
		CXXFLAGS="$(TARGET_CFLAGS) -fno-builtin -nostdinc++ -nodefaultlibs " \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib  -fno-builtin -nostdinc++ -nodefaultlibs -luClibc++ -lc -lm" \
		ac_cv_prog_CXX="$(TARGET_CXX)" \
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
		--without-openssl \
		--with-libpcap="$(STAGING_DIR)/usr" \
		--with-libpcre="$(STAGING_DIR)/usr" \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CCOPT="-fno-builtin -nostdinc++ -nodefaultlibs $(TARGET_CFLAGS)"

$(IPKG_NMAP): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_NMAP) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_NMAP)/usr/share/nmap
	for file in mac-prefixes os-fingerprints protocols rpc service-probes services; do \
		cp $(PKG_BUILD_DIR)/nmap-$$file $(IDIR_NMAP)/usr/share/nmap; \
	done
	mkdir -p $(IDIR_NMAP)/usr/sbin
	cp $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_NMAP)/usr/sbin/
	$(STRIP) $(IDIR_NMAP)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_NMAP) $(PACKAGE_DIR)

$(INFO_NMAP): $(IPKG_NMAP)
	$(IPKG) install $(IPKG_NMAP)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_NMAP)
install: $(INFO_NMAP)

clean:
	rm -rf $(PKG_BUILD_DIR) $(IPKG_NMAP)
