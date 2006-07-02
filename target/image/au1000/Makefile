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

define Image/Build
	$(TARGET_CROSS)objcopy -O srec -I binary --adjust-vma 0xbe000000 $(KDIR)/root.$(1) $(KDIR)/root.$(1).srec
	grep -v S7 $(KDIR)/root.$(1).srec > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
	grep -v S0 $(LINUX_DIR)/arch/mips/boot/compressed/images/zImage.flash.srec >> $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
endef

$(eval $(call BuildImage))
