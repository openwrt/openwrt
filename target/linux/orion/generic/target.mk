#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

BOARDNAME:=Generic
FEATURES:=squashfs jffs2

DEFAULT_PACKAGES += kmod-ath9k wpad-mini

define Target/Description
	Build firmware images for Marvell Orion based boards that boot from internal flash.
	(e.g.: Linksys WRT350N v2, Netgear WNR854T, ...)
endef
