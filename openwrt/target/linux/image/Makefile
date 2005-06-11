include $(TOPDIR)/rules.mk

ifeq ($(BOARD),)
BOARD:=brcm
endif

KDIR:=$(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)

ifeq ($(BR2_TARGET_ROOTFS_JFFS2),y)
include ./jffs2.mk
endif

ifeq ($(BR2_TARGET_ROOTFS_SQUASHFS_LZMA),y)
include ./squashfs.mk
endif

prepare:
	$(MAKE) -C $(BOARD) prepare
compile:
	$(MAKE) -C $(BOARD) compile
install:
rebuild: clean prepare compile install
clean:

