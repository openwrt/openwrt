# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=maradns
PKG_VERSION:=1.0.26
PKG_RELEASE:=1
PKG_MD5SUM:=b3180ad1f250a4e3820613e711ae159b

PKG_SOURCE_URL:=http://www.maradns.org/download
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_MARADNS:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_MARADNS:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_MARADNS:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
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
		CFLAGS="$(TARGET_CFLAGS)" \
		FLAGS="$(TARGET_CFLAGS)" \
		CC=$(TARGET_CC) \
		HOSTCC="$(HOSTCC)"

$(IPKG_MARADNS): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_MARADNS) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_MARADNS)/usr/bin
	mkdir -p $(IDIR_MARADNS)/usr/sbin
	cp $(PKG_BUILD_DIR)/server/maradns $(IDIR_MARADNS)/usr/sbin/
	cp $(PKG_BUILD_DIR)/tuzona/getzone $(IDIR_MARADNS)/usr/bin/
	cp $(PKG_BUILD_DIR)/tuzona/zoneserver $(IDIR_MARADNS)/usr/sbin/
	cp $(PKG_BUILD_DIR)/tools/askmara $(IDIR_MARADNS)/usr/bin/
	$(STRIP) $(IDIR_MARADNS)/usr/sbin/*
	$(STRIP) $(IDIR_MARADNS)/usr/bin/*
	cp -a ./files/* $(IDIR_MARADNS)/
	find $(IDIR_MARADNS) -name CVS | xargs rm -rf
	echo "/etc/maradns/maradnsrc" > $(IDIR_MARADNS)/CONTROL/conffiles
	chmod 755 $(IDIR_MARADNS)/etc/init.d/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_MARADNS) $(PACKAGE_DIR)

$(INFO_MARADNS): $(IPKG_MARADNS)
	$(IPKG) install $(IPKG_MARADNS)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_MARADNS)
install: $(INFO_MARADNS)

clean:
	rm -rf $(PKG_BUILD_DIR) $(IPKG_MARADNS)
