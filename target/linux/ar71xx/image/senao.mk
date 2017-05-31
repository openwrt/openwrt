define Build/senao-factory-image
	$(eval board=$(word 1,$(1)))
	$(eval rootfs=$(word 2,$(1)))

	mkdir -p $@.senao

	touch $@.senao/FWINFO-OpenWrt-$(REVISION)-$(board)
	$(CP) $(IMAGE_KERNEL) $@.senao/openwrt-senao-$(board)-uImage-lzma.bin
	$(CP) $(rootfs) $@.senao/openwrt-senao-$(board)-root.squashfs

	$(TAR) -czf $@ -C $@.senao .
	rm -rf $@.senao
endef


define Device/ens202ext
  DEVICE_TITLE := EnGenius ENS202EXT
  BOARDNAME := ENS202EXT
  DEVICE_PACKAGES += rssileds
  KERNEL_SIZE := 1536k
  KERNEL := kernel-bin | patch-cmdline | lzma | uImage lzma | check-size $$(KERNEL_SIZE)
  IMAGE_SIZE := 12096k
  IMAGES += factory.bin
  IMAGE/factory.bin/squashfs := append-rootfs | pad-rootfs | senao-factory-image ens202ext $$$$@
  MTDPARTS := spi0.0:256k(u-boot)ro,64k(u-boot-env),320k(custom)ro,13632k(firmware),2048k(failsafe)ro,64k(art)ro
endef
TARGET_DEVICES += ens202ext
