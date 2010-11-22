#
# arch/m68k/boot/Makefile
#
# Based on arch/sh/boot/Makefile by Stuart Menefy
#
# Copyright (c) 2008 Freescale Semiconductor, Inc.  All Rights Reserved.
#	by Kurt Mahan <kmahan@freescale.com>
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#

MKIMAGE := $(srctree)/scripts/mkuboot.sh

#
# Assign safe dummy values if these variables are not defined,
# in order to suppress error message.
#
CONFIG_SDRAM_BASE	?= 0x40000000
CONFIG_IMG_START	?= 0x00020000

export CONFIG_SDRAM_BASE CONFIG_IMG_START

targets := zImage zImage.srec vmlinux.srec uImage uImage.srec

$(obj)/zImage: $(obj)/vmlinux.bin FORCE
	$(call if_changed,gzip)
	@echo '  Image $@ is ready'

OBJCOPYFLAGS_zImage.srec := -I binary -O srec
$(obj)/zImage.srec: $(obj)/zImage
	$(call if_changed,objcopy)

KERNEL_LOAD	:= $(shell /bin/bash -c 'printf "0x%08x" \
		     $$[$(CONFIG_SDRAM_BASE)  + \
			$(CONFIG_IMG_START)]')

KERNEL_ENTRY	:= $(shell /bin/bash -c 'printf "0x%08x" \
		     $$[$(CONFIG_SDRAM_BASE)  + \
			$(CONFIG_IMG_START)]')

quiet_cmd_uimage = UIMAGE  $@
	cmd_uimage = $(CONFIG_SHELL) $(MKIMAGE) -A m68k -O linux -T kernel \
		   -C gzip -a $(KERNEL_LOAD) -e $(KERNEL_ENTRY) \
		   -n 'Linux-$(KERNELRELEASE)' -d $< $@

$(obj)/uImage: $(obj)/vmlinux.bin.gz FORCE
	$(call if_changed,uimage)
	@echo '  Image $@ is ready'

$(obj)/vmlinux.bin: vmlinux FORCE
	$(call if_changed,objcopy)

$(obj)/vmlinux.bin.gz: $(obj)/vmlinux.bin FORCE
	$(call if_changed,gzip)

OBJCOPYFLAGS_vmlinux.srec := -I binary -O srec
$(obj)/vmlinux.srec: $(obj)/vmlinux.bin
	$(call if_changed,objcopy)

OBJCOPYFLAGS_uImage.srec := -I binary -O srec
$(obj)/uImage.srec: $(obj)/uImage
	$(call if_changed,objcopy)

clean-files	+= uImage uImage.srec \
		   zImage zImage.srec \
		   vmlinux.srec vmlinux.bin vmlinux.bin.gz
