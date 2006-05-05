include $(TOPDIR)/rules.mk

LOADADDR = 0x8108c8f4		# RAM start + 16M 
KERNEL_ENTRY = 0x80100000	# Default kernel entry in arch/mips/Makefile
RAMSIZE = 0x04000000		# 64MB

LOADER_MAKEOPTS= \
		KDIR=$(KDIR) \
		LOADADDR=$(LOADADDR) \
		KERNEL_ENTRY=$(KERNEL_ENTRY) \
		RAMSIZE=$(RAMSIZE)

KDIR:=$(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)

lzma-loader-clean:
	$(MAKE) -C ../generic/lzma-loader $(LOADER_MAKEOPTS) clean

lzma-loader-prepare:
	$(MAKE) -C ../generic/lzma-loader $(LOADER_MAKEOPTS) prepare

lzma-loader-compile: $(KDIR)/vmlinux.lzma lzma-loader-prepare
	$(MAKE) -C ../generic/lzma-loader $(LOADER_MAKEOPTS) compile

ifeq ($(IB),)
$(KDIR)/vmlinux.lzma: $(KDIR)/vmlinux
	cat $^ | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $@ || (rm -f $@ && false)
	
install: $(KDIR)/vmlinux.lzma
endif

ifeq ($(FS),jffs2-8MB)

$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FSNAME).bin: $(KDIR)/root.$(FS)
	$(TARGET_CROSS)objcopy -O srec -I binary --adjust-vma 0xbe000000 $(KDIR)/root.$(FS) $(KDIR)/root.srec
	grep -v S7 $(KDIR)/root.srec > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).img
	grep -v S0 $(LINUX_DIR)/arch/mips/boot/compressed/images/zImage.flash.srec >> $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).img

install: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FSNAME).bin
endif

ifeq ($(FS),tgz)

$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-rootfs.tar.gz: $(KDIR)/root.tar.gz
	$(CP) $^ $@

install: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-rootfs.tar.gz

endif

ifeq ($(FS),squashfs)
$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FSNAME).bin: $(KDIR)/root.$(FS)
	$(TARGET_CROSS)objcopy -O srec -I binary --adjust-vma 0xbe000000 $(KDIR)/root.$(FS) $(KDIR)/root.srec
	grep -v S7 $(KDIR)/root.srec > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).img
	grep -v S0 $(LINUX_DIR)/arch/mips/boot/compressed/images/zImage.flash.srec >> $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).img

endif

$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinuz: $(KDIR)/zImage.flash
	$(CP) $^ $@

ifeq ($(IB),)
clean: lzma-loader-clean
prepare: lzma-loader-prepare
compile: lzma-loader-compile
else
clean:
prepare:
compile:
endif
install-ib:
	mkdir -p $(IB_DIR)/staging_dir_$(ARCH)/bin
	mkdir -p $(IB_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD)
	$(CP) $(KDIR)/loader.elf $(IB_DIR)/build_$(ARCH)/
	$(CP) $(KDIR)/vmlinux.lzma $(IB_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD)/
	$(TARGET_CROSS)objcopy -O srec -I binary --adjust-vma 0xbe000000 $(KDIR)/loader.elf $(KDIR)/loader.srec
	grep -v S0 $(KDIR)/loader.srec > $(KDIR)/kernel.srec
