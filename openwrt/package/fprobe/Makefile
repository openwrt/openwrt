# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := fprobe
PKG_VERSION := 1.1
PKG_RELEASE := 1
PKG_MD5SUM := 65850d0470078269b33eee58cba77ac2

PKG_SOURCE := $(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL := @SF/fprobe
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT := bzcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,FPROBE,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR) ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/usr/bin \
			--sbindir=/usr/sbin \
			--libexecdir=/usr/lib \
			--datadir=/usr/share \
			--infodir=/usr/share/info \
			--mandir=/usr/share/man \
			--localstatedir=/var \
			--sysconfdir=/etc \
			--with-piddir=/var \
			--with-pcap=$(STAGING_DIR)/usr/include \
			--with-libpcap=$(STAGING_DIR)/usr/lib \
			--with-membulk=index8 \
			--with-hash=xor8 \
			$(DISABLE_NLS) \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $@

$(IPKG_FPROBE): 
	mkdir -p $(IDIR_FPROBE)/usr/bin
	cp $(PKG_BUILD_DIR)/src/fprobe $(IDIR_FPROBE)/usr/bin/
	$(STRIP) $(IDIR_FPROBE)/usr/bin/*
	$(IPKG_BUILD) $(IDIR_FPROBE) $(PACKAGE_DIR)
