# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=udev
PKG_VERSION:=077
PKG_RELEASE:=1
PKG_MD5SUM:=61ec646daf7795e9777e8f33975408fe

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.kernel.org/pub/linux/utils/kernel/hotplug/ \
	http://ftp.kernel.org/pub/linux/utils/kernel/hotplug/ \
	ftp://ftp.de.kernel.org/pub/linux/utils/kernel/hotplug/ \
	http://ftp.de.kernel.org/pub/linux/utils/kernel/hotplug/ \
	ftp://ftp.fr.kernel.org/pub/linux/utils/kernel/hotplug/ \
	http://ftp.fr.kernel.org/pub/linux/utils/kernel/hotplug/
PKG_CAT:=bzcat

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

define Package/udev/install
	install -d -m0775 $(IDIR_UDEV)/etc/udev
	install -d -m0775 $(IDIR_UDEV)/lib/udev
	install -d -m0775 $(IDIR_UDEV)/sbin/
	install -d -m0775 $(IDIR_UDEV)/usr/bin/
	install -d -m0775 $(IDIR_UDEV)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/etc/udev/* $(IDIR_UDEV)/etc/udev/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udev $(IDIR_UDEV)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udevcontrol $(IDIR_UDEV)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udevd $(IDIR_UDEV)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udevsend $(IDIR_UDEV)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/udevstart $(IDIR_UDEV)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/udevinfo $(IDIR_UDEV)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/udevtest $(IDIR_UDEV)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/udevmonitor $(IDIR_UDEV)/usr/sbin/
endef

$(eval $(call BuildPackage,udev))
