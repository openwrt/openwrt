# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=iproute2
PKG_VERSION:=2.6.15-060110
PKG_RELEASE:=1
PKG_MD5SUM:=04f57a6d366d36426d276178b600f5c5

PKG_SOURCE_URL:=http://developer.osdl.org/dev/iproute2/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/iproute2-$(PKG_VERSION)
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IP,ip,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,TC,tc,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	$(SED) "s:-O2:${TARGET_CFLAGS}:g" $(PKG_BUILD_DIR)/Makefile
	$(SED) "s,-I/usr/include/db3,," $(PKG_BUILD_DIR)/Makefile
	$(SED) "s,^KERNEL_INCLUDE.*,KERNEL_INCLUDE=$(LINUX_DIR)/include," \
		$(PKG_BUILD_DIR)/Makefile
	$(SED) "s,^LIBC_INCLUDE.*,LIBC_INCLUDE=$(STAGING_DIR)/include," \
		$(PKG_BUILD_DIR)/Makefile
	# For now disable compiling of the misc directory because it seems to fail
	rm -rf $(PKG_BUILD_DIR)/misc 
	$(SED) "s, misc,," $(PKG_BUILD_DIR)/Makefile
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -j1 -C $(PKG_BUILD_DIR)/netem HOSTCC=$(HOSTCC) CFLAGS="-D_GNU_SOURCE -O2 -Wstrict-prototypes -Wall -I ../include -DRESOLVE_HOSTNAMES"
	$(MAKE) -j1 -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) KERNEL_INCLUDE=$(LINUX_DIR)/include all tc/tc ip/ip
	touch $@

$(IPKG_IP):
	mkdir -p $(IDIR_IP)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/ip/ip $(IDIR_IP)/usr/sbin/
	$(STRIP) $(IDIR_IP)/usr/sbin/*
	$(IPKG_BUILD) $(IDIR_IP) $(PACKAGE_DIR)

$(IPKG_TC):
	mkdir -p $(IDIR_TC)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/tc/tc $(IDIR_TC)/usr/sbin/
	$(STRIP) $(IDIR_TC)/usr/sbin/*
	$(IPKG_BUILD) $(IDIR_TC) $(PACKAGE_DIR)

