# $Id$

include $(TOPDIR)/rules.mk
include kernelconfig.mk

PKG_NAME:=iptables
PKG_VERSION:=1.3.1
PKG_RELEASE:=1
PKG_MD5SUM:=c3358a3bd0d7755df0b64a5063db296b

PKG_SOURCE_URL:=http://www.netfilter.org/files
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IPTABLES,iptables,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,IPTABLES_EXTRA,iptables-extra,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,IPTABLES_UTILS,iptables-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,IP6TABLES,ip6tables,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.built:
	chmod a+x $(PKG_BUILD_DIR)/extensions/.*-test
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CC=$(TARGET_CC) COPT_FLAGS="$(TARGET_CFLAGS)" \
		KERNEL_DIR=$(LINUX_DIR) PREFIX=/usr \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install install-devel
	touch $(PKG_BUILD_DIR)/.built
		
$(IPKG_IPTABLES):
	install -d -m0755 $(IDIR_IPTABLES)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/iptables $(IDIR_IPTABLES)/usr/sbin/
	install -d -m0755 $(IDIR_IPTABLES)/usr/lib/iptables
	(cd $(PKG_INSTALL_DIR)/usr/lib/iptables ; \
	 cp -fpR $(patsubst %,libipt_%.so,$(ext-y)) $(IDIR_IPTABLES)/usr/lib/iptables \
	)
	$(RSTRIP) $(IDIR_IPTABLES)
	$(IPKG_BUILD) $(IDIR_IPTABLES) $(PACKAGE_DIR)

$(IPKG_IPTABLES_EXTRA):
	install -d -m0755 $(IDIR_IPTABLES_EXTRA)/usr/lib/iptables
	(cd $(PKG_INSTALL_DIR)/usr/lib/iptables ; \
	 cp -fpR $(patsubst %,libipt_%.so,$(ext-m)) $(IDIR_IPTABLES_EXTRA)/usr/lib/iptables \
	)
	$(RSTRIP) $(IDIR_IPTABLES_EXTRA)
	$(IPKG_BUILD) $(IDIR_IPTABLES_EXTRA) $(PACKAGE_DIR)

$(IPKG_IPTABLES_UTILS):
	install -d -m0755 $(IDIR_IPTABLES_UTILS)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/iptables-{save,restore} $(IDIR_IPTABLES_UTILS)/usr/sbin
	$(RSTRIP) $(IDIR_IPTABLES_UTILS)
	$(IPKG_BUILD) $(IDIR_IPTABLES_UTILS) $(PACKAGE_DIR)

$(IPKG_IP6TABLES):
	install -d -m0755 $(IDIR_IP6TABLES)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/ip6tables $(IDIR_IP6TABLES)/usr/sbin/
	install -d -m0755 $(IDIR_IP6TABLES)/usr/lib/iptables
	(cd $(PKG_INSTALL_DIR)/usr/lib/iptables ; \
	 cp -fpR libip6t_*.so $(IDIR_IP6TABLES)/usr/lib/iptables \
	)
	$(RSTRIP) $(IDIR_IP6TABLES)
	$(IPKG_BUILD) $(IDIR_IP6TABLES) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libipq.a: $(PKG_BUILD_DIR)/.built
	cp -fpR $(PKG_INSTALL_DIR)/usr/include/libipq.h $(STAGING_DIR)/usr/include/
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libipq.a $(STAGING_DIR)/usr/lib/

install-dev: $(STAGING_DIR)/usr/lib/libipq.a

uninstall-dev:
	rm -rf $(STAGING_DIR)/usr/include/libipq.h
	rm -rf $(STAGING_DIR)/usr/lib/libipq.a

compile: install-dev
clean: uninstall-dev
