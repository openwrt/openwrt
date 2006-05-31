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

prepare: FORCE
	$(MAKE) prepare-targets
compile: FORCE
	$(MAKE) compile-targets
install: FORCE
	$(MAKE) install-targets

$(BOARD)-compile: FORCE
	$(MAKE) -C $(BOARD) prepare
	$(MAKE) -C $(BOARD) compile

install-ib: FORCE
	-$(MAKE) -C $(BOARD) IB_DIR="$(IB_DIR)" install-ib
	mkdir -p $(IB_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD)
	$(CP) $(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)/kernel[-_]*.ipk $(IB_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD)/	

install-prepare: FORCE
	find $(BUILD_DIR)/root -type f -not -perm +0100 | xargs chmod 0644
	find $(BUILD_DIR)/root -type f -perm +0100 | xargs chmod 0755
	find $(BUILD_DIR)/root -type d | xargs chmod 0755
	mkdir -p $(BUILD_DIR)/root/tmp
	chmod 0777 $(BUILD_DIR)/root/tmp

rebuild: clean prepare compile install
clean: FORCE
	$(MAKE) clean-targets
