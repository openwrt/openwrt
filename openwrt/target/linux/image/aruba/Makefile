include ../image.mk

LOADADDR = 0x81000000		# RAM start + 16M 
KERNEL_ENTRY = 0x80100000
RAMSIZE = 0x00100000		# 1MB

LOADER_MAKEOPTS= \
		KDIR=$(KDIR) \
		LOADADDR=$(LOADADDR) \
		KERNEL_ENTRY=$(KERNEL_ENTRY) \
		RAMSIZE=$(RAMSIZE)

define Build/Clean
	$(MAKE) -C ../generic/lzma-loader $(LOADER_MAKEOPTS) clean
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
	$(MAKE) -C ../generic/lzma-loader $(LOADER_MAKEOPTS) clean compile
endef

define Image/Build
	./addVersion -n ArubaOS $(KDIR)/loader.elf $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL).ari version
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
ifeq ($(1),jffs2-64k)
endif
	@dd if=$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL).ari of=$(KDIR)/image.tmp bs=655360 conv=sync
	@cat $(KDIR)/root.$(1) >> $(KDIR)/image.tmp
	@dd if=$(KDIR)/image.tmp of=$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(patsubst jffs2-%,jffs2,$(1)).bin bs=3604480 conv=sync
	@rm -f $(KDIR)/image.tmp
endif
endef

$(eval $(call BuildImage))
