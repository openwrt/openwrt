# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=iproute2
PKG_VERSION:=2.6.9
PKG_RELEASE:=1
PKG_MD5SUM:=4b3f335b0139cb909c6a2ae8bf72e548

PKG_SOURCE_URL:=http://developer.osdl.org/dev/iproute2/download/
PKG_SOURCE:=$(PKG_NAME)-2.6.9-041019.tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/iproute2-2.6.9
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IPROUTE2_IP,ip,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,IPROUTE2_TC,tc,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
	$(MAKE) -j1 -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) KERNEL_INCLUDE=$(LINUX_DIR)/include all tc/tc ip/ip
	touch $@

$(IPKG_IPROUTE2_IP):
	mkdir -p $(IDIR_IPROUTE2_IP)/usr/sbin
	cp $(PKG_BUILD_DIR)/ip/ip $(IDIR_IPROUTE2_IP)/usr/sbin/
	$(STRIP) $(IDIR_IPROUTE2_IP)/usr/sbin/*
	$(IPKG_BUILD) $(IDIR_IPROUTE2_IP) $(PACKAGE_DIR)

$(IPKG_IPROUTE2_TC):
	mkdir -p $(IDIR_IPROUTE2_TC)/usr/sbin
	cp $(PKG_BUILD_DIR)/ip/ip $(IDIR_IPROUTE2_TC)/usr/sbin/
	$(STRIP) $(IDIR_IPROUTE2_TC)/usr/sbin/*
	$(IPKG_BUILD) $(IDIR_IPROUTE2_TC) $(PACKAGE_DIR)

