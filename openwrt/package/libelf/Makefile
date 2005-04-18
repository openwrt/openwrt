# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libelf
PKG_VERSION:=0.8.5
PKG_RELEASE:=1
PKG_MD5SUM:=c1daf069367871350ece779b7de20047

PKG_SOURCE_URL:=\
	http://www.mr511.de/software/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_LIBELF:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_LIBELF:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_LIBELF:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

.NOTPARALLEL:

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache config.status; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		mr_cv_target_elf=yes \
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
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) -j1 \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld ARCH=$(ARCH) \
		
	touch $(PKG_BUILD_DIR)/.built

$(STAGING_DIR)/usr/lib/libelf.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C $(PKG_BUILD_DIR) -j1 \
		instroot="$(STAGING_DIR)" \
		install

$(IPKG_LIBELF): $(STAGING_DIR)/usr/lib/libelf.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_LIBELF) ./ipkg/$(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_LIBELF)/usr/lib
	cp -fpR $(PKG_BUILD_DIR)/lib/libelf.so.* $(IDIR_LIBELF)/usr/lib/
	$(STRIP) $(IDIR_LIBELF)/usr/lib/lib*.so*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBELF) $(PACKAGE_DIR)

$(INFO_LIBELF): $(IPKG_LIBELF)
	$(IPKG) install $(IPKG_LIBELF)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_LIBELF)
install: $(INFO_LIBELF)

clean:
	-$(MAKE) -C $(PKG_BUILD_DIR) -j1 \
		instroot="$(STAGING_DIR)" \
		uninstall
	rm -rf $(PKG_BUILD_DIR) $(IPKG_LIBELF)
