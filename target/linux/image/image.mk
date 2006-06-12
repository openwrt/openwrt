include $(TOPDIR)/rules.mk

KDIR:=$(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)

ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
ifeq ($(CONFIG_TARGET_ROOTFS_JFFS2),y)
include $(TOPDIR)/target/linux/image/jffs2.mk
endif

ifeq ($(CONFIG_TARGET_ROOTFS_SQUASHFS_LZMA),y)
include $(TOPDIR)/target/linux/image/squashfs.mk
endif

ifeq ($(CONFIG_TARGET_ROOTFS_TGZ),y)
include $(TOPDIR)/target/linux/image/tgz.mk
endif
endif

define Image/mkfs/prepare/default
	find $(BUILD_DIR)/root -type f -not -perm +0100 | xargs chmod 0644
	find $(BUILD_DIR)/root -type f -perm +0100 | xargs chmod 0755
	find $(BUILD_DIR)/root -type d | xargs chmod 0755
	mkdir -p $(BUILD_DIR)/root/tmp
	chmod 0777 $(BUILD_DIR)/root/tmp
endef

define Image/mkfs/prepare
	$(call Image/mkfs/prepare/default)
endef

define BuildImage
compile:
	$(call Build/Compile)

install:
	$(call Image/Prepare)
	$(call Image/mkfs/prepare)
	$(call Image/BuildKernel)
	$(call Image/mkfs/jffs2)
	$(call Image/mkfs/squashfs)
	$(call Image/mkfs/tgz)
	
clean:
	$(call Build/Clean)
endef

compile-targets:
install-targets:
clean-targets:

prepare:
compile: compile-targets
install: compile install-targets
clean: clean-targets
