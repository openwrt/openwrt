include ../image.mk

define Build/Compile
	rm -f $(KDIR)/loader.gz
	$(MAKE) -C lzma-loader \
		BUILD_DIR="$(KDIR)" \
		TARGET="$(KDIR)" \
		install
	echo -ne "\\x00" >> $(KDIR)/loader.gz
endef

define Build/Clean
	$(MAKE) -C lzma-loader clean
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
endef

define Image/Build/wgt634u
	dd if=$(KDIR)/loader.elf of=$(BIN_DIR)/openwrt-wgt634u-$(KERNEL)-$(2).bin bs=131072 conv=sync
	cat $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).trx >> $(BIN_DIR)/openwrt-wgt634u-$(KERNEL)-$(2).bin
endef

define Image/Build/CyberTAN
	$(STAGING_DIR)/bin/addpattern -4 -p $(3) -v v$(4) -i $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).trx -o $(BIN_DIR)/openwrt-$(2)-$(KERNEL)-$(5).bin
endef

define Image/Build/Motorola
	$(STAGING_DIR)/bin/motorola-bin -$(3) $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).trx $(BIN_DIR)/openwrt-$(2)-$(4).bin
endef

define trxalign/jffs2-128k
-a 0x20000
endef
define trxalign/jffs2-64k
-a 0x10000
endef
define trxalign/squashfs
-a 1024
endef

define Image/Build
	$(STAGING_DIR)/bin/trx -o $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).trx $(KDIR)/loader.gz $(KDIR)/vmlinux.lzma $(call trxalign/$(1)) $(KDIR)/root.$(1)
ifneq ($(1),jffs2-128k)
	$(call Image/Build/CyberTAN,$(1),wrt54g3g,W54F,1.99.5,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt54g,W54G,4.40.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt54gs_v4,W54s,1.09.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Motorola,$(1),wa840g,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Motorola,$(1),we800g,3,$(patsubst jffs2-%,jffs2,$(1)))
endif
ifneq ($(1),jffs2-64k)
	$(call Image/Build/CyberTAN,$(1),wrt54gs,W54S,4.80.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrtsl54gs,W54U,2.00.5,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/wgt634u,$(1),$(patsubst jffs2-%,jffs2,$(1)))
endif
	$(call Image/Build/Motorola,$(1),wr850g,1,$(1))
endef

$(eval $(call BuildImage))
