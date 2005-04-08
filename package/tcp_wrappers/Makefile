# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := tcp_wrappers
PKG_VERSION := 7.6
PKG_RELEASE := 1
PKG_MD5SUM := e6fa25f71226d090f34de3f6b122fb5a

PKG_SOURCE_SITE := ftp://ftp.porcupine.org/pub/security
PKG_SOURCE_FILE := $(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)_$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)

# only need libwrap.a at the moment
#PKG_IPK := $(PACKAGE_DIR)/zlib_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

.NOTPARALLEL:

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/libwrap.a: $(PKG_BUILD_DIR)/.patched
	$(MAKE) -j1 -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		COPTS="$(TARGET_CFLAGS)" \
		LIBS=-lnsl \
		NETGROUP= \
		VSYSLOG= \
		BUGS= \
		EXTRA_CFLAGS="-DSYS_ERRLIST_DEFINED -DHAVE_STRERROR -DHAVE_WEAKSYMS -D_REENTRANT -DINET6=1 \
			-Dss_family=__ss_family -Dss_len=__ss_len" \
		FACILITY=LOG_DAEMON \
		SEVERITY=LOG_INFO \
		REAL_DAEMON_DIR=/usr/sbin \
		STYLE="-DPROCESS_OPTIONS" \
		tidy all

$(STAGING_DIR)/lib/libwrap.a: $(PKG_BUILD_DIR)/libwrap.a
	mkdir -p $(STAGING_DIR)/include/
	cp -f $(PKG_BUILD_DIR)/tcpd.h $(STAGING_DIR)/include/
	cp -f $^ $@

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(STAGING_DIR)/lib/libwrap.a
install:

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(STAGING_DIR)/lib/libwrap.a

