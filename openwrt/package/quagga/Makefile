# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=quagga
PKG_VERSION:=0.98.3
PKG_RELEASE:=1
PKG_MD5SUM:=68be5e911e4d604c0f5959338263356e

PKG_SOURCE_URL:=http://www.quagga.net/download/ \
                http://www.de.quagga.net/download/ \
                http://www.uk.quagga.net/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)-all_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg/quagga

PKG_ZEBRA_DIR:=$(PKG_BUILD_DIR)/ipkg/zebra
PKG_BGPD_DIR:=$(PKG_BUILD_DIR)/ipkg/bgpd
PKG_OSPFD_DIR:=$(PKG_BUILD_DIR)/ipkg/ospfd
PKG_RIPD_DIR:=$(PKG_BUILD_DIR)/ipkg/ripd

# separate ipkg for zebra,bgpd,ospf and ripd
PKG_ZEBRA:=$(PACKAGE_DIR)/$(PKG_NAME)-zebra_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_BGPD:=$(PACKAGE_DIR)/$(PKG_NAME)-bgpd_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_RIPD:=$(PACKAGE_DIR)/$(PKG_NAME)-ripd_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_OSPFD:=$(PACKAGE_DIR)/$(PKG_NAME)-ospfd_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

TARGETS:=$(PKG_IPK)
ifneq ($(BR2_PACKAGE_QUAGGA_ZEBRA),)
TARGETS += $(PKG_ZEBRA)
endif
ifneq ($(BR2_PACKAGE_QUAGGA_BGPD),)
TARGETS += $(PKG_BGPD)
endif
ifneq ($(BR2_PACKAGE_QUAGGA_RIPD),)
TARGETS += $(PKG_RIPD)
endif
ifneq ($(BR2_PACKAGE_QUAGGA_OSPFD),)
TARGETS += $(PKG_OSPFD)
endif

INSTALL_TARGETS := $(IPKG_STATE_DIR)/info/quagga.list
ifeq ($(BR2_PACKAGE_QUAGGA_ZEBRA),y)
INSTALL_TARGETS := $(IPKG_STATE_DIR)/info/quagga-zebra.list
endif
ifeq ($(BR2_PACKAGE_QUAGGA_BGPD),y)
INSTALL_TARGETS := $(IPKG_STATE_DIR)/info/quagga-bgpd.list
endif
ifeq ($(BR2_PACKAGE_QUAGGA_RIPD),y)
INSTALL_TARGETS := $(IPKG_STATE_DIR)/info/quagga-ripd.list
endif
ifeq ($(BR2_PACKAGE_QUAGGA_OSPFD),y)
INSTALL_TARGETS := $(IPKG_STATE_DIR)/info/quagga-ospfd.list
endif

$(DL_DIR)/$(PKG_SOURCE):
	 $(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.unpacked
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
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
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		--disable-static \
		--disable-ipv6 \
		--disable-vtysh \
		--enable-user=quagga \
		--enable-group=quagga \
		--sysconfdir=/etc/quagga \
		--disable-exampledir \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_IPK_DIR)/zebra: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)
	$(MAKE) DESTDIR=$(PKG_IPK_DIR) -C $(PKG_BUILD_DIR) install

