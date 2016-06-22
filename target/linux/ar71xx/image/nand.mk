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
