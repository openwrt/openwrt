#
# Copyright (C) 2006-2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

GRUB2_VARIANT =
GRUB_TERMINALS =
GRUB_SERIAL_CONFIG =
GRUB_TERMINAL_CONFIG =
GRUB_CONSOLE_CMDLINE =

ifneq ($(strip $(foreach subtarget,$(USE_ATKBD),$(CONFIG_TARGET_x86_$(subtarget)))),)
  GRUB2_VARIANT := generic
else
  GRUB2_VARIANT := legacy
endif

ifneq ($(CONFIG_GRUB_CONSOLE),)
  GRUB_CONSOLE_CMDLINE += console=tty0
  GRUB_TERMINALS += console
endif

GRUB_SERIAL:=$(call qstrip,$(CONFIG_GRUB_SERIAL))

ifneq ($(GRUB_SERIAL),)
  GRUB_CONSOLE_CMDLINE += console=$(GRUB_SERIAL),$(CONFIG_GRUB_BAUDRATE)n8$(if $(CONFIG_GRUB_FLOWCONTROL),r,)
  GRUB_SERIAL_CONFIG := serial --unit=0 --speed=$(CONFIG_GRUB_BAUDRATE) --word=8 --parity=no --stop=1 --rtscts=$(if $(CONFIG_GRUB_FLOWCONTROL),on,off)
  GRUB_TERMINALS += serial
endif

ifneq ($(GRUB_TERMINALS),)
  GRUB_TERMINAL_CONFIG := terminal_input $(GRUB_TERMINALS); terminal_output $(GRUB_TERMINALS)
endif

ROOTPART:=$(call qstrip,$(CONFIG_TARGET_ROOTFS_PARTNAME))
ROOTPART:=$(if $(ROOTPART),$(ROOTPART),PARTUUID=$(IMG_PART_SIGNATURE)-02)

GRUB_TIMEOUT:=$(call qstrip,$(CONFIG_GRUB_TIMEOUT))
GRUB_TITLE:=$(call qstrip,$(CONFIG_GRUB_TITLE))

BOOTOPTS:=$(call qstrip,$(CONFIG_GRUB_BOOTOPTS))

define Build/combined
	$(CP) $(KDIR)/$(KERNEL_NAME) $@.boot/boot/vmlinuz
	-$(CP) $(STAGING_DIR_ROOT)/boot/. $@.boot/boot/
	PADDING="$(CONFIG_TARGET_IMAGES_PAD)" SIGNATURE="$(IMG_PART_SIGNATURE)" $(SCRIPT_DIR)/gen_image_generic.sh \
		$@ \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) $@.boot \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) $(IMAGE_ROOTFS) \
		256
endef

define Build/grub-config
	rm -fR $@.boot
	$(INSTALL_DIR) $@.boot/boot/grub
	sed \
		-e 's#@SERIAL_CONFIG@#$(strip $(GRUB_SERIAL_CONFIG))#g' \
		-e 's#@TERMINAL_CONFIG@#$(strip $(GRUB_TERMINAL_CONFIG))#g' \
		-e 's#@ROOTPART@#root=$(ROOTPART) rootwait#g' \
		-e 's#@CMDLINE@#$(BOOTOPTS) $(GRUB_CONSOLE_CMDLINE)#g' \
		-e 's#@TIMEOUT@#$(GRUB_TIMEOUT)#g' \
		-e 's#@TITLE@#$(GRUB_TITLE)#g' \
		./grub-$(1).cfg > $@.boot/boot/grub/grub.cfg
endef

define Build/grub-install
	rm -fR $@.grub2
	$(INSTALL_DIR) $@.grub2
	$(CP) $(STAGING_DIR_HOST)/lib/grub/i386-pc/*.img \
		$(STAGING_DIR_HOST)/lib/grub/grub2-$(GRUB2_VARIANT)/core.img \
		$@.grub2/
	echo '(hd0) $@' > $@.grub2/device.map
	$(STAGING_DIR_HOST)/bin/grub-bios-setup \
		-m "$@.grub2/device.map" \
		-d "$@.grub2" \
		-r "hd0,msdos1" \
		$@
endef

define Build/iso
	$(CP) $(KDIR)/$(KERNEL_NAME) $@.boot/boot/vmlinuz
	cat \
		$(STAGING_DIR_HOST)/lib/grub/i386-pc/cdboot.img \
		$(STAGING_DIR_HOST)/lib/grub/grub2-iso/eltorito.img \
		> $@.boot/boot/grub/eltorito.img
	-$(CP) $(STAGING_DIR_ROOT)/boot/. $@.boot/boot/
	mkisofs -R -b boot/grub/eltorito.img -no-emul-boot -boot-info-table \
		-o $@ $@.boot $(TARGET_DIR)
endef

define Device/Default
  ARTIFACT/image.iso := grub-config iso | iso
  IMAGES := combined.img.gz
  IMAGE/combined.img := append-rootfs | pad-extra 128k | grub-config pc | combined | grub-install
  IMAGE/combined.img.gz := append-rootfs | pad-extra 128k | grub-config pc | combined | grub-install | gzip
  IMAGE/combined.vdi := append-rootfs | pad-extra 128k | grub-config pc | combined | grub-install | qemu-image vdi
  IMAGE/combined.vmdk := append-rootfs | pad-extra 128k | grub-config pc | combined | grub-install | qemu-image vmdk
  KERNEL := kernel-bin
  KERNEL_INSTALL := 1
  KERNEL_NAME := bzImage
  ifeq ($(CONFIG_TARGET_IMAGES_GZIP),y)
    IMAGES := combined.img.gz
  else
    IMAGES := combined.img
  endif
  ifeq ($(CONFIG_ISO_IMAGES),y)
    ARTIFACTS := image.iso
  endif
  ifeq ($(CONFIG_VDI_IMAGES),y)
    IMAGES += combined.vdi
  endif
  ifeq ($(CONFIG_VMDK_IMAGES),y)
    IMAGES += combined.vmdk
  endif
endef

$(eval $(call Image/gzip-ext4-padded-squashfs))

ifeq ($(SUBTARGET),64)
  include 64.mk
endif

ifeq ($(SUBTARGET),generic)
  include generic.mk
endif

ifeq ($(SUBTARGET),legacy)
  include legacy.mk
endif

$(eval $(call BuildImage))
