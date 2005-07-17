# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ser
PKG_VERSION:=0.8.14
PKG_RELEASE:=1
PKG_MD5SUM:=3e2e12c8dfbd7dad9199304093de0838

PKG_SOURCE_URL:=ftp://ftp.berlios.de/pub/ser/$(PKG_VERSION)/src
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)_src.tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

SER_MODULES := sl tm rr maxfwd usrloc registrar dbtext textops nathelper

SER_MODULE_FILES := $(foreach module,$(SER_MODULES),modules/$(module)/$(module).so)
SER_MODULES := $(patsubst %,modules/%,$(SER_MODULES))

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.patched
	$(MAKE) -C $(PKG_BUILD_DIR) \
		extra_defs="-DUSE_PTHREAD_MUTEX "  \
		CC="$(TARGET_CC)" \
		ARCH="$(ARCH)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		modules all

$(PKG_IPK): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/sbin
	cp -a $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_IPK_DIR)/usr/sbin/
	-$(STRIP) $(PKG_IPK_DIR)/usr/sbin/*
	cp -a $(PKG_BUILD_DIR)/scripts/sc $(PKG_IPK_DIR)/usr/sbin/serctl
	mkdir -p $(PKG_IPK_DIR)/usr/lib/ser/modules
	(cd $(PKG_BUILD_DIR);\
		cp -a $(SER_MODULE_FILES) $(PKG_IPK_DIR)/usr/lib/ser/modules/; \
	)
	$(STRIP) $(PKG_IPK_DIR)/usr/lib/ser/modules/*
	mkdir -p $(PKG_IPK_DIR)/etc/ser
	cp -a $(PKG_BUILD_DIR)/etc/ser.cfg $(PKG_IPK_DIR)/etc/ser/
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
