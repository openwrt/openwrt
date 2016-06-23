# UBNT_BOARD e.g. one of (XS2, XS5, RS, XM)
# UBNT_TYPE e.g. one of (BZ, XM, XW)
# UBNT_CHIP e.g. one of (ar7240, ar933x, ar934x)

# mkubntimage is using the kernel image direct
# routerboard creates partitions out of the ubnt header
define Build/mkubntimage
	$(STAGING_DIR_HOST)/bin/mkfwimage \
		-B $(UBNT_BOARD) -v $(UBNT_TYPE).$(UBNT_CHIP).v6.0.0-OpenWrt-$(REVISION) \
		-k $(word 1,$^) \
		-r $@ \
		-o $@
endef

# all UBNT XM device expect the kernel image to have 1024k while flash, when
# booting the image, the size doesn't matter.
define Build/mkubntimage-split
	dd if=$@ of=$@.old1 bs=1024k count=1
	dd if=$@ of=$@.old2 bs=1024k skip=1
	$(STAGING_DIR_HOST)/bin/mkfwimage \
		-B $(UBNT_BOARD) -v $(UBNT_TYPE).$(UBNT_CHIP).v6.0.0-OpenWrt-$(REVISION) \
		-k $@.old1 \
		-r $@.old2 \
		-o $@
	rm $@.old1 $@.old2
endef

define Build/mkubntimage2
	$(STAGING_DIR_HOST)/bin/mkfwimage2 -f 0x9f000000 \
		-v $(UBNT_TYPE).$(UBNT_CHIP).v6.0.0-OpenWrt-$(REVISION) \
		-p jffs2:0x50000:0xf60000:0:0:$@ \
		-o $@.new
	@mv $@.new $@
endef

DEVICE_VARS += UBNT_BOARD UBNT_CHIP UBNT_TYPE

# UBNT_BOARD e.g. one of (XS2, XS5, RS, XM)
# UBNT_TYPE e.g. one of (BZ, XM, XW)
# UBNT_CHIP e.g. one of (ar7240, ar933x, ar934x)
define Device/ubnt-xm
  DEVICE_PROFILE := UBNT
  IMAGE_SIZE := 7552k
  MTDPARTS = spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,7552k(firmware),256k(cfg)ro,64k(EEPROM)ro
  UBNT_TYPE := XM
  UBNT_BOARD := XM
  UBNT_CHIP := ar7240
  IMAGES := sysupgrade.bin factory.bin
  IMAGE/factory.bin = $$(IMAGE/sysupgrade.bin) | mkubntimage-split
  IMAGE/sysupgrade.bin = append-kernel $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | check-size $$$$(IMAGE_SIZE)
endef

define Device/ubnt-xw
  DEVICE_PROFILE := UBNT
  IMAGE_SIZE := 7552k
  MTDPARTS = spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,7552k(firmware),256k(cfg)ro,64k(EEPROM)ro
  UBNT_TYPE := XW
  UBNT_BOARD := XM
  UBNT_CHIP := ar934x
  IMAGES := sysupgrade.bin factory.bin
  IMAGE/factory.bin = $$(IMAGE/sysupgrade.bin) | mkubntimage-split
  IMAGE/sysupgrade.bin = append-kernel $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | check-size $$$$(IMAGE_SIZE)
endef

define Device/ubnt-bz
  DEVICE_PROFILE := UBNT
  IMAGE_SIZE := 7552k
  MTDPARTS = spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,7552k(firmware),256k(cfg)ro,64k(EEPROM)ro
  UBNT_TYPE := BZ
  UBNT_BOARD := XM
  UBNT_CHIP := ar934x
  IMAGES := sysupgrade.bin factory.bin
  IMAGE/factory.bin = $$(IMAGE/sysupgrade.bin) | mkubntimage-split
  IMAGE/sysupgrade.bin = append-kernel $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | check-size $$$$(IMAGE_SIZE)
endef

define Device/ubnt-unifiac
  DEVICE_PROFILE := UBNT
  IMAGE_SIZE := 7744k
  MTDPARTS = spi0.0:384k(u-boot)ro,64k(u-boot-env)ro,7744k(firmware),7744k(ubnt-airos)ro,128k(bs)ro,256k(cfg)ro,64k(EEPROM)ro
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin = append-kernel $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | check-size $$$$(IMAGE_SIZE)
endef

define Device/rw2458n
  $(Device/ubnt-xm)
  BOARDNAME := RW2458N
endef

define Device/ubnt-airrouter
  $(Device/ubnt-xm)
  BOARDNAME := UBNT-AR
endef

define Device/ubnt-bullet-m
  $(Device/ubnt-xm)
  BOARDNAME := UBNT-BM
endef

define Device/ubnt-rocket-m
  $(Device/ubnt-xm)
  BOARDNAME := UBNT-RM
endef

define Device/ubnt-nano-m
  $(Device/ubnt-xm)
  BOARDNAME := UBNT-NM
endef
TARGET_DEVICES += rw2458n ubnt-airrouter ubnt-bullet-m ubnt-rocket-m ubnt-nano-m

define Device/ubnt-unifi
  $(Device/ubnt-bz)
  BOARDNAME := UBNT-UF
  DEVICE_PROFILE := UBNT UBNTUNIFI
endef

