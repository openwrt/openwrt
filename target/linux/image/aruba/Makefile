include $(TOPDIR)/rules.mk

LOADADDR = 0x81000000		# RAM start + 16M 
KERNEL_ENTRY = 0x80100000
RAMSIZE = 0x00100000		# 1MB

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

$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL).ari: $(KDIR)/vmlinux.lzma
install: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL).ari
endif

ifeq ($(KERNEL),2.6)
FSNAME:=$(patsubst jffs2-%,jffs2,$(FS))

ifeq ($(FS),jffs2-4MB)
$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL).ari: $(KDIR)/loader.elf
	./addVersion -n ArubaOS $(KDIR)/loader.elf $@ version

ifneq ($(BR2_ARUBA_RAMDISK),y)
$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FSNAME).bin: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL).ari $(KDIR)/root.$(FS)
	@dd if=$< of=$@.tmp bs=655360 conv=sync
	@cat $(KDIR)/root.$(FS) >> $@.tmp
	@dd if=$@.tmp of=$@ bs=3604480 conv=sync
	@rm -f $@.tmp

install: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FSNAME).bin
endif

install: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL).ari
endif
endif

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
