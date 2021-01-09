DEVICE_VARS += ENGENIUS_IMGNAME

# This needs to make /tmp/_sys/sysupgrade.tgz an empty file prior to
# sysupgrade, as otherwise it will implant the old configuration from
# OEM firmware when writing rootfs from factory.bin
define Build/engenius-tar-gz
	-[ -f "$@" ] && \
	mkdir -p $@.tmp && \
	touch $@.tmp/failsafe.bin && \
	echo '#!/bin/sh' > $@.tmp/before-upgrade.sh && \
	echo ': > /tmp/_sys/sysupgrade.tgz' >> $@.tmp/before-upgrade.sh && \
	$(CP) $(KDIR)/loader-$(DEVICE_NAME).uImage \
		$@.tmp/openwrt-$(word 1,$(1))-uImage-lzma.bin && \
	$(CP) $@ $@.tmp/openwrt-$(word 1,$(1))-root.squashfs && \
	$(TAR) -cp --numeric-owner --owner=0 --group=0 --mode=a-s --sort=name \
		$(if $(SOURCE_DATE_EPOCH),--mtime="@$(SOURCE_DATE_EPOCH)") \
		-C $@.tmp . | gzip -9n > $@ && \
	rm -rf $@.tmp
endef

define Device/engenius_loader_okli
  DEVICE_VENDOR := EnGenius
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma -M 0x4f4b4c49
  LOADER_TYPE := bin
  COMPILE := loader-$(1).bin loader-$(1).uImage
  COMPILE/loader-$(1).bin := loader-okli-compile
  COMPILE/loader-$(1).uImage := append-loader-okli $(1) | pad-to 64k | lzma | \
	uImage lzma
  IMAGES += factory.bin
  IMAGE/factory.bin := append-squashfs-fakeroot-be | pad-to $$$$(BLOCKSIZE) | \
	append-kernel | pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | \
	check-size | engenius-tar-gz $$$$(ENGENIUS_IMGNAME)
endef