define Device/ubnt-unifiac-lite
  $(Device/ubnt-unifiac)
  DEVICE_PROFILE := UBNT UBNTUNIFIACLITE
  BOARDNAME := UBNT-UF-AC-LITE
endef

define Device/ubnt-unifiac-pro
  $(Device/ubnt-unifiac)
  DEVICE_PROFILE := UBNT UBNTUNIFIACPRO
  BOARDNAME := UBNT-UF-AC-PRO
endef

define Device/ubnt-unifi-outdoor
  $(Device/ubnt-bz)
  BOARDNAME := UBNT-U20
  DEVICE_PROFILE := UBNT UBNTUNIFIOUTDOOR
endef
TARGET_DEVICES += ubnt-unifi ubnt-unifiac-lite ubnt-unifiac-pro ubnt-unifi-outdoor

define Device/ubnt-nano-m-xw
  $(Device/ubnt-xw)
  BOARDNAME := UBNT-NM-XW
endef

define Device/ubnt-loco-m-xw
  $(Device/ubnt-xw)
  BOARDNAME := UBNT-LOCO-XW
endef

define Device/ubnt-rocket-m-xw
  $(Device/ubnt-xw)
  BOARDNAME := UBNT-RM-XW
endef

define Device/ubnt-rocket-m-ti
  $(Device/ubnt-xw)
  BOARDNAME := UBNT-RM-TI
  UBNT_TYPE := TI
  UBNT_BOARD := XM
endef
TARGET_DEVICES += ubnt-nano-m-xw ubnt-loco-m-xw ubnt-rocket-m-xw ubnt-rocket-m-ti

define Device/ubnt-air-gateway
  $(Device/ubnt-xm)
  BOARDNAME := UBNT-AGW
  UBNT_BOARD := XM
  UBNT_TYPE := AirGW
  UBNT_CHIP := ar933x
  CONSOLE = ttyATH0,115200
endef
TARGET_DEVICES += ubnt-air-gateway

define Device/ubnt-air-gateway-pro
  $(Device/ubnt-xm)
  BOARDNAME := UBNT-AGWP
  UBNT_TYPE := AirGWP
  UBNT_CHIP := ar934x
  CONSOLE = ttyS0,115200
endef
TARGET_DEVICES += ubnt-air-gateway-pro

define Device/ubdev01
  $(Device/ubnt-xm)
  MTDPARTS := spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,7488k(firmware),64k(certs),256k(cfg)ro,64k(EEPROM)ro
  BOARDNAME := UBNT-UF
  UBNT_BOARD := UBDEV01
  UBNT_TYPE := XM
  UBNT_CHIP := ar7240
endef

TARGET_DEVICES += ubdev01

define Device/ubnt-routerstation
  IMAGE_SIZE := 16128k
  IMAGES := sysupgrade.bin factory.bin
  IMAGE/factory.bin = append-rootfs | pad-rootfs | mkubntimage
  IMAGE/sysupgrade.bin = append-rootfs | pad-rootfs | combined-image | check-size $$$$(IMAGE_SIZE)
  KERNEL := kernel-bin | patch-cmdline | lzma | pad-to $$(BLOCKSIZE)
endef

define Device/ubnt-rs
$(Device/ubnt-routerstation)
  BOARDNAME := UBNT-RS
  DEVICE_PROFILE := Madwifi UBNT UBNTRS
  UBNT_BOARD := RS
  UBNT_TYPE := RSx
  UBNT_CHIP := ar7100
endef

define Device/ubnt-rspro
$(Device/ubnt-routerstation)
  BOARDNAME := UBNT-RSPRO
  DEVICE_PROFILE := Madwifi UBNT UBNTRSPRO
  UBNT_BOARD := RSPRO
  UBNT_TYPE := RSPRO
  UBNT_CHIP := ar7100pro
endef

define Device/ubnt-ls-sr71
$(Device/ubnt-routerstation)
  BOARDNAME := UBNT-LS-SR71
  DEVICE_PROFILE := Madwifi UBNT
  UBNT_BOARD := LS-SR71
  UBNT_TYPE := LS-SR71
  UBNT_CHIP := ar7100
endef

TARGET_DEVICES += ubnt-rs ubnt-rspro ubnt-ls-sr71

define Device/ubnt-uap-pro
  KERNEL_SIZE := 1536k
  IMAGE_SIZE := 15744k
  MTDPARTS := spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,1536k(kernel),14208k(rootfs),256k(cfg)ro,64k(EEPROM)ro,15744k@0x50000(firmware)
  UBNT_TYPE := BZ
  UBNT_CHIP := ar934x
  BOARDNAME := UAP-PRO
  DEVICE_PROFILE := UBNT UAPPRO
  KERNEL := kernel-bin | patch-cmdline | lzma | uImage lzma | jffs2 kernel0
  IMAGES := sysupgrade.bin factory.bin
  IMAGE/sysupgrade.bin = append-kernel $$$$(KERNEL_SIZE) | append-rootfs | pad-rootfs | check-size $$$$(IMAGE_SIZE)
  IMAGE/factory.bin = $$(IMAGE/sysupgrade.bin) | mkubntimage2
endef

define Device/ubnt-unifi-outdoor-plus
$(Device/ubnt-uap-pro)
  UBNT_CHIP := ar7240
  BOARDNAME := UBNT-UOP
  DEVICE_PROFILE := UBNT
endef

TARGET_DEVICES += ubnt-uap-pro ubnt-unifi-outdoor-plus
