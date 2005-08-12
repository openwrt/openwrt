include $(TOPDIR)/rules.mk

KDIR:=$(BUILD_DIR)/linux-$(KERNEL)-ar7

PKG_BUILD_DIR:=$(BUILD_DIR)/ar7loader

LOADADDR := 0x94020000
KERNEL_ENTRY := 0x${shell nm $(KDIR)/linux-$(KERNEL)*/vmlinux | grep kernel_entry | cut -d' ' -f1}
OUTPUT_FORMAT := elf32-tradlittlemips

CFLAGS := -D__KERNEL__ -Wall -Wstrict-prototypes -Wno-trigraphs -Os \
	-fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic \
	-pipe -mlong-calls -fno-common \
	-mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap \
	-DLOADADDR=$(LOADADDR)

$(PKG_BUILD_DIR):
	mkdir -p $(PKG_BUILD_DIR)

$(PKG_BUILD_DIR)/zimage.script: src/zimage.script.in
	sed -e 's/@@OUTPUT_FORMAT@@/$(OUTPUT_FORMAT)/' \
	    -e 's/@@LOADADDR@@/$(LOADADDR)/' <$< >$@

$(PKG_BUILD_DIR)/ld.script: src/ld.script.in
	sed -e 's/@@OUTPUT_FORMAT@@/$(OUTPUT_FORMAT)/' \
	    -e 's/@@LOADADDR@@/$(LOADADDR)/' <$< >$@

$(PKG_BUILD_DIR)/LzmaDecode.o: src/LzmaDecode.c
	$(TARGET_CC) $(CFLAGS) -c -o $@ $<

$(PKG_BUILD_DIR)/loader.o: src/loader.c
	$(TARGET_CC) $(CFLAGS) -c -o $@ $<

$(PKG_BUILD_DIR)/srec2bin: src/srec2bin.c
	$(HOSTCC) -o $@ $<

$(KDIR)/vmlinux.gz: $(KDIR)/vmlinux
	gzip -c -vf9 < $< > $@

$(KDIR)/vmlinux.lzma: $(KDIR)/vmlinux
	cat $^ | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $@ || (rm -f $@ && false)

$(BIN_DIR)/openwrt-ar7-2.4-kernel.bin: $(KDIR)/vmlinux.lzma compile
	$(TARGET_CROSS)ld -T $(PKG_BUILD_DIR)/zimage.script -r -b binary $< -o $(KDIR)/zimage.o
	$(TARGET_CROSS)ld -static -G 0 --defsym kernel_entry=$(KERNEL_ENTRY) -T $(PKG_BUILD_DIR)/ld.script \
		$(PKG_BUILD_DIR)/loader.o \
		$(PKG_BUILD_DIR)/LzmaDecode.o \
		$(KDIR)/zimage.o \
		-o $(KDIR)/loader
	$(TARGET_CROSS)objcopy -O srec $(KDIR)/loader $(KDIR)/ram_zimage.sre
	$(PKG_BUILD_DIR)/srec2bin $(KDIR)/ram_zimage.sre $@

ifeq ($(FS),jffs2-8MB)
ALIGN:=bs=131072 conv=sync
endif
ifeq ($(FS),jffs2-4MB)
ALIGN:=bs=65536 conv=sync
endif

$(BIN_DIR)/openwrt-ar7-$(KERNEL)-$(FS).bin: $(BIN_DIR)/openwrt-ar7-2.4-kernel.bin
	dd if=$< $(ALIGN) > $@
	cat $(BUILD_DIR)/linux-$(KERNEL)-ar7/root.$(FS) >> $@

define pattern_template
$(BIN_DIR)/openwrt-ar7-$(KERNEL)-$(FS)-$(1).bin: $(BIN_DIR)/openwrt-ar7-$(KERNEL)-$(FS).bin
	(dd if=/dev/zero bs=16 count=1; cat $(BIN_DIR)/openwrt-ar7-$(KERNEL)-$(FS).bin) | \
		$(STAGING_DIR)/bin/addpattern -p $(1) $(2) -o $(BIN_DIR)/openwrt-ar7-$(KERNEL)-$(FS)-$(1).bin

install: $(BIN_DIR)/openwrt-ar7-$(KERNEL)-$(FS)-$(1).bin
endef

$(eval $(call pattern_template,WA22))
$(eval $(call pattern_template,WAG2))
$(eval $(call pattern_template,WA21))
$(eval $(call pattern_template,WA32,-b))

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(BIN_DIR)/openwrt-ar7*

prepare: $(PKG_BUILD_DIR) $(PKG_BUILD_DIR)/zimage.script $(PKG_BUILD_DIR)/ld.script
compile: prepare $(PKG_BUILD_DIR)/loader.o $(PKG_BUILD_DIR)/LzmaDecode.o $(PKG_BUILD_DIR)/srec2bin
install: $(BIN_DIR)/openwrt-ar7-2.4-kernel.bin $(BIN_DIR)/openwrt-ar7-$(KERNEL)-$(FS).bin

