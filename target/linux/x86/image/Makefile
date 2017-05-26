# 
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

export PATH=$(TARGET_PATH):/sbin

GRUB2_MODULES = biosdisk boot chain configfile ext2 linux ls part_msdos reboot serial vga
GRUB2_MODULES_LEGACY = $(GRUB2_MODULES)
GRUB2_MODULES_LEGACY += part_gpt search fat exfat
GRUB2_MODULES_EFI = boot chain configfile ext2 linux ls part_msdos reboot serial part_gpt part_msdos search fat exfat ext2 efi_gop efi_uga gfxterm
GRUB2_MODULES_ISO = biosdisk boot chain configfile iso9660 linux ls part_msdos reboot serial vga
GRUB_TERMINALS =
GRUB_SERIAL_CONFIG =
GRUB_TERMINAL_CONFIG =
GRUB_CONSOLE_CMDLINE =

USE_ATKBD = generic 64

ifneq ($(strip $(foreach subtarget,$(USE_ATKBD),$(CONFIG_TARGET_x86_$(subtarget)))),)
  GRUB2_MODULES += at_keyboard
  GRUB2_MODULES_ISO += at_keyboard
endif

ifneq ($(CONFIG_GRUB_CONSOLE),)
  GRUB_CONSOLE_CMDLINE += console=tty0
  GRUB_TERMINALS += console
endif

GRUB_SERIAL:=$(call qstrip,$(CONFIG_GRUB_SERIAL))

ifneq ($(GRUB_SERIAL),)
  GRUB_CONSOLE_CMDLINE += console=$(GRUB_SERIAL),$(CONFIG_GRUB_BAUDRATE)n8
  GRUB_SERIAL_CONFIG := serial --unit=0 --speed=$(CONFIG_GRUB_BAUDRATE) --word=8 --parity=no --stop=1 --rtscts=off
  GRUB_TERMINALS += serial
endif

ifneq ($(GRUB_TERMINALS),)
  GRUB_TERMINAL_CONFIG := terminal_input $(GRUB_TERMINALS); terminal_output $(GRUB_TERMINALS)
endif

SIGNATURE:=$(shell dd if=/dev/urandom bs=4 count=1 2>/dev/null | hexdump -v -e '"%08x"')
EFI_SIGNATURE:=$(strip $(shell uuidgen | sed "s/[a-zA-Z0-9][a-zA-Z0-9][a-zA-Z0-9][a-zA-Z0-9]$$/0002/" | tr '[a-z]' '[A-Z]'))
ROOTPART:=$(call qstrip,$(CONFIG_TARGET_ROOTFS_PARTNAME))
ROOTPART:=$(if $(ROOTPART),$(ROOTPART),PARTUUID=$(SIGNATURE)-02)

GRUB_TIMEOUT:=$(call qstrip,$(CONFIG_GRUB_TIMEOUT))

ifneq ($(CONFIG_GRUB_IMAGES),)

  BOOTOPTS:=$(call qstrip,$(CONFIG_GRUB_BOOTOPTS))

  define Image/cmdline/ext4
    root=$(ROOTPART) rootfstype=ext4 rootwait
  endef

  define Image/cmdline/jffs2-64k
    block2mtd.block2mtd=$(ROOTPART),65536,rootfs,5 root=/dev/mtdblock0 rootfstype=jffs2 rootwait
  endef

  define Image/cmdline/jffs2-128k
    block2mtd.block2mtd=$(ROOTPART),131072,rootfs,5 root=/dev/mtdblock0 rootfstype=jffs2 rootwait
  endef

  define Image/cmdline/squashfs
    block2mtd.block2mtd=$(ROOTPART),65536,rootfs,5 root=/dev/mtdblock0 rootfstype=squashfs rootwait
  endef

  define Image/cmdline/efi
    $(subst $(SIGNATURE)-02,$2,$(call Image/cmdline/$(1)))
  endef

