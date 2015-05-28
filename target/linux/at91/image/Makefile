# 
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UBIFS_OPTS = -F -m 2048 -e 126KiB -c 2048 -U
UBI_OPTS = -m 2048 -p 128KiB -s 512

AT91SAMA5D3XPLAINED_UBIFS_OPTS = -m 2048 -e 124KiB -c 2048
AT91SAMA5D3XPLAINED_UBI_OPTS = -m 2048 -p 128KiB -s 2048

define Build/Clean
	$(MAKE) -C u-boot clean
endef

define Build/Compile
	if [ $(CONFIG_AT91_UBOOT) ]; then \
		$(MAKE) -C u-boot compile; \
	fi
endef

define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/Image $(KDIR)/Image
	cp $(LINUX_DIR)/arch/arm/boot/zImage $(KDIR)/zImage
endef

define MkuImageDtb
	cat $(KDIR)/zImage $(DTS_DIR)/$(2).dtb > $(KDIR)/zImage-$(1)
	mkimage -A arm -T kernel -C none -a 0x20008000 -e 0x20008000 \
		-n "OpenWrt ARM $(LINUX_VERSION)" \
		-d $(KDIR)/zImage-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage
endef

define MkOftree
	cp -u $(KDIR)/zImage $(BIN_DIR)/$(IMG_PREFIX)-zImage
	cp $(DTS_DIR)/$(2).dtb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-oftree.dtb
endef

# Atmel
Image/Build/Kernel/AT91SAM9263EK=$(call MkuImageDtb,9263ek,at91sam9263ek)
Image/Build/Kernel/AT91SAM9G15EK=$(call MkuImageDtb,9g15ek,at91sam9g15ek)
Image/Build/Kernel/AT91SAM9G20EK=$(call MkOftree,9g20ek,at91sam9g20ek)
Image/Build/Kernel/AT91SAM9G20EK-2MMC=$(call MkOftree,9g20ek_2mmc,at91sam9g20ek_2mmc)
Image/Build/Kernel/AT91SAM9G25EK=$(call MkuImageDtb,9g25ek,at91sam9g25ek)
Image/Build/Kernel/AT91SAM9G35EK=$(call MkuImageDtb,9g35ek,at91sam9g35ek)
Image/Build/Kernel/AT91SAM9M10G45EK=$(call MkuImageDtb,9m10g45ek,at91sam9m10g45ek)
Image/Build/Kernel/AT91SAM9X25EK=$(call MkuImageDtb,9x25ek,at91sam9x25ek)
Image/Build/Kernel/AT91SAM9X35EK=$(call MkuImageDtb,9x35ek,at91sam9x35ek)
Image/Build/Kernel/AT91SAMA5D3XPLAINED=$(call MkuImageDtb,sama5,at91-sama5d3_xplained)
# CalAmp
Image/Build/Kernel/LMU5000=$(call MkuImageDtb,lmu5000,lmu5000)
# Calao
Image/Build/Kernel/TNYA9260=$(call MkuImageDtb,tny_a9260,tny_a9260)
Image/Build/Kernel/TNYA9263=$(call MkuImageDtb,tny_a9263,tny_a9263)
Image/Build/Kernel/TNYA9G20=$(call MkuImageDtb,tny_a9g20,tny_a9g20)
Image/Build/Kernel/USBA9260=$(call MkuImageDtb,usb_a9260,usb_a9260)
Image/Build/Kernel/USBA9263=$(call MkuImageDtb,usb_a9263,usb_a9263)
Image/Build/Kernel/USBA9G20=$(call MkuImageDtb,usb_a9g20,usb_a9g20)
# Ethernut
Image/Build/Kernel/ETHERNUT5=$(call MkuImageDtb,ethernut5,ethernut5)
# Exegin
Image/Build/Kernel/Q5XR5=$(call MkOftree,q5xr5,at91-q5xr5)


define Image/Build/Kernel/Default
	$(call Image/Build/Kernel/AT91SAM9263EK)
	$(call Image/Build/Kernel/AT91SAM9G15EK)
	$(call Image/Build/Kernel/AT91SAM9G20EK)
	$(call Image/Build/Kernel/AT91SAM9G20EK-2MMC)
	$(call Image/Build/Kernel/AT91SAM9G25EK)
	$(call Image/Build/Kernel/AT91SAM9G35EK)
	$(call Image/Build/Kernel/AT91SAM9M10G45EK)
	$(call Image/Build/Kernel/AT91SAM9X25EK)
	$(call Image/Build/Kernel/AT91SAM9X35EK)
	$(call Image/Build/Kernel/AT91SAMA5D3XPLAINED)
	$(call Image/Build/Kernel/LMU5000)
	$(call Image/Build/Kernel/TNYA9260)
	$(call Image/Build/Kernel/TNYA9263)
	$(call Image/Build/Kernel/TNYA9G20)
	$(call Image/Build/Kernel/USBA9260)
	$(call Image/Build/Kernel/USBA9263)
	$(call Image/Build/Kernel/USBA9G20)
	$(call Image/Build/Kernel/ETHERNUT5)
	$(call Image/Build/Kernel/Q5XR5)
endef

define Image/BuildKernel
	mkimage -A arm -T kernel -C none -a 0x20008000 -e 0x20008000 -n linux-2.6 \
		-d $(KDIR)/Image $(BIN_DIR)/$(IMG_PREFIX)-uImage
	if [ $(CONFIG_FLEXIBITY_ROOT) ]; then \
		$(INSTALL_BIN) $(BIN_DIR)/$(IMG_PREFIX)-uImage $(TARGET_DIR)/uImage ; \
	fi
	$(call Image/Build/Kernel/$(PROFILE))
endef

define Image/Build
	$(call Image/Build/$(1))
	cp $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-root.$(1)
endef

$(eval $(call BuildImage))
