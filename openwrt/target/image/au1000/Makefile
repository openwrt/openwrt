include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

LOADADDR = 0x8108c8f4		# RAM start + 16M 
KERNEL_ENTRY = 0x80100000	# Default kernel entry in arch/mips/Makefile
RAMSIZE = 0x04000000		# 64MB

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
	$(MAKE) -C ../generic/lzma-loader $(LOADER_MAKEOPTS) compile
endef

define Image/Build
	$(TARGET_CROSS)objcopy -O srec -I binary --adjust-vma 0xbe000000 $(KDIR)/root.$(1) $(KDIR)/root.srec
	grep -v S7 $(KDIR)/root.srec > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
	grep -v S0 $(LINUX_DIR)/arch/mips/boot/compressed/images/zImage.flash.srec >> $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
endef

$(eval $(call BuildImage))
