# ARM SBAS image makefile
# Copyright (C) 2017 Mathew McBride <matt@traverse.com.au>
# Copyright (C) 2016-2017 Yousong Zhou <yszhou4tech@gmail.com>
#
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

# Note: ARM Server's use EFI, much like x86 (obviously, an MBR is not
# required), so the grub setup is based on target/linux/x86/image/Makefile

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

# Generate random partition GUIDs for both partitions, this is so
# grub can do a GUID partition search (fallback), and so we can use
# root=PARTUUID=... for the bootloader
# (root=PARTUUID= is important as the actual storage medium may
# differ between hosts, i.e virtio, SCSI, NVMe)
BOOT_SIGNATURE:=$(shell perl -e 'printf("%04x-%04x", rand(0xFFFF), rand(0xFFFF))')
EFI_SIGNATURE:=$(shell perl -e 'printf("%08x-%04x-%04x-%04x-%06x%06x", rand(0xFFFFFFFF), rand(0xFFFF), rand(0xFFFF), rand(0xFFFF), rand(0xFFFFFF), rand(0xFFFFFF))')

GRUB2_MODULES = part_gpt disk fat search search_fs_uuid search_label linux ls

GRUB_TERMINALS =
GRUB_SERIAL_CONFIG =
GRUB_TERMINAL_CONFIG =
GRUB_CONSOLE_CMDLINE =

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

GRUB_TIMEOUT:=$(call qstrip,$(CONFIG_GRUB_TIMEOUT))

define Image/BuildKernel
	$(foreach k,$(filter zImage Image,$(KERNELNAME)), \
		cp $(KDIR)/$(KERNELNAME) $(BIN_DIR)/$(IMG_PREFIX)-$(k) \
	)
endef

define Image/Build/Initramfs
	$(foreach k,$(filter zImage Image,$(KERNELNAME)), \
		cp $(KDIR)/$(k)-initramfs $(BIN_DIR)/$(IMG_PREFIX)-$(k)-initramfs \
	)
endef

ifneq ($(CONFIG_GRUB_IMAGES),)
define Image/Build/efi
	
	$(INSTALL_DIR) $(KDIR)/root.efi/grub/arm64-efi
	$(INSTALL_DIR) $(KDIR)/root.efi/EFI/boot/
	sed \
	-e 's#@BOOT_UUID@#$(strip $(BOOT_SIGNATURE))#g' \
	./grub-early.cfg > $(KDIR)/grub-early.cfg

	$(STAGING_DIR_HOST)/bin/grub-mkimage \
		-p "/grub" \
                -d $(STAGING_DIR_HOST)/lib/grub/arm64-efi \
                -o $(KDIR)/root.efi/EFI/boot/bootaa64.efi \
                -O arm64-efi \
                -c $(KDIR)/grub-early.cfg \
                $(GRUB2_MODULES)
	$(CP) $(STAGING_DIR_HOST)/lib/grub/arm64-efi/*.mod $(KDIR)/root.efi/grub/arm64-efi/

	sed \
	-e 's#@SERIAL_CONFIG@#$(strip $(GRUB_SERIAL_CONFIG))#g' \
	-e 's#@TERMINAL_CONFIG@#$(strip $(GRUB_TERMINAL_CONFIG))#g' \
	-e 's#@CMDLINE@#$(strip $(call Image/cmdline/$(1)) root=PARTUUID=$(EFI_SIGNATURE) $(BOOTOPTS) $(GRUB_CONSOLE_CMDLINE))#g' \
	-e 's#@TIMEOUT@#$(GRUB_TIMEOUT)#g' \
	-e 's#@ROOT@#$(GRUB_ROOT)#g' \
	./grub.cfg > $(KDIR)/root.efi/grub/grub.cfg

	$(CP) $(KDIR)/vmlinux $(KDIR)/root.efi/vmlinux

	rm "$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img" || true
 
	PADDING="$(CONFIG_TARGET_IMAGES_PAD)" SIGNATURE="$(SIGNATURE)" PATH="$(TARGET_PATH)" $(SCRIPT_DIR)/gen_image_efi.sh \
                $(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) $(KDIR)/root.efi $(BOOT_SIGNATURE) \
                $(CONFIG_TARGET_ROOTFS_PARTSIZE) $(KDIR)/root.$(1) \
                256
	dd if=/dev/zero of="$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img" bs=512 count=1024 conv=notrunc oflag=append
	sgdisk -g "$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img"
	sgdisk -t 1:EF00 "$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img"
	sgdisk -u 2:$(EFI_SIGNATURE) "$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img"
	sgdisk -h "$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img"
endef
endif

ifneq ($(CONFIG_QCOW_IMAGES),)
   define Image/Build/qcow
		rm $(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).qcow2 || true
		$(STAGING_DIR_HOST)/bin/qemu-img convert -f raw -O qcow2 \
			$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img \
			$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).qcow2
   endef
endif

ifneq ($(CONFIG_TARGET_IMAGES_GZIP),)
   define Image/Build/gzip
	gzip -f9n $(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img
   endef
endif

define Image/Build
	$(call Image/Build/$(1))
   ifneq ($(1),squashfs)
	$(call Image/Build/efi,$(1)) # Do not build for squashfs etc.
	$(call Image/Build/qcow,$(1))
	$(call Image/Build/gzip,$(1))
   endif
	dd if=$(KDIR)/root.$(1) bs=128k conv=sync | \
		gzip -9n >$(BIN_DIR)/$(IMG_PREFIX)-root.$(1).gz
endef

$(eval $(call BuildImage))
