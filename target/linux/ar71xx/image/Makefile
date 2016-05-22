#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

JFFS2_BLOCKSIZE = 64k 128k 256k

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

IMAGE_PROFILE:=$(if $(PROFILE),$(PROFILE),Default)

KERNEL_LOADADDR = 0x80060000

DEVICE_VARS += NETGEAR_KERNEL_MAGIC NETGEAR_BOARD_ID NETGEAR_HW_ID CMDLINE CONSOLE IMAGE_SIZE BOARDNAME LOADER_FLASH_OFFS LOADER_TYPE

include ./tp-link.mk
include ./ubnt.mk
include ./legacy.mk

define Build/netgear-squashfs
	rm -rf $@.fs $@.squashfs
	mkdir -p $@.fs/image
	cp $@ $@.fs/image/uImage
	$(STAGING_DIR_HOST)/bin/mksquashfs-lzma \
		$@.fs $@.squashfs \
		-noappend -root-owned -be -b 65536 \
		$(if $(SOURCE_DATE_EPOCH),-fixed-time $(SOURCE_DATE_EPOCH))

	dd if=/dev/zero bs=1k count=1 >> $@.squashfs
	mkimage \
		-A mips -O linux -T filesystem -C none \
		-M $(NETGEAR_KERNEL_MAGIC) \
		-a 0xbf070000 -e 0xbf070000 \
		-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $@.squashfs $@
	rm -rf $@.squashfs $@.fs
endef

define Build/netgear-uImage
	$(call Build/uImage,$(1) -M $(NETGEAR_KERNEL_MAGIC))
endef

define Build/loader-common
	rm -rf $@.src
	$(MAKE) -C lzma-loader \
		PKG_BUILD_DIR="$@.src" \
		TARGET_DIR="$(dir $@)" LOADER_NAME="$(notdir $@)" \
		BOARD="$(BOARDNAME)" \
		LZMA_TEXT_START=0x80a00000 LOADADDR=0x80060000 \
		$(1) compile loader.$(LOADER_TYPE)
	mv "$@.$(LOADER_TYPE)" "$@"
	rm -rf $@.src
endef

define Build/loader-okli-compile
	$(call Build/loader-common,FLASH_OFFS=$(LOADER_FLASH_OFFS) FLASH_MAX=0 KERNEL_CMDLINE="$(CMDLINE)")
endef

define Build/loader-kernel
	$(call Build/loader-common,LOADER_DATA="$@")
endef

define Build/loader-kernel-cmdline
	$(call Build/loader-common,LOADER_DATA="$@" KERNEL_CMDLINE="$(CMDLINE)")
endef

define Build/loader-okli
	dd if=$(KDIR)/loader-$(1).gz bs=7680 conv=sync of="$@.new"
	cat "$@" >> "$@.new"
	mv "$@.new" "$@"
endef

define Build/copy-file
	cat "$(1)" > "$@"
endef

define Device/Default
  BOARDNAME :=
  DEVICE_PROFILE = $$(BOARDNAME)
  PROFILES = Default Minimal $$(DEVICE_PROFILE)
  MTDPARTS :=
  IMAGES := sysupgrade.bin
  BLOCKSIZE := 64k
  FILESYSTEMS = $(filter-out jffs2-%,$(TARGET_FILESYSTEMS)) jffs2-$$(BLOCKSIZE)
  CONSOLE = ttyS0,115200
  CMDLINE = $$(if $$(BOARDNAME),board=$$(BOARDNAME)) $$(if $$(MTDPARTS),mtdparts=$$(MTDPARTS)) $$(if $$(CONSOLE),console=$$(CONSOLE))
  KERNEL := kernel-bin | patch-cmdline | lzma | uImage lzma
  COMPILE :=
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin = append-kernel $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | check-size $$$$(IMAGE_SIZE)
endef

define Device/bsb
  BOARDNAME = BSB
  IMAGE_SIZE = 16000k
  CONSOLE = ttyATH0,115200
  MTDPARTS = spi0.0:128k(u-boot)ro,64k(u-boot-env)ro,16128k(firmware),64k(art)ro
endef
TARGET_DEVICES += bsb

define Device/carambola2
  BOARDNAME = CARAMBOLA2
  IMAGE_SIZE = 16000k
  CONSOLE = ttyATH0,115200
  MTDPARTS = spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,16000k(firmware),64k(art)ro
endef
TARGET_DEVICES += carambola2

