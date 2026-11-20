#
# Copyright (C) 2021 TDT AG <development@tdt.de>
#
# This is free software, licensed under the GNU General Public License v2.
# See https://www.gnu.org/licenses/gpl-2.0.txt for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=system-notification
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_MAINTAINER:=Florian Eckert <fe@dev.tdt.de>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/system-notification
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=System notification
endef

define Package/system-notification/description
  This package provides a ubus backend that can be used to save system
  notification. The notifications are not persistent. They are lost after
  every reboot.

  The following ubus methods are available:

  add:
  The <add> command adds a notification to the ubus backend.
  ubus call system.notification add '{"title":"<string>","description":"<description>"}'
  In addition, the following json string could be optional set.
  * url: {"url": "<string>"} Additional information are available at this extrenal URL.
  * opkg: {"package": "<string>"} Package can be updated.

  remove:
  The <remove> command removes a notification from the ubus backend.
  ubus call system.notification remove '{"uuid": "<string>"}'

  list:
  The <list> command dumps all staged notification from the ubus backend.
  ubus call system.notification list

  flush:
  The <flush> command removes all stage notification from the ubus backend.
  ubus call system.notification flush
endef

define Build/Compile
endef

define Package/system-notification/postinst
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ] && [ -x /etc/init.d/rpcd ]; then
	/etc/init.d/rpcd restart
fi
exit 0
endef

define Package/system-notification/postrm
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ] && [ -x /etc/init.d/rpcd ]; then
	/etc/init.d/rpcd restart
fi
exit 0
endef

define Package/system-notification/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/usr/bin/system-notification \
		$(1)/usr/bin/system-notification

	$(INSTALL_DIR) $(1)/usr/libexec/rpcd
	$(INSTALL_BIN) ./files/usr/libexec/rpcd/system.notification \
		$(1)/usr/libexec/rpcd/
endef

$(eval $(call BuildPackage,system-notification))
