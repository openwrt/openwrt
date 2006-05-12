include $(TOPDIR)/rules.mk

KDIR:=$(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)

ifeq ($(CONFIG_TARGET_ROOTFS_JFFS2),y)
include ./jffs2.mk
endif

ifeq ($(CONFIG_TARGET_ROOTFS_SQUASHFS_LZMA),y)
include ./squashfs.mk
endif

ifeq ($(CONFIG_TARGET_ROOTFS_TGZ),y)
include ./tgz.mk
endif

prepare:
	$(MAKE) prepare-targets
compile:
	$(MAKE) compile-targets
install:
	$(MAKE) install-targets

.PHONY: prepare compile install

$(BOARD)-compile:
	$(MAKE) -C $(BOARD) prepare
	$(MAKE) -C $(BOARD) compile

install-ib:
	-$(MAKE) -C $(BOARD) IB_DIR="$(IB_DIR)" install-ib
	mkdir -p $(IB_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD)
	$(CP) $(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)/kernel[-_]*.ipk $(IB_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD)/	

install-prepare:
	find $(KDIR)/root -type f -not -perm +0100 | xargs chmod 0644
	find $(KDIR)/root -type f -perm +0100 | xargs chmod 0755
	find $(KDIR)/root -type d | xargs chmod 0755
	mkdir -p $(KDIR)/root/tmp
	chmod 0777 $(KDIR)/root/tmp

rebuild: clean prepare compile install
clean:
	$(MAKE) clean-targets