define Device/cf-e316n-v2
  BOARDNAME = CF-E316N-V2
  IMAGE_SIZE = 16192k
  CONSOLE = ttyS0,115200
  MTDPARTS = spi0.0:64k(u-boot)ro,64k(art)ro,16192k(firmware),64k(nvram)ro
endef
TARGET_DEVICES += cf-e316n-v2

define Device/weio
  BOARDNAME = WEIO
  IMAGE_SIZE = 16000k
  CONSOLE = ttyATH0,115200
  MTDPARTS = spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,16000k(firmware),64k(art)ro
endef
TARGET_DEVICES += weio

define Device/gl-ar150
  BOARDNAME = GL-AR150
  IMAGE_SIZE = 16000k
  CONSOLE = ttyATH0,115200
  MTDPARTS = spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,16000k(firmware),64k(art)ro
endef
TARGET_DEVICES += gl-ar150

define Device/gl-ar300
  BOARDNAME = GL-AR300
  IMAGE_SIZE = 16000k
  CONSOLE = ttyS0,115200
  MTDPARTS = spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,16000k(firmware),64k(art)ro
endef
TARGET_DEVICES += gl-ar300

define Device/gl-domino
  BOARDNAME = DOMINO
  IMAGE_SIZE = 16000k
  CONSOLE = ttyATH0,115200
  MTDPARTS = spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,16000k(firmware),64k(art)ro
endef
TARGET_DEVICES += gl-domino

define Device/wndr3700
  BOARDNAME = WNDR3700
  NETGEAR_KERNEL_MAGIC = 0x33373030
  NETGEAR_BOARD_ID = WNDR3700
  IMAGE_SIZE = 7680k
  MTDPARTS = spi0.0:320k(u-boot)ro,128k(u-boot-env)ro,7680k(firmware),64k(art)ro
  IMAGES := sysupgrade.bin factory.img factory-NA.img
  KERNEL := kernel-bin | patch-cmdline | lzma -d20 | netgear-uImage lzma
  IMAGE/default = append-kernel $$$$(BLOCKSIZE) | netgear-squashfs | append-rootfs | pad-rootfs
  IMAGE/sysupgrade.bin = $$(IMAGE/default) | check-size $$$$(IMAGE_SIZE)
  IMAGE/factory.img = $$(IMAGE/default) | netgear-dni | check-size $$$$(IMAGE_SIZE)
  IMAGE/factory-NA.img = $$(IMAGE/default) | netgear-dni NA | check-size $$$$(IMAGE_SIZE)
endef

define Device/wndr3700v2
$(Device/wndr3700)
  NETGEAR_BOARD_ID = WNDR3700v2
  NETGEAR_KERNEL_MAGIC = 0x33373031
  NETGEAR_HW_ID = 29763654+16+64
  IMAGE_SIZE = 15872k
  MTDPARTS = spi0.0:320k(u-boot)ro,128k(u-boot-env)ro,15872k(firmware),64k(art)ro
  IMAGES := sysupgrade.bin factory.img
endef

define Device/wndr3800
$(Device/wndr3700v2)
  NETGEAR_BOARD_ID = WNDR3800
  NETGEAR_HW_ID = 29763654+16+128
endef

define Device/wndr3800ch
$(Device/wndr3800)
  NETGEAR_BOARD_ID = WNDR3800CH
endef

define Device/wndrmac
$(Device/wndr3700v2)
  NETGEAR_BOARD_ID = WNDRMAC
endef

define Device/wndrmacv2
$(Device/wndr3800)
  NETGEAR_BOARD_ID = WNDRMACv2
endef

TARGET_DEVICES += wndr3700 wndr3700v2 wndr3800 wndr3800ch wndrmac wndrmacv2

define Device/cap324
  BOARDNAME := CAP324
  DEVICE_PROFILE := CAP324
  IMAGE_SIZE = 15296k
  MTDPARTS = spi0.0:256k(u-boot),64k(u-boot-env)ro,15296k(firmware),640k(certs),64k(nvram),64k(art)
endef

TARGET_DEVICES += cap324

define Device/cap324-nocloud
  BOARDNAME := CAP324
  DEVICE_PROFILE := CAP324
  IMAGE_SIZE = 16000k
  MTDPARTS = spi0.0:256k(u-boot),64k(u-boot-env)ro,16000k(firmware),64k(art)
endef

TARGET_DEVICES += cap324-nocloud

define Device/cr3000
  BOARDNAME := CR3000
  DEVICE_PROFILE := CR3000
  IMAGE_SIZE = 7104k
  MTDPARTS = spi0.0:256k(u-boot),64k(u-boot-env)ro,7104k(firmware),640k(certs),64k(nvram),64k(art)
