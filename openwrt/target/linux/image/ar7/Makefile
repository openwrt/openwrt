include $(TOPDIR)/rules.mk

KDIR:=$(BUILD_DIR)/linux-$(KERNEL)-ar7

PKG_BUILD_DIR:=$(BUILD_DIR)/ar7loader

LOADADDR := 0x94020000
OUTPUT_FORMAT := elf32-tradlittlemips

CFLAGS := -D__KERNEL__ -Wall -Wstrict-prototypes -Wno-trigraphs -Os \
	-fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic \
	-pipe -mlong-calls -fno-common \
	-mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap \


$(PKG_BUILD_DIR):
	mkdir -p $(PKG_BUILD_DIR)

$(PKG_BUILD_DIR)/zimage.script: src/zimage.script.in
	sed -e 's/@@OUTPUT_FORMAT@@/$(OUTPUT_FORMAT)/' \
	    -e 's/@@LOADADDR@@/$(LOADADDR)/' <$< >$@

$(PKG_BUILD_DIR)/ld.script: src/ld.script.in
	sed -e 's/@@OUTPUT_FORMAT@@/$(OUTPUT_FORMAT)/' \
	    -e 's/@@LOADADDR@@/$(LOADADDR)/' <$< >$@

$(PKG_BUILD_DIR)/loader.o: src/loader.c
	$(TARGET_CC) $(CFLAGS) -c -o $@ $<

$(PKG_BUILD_DIR)/srec2bin: src/srec2bin.c
	$(HOSTCC) -o $@ $<

$(KDIR)/vmlinux.gz: $(KDIR)/vmlinux
	gzip -c -vf9 < $< > $@

$(BIN_DIR)/openwrt-ar7-zimage.bin: $(KDIR)/vmlinux.gz compile
	$(TARGET_CROSS)ld -T $(PKG_BUILD_DIR)/zimage.script -r -b binary $< -o $(KDIR)/zimage.o
	$(TARGET_CROSS)ld -static -G 0 -no-warn-mismatch -R $(KDIR)/linux-$(KERNEL)*/vmlinux -T $(PKG_BUILD_DIR)/ld.script \
		$(PKG_BUILD_DIR)/loader.o \
		$(KDIR)/zimage.o \
		-o $(KDIR)/loader
	$(TARGET_CROSS)objcopy -O srec $(KDIR)/loader $(KDIR)/ram_zimage.sre
	$(PKG_BUILD_DIR)/srec2bin $(KDIR)/ram_zimage.sre $@
	
clean:
	rm -rf $(PKG_BUILD_DIR)
prepare: $(PKG_BUILD_DIR) $(PKG_BUILD_DIR)/zimage.script $(PKG_BUILD_DIR)/ld.script
compile: prepare $(PKG_BUILD_DIR)/loader.o $(PKG_BUILD_DIR)/srec2bin
install: $(BIN_DIR)/openwrt-ar7-zimage.bin
	
