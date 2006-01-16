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

$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL).ari: $(KDIR)/vmlinux.lzma
install: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL).ari
endif

ifeq ($(KERNEL),2.6)
FSNAME:=$(patsubst jffs2-%,jffs2,$(FS))

ifeq ($(FS),jffs2-4MB)
$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL).ari: $(KDIR)/loader.elf
	./addVersion -n ArubaOS $(KDIR)/loader.elf $@ version

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
	cp -fpR $(KDIR)/loader.elf $(IB_DIR)/build_$(ARCH)/
	cp -fpR $(KDIR)/vmlinux.lzma $(IB_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD)/
