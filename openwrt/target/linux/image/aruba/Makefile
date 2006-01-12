include $(TOPDIR)/rules.mk

KDIR:=$(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)

lzma-loader-clean:
	$(MAKE) -C lzma-loader clean

lzma-loader-prepare:
	$(MAKE) -C lzma-loader prepare

lzma-loader-compile: $(KDIR)/vmlinux.lzma lzma-loader-prepare
	$(MAKE) -C lzma-loader compile KDIR=$(KDIR)

ifeq ($(IB),)
$(KDIR)/vmlinux.lzma: $(KDIR)/vmlinux
	cat $^ | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $@ || (rm -f $@ && false)

$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).ari: $(KDIR)/vmlinux.lzma
install: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).ari
endif

ifeq ($(FS),jffs2-8MB)
TRXALIGN:=-a 0x20000
endif
ifeq ($(FS),jffs2-4MB)
TRXALIGN:=-a 0x10000
endif

ifeq ($(KERNEL),2.6)
FSNAME:=$(patsubst jffs2-%,jffs2,$(FS))

ifeq ($(FS),jffs2-4MB)
$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FSNAME).ari: $(KDIR)/loader.elf
	./addVersion -n ArubaOS $(KDIR)/loader.elf $@ version

$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FSNAME).bin: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FSNAME).ari $(KDIR)/root.$(FS)
	@dd if=$< of=$@.tmp bs=655360 conv=sync
	@cat $(KDIR)/root.$(FS) >> $@.tmp
	@dd if=$@.tmp of=$@ bs=3604480 conv=sync
	@rm -f $@.tmp

install: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FSNAME).bin
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
	cp -fpR $(KDIR)/loader.elf $(IB_DIR)/build_$(ARCH)/
	cp -fpR $(KDIR)/vmlinux.lzma $(IB_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD)/