$(PKG_IPK): $(PKG_IPK_DIR)/zebra
	cp -a ./ipkg/* $(PKG_IPK_DIR)/
	find $(PKG_IPK_DIR) -name CVS | xargs rm -rf
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) control/$(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/sbin
	mkdir -p $(PKG_IPK_DIR)/usr/lib
	mkdir -p $(PKG_IPK_DIR)/etc/quagga
	rm $(PKG_IPK_DIR)/usr/lib/*.la
	rm -rf $(PKG_IPK_DIR)/usr/include
	rm -rf $(PKG_IPK_DIR)/usr/info
	rm -rf $(PKG_IPK_DIR)/usr/man
	$(STRIP) $(PKG_IPK_DIR)/usr/lib/*
	$(STRIP) $(PKG_IPK_DIR)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/quagga.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

$(PKG_ZEBRA): $(PKG_IPK_DIR)/zebra
	mkdir -p $(PKG_ZEBRA_DIR)/etc/quagga
	mkdir -p $(PKG_ZEBRA_DIR)/usr/sbin
	mkdir -p $(PKG_ZEBRA_DIR)/usr/lib
	cp -a ./ipkg/* $(PKG_ZEBRA_DIR)/
	find $(PKG_ZEBRA_DIR) -name CVS | xargs rm -rf
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_ZEBRA_DIR) control/$(PKG_NAME)-zebra.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	cp $(PKG_IPK_DIR)/usr/sbin/zebra $(PKG_ZEBRA_DIR)/usr/sbin
	cp $(PKG_IPK_DIR)/usr/lib/libzebra* $(PKG_ZEBRA_DIR)/usr/lib 	
	$(IPKG_BUILD) $(PKG_ZEBRA_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/quagga-zebra.list: $(PKG_ZEBRA)
	$(IPKG) install $(PKG_ZEBRA)

$(PKG_BGPD): $(PKG_IPK_DIR)/zebra
	mkdir -p $(PKG_BGPD_DIR)/etc/quagga
	mkdir -p $(PKG_BGPD_DIR)/usr/sbin
	cp -a ./ipkg/* $(PKG_BGPD_DIR)/
	find $(PKG_BGPD_DIR) -name CVS | xargs rm -rf
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_BGPD_DIR) control/$(PKG_NAME)-bgpd.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	cp $(PKG_IPK_DIR)/usr/sbin/bgpd $(PKG_BGPD_DIR)/usr/sbin
	$(IPKG_BUILD) $(PKG_BGPD_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/quagga-bgpd.list: $(PKG_BGPD)
	$(IPKG) install $(PKG_BGPD)

$(PKG_RIPD): $(PKG_IPK_DIR)/zebra
	mkdir -p $(PKG_RIPD_DIR)/etc/quagga
	mkdir -p $(PKG_RIPD_DIR)/usr/sbin
	cp -a ./ipkg/* $(PKG_RIPD_DIR)/
	find $(PKG_RIPD_DIR) -name CVS | xargs rm -rf
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_RIPD_DIR) control/$(PKG_NAME)-ripd.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	cp $(PKG_IPK_DIR)/usr/sbin/ripd $(PKG_RIPD_DIR)/usr/sbin
	$(IPKG_BUILD) $(PKG_RIPD_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/quagga-ripd.list: $(PKG_RIPD)
	$(IPKG) install $(PKG_RIPD)

$(PKG_OSPFD): $(PKG_IPK_DIR)/zebra
	mkdir -p $(PKG_OSPFD_DIR)/etc/quagga
	mkdir -p $(PKG_OSPFD_DIR)/usr/sbin
	mkdir -p $(PKG_OSPFD_DIR)/usr/lib
	cp -a ./ipkg/* $(PKG_OSPFD_DIR)/
	find $(PKG_OSPFD_DIR) -name CVS | xargs rm -rf
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_OSPFD_DIR) control/$(PKG_NAME)-ospfd.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	cp $(PKG_IPK_DIR)/usr/sbin/ospfd $(PKG_OSPFD_DIR)/usr/sbin
	cp $(PKG_IPK_DIR)/usr/lib/libospf* $(PKG_OSPFD_DIR)/usr/lib 	
	$(IPKG_BUILD) $(PKG_OSPFD_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/quagga-ospfd.list: $(PKG_OSPFD)
	$(IPKG) install $(PKG_OSPFD)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(TARGETS)
install: $(INSTALL_TARGETS)

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK) $(PKG_ZEBRA) $(PKG_OSPFD) $(PKG_RIPD) $(PKG_BGPD)
