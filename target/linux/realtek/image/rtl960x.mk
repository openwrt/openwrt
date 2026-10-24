# SPDX-License-Identifier: GPL-2.0-only

define Device/bt-pon_bt-g711ax
  SOC := rtl9607
  DEVICE_VENDOR := BT-PON
  DEVICE_MODEL := BT-G711AX
  KERNEL_SIZE := 10m
  IMAGE_SIZE := 25m
  $(Device/kernel-lzma)
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | sysupgrade-tar rootfs=$$$$@ | append-metadata
endef
TARGET_DEVICES += bt-pon_bt-g711ax

define Device/buffalo_wsr-1500ax2s
  SOC := rtl8198d
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WSR-1500AX2S
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 6144k
  $(Device/kernel-lzma)
  KERNEL := $$(KERNEL) | check-size $$(KERNEL_SIZE)
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | sysupgrade-tar rootfs=$$$$@ | append-metadata
  DEVICE_PACKAGES := dnsmasq odhcpd-ipv6only ppp ppp-mod-pppoe
endef
TARGET_DEVICES += buffalo_wsr-1500ax2s
