include $(TOPDIR)/rules.mk

PKG_NAME:=radvd
PKG_VERSION:=0.7.3
PKG_RELEASE:=1
PKG_MD5SUM:=56ce3f8cbf5966a0d531c21813320423

PKG_SOURCE_URL:=http://v6web.litech.org/radvd/dist
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat
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
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--with-logfile=/tmp/radvd.log \
		--with-pidfile=/tmp/run/radvd.pid \
		--with-configfile=/etc/radvd.conf \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.configured
	$(MAKE) $(TARGET_CONFIGURE_OPTS) LD=$(TARGET_CC) LDFLAGS=-L$(PKG_BUILD_DIR) -C $(PKG_BUILD_DIR) radvd_LDADD=""

$(PKG_IPK): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/sbin $(PKG_IPK_DIR)/etc/init.d
	install -m 755 $(PKG_BUILD_DIR)/radvd $(PKG_IPK_DIR)/usr/sbin/
	install -m 755 $(PKG_BUILD_DIR)/radvdump $(PKG_IPK_DIR)/usr/sbin/
	install -m 755 ./files/S51radvd $(PKG_IPK_DIR)/etc/init.d/
	install -m 644 ./files/radvd.conf.example $(PKG_IPK_DIR)/etc/
	$(STRIP) $(PKG_IPK_DIR)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/$(PKG_NAME).list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