endef

TARGET_DEVICES += cr3000

define Device/cr3000-nocloud
  BOARDNAME := CR3000
  DEVICE_PROFILE := CR3000
  IMAGE_SIZE = 7808k
  MTDPARTS = spi0.0:256k(u-boot),64k(u-boot-env)ro,7808k(firmware),64k(art)
endef

TARGET_DEVICES += cr3000-nocloud

define Device/cr5000
  BOARDNAME := CR5000
  DEVICE_PROFILE := CR5000
  IMAGE_SIZE = 7104k
  MTDPARTS = spi0.0:256k(u-boot),64k(u-boot-env)ro,7104k(firmware),640k(certs),64k(nvram),64k(art)
endef

TARGET_DEVICES += cr5000

define Device/cr5000-nocloud
  BOARDNAME := CR5000
  DEVICE_PROFILE := CR5000
  IMAGE_SIZE = 7808k
  MTDPARTS = spi0.0:256k(u-boot),64k(u-boot-env)ro,7808k(firmware),64k(art)
endef

TARGET_DEVICES += cr5000-nocloud

define Device/antminer-s1
    $(Device/tplink-8mlzma)
    BOARDNAME := ANTMINER-S1
    DEVICE_PROFILE := ANTMINERS1
    TPLINK_HWID := 0x04440101
    CONSOLE := ttyATH0,115200
endef

define Device/antminer-s3
    $(Device/tplink-8mlzma)
    BOARDNAME := ANTMINER-S3
    DEVICE_PROFILE := ANTMINERS3
    TPLINK_HWID := 0x04440301
    CONSOLE := ttyATH0,115200
endef

define Device/antrouter-r1
    $(Device/tplink-8mlzma)
    BOARDNAME := ANTROUTER-R1
    DEVICE_PROFILE := ANTROUTERR1
    TPLINK_HWID := 0x44440101
    CONSOLE := ttyATH0,115200
endef

define Device/el-m150
    $(Device/tplink-8mlzma)
    BOARDNAME := EL-M150
    DEVICE_PROFILE := ELM150
    TPLINK_HWID := 0x01500101
    CONSOLE := ttyATH0,115200
endef

define Device/el-mini
    $(Device/tplink-8mlzma)
    BOARDNAME := EL-MINI
    DEVICE_PROFILE := ELMINI
    TPLINK_HWID := 0x01530001
    CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += antminer-s1 antminer-s3 antrouter-r1 el-m150 el-mini

define Device/gl-inet-6408A-v1
    $(Device/tplink-8mlzma)
    BOARDNAME := GL-INET
    DEVICE_PROFILE := GLINET
    TPLINK_HWID := 0x08000001
    CONSOLE := ttyATH0,115200
endef

define Device/gl-inet-6416A-v1
    $(Device/tplink-16mlzma)
    BOARDNAME := GL-INET
    DEVICE_PROFILE := GLINET
    TPLINK_HWID := 0x08000001
    CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += gl-inet-6408A-v1 gl-inet-6416A-v1

define Device/rnx-n360rt
    $(Device/tplink-4m)
    BOARDNAME := TL-WR941ND
    DEVICE_PROFILE := RNXN360RT
    TPLINK_HWID := 0x09410002
    TPLINK_HWREV := 0x00420001
endef
TARGET_DEVICES += rnx-n360rt

define Device/mc-mac1200r
    $(Device/tplink-8mlzma)
    BOARDNAME := MC-MAC1200R
    DEVICE_PROFILE := MAC1200R
    TPLINK_HWID := 0x12000001
endef
TARGET_DEVICES += mc-mac1200r

define Device/minibox-v1
    $(Device/tplink-16mlzma)
    BOARDNAME := MINIBOX-V1
    DEVICE_PROFILE := MINIBOXV1
    TPLINK_HWID := 0x3C000201
    CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += minibox-v1

define Device/omy-g1
    $(Device/tplink-16mlzma)
    BOARDNAME := OMY-G1
    DEVICE_PROFILE := OMYG1
    TPLINK_HWID := 0x06660101
endef

define Device/omy-x1
    $(Device/tplink-8mlzma)
    BOARDNAME := OMY-X1
    DEVICE_PROFILE := OMYX1
    TPLINK_HWID := 0x06660201
endef
TARGET_DEVICES += omy-g1 omy-x1

define Device/onion-omega
    $(Device/tplink-16mlzma)
    BOARDNAME := ONION-OMEGA
    DEVICE_PROFILE := OMEGA
    TPLINK_HWID := 0x04700001
    CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += onion-omega

