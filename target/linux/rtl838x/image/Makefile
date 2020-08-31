#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR = 0x80000000
KERNEL_ENTRY = 0x80000400

define Device/Default
  PROFILES = Default
  KERNEL := kernel-bin | append-dtb | gzip | uImage gzip
  KERNEL_INITRAMFS := kernel-bin | append-dtb | gzip | uImage gzip
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS = $$(SOC)_$(1)
  IMAGES := sysupgrade.bin
  SUPPORTED_DEVICES := $(subst _,$(comma),$(1))
  sysupgrade_bin := append-kernel | pad-to 64k | append-rootfs | pad-rootfs
  IMAGE/sysupgrade.bin := append-kernel | pad-to 64k | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
endef

define Build/netgear-uImage
	$(call Build/uImage,$(1) -M $(NETGEAR_KERNEL_MAGIC))
endef

define Build/custom-uimage
	mkimage -A $(LINUX_KARCH) \
		-O linux -T kernel \
		-C gzip -a $(KERNEL_LOADADDR) $(if $(UIMAGE_MAGIC),-M $(UIMAGE_MAGIC),) \
		-e $(if $(KERNEL_ENTRY),$(KERNEL_ENTRY),$(KERNEL_LOADADDR)) \
		-n '$(1)' -d $@ $@.new
	mv $@.new $@
endef

define Device/allnet_all-sg8208m
  SOC := rtl8382
  IMAGE_SIZE := 7168k
  DEVICE_VENDOR := ALLNET
  DEVICE_MODEL := ALL-SG8208M
  UIMAGE_MAGIC := 0x00000006
  KERNEL := kernel-bin | append-dtb | gzip | custom-uimage 2.2.2.0
  KERNEL_INITRAMFS := kernel-bin | append-dtb | gzip | custom-uimage 2.2.2.0
  DEVICE_PACKAGES := ip-full ip-bridge kmod-gpio-button-hotplug tc
endef
TARGET_DEVICES += allnet_all-sg8208m

define Device/d-link_dgs-1210-10p
  SOC := rtl8382
  IMAGE_SIZE := 13824k
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DGS-1210-10P
  DEVICE_PACKAGES := ip-full ip-bridge ethtool tc lua-rs232
endef
TARGET_DEVICES += d-link_dgs-1210-10p

define Device/d-link_dgs-1210-16
  SOC := rtl8382
  IMAGE_SIZE := 13824k
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DGS-1210-16
  DEVICE_PACKAGES := ip-full ip-bridge ethtool tc
endef
TARGET_DEVICES += d-link_dgs-1210-16

define Device/inaba_aml2-17gp
  SOC := rtl8382
  IMAGE_SIZE := 13504k
  DEVICE_VENDOR := INABA
  DEVICE_MODEL := Abaniact AML2-17GP
  DEVICE_PACKAGES := ip-bridge ethtool
endef
TARGET_DEVICES += inaba_aml2-17gp

define Device/netgear_gs728tpv2
  SOC := rtl8391
  IMAGE_SIZE := 13824k
  NETGEAR_KERNEL_MAGIC := 0x174e4742
  KERNEL := kernel-bin | append-dtb | lzma -d20 | netgear-uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma -d20 | netgear-uImage lzma
  DEVICE_VENDOR := Netgear
  DEVICE_MODEL := GS728TPv2
  DEVICE_PACKAGES := ip-full ip-bridge ethtool tc
endef
TARGET_DEVICES += netgear_gs728tpv2

define Device/tp-link_t2500g-10ts
  SOC := rtl8380
  IMAGE_SIZE := 7936k
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := T2500G-10TS
  DEVICE_PACKAGES := ip-full ip-bridge ethtool
endef
TARGET_DEVICES += tp-link_t2500g-10ts

define Device/zyxel_gs1900-10hp
  SOC := rtl8380
  IMAGE_SIZE := 6976k
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := GS1900-10HP
  DEVICE_PACKAGES := ip-bridge ethtool
endef
TARGET_DEVICES += zyxel_gs1900-10hp

$(eval $(call BuildImage))
