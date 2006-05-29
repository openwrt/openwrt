# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=udev
PKG_VERSION:=077
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.kernel.org/pub/linux/utils/kernel/hotplug/ \
	http://ftp.kernel.org/pub/linux/utils/kernel/hotplug/ \
	ftp://ftp.de.kernel.org/pub/linux/utils/kernel/hotplug/ \
	http://ftp.de.kernel.org/pub/linux/utils/kernel/hotplug/ \
	ftp://ftp.fr.kernel.org/pub/linux/utils/kernel/hotplug/ \
	http://ftp.fr.kernel.org/pub/linux/utils/kernel/hotplug/
PKG_MD5SUM:=61ec646daf7795e9777e8f33975408fe
PKG_CAT:=bzcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

define Package/udev
SECTION:=base
# CATEGORY:=FIXME
DEFAULT:=n
TITLE:=Dynamic device management subsystem
DESCRIPTION:=udev provides a dynamic device directory containing only the files for \\\
actually present devices. It creates or removes device node files in \\\
the /dev directory, or it renames network interfaces. \\\
URL:=http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CROSS="$(TARGET_CROSS)" \
		STRIP="/bin/true" \
		OPTFLAGS="$(TARGET_CFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		INSTALL="install -c" \
		all install
endef

define Package/udev/install
	install -d -m0775 $(1)/etc/udev
	$(CP) $(PKG_INSTALL_DIR)/etc/udev/* $(1)/etc/udev/
	install -d -m0775 $(1)/lib/udev
	install -d -m0775 $(1)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udev $(1)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udevcontrol $(1)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udevd $(1)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udevsend $(1)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udevstart $(1)/sbin/
	install -d -m0775 $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/udevinfo $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/udevtest $(1)/usr/bin/
	install -d -m0775 $(1)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/udevmonitor $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,udev))
