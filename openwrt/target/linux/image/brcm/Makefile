include $(TOPDIR)/rules.mk

KDIR:=$(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)

lzma-loader-clean:
	$(MAKE) -C lzma-loader clean

lzma-loader-prepare:
	$(MAKE) -C lzma-loader prepare

lzma-loader-compile: lzma-loader-prepare
	$(MAKE) -C lzma-loader compile

ifeq ($(IB),)
$(KDIR)/vmlinux.lzma: $(KDIR)/vmlinux
	cat $^ | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $@ || (rm -f $@ && false)

$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).trx: $(KDIR)/vmlinux.lzma compile
endif

ifeq ($(FS),jffs2-8MB)
TRXALIGN:=-a 0x20000
endif
ifeq ($(FS),jffs2-4MB)
TRXALIGN:=-a 0x10000
endif
$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).trx:
	$(STAGING_DIR)/bin/trx -o $@ $(BUILD_DIR)/loader.gz $(KDIR)/vmlinux.lzma $(TRXALIGN) $(KDIR)/root.$(FS)


ifeq ($(KERNEL),2.4)
FSNAME:=$(patsubst jffs2-%,jffs2,$(FS))

ifneq ($(FS),jffs2-8MB)
$(BIN_DIR)/openwrt-wrt54g-$(FSNAME).bin: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).trx
	$(STAGING_DIR)/bin/addpattern -4 -p W54G -v v4.20.6 -i $< -o $@ -g

install: $(BIN_DIR)/openwrt-wrt54g-$(FSNAME).bin
endif

ifneq ($(FS),jffs2-4MB)
$(BIN_DIR)/openwrt-wrt54gs-$(FSNAME).bin: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).trx
	$(STAGING_DIR)/bin/addpattern -4 -p W54S -v v4.70.6 -i $< -o $@ -g

install: $(BIN_DIR)/openwrt-wrt54gs-$(FSNAME).bin
endif

$(BIN_DIR)/openwrt-motorola-$(FS).bin: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).trx
	$(STAGING_DIR)/bin/motorola-bin $< $@
install: $(BIN_DIR)/openwrt-motorola-$(FS).bin
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
install: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).trx
install-ib:
	mkdir -p $(IB_DIR)/staging_dir_$(ARCH)/bin
	cp -dpR $(STAGING_DIR)/bin/addpattern $(STAGING_DIR)/bin/trx $(STAGING_DIR)/bin/motorola-bin $(IB_DIR)/staging_dir_$(ARCH)/bin
	mkdir -p $(IB_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD)
	cp -dpR $(BUILD_DIR)/loader.gz $(IB_DIR)/build_$(ARCH)/
	cp -dpR $(KDIR)/vmlinux.lzma $(IB_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD)/
