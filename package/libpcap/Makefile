# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := libpcap
PKG_VERSION := 0.8.3
PKG_RELEASE := 1
PKG_MD5SUM := 56a9d4615d8354fcfe8cff8c8443c77b

PKG_SOURCE_SITE := http://www.tcpdump.org/release
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.patched
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		ac_cv_linux_vers=$(BR2_DEFAULT_KERNEL_HEADERS) \
		BUILD_CC=$(TARGET_CC) HOSTCC=$(HOSTCC) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
		--enable-shared \
		--enable-static \
		--disable-yydebug \
		--with-build-cc=$(HOSTCC) \
		--with-pcap=linux \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/libpcap.a: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CCOPT="$(TARGET_CFLAGS)" \

$(STAGING_DIR)/usr/lib/libpcap.a: $(PKG_BUILD_DIR)/libpcap.a
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		install

$(PKG_IPK): $(STAGING_DIR)/usr/lib/libpcap.a
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) ./ipkg/$(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/lib
	cp -fpR $(STAGING_DIR)/usr/lib/libpcap.so.* $(PKG_IPK_DIR)/usr/lib/
	$(STRIP) $(PKG_IPK_DIR)/usr/lib/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(STAGING_DIR)/usr/lib/libpcap.a
install: $(IPKG_STATE_DIR)/info/$(PKG_NAME).list

clean:
	-$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		uninstall clean
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
