include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE=128k 16k
#JFFS2OPTS += -n -s 2048
JFFS2OPTS += -n

define Image/BuildKernel
	mkdir -p $(BIN_DIR)
	cp $(KDIR)/vmlinuz $(BIN_DIR)/openwrt-$(BOARD)-zImage
endef

define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/zImage $(KDIR)/vmlinuz
endef


define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-root.$(1))
endef

$(eval $(call BuildImage))