ifneq ($(CONFIG_EFI_IMAGES),)
  define Image/Build/efi
	# left here because the image builder doesnt need these
	rm -rf $(KDIR)/root.grub/ || true
	$(INSTALL_DIR) $(KDIR)/root.grub/boot/grub $(KDIR)/grub2
	$(CP) $(KDIR)/bzImage $(KDIR)/root.grub/boot/vmlinuz
	echo '(hd0) $(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img' > $(KDIR)/grub2/device.map
	sed \
		-e 's#@SERIAL_CONFIG@#$(strip $(GRUB_SERIAL_CONFIG))#g' \
		-e 's#@TERMINAL_CONFIG@#$(strip $(GRUB_TERMINAL_CONFIG))#g' \
		-e 's#@CMDLINE@#$(strip $(call Image/cmdline/efi,$(1),$(EFI_SIGNATURE)) $(BOOTOPTS) $(GRUB_CONSOLE_CMDLINE))#g' \
		-e 's#@TIMEOUT@#$(GRUB_TIMEOUT)#g' \
		-e 's#set root.*#search --file /boot/grub/$(SIGNATURE).cfg --set=root#g' \
		./grub.cfg > $(KDIR)/root.grub/boot/grub/grub.cfg
	$(CP) $(KDIR)/root.grub/boot/grub/grub.cfg $(KDIR)/root.grub/boot/grub/$(SIGNATURE).cfg
	grub-mkimage \
		-d $(STAGING_DIR_HOST)/lib/grub/i386-pc \
		-o $(KDIR)/grub2/core.img \
		-O i386-pc \
		-p '(hd0,gpt1)/boot/grub' \
		-c $(KDIR)/root.grub/boot/grub/grub.cfg \
		$(GRUB2_MODULES_LEGACY)
	$(CP) $(STAGING_DIR_HOST)/lib/grub/i386-pc/*.img $(KDIR)/grub2/

	# Build the efi grub version
	rm -rf $(KDIR)/grub2.efi/ || true
	$(INSTALL_DIR) $(KDIR)/grub2.efi/efi/boot/

	# Generate the grub search root config (grub will search for the $(SIGNATURE).cfg file placed on the boot partition as grub does not support search of GPT UUID yet)
	echo "search --file /boot/grub/$(SIGNATURE).cfg --set=root" > $(KDIR)/grub2.efi/efi/boot/grub.cfg
	echo "configfile /boot/grub/grub.cfg" >> $(KDIR)/grub2.efi/efi/boot/grub.cfg

	# Create the EFI grub binary
	grub-mkimage-efi \
		-d $(STAGING_DIR_HOST)/lib/grub/x86_64-efi \
		-o $(KDIR)/grub2.efi/efi/boot/bootx64.efi \
		-O x86_64-efi \
		-p /efi/boot \
		-c $(KDIR)/grub2.efi/efi/boot/grub.cfg \
		$(GRUB2_MODULES_EFI)

	# Generate the EFI VFAT bootfs
	rm $(KDIR)/kernel.efi || true
	mkfs.fat -C $(KDIR)/kernel.efi -S 512 1024
	mcopy -s -i "$(KDIR)/kernel.efi" $(KDIR)/grub2.efi/* ::/

	SIGNATURE="$(SIGNATURE)" PATH="$(TARGET_PATH)" ./gen_image_efi.sh \
		$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) $(KDIR)/root.grub \
		1 $(KDIR)/kernel.efi \
		1 \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) $(KDIR)/root.$(1) \
		256

	# Setup legacy bios for hybrid MBR (optional)
	grub-bios-setup \
		--device-map="$(KDIR)/grub2/device.map" \
		-d "$(KDIR)/grub2" \
		-r "hd0,msdos1" \
		"$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img"

	# Convert the MBR partition to GPT and set EFI ROOTFS signature
	dd if=/dev/zero of="$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img" bs=512 count=33 conv=notrunc oflag=append
	sgdisk -g "$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img"
	sgdisk -t 2:EF00 "$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img"
	sgdisk -t 3:EF02 "$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img"
	sgdisk -u 4:$(EFI_SIGNATURE) "$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img"
	sgdisk -h "$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img"

	# Setup EFI grub
	grub-bios-setup-efi \
		--device-map="$(KDIR)/grub2/device.map" \
		-d "$(KDIR)/grub2" \
		-r "hd0,gpt1" \
		"$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img"
  endef
endif

  define Image/Build/grub2
	# left here because the image builder doesnt need these
	$(INSTALL_DIR) $(KDIR)/root.grub/boot/grub $(KDIR)/grub2
	$(CP) $(KDIR)/bzImage $(KDIR)/root.grub/boot/vmlinuz
	grub-mkimage \
		-d $(STAGING_DIR_HOST)/lib/grub/i386-pc \
		-o $(KDIR)/grub2/core.img \
		-O i386-pc \
		-c ./grub-early.cfg \
		$(GRUB2_MODULES)
	$(CP) $(STAGING_DIR_HOST)/lib/grub/i386-pc/*.img $(KDIR)/grub2/
	echo '(hd0) $(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img' > $(KDIR)/grub2/device.map
	sed \
		-e 's#@SERIAL_CONFIG@#$(strip $(GRUB_SERIAL_CONFIG))#g' \
		-e 's#@TERMINAL_CONFIG@#$(strip $(GRUB_TERMINAL_CONFIG))#g' \
		-e 's#@CMDLINE@#$(strip $(call Image/cmdline/$(1)) $(BOOTOPTS) $(GRUB_CONSOLE_CMDLINE))#g' \
		-e 's#@TIMEOUT@#$(GRUB_TIMEOUT)#g' \
		./grub.cfg > $(KDIR)/root.grub/boot/grub/grub.cfg
	PADDING="$(CONFIG_TARGET_IMAGES_PAD)" SIGNATURE="$(SIGNATURE)" PATH="$(TARGET_PATH)" ./gen_image_generic.sh \
		$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) $(KDIR)/root.grub \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) $(KDIR)/root.$(1) \
		256
	grub-bios-setup \
		--device-map="$(KDIR)/grub2/device.map" \
		-d "$(KDIR)/grub2" \
		-r "hd0,msdos1" \
		"$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img"
	$(call Image/Build/grub/$(1))
  endef
endif

ROOTDELAY=10

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build/iso
	$(INSTALL_DIR) $(KDIR)/root.grub/boot/grub $(KDIR)/grub2
	$(CP) $(KDIR)/bzImage $(KDIR)/root.grub/boot/vmlinuz
	grub-mkimage \
		-d $(STAGING_DIR_HOST)/lib/grub/i386-pc \
		-o $(KDIR)/grub2/eltorito.img \
		-O i386-pc \
		-c ./grub-early.cfg \
		$(GRUB2_MODULES_ISO)
	cat \
		$(STAGING_DIR_HOST)/lib/grub/i386-pc/cdboot.img \
		$(KDIR)/grub2/eltorito.img \
		> $(KDIR)/root.grub/boot/grub/eltorito.img
	sed \
		-e 's#@SERIAL_CONFIG@#$(strip $(GRUB_SERIAL_CONFIG))#g' \
		-e 's#@TERMINAL_CONFIG@#$(strip $(GRUB_TERMINAL_CONFIG))#g' \
		-e 's#@CMDLINE@#root=/dev/sr0 rootfstype=iso9660 rootwait $(strip $(call Image/cmdline/$(1)) $(BOOTOPTS) $(GRUB_CONSOLE_CMDLINE))#g' \
		-e 's#@TIMEOUT@#$(GRUB_TIMEOUT)#g' \
		./grub-iso.cfg > $(KDIR)/root.grub/boot/grub/grub.cfg
	$(CP) $(KDIR)/bzImage $(KDIR)/root.grub/boot/vmlinuz
	mkisofs -R -b boot/grub/eltorito.img -no-emul-boot -boot-info-table \
		-o $(KDIR)/root.iso $(KDIR)/root.grub $(TARGET_DIR)
endef

ifneq ($(CONFIG_VDI_IMAGES),)
  define Image/Build/vdi
	rm $(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).vdi || true
	qemu-img convert -f raw -O vdi \
		$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img \
		$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).vdi
	# XXX: VBoxManage insists on setting perms to 0600
	chmod 0644 $(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).vdi
  endef
  define Image/Build/vdi_efi
	rm $(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).vdi || true
	qemu-img convert -f raw -O vdi \
		$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img \
		$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).vdi
	# XXX: VBoxManage insists on setting perms to 0600
	chmod 0644 $(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).vdi
  endef
endif

ifneq ($(CONFIG_VMDK_IMAGES),)
  define Image/Build/vmdk
	rm $(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).vmdk || true
	qemu-img convert -f raw -O vmdk \
		$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img \
		$(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).vmdk
  endef
  define Image/Build/vmdk_efi
	rm $(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).vmdk || true
	qemu-img convert -f raw -O vmdk \
		$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img \
		$(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).vmdk
  endef
endif

define Image/Build/gzip
	gzip -f9n $(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img
	gzip -f9n $(BIN_DIR)/$(IMG_PREFIX)-rootfs-$(1).img
endef

define Image/Build/gzip_efi
	gzip -f9n $(BIN_DIR)/$(IMG_PREFIX)-uefi-gpt-$(1).img
endef

ifneq ($(CONFIG_TARGET_IMAGES_GZIP),)
  define Image/Build/gzip/ext4
	$(call Image/Build/gzip,ext4)
  endef
  ifneq ($(CONFIG_TARGET_IMAGES_PAD),)
    define Image/Build/gzip/squashfs
	$(call Image/Build/gzip,squashfs)
    endef
    define Image/Build/gzip/jffs2-64k
	$(call Image/Build/gzip,jffs2-64k)
    endef
    define Image/Build/gzip/jffs2-128k
	$(call Image/Build/gzip,jffs2-128k)
    endef
  endif
endif

define Image/BuildKernel
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/$(IMG_PREFIX)-vmlinuz
  ifneq ($(CONFIG_X86_ETHERBOOT_IMAGES),)
	rm -f $(BIN_DIR)/$(IMG_PREFIX)-etherboot
	$(STAGING_DIR_HOST)/bin/mkelfImage \
		--append=$(CONFIG_X86_ETHERBOOT_BOOTOPTS) \
		$(KDIR)/bzImage \
		$(BIN_DIR)/$(IMG_PREFIX)-etherboot
  endif
endef

define Image/Build/Profile/ar525w
	cp $(KDIR)/bzImage $(KDIR)/bzImage.tmp
	$(SCRIPT_DIR)/pad_image $(1) $(KDIR)/bzImage.tmp $(KDIR)/root.$(1) 32
	$(STAGING_DIR_HOST)/bin/airlink -b 1 $(KDIR)/bzImage.tmp $(KDIR)/root.$(1) \
		$(BIN_DIR)/$(IMG_PREFIX)-$(1)-ar525w.img
	$(STAGING_DIR_HOST)/bin/airlink -e -b 1 $(KDIR)/bzImage.tmp $(KDIR)/root.$(1) \
		$(BIN_DIR)/$(IMG_PREFIX)-$(1)-ar525w-web.img
endef

define Image/Build/Profile/sitecom
	cp $(KDIR)/bzImage $(KDIR)/bzImage.tmp
	#32k config data + 20 bytes header + 2 bytes checksum after kernel image
	$(SCRIPT_DIR)/pad_image $(1) $(KDIR)/bzImage.tmp $(KDIR)/root.$(1) 32790
	$(TOPDIR)/target/linux/rdc/image/mkimg_sitecom.pl $(KDIR)/bzImage.tmp > $(KDIR)/tmp.img
	cat $(KDIR)/root.$(1) >> $(KDIR)/tmp.img
	cp $(KDIR)/tmp.img $(BIN_DIR)/$(IMG_PREFIX)-$(1)-sitecom.img
	rm $(KDIR)/tmp.img $(KDIR)/bzImage.tmp
endef

define Image/Build/Profile/bifferboard
	$(TOPDIR)/target/linux/rdc/image/mkimg_bifferboard.py $(KDIR)/bzImage $(KDIR)/root.$(1) \
		$(BIN_DIR)/$(IMG_PREFIX)-$(1)-bifferboard.img
endef

define Image/Prepare
	$(call Image/Prepare/grub2)
endef

define Image/Build/Initramfs
	$(CP) $(KDIR)/bzImage-initramfs $(BIN_DIR)/$(IMG_PREFIX)-ramfs.bzImage
endef

define Image/Build
	$(call Image/Build/$(1))
  ifneq ($(1),iso)
	$(call Image/Build/grub2,$(1))
	$(call Image/Build/efi,$(1))
	$(call Image/Build/vdi,$(1))
	$(call Image/Build/vmdk,$(1))
ifeq ($(CONFIG_EFI_IMAGES),y)
	$(call Image/Build/vdi_efi,$(1))
	$(call Image/Build/vmdk_efi,$(1))
endif
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-rootfs-$(1).img
  else
	$(CP) $(KDIR)/root.iso $(BIN_DIR)/$(IMG_PREFIX).iso
  endif
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/$(IMG_PREFIX)-vmlinuz
	$(call Image/Build/gzip/$(1))
ifeq ($(CONFIG_EFI_IMAGES),y)
	$(call Image/Build/gzip_efi,$(1))
endif
	$(call Image/Build/Profile/$(PROFILE),$(1))
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	$(call Image/Build/Initramfs)
endif
endef

$(eval $(call BuildImage))
