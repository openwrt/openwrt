define Device/glinet_gl-sft1200
  ATH_SOC := sf16a18
  DEVICE_TITLE := GL-SFT1200
  KERNEL_SIZE := 4096k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  VID_HDR_OFFSET := 2048
  IMAGES += factory.img sysupgrade.tar
  IMAGE/sysupgrade.tar := sysupgrade-tar | append-gl-metadata
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi
endef

ifdef CONFIG_TARGET_siflower_sf19a28_fullmask_SF19A28-GL-SFT1200
TARGET_DEVICES += glinet_gl-sft1200
endif
