#############################################################
#
# ebtables
#
#############################################################
include $(TOPDIR)/rules.mk

PKG_NAME:=pptpd
PKG_VERSION:=1.2.3
PKG_RELEASE:=1
PKG_SOURCE_URL:=@SF/poptop
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	 $(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) x $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.unpacked
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
	  $(DISABLE_NLS) \
	  --with-pppd-ip-alloc)
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
	 CC=$(TARGET_CC) \
	 CFLAGS="$(TARGET_CFLAGS) -DSBINDIR=\\\"/usr/sbin\\\"" \
	 all

$(PKG_IPK): $(PKG_BUILD_DIR)/$(PKG_NAME)
	mkdir -p $(PKG_IPK_DIR)/usr/sbin
	cp -a ./ipkg/* $(PKG_IPK_DIR)/
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	cp $(PKG_BUILD_DIR)/pptpd $(PKG_IPK_DIR)/usr/sbin/
	cp $(PKG_BUILD_DIR)/pptpctrl $(PKG_IPK_DIR)/usr/sbin/
	$(STRIP) $(PKG_IPK_DIR)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/pptpd.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/pptpd.list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
