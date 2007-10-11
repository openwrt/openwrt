#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

BOARD:=adm5120
LINUX_VERSION:=2.6.23
FEATURES:=squashfs jffs2 tgz broken

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += admswconfig

$(eval $(call Target,router_be))
$(eval $(call Target,router_le))
$(eval $(call BuildTarget))
