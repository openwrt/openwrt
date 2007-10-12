# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=i386
BOARD:=rdc
BOARDNAME:=RDC321x x86
FEATURES:=squashfs jffs2 pci

LINUX_VERSION:=2.6.23

include $(INCLUDE_DIR)/target.mk

define Target/Description
        Build firmware images for RDC321x based routers
        (e.g. Airlink101 AR525W, Linksys WRT54R, Sitecom WL-153)
endef

ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
  define Kernel/SetInitramfs
	echo "r6040 parent=wlan0" > $(TARGET_DIR)/etc/modules.d/99-r6040
	$(RM) $(TARGET_DIR)/sbin/init
	ln -s /etc/preinit $(TARGET_DIR)/sbin/init
	sed -i 's,exec /sbin/init,exec /bin/busybox init,g' $(TARGET_DIR)/etc/preinit
	sed -i 's,/sbin/init,/bin/busybox init,g' $(TARGET_DIR)/init
	mv $(TARGET_DIR)/init $(TARGET_DIR)/linuxrc
	sed -i 's,eth0,eth1,g' $(TARGET_DIR)/etc/config/network
  endef
endif

$(eval $(call BuildTarget))
