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

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) . ./patches
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
		--with-build-cc=$(HOSTCC) \
		--disable-yydebug \
		--prefix=/usr \
		--with-pcap=linux \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/libpcap.a: $(PKG_BUILD_DIR)/.configured
	$(MAKE) AR="$(TARGET_CROSS)ar" -C $(PKG_BUILD_DIR)

$(STAGING_DIR)/lib/libpcap.a: $(PKG_BUILD_DIR)/libpcap.a
	$(MAKE) -C $(PKG_BUILD_DIR) \
		prefix=$(STAGING_DIR) \
		exec_prefix=$(STAGING_DIR) \
		bindir=$(STAGING_DIR)/bin \
		datadir=$(STAGING_DIR)/share \
		install

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(STAGING_DIR)/lib/libpcap.a
install:

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(STAGING_DIR)/lib/libpcap*
