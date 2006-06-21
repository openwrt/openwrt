include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

LOADADDR = 0x81000000		# RAM start + 16M 
KERNEL_ENTRY = 0x80101000
RAMSIZE = 0x00100000		# 1MB
IMAGE_COPY = 1

LOADER_MAKEOPTS= \
		KDIR=$(KDIR) \
		LOADADDR=$(LOADADDR) \
		KERNEL_ENTRY=$(KERNEL_ENTRY) \
		RAMSIZE=$(RAMSIZE) \
		IMAGE_COPY=$(IMAGE_COPY)

define Build/Clean
	$(MAKE) -C ../generic/lzma-loader $(LOADER_MAKEOPTS) clean
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
	$(MAKE) -C ../generic/lzma-loader $(LOADER_MAKEOPTS) clean compile
endef

define Image/BuildKernel
	$(CP) $(KDIR)/loader.elf $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinux
endef

PARTITION1=\x80\x01\x01\x00\x27\x01\x20\x7b\x20\x00\x00\x00\xe0\x1e\x00\x00# 4 MB  (kernel part)
PARTITION2=\x00\x00\x01\x7c\x83\x01\xa0\x64\x00\x1f\x00\x00\x40\x7a\x00\x00# 16 MB (rootfs part)

define Image/Build/jffs2-128k
	( \
		echo -ne OWRT | dd bs=$$$$((0x1be)) conv=sync; \
		( \
			echo -ne '$(strip $(PARTITION1))'; \
			echo -ne '$(strip $(PARTITION2))'; \
		) | dd bs=$$$$((0x40)) conv=sync; \
		echo -ne '\x55\xaa'; \
		dd if=/dev/zero bs=$$$$((0x3e00)) conv=sync count=1; \
		dd if=$(KDIR)/loader.elf bs=$$$$((0x3dc000)) conv=sync; \
		cat $(KDIR)/root.$(1); \
		echo -ne '\xde\xad\xc0\xde'; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).bin
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))
