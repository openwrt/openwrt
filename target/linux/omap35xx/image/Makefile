# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE=128k
JFFS2OPTS += --little-endian --pagesize=0x800 --no-cleanmarkers --pad

UBIFS_OPTS = -m 2048 -e 126KiB -c 1996
UBINIZE_OPTS = -m 2048 -p 128KiB -s 512

define Image/BuildKernel
	$(TARGET_CROSS)objcopy -O binary -R .note -R .comment -S $(LINUX_DIR)/arch/arm/boot/compressed/vmlinux linux.bin
	mkimage -A arm -O linux -T kernel -C none -a 80008000 -e 80008000 -n "OpenWRT Kernel Image gumstix" -d linux.bin uImage
	mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n 'uImage.bin+mmc2' -d ../files/boot-mmc.cmd boot.scr

	mv uImage $(BIN_DIR)/$(IMG_PREFIX)-$(KERNEL)-uImage
	mv boot.scr $(BIN_DIR)/boot.scr
	rm linux.bin
endef

define Image/Build
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(KERNEL)-root.$(1)
#	PADDING="true" PATH="$(TARGET_PATH)" ./gen_image.sh $(BIN_DIR)/$(IMG_PREFIX)-$(1).image 4 $(KDIR)/root.bootscript 240 $(KDIR)/root.$(1)
	$(call Image/Build/$(1),$(1))
endef

ifdef CONFIG_TARGET_omap35xx_gumstix
define Image/Prepare
        $(CP) ../gumstix/base-files/lib/wifi/mac80211.sh $(TARGET_DIR)/lib/wifi
endef
endif

$(eval $(call BuildImage))
