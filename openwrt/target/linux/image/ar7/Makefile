include ../image.mk

PKG_BUILD_DIR:=$(BUILD_DIR)/ar7loader

LOADADDR := 0x94020000
OUTPUT_FORMAT := elf32-tradlittlemips

CFLAGS := -D__KERNEL__ -Wall -Wstrict-prototypes -Wno-trigraphs -Os \
	-fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic \
	-pipe -mlong-calls -fno-common \
	-mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap \
	-DLOADADDR=$(LOADADDR)

$(PKG_BUILD_DIR)/cksum.o: $(PKG_BUILD_DIR)/cksum.c
	$(HOSTCC) -o $@ $<

$(PKG_BUILD_DIR)/ckmain.o: $(PKG_BUILD_DIR)/ckmain.c
	$(HOSTCC) -o $@ $<

$(PKG_BUILD_DIR)/tichksum: $(PKG_BUILD_DIR)/ckmain.o $(PKG_BUILD_DIR)/cksum.o
	$(HOSTCC) -o $@ $<

$(PKG_BUILD_DIR)/LzmaDecode.o: src/LzmaDecode.c
	$(TARGET_CC) $(CFLAGS) -c -o $@ $<

$(PKG_BUILD_DIR)/loader.o: src/loader.c
	$(TARGET_CC) $(CFLAGS) -c -o $@ $<

$(STAGING_DIR)/bin/srec2bin: src/srec2bin.c
	$(HOSTCC) -o $@ $<


define Build/Compile
	mkdir -p $(PKG_BUILD_DIR)
	sed -e 's/@@OUTPUT_FORMAT@@/$(OUTPUT_FORMAT)/' \
	    -e 's/@@LOADADDR@@/$(LOADADDR)/' \
			< src/zimage.script.in \
			> $(PKG_BUILD_DIR)/zimage.script
	sed -e 's/@@OUTPUT_FORMAT@@/$(OUTPUT_FORMAT)/' \
	    -e 's/@@LOADADDR@@/$(LOADADDR)/' \
			< src/ld.script.in \
			> $(PKG_BUILD_DIR)/ld.script
	$(MAKE) $(PKG_BUILD_DIR)/loader.o $(PKG_BUILD_DIR)/LzmaDecode.o $(STAGING_DIR)/bin/srec2bin
endef

define Build/Clean
	rm -rf $(PKG_BUILD_DIR)
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
	$(TARGET_CROSS)ld -T $(PKG_BUILD_DIR)/zimage.script -r -b binary $(KDIR)/vmlinux.lzma -o $(KDIR)/zimage.o
	$(TARGET_CROSS)ld -static -G 0 --defsym kernel_entry=0x$${shell nm $(KDIR)/linux-*/vmlinux | grep kernel_entry | cut -d' ' -f1} -T $(PKG_BUILD_DIR)/ld.script \
		$(PKG_BUILD_DIR)/loader.o \
		$(PKG_BUILD_DIR)/LzmaDecode.o \
		$(KDIR)/zimage.o \
		-o $(KDIR)/loader
	$(TARGET_CROSS)objcopy -O srec $(KDIR)/loader $(KDIR)/ram_zimage.sre
	$(STAGING_DIR)/bin/srec2bin $(KDIR)/ram_zimage.sre $(KDIR)/vmlinux.bin
endef

ifeq ($(FS),jffs2-8MB)
ALIGN:=bs=131072 conv=sync
endif
ifeq ($(FS),jffs2-4MB)
ALIGN:=bs=65536 conv=sync
endif

define Image/Build/CyberTAN
	(dd if=/dev/zero bs=16 count=1; cat $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).bin) | \
		$(STAGING_DIR)/bin/addpattern -p $(3) -o $(BIN_DIR)/openwrt-$(2)-$(KERNEL)-$(4).bin
endef

define Image/Build/sErCoMm
	cat sercomm/adam2.bin "$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).bin" > "$(BIN_DIR)/openwrt-$(2)-$(KERNEL)-$(3).img"
	dd if=sercomm/$(2) of="$(KDIR)/dgfw.tmp" bs=$$$$((0x3e0000 - 80)) seek=1 conv=notrunc
	$(STAGING_DIR)/bin/dgfirmware -f -w "$(BIN_DIR)/openwrt-$(2)-$(KERNEL)-$(3).img" "$(KDIR)/dgfw.tmp"
	rm -f "$(KDIR)/dgfw.tmp"
endef

define Image/Build
	dd if=$(KDIR)/vmlinux.bin $(ALIGN) > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).bin
	cat $(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)/root.$(1) >> $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).bin
	$(call Image/Build/CyberTAN,$(1),AG1B,AG1B,$(1))
	$(call Image/Build/CyberTAN,$(1),WA21,WA21,$(1))
	$(call Image/Build/CyberTAN,$(1),WA22,WA22,$(1))
	$(call Image/Build/CyberTAN,$(1),WAG2,WAG2,$(1))
	$(call Image/Build/CyberTAN,$(1),WA31,WA31 -b,$(1))
	$(call Image/Build/CyberTAN,$(1),WA32,WA32 -b,$(1))
	$(call Image/Build/sErCoMm,$(1),dg834,$(1))
	$(call Image/Build/sErCoMm,$(1),jdr454wb,$(1))
endef

$(eval $(call BuildImage))