define Device/smart-300
    $(Device/tplink-8mlzma)
    BOARDNAME := SMART-300
    DEVICE_PROFILE := SMART-300
    TPLINK_HWID := 0x93410001
endef
TARGET_DEVICES += smart-300

define Device/tellstick-znet-lite
    $(Device/tplink-16mlzma)
    BOARDNAME := TELLSTICK-ZNET-LITE
    DEVICE_PROFILE := TELLSTICKZNETLITE
    TPLINK_HWID := 0x00726001
    CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tellstick-znet-lite

define Device/oolite
    $(Device/tplink-16mlzma)
    BOARDNAME := GS-OOLITE
    DEVICE_PROFILE := OOLITE
    TPLINK_HWID := 0x3C000101
    CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += oolite


define Device/NBG6616
	BOARDNAME = NBG6616
	KERNEL_SIZE = 2048k
	IMAGE_SIZE = 15323k
	MTDPARTS = spi0.0:192k(u-boot)ro,64k(env)ro,64k(RFdata)ro,384k(zyxel_rfsd),384k(romd),64k(header),2048k(kernel),13184k(rootfs),15232k@0x120000(firmware)
	CMDLINE += mem=128M
	IMAGES := sysupgrade.bin
	KERNEL := kernel-bin | patch-cmdline | lzma | uImage lzma | jffs2 boot/vmlinux.lzma.uImage
	IMAGE/sysupgrade.bin = append-kernel $$$$(KERNEL_SIZE) | append-rootfs | pad-rootfs | check-size $$$$(IMAGE_SIZE)
	# We cannot currently build a factory image. It is the sysupgrade image
	# prefixed with a header (which is actually written into the MTD device).
	# The header is 2kiB and is filled with 0xff. The format seems to be:
	#   2 bytes:  0x0000
	#   2 bytes:  checksum of the data partition (big endian)
	#   4 bytes:  length of the contained image file (big endian)
	#  32 bytes:  Firmware Version string (NUL terminated, 0xff padded)
	#   2 bytes:  0x0000
	#   2 bytes:  checksum over the header partition (big endian)
	#  32 bytes:  Model (e.g. "NBG6616", NUL termiated, 0xff padded)
	#      rest: 0xff padding
	#
	# The checksums are calculated by adding up all bytes and if a 16bit
	# overflow occurs, one is added and the sum is masked to 16 bit:
	#   csum = csum + databyte; if (csum > 0xffff) { csum += 1; csum &= 0xffff };
	# Should the file have an odd number of bytes then the byte len-0x800 is
	# used additionally.
	# The checksum for the header is calcualted over the first 2048 bytes with
	# the firmware checksum as the placeholder during calculation.
	#
	# The header is padded with 0xff to the erase block size of the device.
endef

TARGET_DEVICES += NBG6616

ifeq ($(SUBTARGET),generic)
define Device/c-55
	BOARDNAME = C-55
	KERNEL_SIZE = 2048k
	IMAGE_SIZE = 15872k
	MTDPARTS = spi0.0:256k(u-boot)ro,128k(u-boot-env)ro,2048k(kernel),13824k(rootfs),13824k(opt)ro,2624k(failsafe)ro,64k(art)ro,15872k@0x60000(firmware)
	IMAGE/sysupgrade.bin = append-kernel $$$$(KERNEL_SIZE) | append-rootfs | pad-rootfs | check-size $$$$(IMAGE_SIZE)
endef

TARGET_DEVICES += c-55

endif # ifeq ($(SUBTARGET),generic)

ifeq ($(SUBTARGET),nand)

define Build/MerakiNAND
	-$(STAGING_DIR_HOST)/bin/mkmerakifw \
		-B $(BOARDNAME) -s \
		-i $@ \
		-o $@.new
	@mv $@.new $@
endef

define Device/mr18
  BOARDNAME = MR18
  BLOCKSIZE := 64k
  CONSOLE = ttyS0,115200
  MTDPARTS = ar934x-nfc:512k(nandloader)ro,8M(kernel),8M(recovery),113664k(ubi),128k@130944k(odm-caldata)ro
  IMAGES := sysupgrade.tar
  KERNEL := kernel-bin | patch-cmdline | MerakiNAND
  KERNEL_INITRAMFS := kernel-bin | patch-cmdline | MerakiNAND
  IMAGE/sysupgrade.tar := sysupgrade-nand
endef
TARGET_DEVICES += mr18

endif # ifeq ($(SUBTARGET),nand)

$(eval $(call BuildImage))
