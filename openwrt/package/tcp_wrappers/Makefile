# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=tcp_wrappers
PKG_VERSION:=7.6
PKG_RELEASE:=1
PKG_MD5SUM:=e6fa25f71226d090f34de3f6b122fb5a

PKG_SOURCE_URL:=ftp://ftp.porcupine.org/pub/security
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)_$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

#$(eval $(call PKG_template,LIBWRAP,libwrap,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

.NOTPARALLEL:

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -j1 -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		OPT_CFLAGS="$(TARGET_CFLAGS)" \
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
	touch $@

$(IPKG_LIBWRAP):
	install -d -m0755 $(IDIR_LIBWRAP)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/shared/libwrap.so.* $(IDIR_LIBWRAP)/usr/lib/
	$(RSTRIP) $(IDIR_LIBWRAP)
	$(IPKG_BUILD) $(IDIR_LIBWRAP) $(PACKAGE_DIR)
	
$(STAGING_DIR)/usr/lib/libwrap.a: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/tcpd.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libwrap.a $(STAGING_DIR)/usr/lib/
#	$(CP) $(PKG_BUILD_DIR)/shared/libwrap.so* $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libwrap.a

uninstall-dev:
	rm -rf \
	  $(STAGING_DIR)/usr/include/tcpd.h \
	  $(STAGING_DIR)/usr/lib/libwrap.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev
