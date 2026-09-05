#
# LZMA loader for RouterBOOT based MikroTik devices on qualcommax.
#
# RouterBOOT only loads bare ARM64 ELF images, neither FIT nor uImage. The
# yaffs kernel partition is also just 8 MiB while the raw arm64 image is about
# 14 MiB. This loader embeds the LZMA compressed kernel and the DTB in
# a single PT_LOAD segment, decompresses them at runtime and jumps to the
# kernel. Called from the image recipe as:
#   $(MAKE) -C .../mikrotik-lzma-loader KERNEL_IMAGE=... DTB=... OUTPUT=...
#
PKG_MAINTAINER:=André Valentin <avalentin@marcant.net>
PKG_LICENSE:=GPL-2.0-or-later LGPL-2.1-or-later
PKG_LICENSE_FILES:=src/LzmaDecode.c

LOADER_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))src
BUILD ?= $(LOADER_DIR)/build

CFLAGS := -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-pic \
	  -mgeneral-regs-only -Wall -Wno-unused-function -I$(LOADER_DIR)

.PHONY: all clean
all: $(OUTPUT)

$(BUILD):
	mkdir -p $@

# "lzma e" writes the .lzma container: 5 bytes of properties, 8 bytes of
# uncompressed size, then the data. The loader reads the size from there, so it
# does not need rebuilding when the kernel grows.
$(BUILD)/kernel.img.lzma: $(KERNEL_IMAGE) | $(BUILD)
	rm -f $@
	$(STAGING_DIR_HOST)/bin/lzma e $< $@

$(BUILD)/chateau.dtb: $(DTB) | $(BUILD)
	cp $< $@

$(BUILD)/%.o: $(LOADER_DIR)/%.S $(BUILD)/kernel.img.lzma $(BUILD)/chateau.dtb | $(BUILD)
	$(TARGET_CC) $(CFLAGS) -I$(BUILD) -c -o $@ $<

$(BUILD)/%.o: $(LOADER_DIR)/%.c | $(BUILD)
	$(TARGET_CC) $(CFLAGS) -c -o $@ $<

$(OUTPUT): $(BUILD)/start.o $(BUILD)/main.o $(BUILD)/LzmaDecode.o $(BUILD)/payload.o
	$(TARGET_CC) -nostdlib -nostartfiles -Wl,-T,$(LOADER_DIR)/loader.lds \
		-Wl,--build-id=none -o $@ $^

clean:
	rm -rf $(BUILD)
