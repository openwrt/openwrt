# $Id: Makefile 1823 2005-09-01 20:07:42Z nbd $

include $(TOPDIR)/rules.mk

PKG_NAME := loader
PKG_VERSION := 0.05

PKG_BUILD_DIR := $(KDIR)/$(PKG_NAME)-$(PKG_VERSION)

$(PKG_BUILD_DIR)/.prepared:
	mkdir $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	touch $@

$(PKG_BUILD_DIR)/lzma.elf: $(PKG_BUILD_DIR)/.prepared $(PKG_BUILD_DIR)/vmlinux.lzma
	$(MAKE) -C $(PKG_BUILD_DIR) CC=$(TARGET_CC) \
		LD=$(TARGET_CROSS)ld CROSS_COMPILE=$(TARGET_CROSS) \
		RAMSIZE=$(RAMSIZE) \
		LOADADDR=$(LOADADDR) \
		KERNEL_ENTRY=$(KERNEL_ENTRY)


$(PKG_BUILD_DIR)/vmlinux.lzma: $(KDIR)/vmlinux.lzma
	$(CP) $< $@

$(KDIR)/loader.elf: $(PKG_BUILD_DIR)/lzma.elf
	$(CP) $< $@

source: 
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(KDIR)/loader.elf
install:

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(KDIR)/loader.elf
