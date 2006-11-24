# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=ar7
BOARDNAME:=TI AR7
FEATURES:=squashfs jffs2

define Target/Description
	Build firmware images for TI AR7 based routers (w.g. Linksys WAG54G v2)
endef

LINUX_VERSION:=2.4.33.4
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=aca0257549c385a227088ad8853a72a2

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += kmod-avalanche-cpmac

define Profile/AnnexA
  NAME:=ADSL Annex A (default)
  PACKAGES:=kmod-sangam-atm-annex-a ppp-mod-pppoa
endef
$(eval $(call Profile,AnnexA))

define Profile/AnnexB
  NAME:=ADSL Annex B
  PACKAGES:=kmod-sangam-atm-annex-b ppp-mod-pppoa
endef
$(eval $(call Profile,AnnexB))

define Profile/None
  NAME:=No ADSL
  PACKAGES:=
endef
$(eval $(call Profile,None))

$(eval $(call BuildKernel))
