# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/Clean
endef

define Image/Prepare
endef

define Image/Prepare
        cat $(KDIR)/vmlinux | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
endef

define Image/Build/ar525w
endef

$(call Image/Build/ar525w,$(1),$(patsubst jffs2-%,jffs2,$(1)))

$(eval $(call BuildImage))
