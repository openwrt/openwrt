# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=udev
PKG_VERSION:=077
PKG_RELEASE:=1
PKG_MD5SUM:=61ec646daf7795e9777e8f33975408fe

PKG_SOURCE_URL:=ftp://ftp.kernel.org/pub/linux/utils/kernel/hotplug/ \
	http://ftp.kernel.org/pub/linux/utils/kernel/hotplug/ \
	ftp://ftp.de.kernel.org/pub/linux/utils/kernel/hotplug/ \
	http://ftp.de.kernel.org/pub/linux/utils/kernel/hotplug/ \
	ftp://ftp.fr.kernel.org/pub/linux/utils/kernel/hotplug/ \
	http://ftp.fr.kernel.org/pub/linux/utils/kernel/hotplug/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,UDEV,udev,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CROSS="$(TARGET_CROSS)" \
		STRIP="/bin/true" \
		OPTFLAGS="$(TARGET_CFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		INSTALL="install -c" \
		all install
	touch $@

$(IPKG_UDEV):
	install -d -m0775 $(IDIR_UDEV)/etc/udev
	$(CP) $(PKG_INSTALL_DIR)/etc/udev/* $(IDIR_UDEV)/etc/udev/
	install -d -m0775 $(IDIR_UDEV)/lib/udev
	install -d -m0775 $(IDIR_UDEV)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udev $(IDIR_UDEV)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udevcontrol $(IDIR_UDEV)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udevd $(IDIR_UDEV)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udevsend $(IDIR_UDEV)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udevstart $(IDIR_UDEV)/sbin/
	install -d -m0775 $(IDIR_UDEV)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/udevinfo $(IDIR_UDEV)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/udevtest $(IDIR_UDEV)/usr/bin/
	install -d -m0775 $(IDIR_UDEV)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/udevmonitor $(IDIR_UDEV)/usr/sbin/
	$(RSTRIP) $(IDIR_UDEV)
	$(IPKG_BUILD) $(IDIR_UDEV) $(PACKAGE_DIR)

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
